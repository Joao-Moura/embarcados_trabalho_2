#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softPwm.h>

#include "modbus.h"
#include "vars.h"

void inicia_gpio () {
    int setup = wiringPiSetup();
    if(setup == -1) {
        printf("Erro ao inicializar a GPIO.\n");
        exit(1);
    }

    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    softPwmCreate(4, 0, 100);
    softPwmCreate(5, 0, 100);
}

void set_resistor (int potencia) {
    softPwmWrite(4, potencia);
    softPwmWrite(5, 0);
}

void set_ventoinha (int potencia) {
    potencia = potencia < 40 ? 40 : potencia;
    softPwmWrite(5, potencia);
    softPwmWrite(4, 0);
}

void set_controle(int potencia) {
    if (potencia > 0)
        set_resistor(potencia > 100 ? 100 : potencia);
    else {
        set_ventoinha(potencia < 100 ? 100 : -potencia);
    }

    monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD1, (void *)&potencia, 4);
}
