#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "vars.h"

void init_csv (char *nome_arquivo) {
    char *tipo_abertura = access(nome_arquivo, F_OK) == 0 ? "a" : "w";
    csv_fd = fopen(nome_arquivo, tipo_abertura);

    if (csv_fd == NULL) {
        printf("Erro ao abrir arquivo CSV.\n");
        exit(1);
    }

    if (tipo_abertura[0] == 'w') {
        fprintf(csv_fd, "Data e hora,Temperatura interna,Temperatura externa"
                ",Temperatura definida,Potencia ventoinha,Potencia Resistor\n");
        fflush(csv_fd);
    }
}

void acessa_datetime (char *datetime) {
    time_t t = time(NULL);
    strftime(datetime, 30, "%d-%m-%Y %H:%M:%S", localtime(&t));
}

void escreve_no_csv () {
    char datetime[30];
    acessa_datetime(datetime);

    fprintf(csv_fd, "%s,%.2f,%.2f,%.2f,%d,%d\n",
            datetime, estado_atual.temperatura_interna,
            estado_atual.temperatura_ambiente, estado_atual.temperatura_referencia,
            estado_atual.potencia_ventoinha, estado_atual.potencia_resistor);
    fflush(csv_fd);
}
