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
#include "loop.h"


void finaliza_programa (int sinal) {
    printf("Finalizando programa ...\n");
    // HACK: como as funções da gpio são inversas, settando uma pra 0 a outra já vai estar em 0
    set_controle(0);
    reseta_uart();
    close(uart0_fd);
    pthread_cancel(tid_temp);
    pthread_cancel(tid_curva);
    exit(0);
}

void init() {
    pid_configura_constantes(30.0, 0.2, 400.0);
    uart0_fd = inicia_uart();
    inicia_gpio();
    inicia_bme280("/dev/i2c-1");
    reseta_uart();
    le_msg(uart0_fd, buffer_escrita, NULL);
    pthread_create(&tid_temp, NULL, executa_temperatura, NULL);
    pthread_create(&tid_curva, NULL, executa_curva, NULL);
}

int main (int argc, char *argv[]) {
    signal(SIGINT, finaliza_programa);
    init();
    loop_principal();
    return 0;
}
