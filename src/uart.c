#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "modbus.h"
#include "vars.h"

int inicia_uart() {
    int uart0_fd = -1;
    uart0_fd = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);

    if (uart0_fd == -1) {
        printf("Erro ao inicializar UART.\n");
        exit(1);
    }

    struct termios options;
    tcgetattr(uart0_fd, &options);

    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;

    tcflush(uart0_fd, TCIFLUSH);
    tcsetattr(uart0_fd, TCSANOW, &options);
    return uart0_fd;
}

void finaliza_uart() {
    int zero = 0;
    monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD3, (void *)&zero, 1);
    monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD4, (void *)&zero, 1);
    monta_msg(buffer_envio, &tamanho_mensagem, 0x16, 0xD5, (void *)&zero, 1);
    close(uart0_fd);
}
