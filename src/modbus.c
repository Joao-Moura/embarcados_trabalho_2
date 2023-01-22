#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "crc16.h"
#include "vars.h"

#define RETRY 3

int retentativas = RETRY;

void envia_msg(unsigned char *buffer_envio, int tamanho_mensagem) {
    if (uart0_fd != -1) {
        int count = write(uart0_fd, &buffer_envio[0], tamanho_mensagem);
        if (count < 0) {
            printf("UART TX error\n");
            return ;
        }
        usleep(250000);
    }
}

void monta_msg(unsigned char *buffer_envio,
               int *tamanho_mensagem, 
               unsigned char codigo,
               unsigned char subcodigo,
               void *dado_envio,
               int tamanho_dado) {
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
            memcpy(p_buffer_envio, (float *)dado_envio, tamanho_dado);
        else
            memcpy(p_buffer_envio, (int *)dado_envio, tamanho_dado);

        p_buffer_envio += tamanho_dado;
        *tamanho_mensagem += tamanho_dado;
    }

    short crc = calcula_CRC(buffer_envio, *tamanho_mensagem);
    memcpy(p_buffer_envio, &crc, 2);
    *tamanho_mensagem += 2;
    envia_msg(buffer_envio, *tamanho_mensagem);
}

int crc_valido(unsigned char *buffer_escrita, int tamanho_buffer) {
    short crc_calculado = calcula_CRC(buffer_escrita, tamanho_buffer - 2);
    short crc_recebido;

    memcpy(&crc_recebido, &buffer_escrita[7], 2);
    if (crc_calculado != crc_recebido) {
        printf("CRC recebido %hd falhou, esperado: %hd.\n", crc_recebido, crc_calculado);
        return 1;
    }

    return 0;
}

int le_msg(int uart0_fd, unsigned char *buffer_escrita, void *dado) {
    memset(buffer_escrita, 0x00, 300);
    int tamanho_buffer = read(uart0_fd, (void*)buffer_escrita, 300);

    if (tamanho_buffer < 0)
        printf("Erro na leitura.\n");
    else if (!retentativas) {
        retentativas = RETRY;
        return -1;
    } else if (tamanho_buffer == 0) {
        printf("Nenhum dado disponível.\n");
        retentativas--;
        usleep(500000);
        return le_msg(uart0_fd, buffer_escrita, dado);
    } else {
        if (crc_valido(buffer_escrita, tamanho_buffer))
            return -1;

        memcpy(dado, &buffer_escrita[3], tamanho_buffer);
    }

    return tamanho_buffer;
}
