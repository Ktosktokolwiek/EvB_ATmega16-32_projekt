/*
 * klawiatura.h
 *
 * Created: 2016-06-20 10:21:33
 *  Author: Alan Szopiñski
 */ 


#ifndef KLAWIATURA_H_
#define KLAWIATURA_H_

#define PRZYCISK_ENTER	0b00010010
#define PRZYCISK_POWROT	0b00010001

#define PRZYCISK_PRAWO	0b00100010
#define PRZYCISK_LEWO	0b00100001

#define PRZYCISK_DOL	0b00010100
#define PRZYCISK_GORA	0b00100100

uint8_t KlawiaturaSkan(void);
uint8_t debouncer(uint8_t key);

#endif /* KLAWIATURA_H_ */