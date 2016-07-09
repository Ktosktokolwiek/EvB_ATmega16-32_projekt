/*
 * RTC.c
 *
 * Created: 2016-06-08 09:54:22
 *  Author: Alan Szopiñski
 */ 
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "../proces.h"
#include "../I2C/I2C.h"
#include "../ext_eeprom/24C64.h"
#include "RTC.h"
		
	 
void ustaw_czas(Czas *czas)
{
	uint8_t bufor[3];
	bufor[0]=bin2bcd(czas->sekundy);
	bufor[1]=bin2bcd(czas->minuty);
	bufor[2]=bin2bcd(czas->godziny);
	
	TWI_write_buf( ADDR_PCF8583, SecondsReg, 3, bufor );
}

void ustaw_date(Data *data)
{
	uint8_t bufor[4];
	bufor[0]=bin2bcd(data->dzien);
	bufor[1]=bin2bcd(data->dzien_tygodnia);
	bufor[2]=bin2bcd(data->miesiac);
	bufor[3]=bin2bcd(data->rok);
	
	TWI_write_buf( ADDR_PCF8583, DaysReg, 4, bufor );
}

void pobierz_czas(Czas *czas)
{
	uint8_t bufor[4];
	
	TWI_read_buf(ADDR_PCF8583, SecondsReg, 3, bufor);
	czas->sekundy=bcd2bin(bufor[0]);
	czas->minuty=bcd2bin(bufor[1]);
	czas->godziny=bcd2bin(bufor[2]);
}

void pobierz_date(Data *data)
{
	uint8_t bufor[4];
	
	TWI_read_buf(ADDR_PCF8583, DaysReg, 4, bufor);
	data->dzien = bcd2bin(bufor[0]);
	data->dzien_tygodnia = bin2bcd(bufor[1]);
	data->miesiac = bcd2bin(bufor[2]);
	data->rok = bcd2bin(bufor[3]);
}

