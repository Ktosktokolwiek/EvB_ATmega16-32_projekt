/*
 * RTC.h
 *
 * Created: 2016-06-08 09:54:33
 *  Author: Alan Szopiñski
 */ 


#ifndef RTC_H_
#define RTC_H_

#define ADDR_PCF8583 0xA2

enum Registers {CntrlReg1=0, CntrlReg2=1, SecondsReg=2, MinutesReg=3, HoursReg=4,
	DaysReg=5, WeekDaysReg=6, MonthsReg=7, YearsReg=8, MinuteAlarmReg=9,
	HourAlarmReg=10, DayAlarmReg=11, WeekDayAlarmReg=12, CLKUOUTFreqReg=13,
TimerCntrlReg=14, TimerCountDownReg=15};
enum Alarm {AlarmDisable=128};
enum Flags {PCF_TimeValid=128, PCF_CLKOUTActivate=128, PCF_CLKOUT1Hz=3,
	PCF_CLKOUT32Hz=2, PCF_CLKOUT1024Hz=1, PCF_CLKOUT32768Hz=0, PCF_EnableTimer=128,
	PCF_TimerClk4096Hz=0, PCF_TimerClk64Hz=1, PCF_TimerClk1Hz=2,
PCF_TimerClk160sHz=3, PCF_AlarmInterruptEnabled=2, PCF_TimerInterruptEnabled=1};
enum Days {PCF_Sunday, PCF_Monday, PCF_Tuesday, PCF_Wednesday, PCF_Thursday,
PCF_Friday, PCF_Saturday};
enum Months {PCF_January=1, PCF_February, PCF_March, PCF_April, PCF_May, PCF_June,
PCF_July, PCF_August, PCF_September, PCF_October, PCF_November, PCF_December};

typedef struct
{
	uint8_t dzien;
	uint8_t dzien_tygodnia;
	uint8_t miesiac;
	uint8_t rok;
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