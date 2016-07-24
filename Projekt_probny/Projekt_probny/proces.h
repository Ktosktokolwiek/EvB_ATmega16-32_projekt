/*
 * proces_variables.h
 *
 * Created: 2016-06-08 10:51:44
 *  Author: Alan Szopiñski
 */ 


#ifndef PROCES_H_
#define PROCES_H_

#define ADDR_VAR_EXT_EEPROM(a) offsetof(EXT_EEPROM_var, a)
//Deklaracje zmiennych

//Wejœcia/wyjœcia
struct porty
{
	volatile uint8_t				:2;	//twi
	volatile uint8_t columns		:3;
	volatile uint8_t rows			:2;
	volatile uint8_t nadajnik_ir	:1;
};

typedef struct  
{
	uint8_t	ustawienia_poczatkowe :1;
} Ustawienia;

#define kolumny		((struct porty *)&PORTC)->columns
#define kolumny_dir ((struct porty *)&DDRC)->columns
#define kolumny_pin ((struct porty *)&PINC)->columns
#define wiersze		((struct porty *)&PORTC)->rows
#define wiersze_dir ((struct porty *)&DDRC)->rows
#define wiersze_pin ((struct porty *)&PINC)->rows
#define ir		((struct porty *)&PORTC)->nadajnik_ir
#define ir_dir ((struct porty *)&DDRC)->nadajnik_ir
#define ir_pin ((struct porty *)&PINC)->nadajnik_ir

extern volatile uint32_t licznik;

struct flagi
{
	uint8_t flaga_lcd			:1; //Flaga pomocnicza przy wyœwietlaniu nazw menu
	volatile uint8_t flaga_rtc	:1; //Flaga wykorzystywana w przerwaniach przez RTC
	uint8_t	flaga_klawiatura	:1;	//Flaga pomocnicza w obs³udze klawiatury
	volatile uint8_t flaga_term	:1; //Flaga pomocnicza przy obs³udze termometru
	uint8_t flaga_menu_func		:1; //Flaga pomocnicza aktywuj¹ca/deaktywuj¹ca funkcje menu
	volatile uint8_t light_lcd	:1; //Flaga pomocnicza przy podœwietlaniu lcd
};
extern struct flagi Flagi;

// Struktura zawieraj¹ca zmienne w zewnêtrznej pamiêci eeprom
typedef struct
{
	char empty[17];
	char temperatura[17];
	char m1[17];
	char m10[17];
	char m11[17];
	char m12[17];
	char eePCF_Sunday[17];
	char eePCF_Monday[17]; 
	char eePCF_Tuesday[17]; 
	char eePCF_Wednesday[17]; 
	char eePCF_Thursday[17];
	char eePCF_Friday[17];
	char eePCF_Saturday[17];
	char eePCF_January[17];
	char eePCF_February[17];
	char eePCF_March[17];
	char eePCF_April[17];
	char eePCF_May[17];
	char eePCF_June[17];
	char eePCF_July[17]; 
	char eePCF_August[17]; 
	char eePCF_September[17]; 
	char eePCF_October[17];
	char eePCF_November[17];
	char eePCF_December[17];
} EXT_EEPROM_var;

// Struktura wykorzystywana do stworzenia menu
typedef struct menu
{
	struct menu *prev;
	struct menu *next;
	struct menu *up;
	struct menu *down;
	uint32_t addr_ext_eeprom;
	uint32_t size_ext_eeprom;
	void (*funkcja)(char *str);
} MENU;

extern uint16_t rok;
#endif /* PROCES_H_ */