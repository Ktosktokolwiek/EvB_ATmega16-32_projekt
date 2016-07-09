/*
 * klawiatura.c
 *
 * Created: 2016-06-20 10:21:17
 *  Author: Alan Szopiñski
 */ 
#include <avr/io.h>
#include "../proces.h"
#include <util/delay.h>
#include <avr/interrupt.h>

uint8_t KlawiaturaSkan(void)
{

	uint8_t rezultat=0;
	
	kolumny_dir =0b111;		//wymuszenie zer dla wszystkich kolumn,
	wiersze_dir = 0b00;
	kolumny= 0b000;
	wiersze= 0b11;
	_delay_us(2);
	rezultat |= wiersze_pin<<4;			//odczy twierszy
				
	kolumny_dir = 0b000;
	wiersze_dir =0b11;		//wymuszenie zer dla wszystkich wierszy,
	kolumny= 0b111;
	wiersze=0b00;
	_delay_us(2);
	rezultat |= kolumny_pin; //odczyt kolumn i uzyskanie kodu skanowania
	rezultat^=0x37;

	return rezultat;
}

uint8_t debouncer(uint8_t key)
{
	static uint8_t stan=0;
	static uint32_t timer=0, offset_timer=0;
	static uint8_t key_old;

	switch(stan)
	{
		case 0:
		{
			key_old=key;
			cli();
			offset_timer=licznik;
			timer=5000; //50ms + offset
			sei();
			stan=1;
			return 0;
		}
		case 1:
		{
			if(timer<=licznik-offset_timer)
			{
				stan=2;
			}
			return 0;
		}
		case 2:
		{
			if(key_old!=key)
			{
				stan=0;
			}
			else
			{
				return key_old;
			}
			return 0;
		}
	}
	return 0;
}