/*
 * _24C64.h
 *
 * Created: 2016-06-10 08:23:39
 *  Author: Alan Szopiñski
 */ 

#ifndef EEM24C64_H_
#define EEM24C64_H_

#define ADDR_EEMEM_24C64	0xA0
#define SIZEOF(s,m) ((size_t) sizeof(((s *)0)->m))
#define DEV_SEL	0x07

void EI2C_read_buf(uint8_t device, uint32_t subAddr, uint32_t len, uint8_t *buf);
void EI2C_write_buf(uint8_t device, uint32_t subAddr, uint32_t len, uint8_t *buf);



#endif /* EEM24C64_H_ */