#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "crc16.h"
#include "uart.h"

void monta_msg(unsigned char *buffer_envio, int *tamanho_mensagem) {
    unsigned char *p_buffer_envio = &buffer_envio[0];

    *p_buffer_envio++ = 0x01;  // Barramento
    *p_buffer_envio++ = 0x16;  // 0x23 - leitura // 0x16 - escrita
    *p_buffer_envio++ = 0xD3;  // Sub-codigo

    // NOTE: Matrícula
    *p_buffer_envio++ = 0;
    *p_buffer_envio++ = 8;
    *p_buffer_envio++ = 7;
    *p_buffer_envio++ = 9;

    *p_buffer_envio += 4;
    *tamanho_mensagem += 7;

    // TODO: Adicionar mensagem se for de escrita
    /* *p_buffer_envio++ = 0;
    *tamanho_mensagem += 1; */

    short crc = calcula_CRC(buffer_envio, *tamanho_mensagem);
    memcpy(p_buffer_envio, &crc, 2);
    *tamanho_mensagem += 2;
}

int main (int argc, char *argv[]) {
    int uart0_fd = inicia_uart();

    /* int data_envio = 10;
    memcpy(p_buffer_envio, &data_envio, 1);
    p_buffer_envio += 1; */

    /* unsigned char data_envio[255] = "(0_0)";
    int tamanho = 5;
    memcpy(p_buffer_leitura, &tamanho, 1);
    p_buffer_leitura += 1;
    memcpy(p_buffer_leitura, &data_envio, 5);
    p_buffer_leitura += 5; */

    int tamanho_mensagem = 0;
    unsigned char buffer_envio[300];
    memset(buffer_envio, 0x00, 300);
    monta_msg(buffer_envio, &tamanho_mensagem);

    if (uart0_fd != -1) {
        int count = write(uart0_fd, &buffer_envio[0], tamanho_mensagem);
        if (count < 0)
            printf("UART TX error\n");
        else {
            printf("Escrito na UART %i bytes.\n", count);
            for (int i = 0; i < count; i++) {
                printf("%02x", buffer_envio[i]);
            }
            printf("\n");
        }
    }

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
            /* int data;
            memcpy(&data, &buffer_escrita[3], tamanho_buffer);
            printf("%d\n", data); */
            float data;
            memcpy(&data, &buffer_escrita[3], 4);
            printf("%f\n", data);
            /* int tam;
            unsigned char data[255];
            memcpy(&tam, &buffer_escrita[3], 1);
            printf("[%d] ", tam);
            memcpy(&data, &buffer_escrita[4], tam);
            printf("%s\n", data); */
        }
    }

    return 0;
}