void lcd_wyswietl_date(Data *data, char *bufor, uint8_t wyl_czesc_daty)
{
	char dzien_tmp[3];
	char dzien_tygodnia_tmp[17];
	char miesiac_tmp[17];
	char rok_tmp[5], tmp[3];
	
	switch(data->dzien_tygodnia)
	{
		case PCF_Sunday: 
		{
			EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, eePCF_Sunday), SIZEOF (EXT_EEPROM_var, eePCF_Sunday), (uint8_t *)dzien_tygodnia_tmp); break;
		}
		case PCF_Monday:
		{
				EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, eePCF_Monday), SIZEOF (EXT_EEPROM_var, eePCF_Monday), (uint8_t *)dzien_tygodnia_tmp);  break; 
		}
		case PCF_Tuesday:
		{
				EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, eePCF_Tuesday), SIZEOF (EXT_EEPROM_var, eePCF_Tuesday), (uint8_t *)dzien_tygodnia_tmp);  break;
		}
		case PCF_Wednesday:
		{
				EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, eePCF_Wednesday), SIZEOF (EXT_EEPROM_var, eePCF_Wednesday), (uint8_t *)dzien_tygodnia_tmp);  break;
		}
		case PCF_Thursday:
		{
				EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, eePCF_Thursday), SIZEOF (EXT_EEPROM_var, eePCF_Thursday), (uint8_t *)dzien_tygodnia_tmp);  break;
		}
		case PCF_Friday:
		{
				EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, eePCF_Friday), SIZEOF (EXT_EEPROM_var, eePCF_Friday), (uint8_t *)dzien_tygodnia_tmp);  break; 
		}
		case PCF_Saturday:
		{
				EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, eePCF_Saturday), SIZEOF (EXT_EEPROM_var, eePCF_Saturday), (uint8_t *)dzien_tygodnia_tmp);  break;
		}
	}
	switch(data->miesiac)
	{
		case PCF_January:
				EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, eePCF_January), SIZEOF (EXT_EEPROM_var, eePCF_January), (uint8_t *)miesiac_tmp); break;
		case PCF_February:
				EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, eePCF_February), SIZEOF (EXT_EEPROM_var, eePCF_February), (uint8_t *)miesiac_tmp); break;
		case PCF_March:
				EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, eePCF_March), SIZEOF (EXT_EEPROM_var, eePCF_March), (uint8_t *)miesiac_tmp); break;
		case PCF_April:
				EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, eePCF_April), SIZEOF (EXT_EEPROM_var, eePCF_April), (uint8_t *)miesiac_tmp); break;
		case PCF_May:
				EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, eePCF_May), SIZEOF (EXT_EEPROM_var, eePCF_May), (uint8_t *)miesiac_tmp); break;
		case PCF_June:
				EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, eePCF_June), SIZEOF (EXT_EEPROM_var, eePCF_June), (uint8_t *)miesiac_tmp); break;
		case PCF_July:
				EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, eePCF_July), SIZEOF (EXT_EEPROM_var, eePCF_July), (uint8_t *)miesiac_tmp); break;
		case PCF_August:
				EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, eePCF_August), SIZEOF (EXT_EEPROM_var, eePCF_August), (uint8_t *)miesiac_tmp); break;
		case PCF_September:
				EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, eePCF_September), SIZEOF (EXT_EEPROM_var, eePCF_September), (uint8_t *)miesiac_tmp); break;
		case PCF_October:
				EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, eePCF_October), SIZEOF (EXT_EEPROM_var, eePCF_October), (uint8_t *)miesiac_tmp); break;
		case PCF_November:
				EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, eePCF_November), SIZEOF (EXT_EEPROM_var, eePCF_November), (uint8_t *)miesiac_tmp); break;
		case PCF_December:
				EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, eePCF_December), SIZEOF (EXT_EEPROM_var, eePCF_December), (uint8_t *)miesiac_tmp); break;
	}
	
	itoa(data->dzien, dzien_tmp, 10);
	if( data->rok < 10 ) 
		strcpy(rok_tmp, "200");
	else if ( data->rok > 99 )
		strcpy(rok_tmp, "2");
	else
		strcpy(rok_tmp, "20");
	strcat(rok_tmp, itoa(data->rok, tmp, 10));
	
	switch(wyl_czesc_daty)
	{
		case 1:
			nadpisz_bufor_spacjami(dzien_tygodnia_tmp); break;
		case 2:
			nadpisz_bufor_spacjami(dzien_tmp); break;
		case 3:
			nadpisz_bufor_spacjami(miesiac_tmp); break;
		case 4:
			nadpisz_bufor_spacjami(rok_tmp); break;	
	}
	if (wyl_czesc_daty==3 || wyl_czesc_daty==4 || wyl_czesc_daty==5)
	{
		if (data->miesiac==PCF_October)
		{
			strcpy(bufor, miesiac_tmp);
			strcat(bufor, " ");
			strcat(bufor, rok_tmp);
			strcat(bufor, "      ");
		}
		else
		{
			strcpy(bufor, dzien_tmp);
			strcat(bufor, " ");
			strcat(bufor, miesiac_tmp);
			strcat(bufor, " ");
			strcat(bufor, rok_tmp);
			strcat(bufor, "      ");
		}

		//sprintf(bufor_,"%s %s %s ", dzien_tmp,  miesiac_tmp, rok_tmp);
	}
	else
	{
		strcpy(bufor, dzien_tygodnia_tmp);
		strcat(bufor, ", ");
		strcat(bufor, dzien_tmp);
		strcat(bufor, " ");
		strcat(bufor, miesiac_tmp);
		strcat(bufor, " ");
		strcat(bufor, rok_tmp);
		strcat(bufor, "      ");
		//sprintf(bufor,"%s, %s %s %s ", dzien_tygodnia_tmp, dzien_tmp,  miesiac_tmp, rok_tmp);
	}
}

//Funkcja pomocnicza

void nadpisz_bufor_spacjami (char *bufor)
{
	size_t rozmiar= strlen(bufor);
	strcpy(bufor, " ");
	for(size_t i=1; i < rozmiar; i++)
	{
		strcat(bufor," ");
	}
}

inline uint8_t bcd2bin(uint8_t n) { return ((((n >> 4) & 0x0F) * 10) + (n & 0x0F)); };
inline uint8_t bin2bcd(uint8_t n) { return (((n / 10) << 4) | (n % 10)); };