/*
 * RTC.h
 *
 * Created: 2016-06-08 09:54:33
 *  Author: Alan Szopiñski
 */ 


#ifndef RTC_H_
#define RTC_H_

#define ADDR_PCF8583 0xA2

#define GODZINY_MASK	0x2F
#define DZIEN_MASK		0x3F
#define DZIEN_TYG_MASK	0xE0
#define MIESIAC_MASK	0x1F
#define ROK_MASK		0xC0

enum Registers {CntrlReg=0, HundredthSecondsReg=1, SecondsReg=2, MinutesReg=3, HoursReg=4,
	YearDateReg=5, WeekDaysMounthReg=6, TimerReg=7, AlarmCntrlReg=8, HundredthSecondsAlarmReg=9,
	SecondsAlarmReg=10, MinutesAlarmReg=11, HoursAlarmReg=12, DataAlarmReg=13,
	MonthAlarmReg=14, TimerAlarmReg=15};
enum Days {PCF_Sunday, PCF_Monday, PCF_Tuesday, PCF_Wednesday, PCF_Thursday,
PCF_Friday, PCF_Saturday};
enum Months {PCF_January=1, PCF_February, PCF_March, PCF_April, PCF_May, PCF_June,
PCF_July, PCF_August, PCF_September, PCF_October, PCF_November, PCF_December};

typedef struct
{
	uint8_t dzien;
	uint8_t dzien_tygodnia;
	uint8_t miesiac;
	uint16_t rok;
} Data;

typedef struct
{
	uint8_t sekundy;
	uint8_t minuty;
	uint8_t godziny;
} Czas;

void ustaw_czas(Czas *czas);
void ustaw_date(Data *data);
void pobierz_czas(Czas *czas);
void pobierz_date(Data *data);
void lcd_wyswietl_date(Data *data, char *bufor_lcd, uint8_t wyl_czesc_daty);
uint8_t bcd2bin(uint8_t n);
uint8_t bin2bcd(uint8_t n);

void nadpisz_bufor_spacjami (char *bufor);
#endif /* RTC_H_ */