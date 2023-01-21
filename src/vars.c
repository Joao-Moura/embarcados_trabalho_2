#include "bme280.h"
#include "vars.h"

struct estado_sistema estado_atual = {0, 0, 0, 0.0, 0.0, 0.0};

int uart0_fd = -1;
struct bme280_data bme280;

int tamanho_mensagem = 0;
unsigned char buffer_envio[300], buffer_escrita[300];

int estado_curva = 0;
int delay_curva[10] = {60, 60, 120, 20, 40, 60, 60, 60, 120, 20};
int temperatura_curva[10] = {25, 38, 46, 54, 57, 61, 63, 54, 33, 25};

pthread_t tid_temp, tid_curva;
