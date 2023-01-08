#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "crc16.h"

void monta_msg(unsigned char *buffer_envio,
               int *tamanho_mensagem, 
               unsigned char codigo,
               unsigned char subcodigo,
               void *dado_envio) {
    memset(buffer_envio, 0x00, 300);
    *tamanho_mensagem = 0;
    unsigned char *p_buffer_envio = &buffer_envio[0];

    *p_buffer_envio++ = 0x01;  // Barramento
    *p_buffer_envio++ = codigo;  // 0x23 - leitura // 0x16 - escrita
    *p_buffer_envio++ = subcodigo;  // Sub-codigo

    // NOTE: Matrícula
    *p_buffer_envio++ = 0;
    *p_buffer_envio++ = 8;
    *p_buffer_envio++ = 7;
    *p_buffer_envio++ = 9;

    *p_buffer_envio += 4;
    *tamanho_mensagem += 7;

    if (codigo == 0x16) {
        if ((subcodigo == 0xD2) | (subcodigo == 0xD6))
            memcpy(p_buffer_envio, (float *)dado_envio, 1);
        else
            memcpy(p_buffer_envio, (int *)dado_envio, 1);

        p_buffer_envio += 1;
        *tamanho_mensagem += 1;
    }

    short crc = calcula_CRC(buffer_envio, *tamanho_mensagem);
    memcpy(p_buffer_envio, &crc, 2);
    *tamanho_mensagem += 2;
}

void envia_msg(int uart0_fd, unsigned char *buffer_envio, int tamanho_mensagem) {
    if (uart0_fd != -1) {
        int count = write(uart0_fd, &buffer_envio[0], tamanho_mensagem);
        if (count < 0) {
            printf("UART TX error\n");
            return ;
        } else {
            printf("Escrito na UART %i bytes.\n", count);
            for (int i = 0; i < count; i++) {
                printf("%02x", buffer_envio[i]);
            }
            printf("\n");
        }
    }
}
