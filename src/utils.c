#include <stdio.h>
#include <unistd.h>

#include "modbus.h"
#include "bme280.h"
#include "vars.h"

float temp_curva = 0.0;

void *executa_curva (void *args) {
    while (1) {
        if (!estado_atual.modo_controle && !estado_atual.em_funcionamento) {
            usleep(500000);
            continue;
        }

        printf("--------------------------> EXECUTANDO CURVA: %d\n", estado_curva);
        int tempo_espera = delay_curva[estado_curva];
        temp_curva = temperatura_curva[estado_curva];

        estado_atual.temperatura_referencia = temp_curva;
        printf("Temperatura referencia settada manualmente = %.2f\n", temp_curva);
        monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD2, (void *)&temp_curva, 4);
        sleep(tempo_espera);

        estado_curva += 1;
        estado_curva = estado_curva > 9 ? 0 : estado_curva;
    }

    return NULL;
}

void *executa_temperatura (void *args) {
    while (1) {
        printf("-----> EXECUTANDO TEMPERATURA\n");
        print_sensor_data(&bme280);
        monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD6, (void *)&estado_atual.temperatura_ambiente, 4);
        sleep(1);
    }

    return NULL;
}
