#ifndef GROOM_COM_H_
#define GROOM_COM_H_

#define COM_IS_READ(x) (x & 0x80)

uint8_t com_master_recv(uint8_t address);
uint8_t com_master_send(uint8_t address, uint8_t value);
uint8_t com_slave_wait_for_request(uint8_t *address);
uint8_t com_slave_recv(uint8_t *data);
uint8_t com_slave_send(uint8_t value);
uint8_t com_set_timeout(uint8_t val);

#endif /* GROOM_COM_H_ */