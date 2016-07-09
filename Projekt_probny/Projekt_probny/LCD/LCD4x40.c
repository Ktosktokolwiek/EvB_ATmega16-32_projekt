//-------------------------------------------------------------------------------------------------
// Biblioteka funkcji obs³ugi wyswietlacza alfanumerycznego HD44780 4x40 znaków
// Kompilator : avr-gcc
// Autor : Rados³aw Kwiecieñ, radek@dxp.pl
//-------------------------------------------------------------------------------------------------
#include "LCD4x40.h"


unsigned char LocationX = 0;
unsigned char LocationY = 0;

//-------------------------------------------------------------------------------------------------
//
// 
//
//-------------------------------------------------------------------------------------------------
void LCD_Enable(unsigned char controller)
{
switch(controller){
case 0: LCD_E1_PORT |= LCD_E1; break;
case 1: LCD_E2_PORT |= LCD_E2; break;
}
asm("nop");
LCD_E1_PORT &= ~LCD_E1; // E1 = 0
LCD_E2_PORT &= ~LCD_E2; // E2 = 0
}

//-------------------------------------------------------------------------------------------------
//
// 
//
//-------------------------------------------------------------------------------------------------
void _LCD_OutNibble(unsigned char nibbleToWrite)
{
if(nibbleToWrite & 0x01)
	LCD_DB4_PORT |= LCD_DB4;
else
	LCD_DB4_PORT  &= ~LCD_DB4;

if(nibbleToWrite & 0x02)
	LCD_DB5_PORT |= LCD_DB5;
else
	LCD_DB5_PORT  &= ~LCD_DB5;

if(nibbleToWrite & 0x04)
	LCD_DB6_PORT |= LCD_DB6;
else
	LCD_DB6_PORT  &= ~LCD_DB6;

if(nibbleToWrite & 0x08)
	LCD_DB7_PORT |= LCD_DB7;
else
	LCD_DB7_PORT  &= ~LCD_DB7;
}

//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu bajtu do wyœwietacza (bez rozró¿nienia instrukcja/dane).
//
//-------------------------------------------------------------------------------------------------
void _LCD_Write(unsigned char dataToWrite, unsigned char controller)
{
_LCD_OutNibble(dataToWrite >> 4);
LCD_Enable(controller);
_LCD_OutNibble(dataToWrite);
LCD_Enable(controller);
_delay_us(50);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu rozkazu do wyœwietlacza
//
//-------------------------------------------------------------------------------------------------
void LCD_WriteCommand(unsigned char commandToWrite,unsigned char controller)
{
LCD_RS_PORT &= ~LCD_RS;
_LCD_Write(commandToWrite, controller);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu danych do pamiêci wyœwietlacza
//
//-------------------------------------------------------------------------------------------------
void LCD_WriteData(unsigned char dataToWrite)
{
LCD_RS_PORT |= LCD_RS;
_LCD_Write(dataToWrite, (LocationY / 2));
LocationX ++;
if(LocationX == 40)
  {
  LocationY++;
  LocationX = 0;
  if(LocationY == 4)
  	LocationY = 0;
  }
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja ustawienia wspó³rzêdnych ekranowych
//
//-------------------------------------------------------------------------------------------------
void LCD_GoTo(unsigned char x, unsigned char y)
{
LCD_WriteCommand(HD44780_DDRAM_SET | (x + (0x40 * (y%2))), (y/2));
LocationX = x;
LocationY = y;
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja wyœwietlenia napisu na wyswietlaczu.
//
//-------------------------------------------------------------------------------------------------
void LCD_WriteText(char * text)
{
while(*text)
  LCD_WriteData(*text++);
}
//-------------------------------------------------------------------------------------------------
//
//
//-------------------------------------------------------------------------------------------------
void LCD_DefineCharacter(unsigned char address, unsigned char * charPattern)
{
unsigned char i;
LCD_WriteCommand(64 + (address*8), 0);
LCD_RS_PORT |= LCD_RS;
for(i = 0; i < 8; i++)
  {
  _LCD_Write(charPattern[i],0);
  }
LCD_WriteCommand(64 + (address*8), 1);
LCD_RS_PORT |= LCD_RS;
for(i = 0; i < 8; i++)
  {
  _LCD_Write(charPattern[i],1);
  }
}
//-------------------------------------------------------------------------------------------------
//
//
//-------------------------------------------------------------------------------------------------
void LCD_Initalize(void)
{
unsigned char i;
LCD_DB4_DIR |= LCD_DB4; // Konfiguracja kierunku pracy wyprowadzeñ
LCD_DB5_DIR |= LCD_DB5; //
LCD_DB6_DIR |= LCD_DB6; //
LCD_DB7_DIR |= LCD_DB7; //
LCD_E1_DIR 	|= LCD_E1;  //
LCD_E2_DIR 	|= LCD_E2;  //
LCD_RS_DIR 	|= LCD_RS;  //

_delay_ms(15); // oczekiwanie na ustalibizowanie siê napiecia zasilajacego

LCD_RS_PORT &= ~LCD_RS; // wyzerowanie linii RS
LCD_E1_PORT &= ~LCD_E1;  // wyzerowanie linii E1
LCD_E2_PORT &= ~LCD_E2;  // wyzerowanie linii E1

for(i = 0; i < 3; i++) // trzykrotne powtórzenie bloku instrukcji
  {
  _LCD_OutNibble(0x03); // tryb 8-bitowy
  LCD_Enable(0);
  LCD_Enable(1);
  _delay_ms(5); // czekaj 5ms
  }
_LCD_OutNibble(0x02); // tryb 4-bitowy

LCD_Enable(0);
LCD_Enable(1);

_delay_ms(1); // czekaj 1ms 
LCD_WriteCommand(HD44780_FUNCTION_SET | HD44780_FONT5x7 | HD44780_TWO_LINE | HD44780_4_BIT, 0); // interfejs 4-bity, 2-linie, znak 5x7
LCD_WriteCommand(HD44780_FUNCTION_SET | HD44780_FONT5x7 | HD44780_TWO_LINE | HD44780_4_BIT, 1); // interfejs 4-bity, 2-linie, znak 5x7

LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF, 0); // wy³¹czenie wyswietlacza
LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF, 1); // wy³¹czenie wyswietlacza

LCD_WriteCommand(HD44780_CLEAR, 0); // czyszczenie zawartosæi pamieci DDRAM
LCD_WriteCommand(HD44780_CLEAR, 1); // czyszczenie zawartosæi pamieci DDRAM
_delay_ms(2);
LCD_WriteCommand(HD44780_ENTRY_MODE | HD44780_EM_SHIFT_CURSOR | HD44780_EM_INCREMENT, 0);// inkrementaja adresu i przesuwanie kursora
LCD_WriteCommand(HD44780_ENTRY_MODE | HD44780_EM_SHIFT_CURSOR | HD44780_EM_INCREMENT, 1);// inkrementaja adresu i przesuwanie kursora

LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF | HD44780_CURSOR_NOBLINK, 0); // w³¹cz LCD, bez kursora i mrugania
LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF | HD44780_CURSOR_NOBLINK, 1); // w³¹cz LCD, bez kursora i mrugania
}
//-------------------------------------------------------------------------------------------------
