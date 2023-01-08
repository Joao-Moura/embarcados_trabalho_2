#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "modbus.h"
#include "uart.h"

int main (int argc, char *argv[]) {
    int uart0_fd = inicia_uart();

    while(1) {
        int tamanho_mensagem = 0;
        unsigned char buffer_envio[300];

        monta_msg(buffer_envio, &tamanho_mensagem, 0x23, 0xC3, NULL);
        envia_msg(uart0_fd, buffer_envio, tamanho_mensagem);

        sleep(1);

        if (uart0_fd != -1) {
            unsigned char buffer_escrita[256];
            int tamanho_buffer = read(uart0_fd, (void*)buffer_escrita, 255);

            if (tamanho_buffer < 0)
                printf("Erro na leitura.\n");
            else if (tamanho_buffer == 0)
                printf("Nenhum dado disponível.\n");
            else {
                printf("%i Bytes lidos\n", tamanho_buffer);
                int dado;
                memcpy(&dado, &buffer_escrita[3], tamanho_buffer);
                printf("%d\n", dado);
                if ((dado == 1) | (dado == 2)) {
                    monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD3, (void *)&dado);  // TODO: Corrigir para enviar só 1 ou 0
                    envia_msg(uart0_fd, buffer_envio, tamanho_mensagem);
                } else if ((dado == 3) | (dado == 4)) {
                    monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD5, (void *)&dado);
                    envia_msg(uart0_fd, buffer_envio, tamanho_mensagem);
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
