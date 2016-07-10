/*
 * KomunikacjaPC.c
 *
 * Created: 2016-06-08 09:14:46
 * Author : Alan Szopiñski
 */

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "proces.h"
#include <util/delay.h>
#include "I2C/I2C.h"
#include "ext_eeprom/24C64.h"
#include "LCD/LCD.h"
#include "klawiatura/klawiatura.h"
#include "RTC/RTC.h"

#define _EXT_EEPROM_EMPTY 0 //Ustaw 1 jeœli pamiêæ external eeprom jest niezaprogramowana;

//****** ZMIENNE GLOBALNE i FUNKCJE LOKALNE ******
#if _EXT_EEPROM_EMPTY==0
volatile uint32_t licznik;
Data data;
Czas czas;
uint8_t key_code;

struct flagi
{
	uint8_t flaga_lcd			:1; //Flaga pomocnicza przy wyœwietlaniu nazw menu
	volatile uint8_t flaga_rtc	:1; //Flaga wykorzystywana w przerwaniach przez RTC
	uint8_t	flaga_klawiatura	:1;	//Flaga pomocnicza w obs³udze klawiatury
}Flagi;

void fifo_bufor_lcd(char *bufor, uint8_t iteracje, size_t rozmiar);
void M0(char *str);
void M1_func_ustawienia(char *str);
void M10_func_ustawienia_data(char *str);
void M11_func_ustawienia_godzina(char *str);
void M12_func_ustawienia_lcd(char *str);

// Zmienne dotycz¹ce menu
/*
typedef struct menu
{
	struct menu *prev;
	struct menu *next;
	struct menu *up;
	struct menu *down;
	uint32_t addr_ext_eeprom;
	uint32_t size_ext_eeprom;
	void (*funkcja)(char *);
} MENU;
*/

struct menu m0_wyswietl, 
			m1_ustawienia, 
				m10_u_data, 
				m11_u_godzina,
				 m12_u_lcd;

struct menu m0_wyswietl = {&m1_ustawienia, &m1_ustawienia, NULL, NULL, offsetof(EXT_EEPROM_var, empty), SIZEOF (EXT_EEPROM_var, empty), M0};
struct menu m1_ustawienia = {&m0_wyswietl, &m0_wyswietl, NULL, &m10_u_data, offsetof(EXT_EEPROM_var, m1), SIZEOF (EXT_EEPROM_var, m1), M1_func_ustawienia };
	struct menu m10_u_data = {& m12_u_lcd, & m11_u_godzina, &m1_ustawienia, NULL, offsetof(EXT_EEPROM_var, m10), SIZEOF (EXT_EEPROM_var, m10), M10_func_ustawienia_data };
	struct menu m11_u_godzina = {& m10_u_data, & m12_u_lcd, &m1_ustawienia, NULL, offsetof(EXT_EEPROM_var, m11), SIZEOF (EXT_EEPROM_var, m11), M11_func_ustawienia_godzina };
	struct menu m12_u_lcd = {&m11_u_godzina, &m10_u_data, &m1_ustawienia, NULL, offsetof(EXT_EEPROM_var, m12), SIZEOF (EXT_EEPROM_var, m12), M12_func_ustawienia_lcd };
MENU *menu_ptr=&m0_wyswietl;

EEMEM Data deafult_data={1, 0xA0, 7, 2016};
EEMEM Czas deafult_czas={0,0,9};
EEMEM uint16_t rok;
EEMEM Ustawienia deafult_flags ={1};
	
// procedura obs³ugi przerwania INT 0
ISR( INT0_vect ) {
	Flagi.flaga_rtc = 1;
}

ISR(TIMER0_COMP_vect)
{
	licznik++;
}
#endif	 // _EXT_EEPROM_EMPTY

