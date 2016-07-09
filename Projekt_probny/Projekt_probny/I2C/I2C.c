/*
 * I2C.c
 *
 * Created: 2016-06-10 09:26:29
 *  Author: Alan Szopiñski
 */ 

#include <avr/io.h>
#include <util/twi.h>
#include "../proces.h"
#include <util/delay.h>
#include "I2C.h"

uint8_t licznik_funkcji_wewnetrznych_i2c;


void i2cSetBitrate(uint32_t bitrateKHz) {
	uint8_t bitrate_div;

	bitrate_div = ((F_CPU/1000l)/bitrateKHz);
	if(bitrate_div >= 16)
	bitrate_div = (bitrate_div-16)/2;

	TWBR = bitrate_div;
}

void TWI_start(void) {
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);
	while (!(TWCR&(1<<TWINT)));
}

void TWI_stop(void) {
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	while ( (TWCR&(1<<TWSTO)));
}

void TWI_write(uint8_t bajt) {
	TWDR = bajt;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ( !(TWCR&(1<<TWINT)));
}

void TWI_read(uint8_t ack) {
	TWCR = (1<<TWINT)|(ack<<TWEA)|(1<<TWEN);
	while ( !(TWCR & (1<<TWINT)));
}

void TWI_write_buf( uint8_t SLA, uint8_t adr, uint8_t len, uint8_t *buf ) {

	TWI_start();
	TWI_write(SLA);
	TWI_write(adr);
	while (len--) TWI_write(*buf++);
	TWI_stop();
}

void TWI_read_buf(uint8_t SLA, uint8_t adr, uint8_t len, uint8_t *buf) {

	TWI_start();
	TWI_write(SLA);
	TWI_write(adr);
	TWI_start();
	TWI_write(SLA + 1);
	while (len--)
	{ 
		TWI_read( len ? ACK : NACK );
		*buf++ = TWDR;
	}
	TWI_stop();
}
