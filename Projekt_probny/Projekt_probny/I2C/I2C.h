/*
 * I2C.h
 *
 * Created: 2016-06-10 09:26:42
 *  Author: Alan Szopiñski
 */ 


#ifndef I2C_H_
#define I2C_H_

#define ACK 1
#define NACK 0

void i2cSetBitrate(uint32_t bitrateKHz);
void TWI_start(void);
void TWI_stop(void);
void TWI_write(uint8_t bajt);
void TWI_read(uint8_t ack);
void TWI_write_buf( uint8_t addrDevice, uint8_t adr, uint8_t len, uint8_t *buf );
void TWI_read_buf(uint8_t addrDevice, uint8_t adr, uint8_t len, uint8_t *buf);

#endif /* I2C_H_ */