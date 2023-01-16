#include "vars.h"

struct estado_sistema estado_atual = {0, 0, 0, 0.0, 0.0};

int uart0_fd = -1;

int tamanho_mensagem = 0;
unsigned char buffer_envio[300], buffer_escrita[300];
