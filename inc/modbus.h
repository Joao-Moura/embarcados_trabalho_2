#ifndef MODBUS_H_
#define MODBUS_H_

void monta_msg(unsigned char *buffer_envio,
               int *tamanho_mensagem,
               unsigned char codigo,
               unsigned char subcodigo,
               void *dado_envio,
               int tamanho_dado);

int le_msg(int uart0_fd, unsigned char *buffer_escrita, void *dado);

#endif /* MODBUS_H_ */
