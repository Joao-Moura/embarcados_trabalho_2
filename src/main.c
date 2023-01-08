#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "modbus.h"
#include "crc16.h"
#include "uart.h"

int main (int argc, char *argv[]) {
    int uart0_fd = inicia_uart();

    while(1) {
        int tamanho_mensagem = 0;
        unsigned char buffer_envio[300], buffer_escrita[300];

        monta_msg(buffer_envio, &tamanho_mensagem, 0x23, 0xC3, NULL);
        envia_msg(uart0_fd, buffer_envio, tamanho_mensagem);

        sleep(1);

        if (uart0_fd != -1) {
            int dado = 0;
            int tamanho_buffer = le_msg(uart0_fd, buffer_escrita, (void *)&dado);

            if (tamanho_buffer > 0) {
                printf("Dado lido: %d\n", dado);
                if ((dado == 1) | (dado == 2)) {
                    dado &= 0x1;
                    monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD3, (void *)&dado);
                    envia_msg(uart0_fd, buffer_envio, tamanho_mensagem);
                    sleep(1);
                    le_msg(uart0_fd, buffer_escrita, (void *)&dado);
                } else if ((dado == 3) | (dado == 4)) {
                    dado &= 0x1;
                    monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD5, (void *)&dado);
                    envia_msg(uart0_fd, buffer_envio, tamanho_mensagem);
                    sleep(1);
                    le_msg(uart0_fd, buffer_escrita, (void *)&dado);
                }

                /* float data;
                memcpy(&data, &buffer_escrita[3], 4);
                printf("%f\n", data); */
            }
        }

        usleep(1000);
    }

    return 0;
}
