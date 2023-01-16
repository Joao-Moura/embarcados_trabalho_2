#ifndef VARS_H_
#define VARS_H_

struct estado_sistema {
    int modo_controle;
    int ligado;
    int em_funcionamento;
    float temperatura_interna;
    float temperatura_referencia;
};
extern struct estado_sistema estado_atual;

extern int uart0_fd;

extern int tamanho_mensagem;
extern unsigned char buffer_envio[300], buffer_escrita[300];

#endif /* VARS_H_ */
