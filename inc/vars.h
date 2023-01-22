#ifndef VARS_H_
#define VARS_H_
#include <pthread.h>
#include <stdio.h>

struct estado_sistema {
    int modo_controle;
    int ligado;
    int em_funcionamento;
    float temperatura_interna;
    float temperatura_referencia;
    float temperatura_ambiente;
    int potencia_ventoinha;
    int potencia_resistor;
};
extern struct estado_sistema estado_atual;

extern int uart0_fd;
extern struct bme280_data bme280;

extern FILE *csv_fd;

extern int tamanho_mensagem;
extern unsigned char buffer_envio[300], buffer_escrita[300];

extern int estado_curva;
extern int delay_curva[10];
extern float temperatura_curva[10];

extern pthread_t tid_temp, tid_curva;

#endif /* VARS_H_ */
