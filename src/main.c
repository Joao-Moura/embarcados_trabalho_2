#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include "modbus.h"
#include "uart.h"
#include "bme280.h"
#include "gpio.h"
#include "vars.h"
#include "pid.h"


void finaliza_programa (int sinal) {
    printf("Finalizando programa ...\n");
    // HACK: como as funções da gpio são inversas, settando uma pra 0 a outra já vai estar em 0
    set_controle(0);
    reseta_uart();
    close(uart0_fd);
    exit(0);
}

void init() {
    pid_configura_constantes(30.0, 0.2, 400.0);
    uart0_fd = inicia_uart();
    inicia_gpio();
    // inicia_bme280("/dev/i2c-20");
    reseta_uart();
    le_msg(uart0_fd, buffer_escrita, NULL);
}

int main (int argc, char *argv[]) {
    signal(SIGINT, finaliza_programa);
    init();
    // set_controle(-100);

    while(1) {
        monta_msg(buffer_envio, &tamanho_mensagem, 0x23, 0xC3, NULL, 0);

        if (uart0_fd != -1) {
            int dado = 0;
            int tamanho_buffer = le_msg(uart0_fd, buffer_escrita, (void *)&dado);

            if (tamanho_buffer > 0) {
                printf("Dado lido: %d\n", dado);
                if ((dado == 161) | (dado == 162)) {
                    dado &= 0x1;
                    monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD3, (void *)&dado, 1);
                    le_msg(uart0_fd, buffer_escrita, (void *)&dado);
                    estado_atual.ligado = dado;
                } else if (((dado == 163) | (dado == 164)) & estado_atual.ligado) {
                    dado &= 0x1;
                    monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD5, (void *)&dado, 1);
                    le_msg(uart0_fd, buffer_escrita, (void *)&dado);
                    estado_atual.em_funcionamento = dado;
                } else if ((dado == 165) & estado_atual.ligado) {
                    estado_atual.modo_controle ^= 1;
                    monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD4, (void *)&estado_atual.modo_controle, 1);
                    le_msg(uart0_fd, buffer_escrita, (void *)&dado);
                } else {
                    monta_msg(buffer_envio, &tamanho_mensagem, 0x23, 0xC1, NULL, 0);
                    le_msg(uart0_fd, buffer_escrita, (void *)&estado_atual.temperatura_interna);
                    monta_msg(buffer_envio, &tamanho_mensagem, 0x23, 0xC2, NULL, 0);
                    le_msg(uart0_fd, buffer_escrita, (void *)&estado_atual.temperatura_referencia);
                    printf("Temperaturas interna: %.2f e de referencia: %.2f\n", estado_atual.temperatura_interna, estado_atual.temperatura_referencia); 

                    if (estado_atual.em_funcionamento) {
                        pid_atualiza_referencia(estado_atual.temperatura_referencia);
                        int potencia = (int) pid_controle(estado_atual.temperatura_interna);
                        printf("=============== NOVA POTENCIA = %d =========\n", potencia);
                        set_controle(potencia);
                    }
                }
            } else {
                usleep(250000);
            }
        }
    }

    return 0;
}
