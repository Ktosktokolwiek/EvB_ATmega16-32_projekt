/*
 * _24C64.c
 *
 * Created: 2016-06-10 08:23:22
 *  Author: Alan Szopiñski
 */
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "../proces.h"
#include <util/delay.h>
#include "../I2C/I2C.h"
#include "24C64.h"

// zapis danych do pamiêci EEPROM
void EI2C_write_buf(uint8_t device, uint32_t subAddr, uint32_t len, uint8_t *buf)
{

	while (len--) {
		TWI_start();
		TWI_write(device | (DEV_SEL << 1) );
		TWI_write((subAddr & 0xFF00) >> 8);
		TWI_write(subAddr & 0x00FF);
		TWI_write(*buf++);
		TWI_stop();
		_delay_ms(5); // oczekiwanie na zapis
		subAddr++;
	}


}

// odczyt danych z pamiêci EEPROM
void EI2C_read_buf(uint8_t device, uint32_t subAddr, uint32_t len, uint8_t *buf)
{

	while (len--) {
		TWI_start();

		TWI_write(device | (DEV_SEL << 1));
		TWI_write((subAddr & 0xFF00) >> 8);
		TWI_write(subAddr & 0x00FF);

		TWI_start();
		TWI_write(device | (DEV_SEL << 1) | 0x01);		
		TWI_read( NACK );
		*buf++ = TWDR;

		TWI_stop();
		subAddr++;
	}
}
