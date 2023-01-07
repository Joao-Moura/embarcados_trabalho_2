#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

int inicia_uart() {
    int uart0_fd = -1;
    uart0_fd = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);

    if (uart0_fd == -1) {
        printf("Erro ao inicializar UART.");
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
