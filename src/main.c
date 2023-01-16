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


void finaliza_programa (int sinal) {
    printf("Finalizando programa ...\n");
    // HACK: como as funções da gpio são inversas, settando uma pra 0 a outra já vai estar em 0
    set_controle(0);
    finaliza_uart();
    exit(0);
}

int main (int argc, char *argv[]) {
    signal(SIGINT, finaliza_programa);
    uart0_fd = inicia_uart();
    inicia_gpio();
    // inicia_bme280("/dev/i2c-20");
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
                    usleep(250000);
                }

                /* float data;
                memcpy(&data, &buffer_escrita[3], 4);
                printf("%f\n", data); */
            } else {
                usleep(250000);
            }
        }
    }

    return 0;
}