int main(void)
{	
	
#if _EXT_EEPROM_EMPTY==1
	DDRB=0xFF;
	PORTB=0x00;
	
	i2cSetBitrate(100); //Ustaw prêdkoœæ i2c na 100kHz
	EXT_EEPROM_var *struktura_eeprom;
	struktura_eeprom=malloc(sizeof(EXT_EEPROM_var));
	strcpy(struktura_eeprom->empty,  "                ");
	strcpy(struktura_eeprom->temperatura, "Temperatura:    ");
	strcpy(struktura_eeprom->m1,  "Ustawienia      ");
	strcpy(struktura_eeprom->m10, "Data:           ");
	strcpy(struktura_eeprom->m11, "Godzina:        ");
	strcpy(struktura_eeprom->m12, "LCD:            ");
	strcpy(struktura_eeprom->eePCF_Sunday, "Niedziela");
	strcpy(struktura_eeprom->eePCF_Monday, "Poniedzialek");
	strcpy(struktura_eeprom->eePCF_Tuesday, "Wtorek");
	strcpy(struktura_eeprom->eePCF_Wednesday, "Sroda");
	strcpy(struktura_eeprom->eePCF_Thursday, "Czwartek");
	strcpy(struktura_eeprom->eePCF_Friday, "Piatek");
	strcpy(struktura_eeprom->eePCF_Saturday, "Sobota");
	strcpy(struktura_eeprom->eePCF_January, "Styczen");
	strcpy(struktura_eeprom->eePCF_February, "Luty");
	strcpy(struktura_eeprom->eePCF_March, "Marzec");
	strcpy(struktura_eeprom->eePCF_April, "Kwiecien");
	strcpy(struktura_eeprom->eePCF_May, "Maj");
	strcpy(struktura_eeprom->eePCF_June, "Czerwiec");
	strcpy(struktura_eeprom->eePCF_July, "Lipiec");
	strcpy(struktura_eeprom->eePCF_August, "Sierpien");
	strcpy(struktura_eeprom->eePCF_September, "Wrzesien");
	strcpy(struktura_eeprom->eePCF_October, "Pazdziernik");
	strcpy(struktura_eeprom->eePCF_November, "Listopad");
	strcpy(struktura_eeprom->eePCF_December, "Grudzien");
	PORTB=0x0F;
	
	EI2C_write_buf(ADDR_EEMEM_24C64, 0, sizeof(EXT_EEPROM_var), (uint8_t*)struktura_eeprom);
	free(struktura_eeprom);
	PORTB=0xFF;
#endif // _EXT_EEPROM_EMPTY

#if _EXT_EEPROM_EMPTY==0
	char bufor_lcd[41];	//Bufor przechowuj¹cy znaki z pamiêci EEPROM
	uint8_t menu_func_active=1;
	uint8_t state=0;
	uint32_t cnt=0, offset_cnt=0;
	
	Ustawienia ustawienia;	//Struktura s³u¿¹ca do odczytania ustawieñ z EEPROM
	ustawienia.ustawienia_poczatkowe=0;
	//Ustaw PORTB jak wyjœcie dla LED:
	DDRB=0xFF;
	PORTB=0x00;
	Flagi.flaga_klawiatura=1;
	
	//// ustawienia TIMER0, przerwanie co 10us
	OCR0 = 19;
	TCCR0 |= (1<<WGM01);		//Tryb ctc
	TCCR0 |= (1<<CS01);			// Preskaler 8
	TIMSK |= (1<<OCIE0);

	// Przerwanie INT0
	MCUCR |= (1<<ISC01);	// wyzwalanie zboczem opadaj¹cym
	GICR |= (1<<INT0);		// odblokowanie przerwania
	PORTD |= (1<<PD2);		// podci¹gniêcie pinu INT0 do VCC
	
	i2cSetBitrate(100); //Ustaw prêdkoœæ i2c na 100kHz
	lcd_init();
	pobierz_czas(&czas);
	pobierz_date(&data);
	sei();
#endif	 // _EXT_EEPROM_EMPTY
    while (1) 
    {	
#if _EXT_EEPROM_EMPTY==0
		eeprom_read_block(&ustawienia, &deafult_flags, sizeof(deafult_flags));
		if (ustawienia.ustawienia_poczatkowe)
		{
			eeprom_read_block(&data, &deafult_data, sizeof(data));
			eeprom_read_block(&czas, &deafult_czas, sizeof(czas));
			ustaw_date(&data);
			ustaw_czas(&czas);

			ustawienia.ustawienia_poczatkowe=0;
			eeprom_update_block(&ustawienia, &deafult_flags, sizeof(deafult_flags));
		}
		
		
		switch(state)
		{
			case 0:
					key_code=debouncer(KlawiaturaSkan());
					if(key_code)
					{
						state=1;
						cli();
						cnt=50000;
						offset_cnt=licznik;
						sei();
					}
					break;
			case 1:
					key_code=0;
					if(cnt<=licznik-offset_cnt)
					{
						state=0;
					}
					break;
		}
		
		if (menu_func_active)
		{
			EI2C_read_buf(ADDR_EEMEM_24C64, menu_ptr->addr_ext_eeprom, menu_ptr->size_ext_eeprom, (uint8_t *)bufor_lcd);
			if(Flagi.flaga_lcd)
			{
				lcd_locate(0, 0);
			}

			(*menu_ptr->funkcja)(bufor_lcd);		//WskaŸnik na funkcje dla danej pozycji menu
		}

		if(Flagi.flaga_klawiatura)
		{	
			if (key_code == PRZYCISK_PRAWO)
			{
				if(menu_ptr->next!=NULL)
				{
					Flagi.flaga_lcd=1;
					menu_ptr=menu_ptr->next;
				}
			}
			else if (key_code == PRZYCISK_LEWO)
			{
				if(menu_ptr->prev!=NULL)
				{
					Flagi.flaga_lcd=1;
					menu_ptr=menu_ptr->prev;
				}
			}
			else if (key_code == PRZYCISK_DOL)
			{
				if(menu_ptr->down!=NULL)
				{
					Flagi.flaga_lcd=1;
					menu_ptr=menu_ptr->down;
				}
			}
			else if (key_code == PRZYCISK_GORA)
			{
				if(menu_ptr->up!=NULL)
				{
					Flagi.flaga_lcd=1;
					menu_ptr=menu_ptr->up;
				}
			}
		}
		
		if(menu_ptr->funkcja!=NULL)
			menu_func_active=1;
		else
			menu_func_active=0;

#endif  // _EXT_EEPROM_EMPTY
    }
}

