#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>

#include "modbus.h"
#include "uart.h"
#include "bme280.h"
#include "gpio.h"
#include "vars.h"
#include "utils.h"
#include "pid.h"

void gerencia_forno (int dado) {
    dado &= 0x1;
    monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD3, (void *)&dado, 1);
    le_msg(uart0_fd, buffer_escrita, (void *)&dado);
    estado_atual.ligado = dado;

    if (!estado_atual.ligado) {
        estado_atual.em_funcionamento = 0;
        monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD5, (void *)&estado_atual.em_funcionamento, 1);
        le_msg(uart0_fd, buffer_escrita, (void *)&dado);
    }
}

void gerencia_funcionamento (int dado) {
    dado &= 0x1;
    monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD5, (void *)&dado, 1);
    le_msg(uart0_fd, buffer_escrita, (void *)&dado);
    estado_atual.em_funcionamento = dado;
}

void gerencia_modo (int dado) {
    estado_atual.modo_controle ^= 1;
    estado_curva = 0;
    monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD4, (void *)&estado_atual.modo_controle, 1);
    le_msg(uart0_fd, buffer_escrita, (void *)&dado);
}

void gerencia_temperaturas () {
    monta_msg(buffer_envio, &tamanho_mensagem, 0x23, 0xC1, NULL, 0);
    le_msg(uart0_fd, buffer_escrita, (void *)&estado_atual.temperatura_interna);

    if (!estado_atual.modo_controle || (!estado_atual.em_funcionamento && estado_atual.modo_controle)) {
        monta_msg(buffer_envio, &tamanho_mensagem, 0x23, 0xC2, NULL, 0);
        le_msg(uart0_fd, buffer_escrita, (void *)&estado_atual.temperatura_referencia);
    } else {
        estado_atual.temperatura_referencia = temp_curva;
    }

    if (estado_atual.em_funcionamento) {
        pid_atualiza_referencia(estado_atual.temperatura_referencia);
        int potencia = (int) pid_controle(estado_atual.temperatura_interna);
        set_controle(potencia);
    }
}

void loop_principal () {
    while(1) {
        monta_msg(buffer_envio, &tamanho_mensagem, 0x23, 0xC3, NULL, 0);

        if (uart0_fd != -1) {
            int dado = 0;
            int tamanho_buffer = le_msg(uart0_fd, buffer_escrita, (void *)&dado);

            if (tamanho_buffer > 0) {
                if ((dado == 161) || (dado == 162))
                    gerencia_forno(dado);
                else if (((dado == 163) || (dado == 164)) & estado_atual.ligado)
                    gerencia_funcionamento(dado);
                else if ((dado == 165) && estado_atual.ligado)
                    gerencia_modo(dado);
                else
                    gerencia_temperaturas();
            } else {
                usleep(250000);
            }
        }
    }
}
