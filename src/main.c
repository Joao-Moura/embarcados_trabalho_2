#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "modbus.h"
#include "uart.h"
#include "bme280.h"

struct estado_sistema {
    int modo_controle;
    int ligado;
    int em_funcionamento;
    float temperatura_interna;
    float temperatura_referencia;
};
struct estado_sistema estado_atual = {0, 0, 0, 0.0, 0.0};

int main (int argc, char *argv[]) {
    int uart0_fd = inicia_uart();
    // initialize_sensor("/dev/i2c-21");

    while(1) {
        int tamanho_mensagem = 0;
        unsigned char buffer_envio[300], buffer_escrita[300];

        monta_msg(buffer_envio, &tamanho_mensagem, 0x23, 0xC3, NULL);
        envia_msg(uart0_fd, buffer_envio, tamanho_mensagem);

        if (uart0_fd != -1) {
            int dado = 0;
            int tamanho_buffer = le_msg(uart0_fd, buffer_escrita, (void *)&dado);

            if (tamanho_buffer > 0) {
                printf("Dado lido: %d\n", dado);
                if ((dado == 161) | (dado == 162)) {
                    dado &= 0x1;
                    monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD3, (void *)&dado);
                    envia_msg(uart0_fd, buffer_envio, tamanho_mensagem);
                    le_msg(uart0_fd, buffer_escrita, (void *)&dado);
                    estado_atual.ligado = dado;
                } else if (((dado == 163) | (dado == 164)) & estado_atual.ligado) {
                    dado &= 0x1;
                    monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD5, (void *)&dado);
                    envia_msg(uart0_fd, buffer_envio, tamanho_mensagem);
                    le_msg(uart0_fd, buffer_escrita, (void *)&dado);
                    estado_atual.em_funcionamento = dado;
                } else if ((dado == 165) & estado_atual.ligado) {
                    estado_atual.modo_controle ^= 1;
                    monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD4, (void *)&estado_atual.modo_controle);
                    envia_msg(uart0_fd, buffer_envio, tamanho_mensagem);
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