#if _EXT_EEPROM_EMPTY==0
void M0(char *str)
{	
	static uint8_t state=0, i=0;
	static uint32_t cnt=0, offset_cnt=0;
	static char buf[41], buf_lcd[9];
	if(Flagi.flaga_lcd)
	{
		lcd_locate(0, 0);
		lcd_str(str);
		lcd_locate(1, 0);
		lcd_str(str);
		Flagi.flaga_lcd=0;
	}
	
	if(Flagi.flaga_rtc)
	{
		pobierz_czas(&czas);
		pobierz_date(&data);
		Flagi.flaga_rtc=0;
	}

	lcd_locate(1,0);
	if( czas.godziny < 10 ) lcd_char('0');
	lcd_int(czas.godziny);
	lcd_char(':');
	if( czas.minuty < 10 ) lcd_char('0');
	lcd_int(czas.minuty);
	lcd_char(':');
	if( czas.sekundy < 10 ) lcd_char('0');
	lcd_int(czas.sekundy);
	
	switch(state)
	{
		case 0:
				state=1;
				
				if(i>= 2*strlen(buf))
					i=0;
				
				lcd_wyswietl_date(&data, buf, 0);
				fifo_bufor_lcd(buf, i, strlen(buf));
				lcd_locate(1,8);
				strncpy(buf_lcd,buf,sizeof(buf_lcd)-1);
				lcd_str(buf_lcd);
				
				cli();
				cnt=25000;
				offset_cnt=licznik;
				sei();
				break;
		case 1:
				if(cnt<=licznik-offset_cnt)
				{
					i++;
					state=0;
				}
				break;
	}
}

