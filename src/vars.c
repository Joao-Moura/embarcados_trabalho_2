#include "bme280.h"
#include "vars.h"

struct estado_sistema estado_atual = {0, 0, 0, 0.0, 0.0, 0.0, 0, 0};

int uart0_fd = -1;
struct bme280_data bme280;

FILE *csv_fd;

int tamanho_mensagem = 0;
unsigned char buffer_envio[300], buffer_escrita[300];

int estado_curva = 0;
int delay_curva[10] = {60, 60, 120, 20, 40, 60, 60, 60, 120, 20};
float temperatura_curva[10] = {25.0, 38.0, 46.0, 54.0, 57.0, 61.0, 63.0, 54.0, 33.0, 25.0};

pthread_t tid_temp, tid_curva;