void M1_func_ustawienia(char *str)
{
	if(Flagi.flaga_lcd)
	{
		lcd_locate(0, 0);
		lcd_str(str);
		EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, empty), SIZEOF (EXT_EEPROM_var, empty), (uint8_t *)str);
		lcd_locate(1, 0);
		lcd_str(str);
		Flagi.flaga_lcd=0;
	}
}
void M10_func_ustawienia_data(char *str)
{
	static uint8_t state=0, edit=0, i=0;
	static uint32_t cnt=0, offset_cnt=0;
	
	if(Flagi.flaga_lcd)
	{
		lcd_locate(0, 10);
		lcd_char(0x7E);
		lcd_str(str);
		EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, empty), SIZEOF (EXT_EEPROM_var, empty), (uint8_t *)str);
		lcd_locate(1, 0);
		lcd_str(str);
		Flagi.flaga_lcd=0;
	}
	
	if (key_code == PRZYCISK_ENTER)
	{
		edit++;
		if (edit>4)
			edit=1;
		Flagi.flaga_klawiatura=0;
	}
	else if (key_code == PRZYCISK_POWROT && Flagi.flaga_klawiatura==0)
	{
		edit=0;
		Flagi.flaga_klawiatura=1;
		ustaw_date(&data);
	}
	
	switch(edit)
	{
		case 1:
			if (key_code == PRZYCISK_GORA && Flagi.flaga_klawiatura==0)
			{
				data.dzien_tygodnia++;
				if (data.dzien_tygodnia>6)
					data.dzien_tygodnia=0;
			}
			else if (key_code == PRZYCISK_DOL && Flagi.flaga_klawiatura==0)
			{
				data.dzien_tygodnia--;
				if ((int8_t)data.dzien_tygodnia<0)
					data.dzien_tygodnia=6;
			}
			break;
		case 2:
			if (key_code == PRZYCISK_GORA && Flagi.flaga_klawiatura==0)
			{
				data.dzien++;
				switch(data.miesiac)
				{
					case PCF_January:
						if (data.dzien>31)
						{	
							data.dzien=1;
						}
						break;
					case PCF_February:
						if(!(data.rok % 4))
						{
							if (data.dzien>29)
							{
								data.dzien=1;
							}	
						}
						else
						{
							if (data.dzien>28)
							{
								data.dzien=1;
							}							
						}

						break;			
					case PCF_March:
						if (data.dzien>31)
						{
							data.dzien=1;
						}
						break;
					case PCF_April:
						if (data.dzien>30)
						{
							data.dzien=1;
						}
						break;
					case PCF_May:
						if (data.dzien>31)
						{
							data.dzien=1;
						}
						break;
					case PCF_June:
						if (data.dzien>30)
						{
							data.dzien=1;
						}
						break;
					case PCF_July:
						if (data.dzien>31)
						{
							data.dzien=1;
						}
						break;
					case PCF_August:
						if (data.dzien>31)
						{
							data.dzien=1;
						}
						break;
					case PCF_September:
						if (data.dzien>30)
						{
							data.dzien=1;
						}
						break;
					case PCF_October:
						if (data.dzien>31)
						{
							data.dzien=1;
						}
						break;
					case PCF_November:
						if (data.dzien>30)
						{
							data.dzien=1;
						}
						break;			
					case PCF_December:
						if (data.dzien>31)
						{
							data.dzien=1;
						}
						break;
				}
			}
			else if (key_code == PRZYCISK_DOL && Flagi.flaga_klawiatura==0)
			{
				data.dzien--;
				switch(data.miesiac)
				{
					case PCF_January:
						if ((int8_t)data.dzien<1)
						{
							data.dzien=31;
						}
						break;
					case PCF_February:
						if(!(data.rok % 4))
						{
							if ((int8_t)data.dzien<1)
							{
								data.dzien=29;
							}
						}
						else
						{
							if ((int8_t)data.dzien<1)
							{
								data.dzien=28;
							}
						}

						break;
					case PCF_March:
						if ((int8_t)data.dzien<1)
						{
							data.dzien=31;
						}
						break;
					case PCF_April:
						if ((int8_t)data.dzien<1)
						{
							data.dzien=30;
						}
						break;
					case PCF_May:
						if ((int8_t)data.dzien<1)
						{
							data.dzien=31;
						}
						break;
					case PCF_June:
						if ((int8_t)data.dzien<1)
						{
							data.dzien=30;
						}
						break;
					case PCF_July:
						if ((int8_t)data.dzien<1)
						{
							data.dzien=31;
						}
						break;
					case PCF_August:
						if ((int8_t)data.dzien<1)
						{
							data.dzien=31;
						}
					break;
					case PCF_September:
						if ((int8_t)data.dzien<1)
						{
							data.dzien=30;
						}
						break;
					case PCF_October:
						if ((int8_t)data.dzien<1)
						{
							data.dzien=31;
						}
						break;
					case PCF_November:
						if ((int8_t)data.dzien<1)
						{
							data.dzien=30;
						}
						break;
					case PCF_December:
						if ((int8_t)data.dzien<1)
						{
							data.dzien=31;
						}
						break;
				}
			}
			break;
		case 3:
			if (key_code == PRZYCISK_GORA && Flagi.flaga_klawiatura==0)
			{
				data.miesiac++;
				if (data.miesiac>12)
					data.miesiac=1;
					
				switch(data.miesiac)
				{
					case PCF_January:
						break;
					case PCF_February:
						if(!(data.rok % 4))
						{
							if (data.dzien>29)
							{
								data.dzien=29;
							}
						}
						else
						{
							if (data.dzien>28)
							{
								data.dzien=28;
							}
						}

						break;
					case PCF_March:

						break;
					case PCF_April:
						if (data.dzien>30)
						{
							data.dzien=30;
						}
						break;
					case PCF_May:
						break;
					case PCF_June:
						if (data.dzien>30)
						{
							data.dzien=30;
						}
						break;
					case PCF_July:
						break;
					case PCF_August:
						break;
					case PCF_September:
						if (data.dzien>30)
						{
							data.dzien=30;
						}
						break;
					case PCF_October:
						break;
					case PCF_November:
						if (data.dzien>30)
						{
							data.dzien=30;
						}
						break;
					case PCF_December:
						break;
				}
			}
			else if (key_code == PRZYCISK_DOL && Flagi.flaga_klawiatura==0)
			{
				data.miesiac--;
				if ((int8_t)data.miesiac<1)
					data.miesiac=12;
			}
			break;
		case 4:
			if (key_code == PRZYCISK_GORA && Flagi.flaga_klawiatura==0)
			{
				data.rok++;
			}
			else if (key_code == PRZYCISK_DOL && Flagi.flaga_klawiatura==0)
			{
				data.rok--;
			}
			break;
		default:
			if(Flagi.flaga_rtc)
			{
				pobierz_date(&data);
				Flagi.flaga_rtc=0;
			}
			break;
	}
	
	switch(state)
	{
		case 0:
			state=1;
			
			if (!i)
			{
			lcd_wyswietl_date(&data, str, edit);
			lcd_locate(1,0);
			lcd_str(str);
			}
			else
			{
				if(edit==3 || edit==4)
					lcd_wyswietl_date(&data, str, 5);
				else
					lcd_wyswietl_date(&data, str, 0);
					lcd_locate(1,0);
					lcd_str(str);
			}
			
			cli();
			cnt=50000;
			offset_cnt=licznik;
			sei();
			break;
		case 1:
			if(cnt<=licznik-offset_cnt)
			{
				i=~i;
				state=0;
			}
			break;
	}
}
void M11_func_ustawienia_godzina(char *str)
{
	static uint8_t i=0, edit=0;
	static uint8_t state=0;
	static uint32_t cnt=0, offset_cnt=0;
	
	if(Flagi.flaga_lcd)
	{
		lcd_locate(0, 10);
		lcd_char(0x7E);
		lcd_str(str);
		EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, empty), SIZEOF (EXT_EEPROM_var, empty), (uint8_t *)str);
		lcd_locate(1, 0);
		lcd_str(str);
		Flagi.flaga_lcd=0;
	}

	switch(state)
	{
		case 0:
			state=1;	
			
			if(i>8)
				i=0;
			lcd_locate(0, 10);
			lcd_char(0x7E);
			fifo_bufor_lcd(str,i,strlen(str));
			lcd_str(str);
			
			cli();
			cnt=25000;
			offset_cnt=licznik;
			sei();
			break;
		case 1:
			if(cnt<=licznik-offset_cnt)
			{
				i++;
				state=0;
			}
			break;
	}
	
	if (key_code == PRZYCISK_ENTER)
	{
		edit++;
		if (edit>3)
			edit=1;
		Flagi.flaga_klawiatura=0;
	}
	else if (key_code == PRZYCISK_POWROT && Flagi.flaga_klawiatura==0)
	{
		edit=0;
		Flagi.flaga_klawiatura=1;
		ustaw_czas(&czas);
	}
	
	
	switch(edit)
	{
		case 1:
			if (key_code == PRZYCISK_GORA && Flagi.flaga_klawiatura==0)
			{
				czas.godziny++;
				if(czas.godziny>23)
					czas.godziny=0;
				lcd_locate(1,0);
				if( czas.godziny < 10 ) lcd_char('0');
				lcd_int(czas.godziny);
			}
			else if (key_code == PRZYCISK_DOL && Flagi.flaga_klawiatura==0)
			{
				czas.godziny--;
				if((int8_t)czas.godziny<0)
					czas.godziny=23;	
				lcd_locate(1,0);
				if( czas.godziny < 10 ) lcd_char('0');
				lcd_int(czas.godziny);
			}

			lcd_cursor_on();
			lcd_blink_on();
			lcd_locate(1,1);
			break;
		case 2:
			if (key_code == PRZYCISK_GORA && Flagi.flaga_klawiatura==0)
			{
				czas.minuty++;
				if(czas.minuty>59)
				czas.minuty=0;
				lcd_locate(1,3);
				if( czas.minuty < 10 ) lcd_char('0');
				lcd_int(czas.minuty);
			}
			else if (key_code == PRZYCISK_DOL && Flagi.flaga_klawiatura==0)
			{
				czas.minuty--;
				if((int8_t)czas.minuty<0)
				czas.minuty=59;
				lcd_locate(1,3);
				if( czas.minuty < 10 ) lcd_char('0');
				lcd_int(czas.minuty);
			}
			lcd_cursor_on();
			lcd_blink_on();
			lcd_locate(1,4);
			break;
		case 3:
			if (key_code == PRZYCISK_GORA && Flagi.flaga_klawiatura==0)
			{
				czas.sekundy++;
				if(czas.sekundy>59)
				czas.sekundy=0;
				lcd_locate(1,6);
				if( czas.sekundy < 10 ) lcd_char('0');
				lcd_int(czas.sekundy);
			}
			else if (key_code == PRZYCISK_DOL && Flagi.flaga_klawiatura==0)
			{
				czas.sekundy--;
				if((int8_t)czas.sekundy<0)
				czas.sekundy=59;
				lcd_locate(1,6);
				if( czas.sekundy < 10 ) lcd_char('0');
				lcd_int(czas.sekundy);
			}
			lcd_cursor_on();
			lcd_blink_on();
			lcd_locate(1,7);
			break;
		default:
			lcd_cursor_off();
			lcd_blink_off();
			if(Flagi.flaga_rtc)
			{
				pobierz_czas(&czas);
				Flagi.flaga_rtc=0;
			}
			lcd_locate(1,0);
			if( czas.godziny < 10 ) lcd_char('0');
			lcd_int(czas.godziny);
			lcd_char(':');
			if( czas.minuty < 10 ) lcd_char('0');
			lcd_int(czas.minuty);
			lcd_char(':');
			if( czas.sekundy < 10 ) lcd_char('0');
			lcd_int(czas.sekundy);
		break;
	}

}
void M12_func_ustawienia_lcd(char *str)
{
	if(Flagi.flaga_lcd)
	{
		lcd_locate(0, 10);
		lcd_char(0x7E);
		lcd_str(str);
		EI2C_read_buf(ADDR_EEMEM_24C64, offsetof(EXT_EEPROM_var, empty), SIZEOF (EXT_EEPROM_var, empty), (uint8_t *)str);
		lcd_locate(1, 0);
		lcd_str(str);
		Flagi.flaga_lcd=0;
	}
}

void fifo_bufor_lcd(char *bufor, uint8_t iteracje, size_t rozmiar)
{
	char buf;
	for (uint8_t cnt=0; cnt < iteracje; cnt ++)
	{
		buf=bufor[0];
		for(uint8_t i=0; i < rozmiar-1; i++)
		{
			bufor[i]=bufor[i+1];
		}	
		bufor[rozmiar-1]=buf;
	}
}
#endif  // _EXT_EEPROM_EMPTY