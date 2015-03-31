#ifndef F_CPU
	#define F_CPU 16000000
#endif

#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <util/delay.h>

volatile int 
	tick1 = 0;					// Zählvar.

volatile unsigned char			// Timer1 Overflows
	count_overflows = 0;

volatile uint32_t 
	freq1 = 0,					// Var. 1s-Gate
	freq_ausgabe = 0,			// 32-bit für Ergebnis
	freq_ausgabe_high1 = 0;		// Var. für Ausgabe hohe Freq. (ab 50kHz)

double korrekturwert = 0.999985750203;	// Korrekturwert ausgemessen mit externem 16MHz Quarz
		
/*** Timer Routinen ***/
ISR(TIMER0_COMPA_vect)		// wird mit 125Hz  aufgerufen (16MHz/1024/125, alle 8ms)
	{
		uint8_t adcin_temp;			// Var. für PINF
		uint16_t counter_temp;		// Var. zur zwischenspeicherung Zählerstand

		adcin_temp = PINF;
		counter_temp = TCNT1;		// Zählerwert in Variable lesen

		tick1++;
		if(tick1 == 125) // Torzeit: 1s, Sekundenbasis (16MHz/1024/125/125=1Hz)
			{
				PORTB |= (1<<PB0);		// 74HC393 clear
				asm volatile ("NOP");
				asm volatile ("NOP");
				PORTB &= ~(1<<PB0);		// 74HC393 auf Ursprung
				TCNT1 = 0;				    // Zählerwert rücksetzen
				tick1 = 0;			    	// Tick rücksetzen

				// Frequenzregister bilden, mit Korrekturwert
				freq1 = ( (((uint32_t)count_overflows)<<24) | (((uint32_t)counter_temp)<<8) | (uint32_t)adcin_temp);
				//freq_change=1;
				adcin_temp = 0;			// Variable Port A rücksetzen
				counter_temp = 0;		// Variable Zählerstand rücksetzen
				count_overflows = 0;	// Timer1 OVF rücksetzen
			}
}

ISR(TIMER1_OVF_vect)	// ca. 244 Hz (16MHz/65536)
	{
		count_overflows++;	// Timer1 zählen lassen
	}

int main()
	{	
		// Init ADC Eingänge
		DDRF = 0b00000000;		// Port F auf Eingang setzen (Prescaler auslesen)
		uint8_t bPortF;			  // Variable für Port F
		bPortF = PINF;			  // Wird in Variable geschrieben
		// Init Timer Ein-/Ausgänge
		DDRD = 0b01000000; 		// PD6 als Eingang (T1)
		DDRD |= (1 << PD7); 	// PD7 als Ausgang (T0) für 74HC393

		/*** Timer-Initialisierungen ***/
		// Timer0: Fenster in dem gezählt wird
 		TCCR0B |= (1<<CS02) | (1<<CS00) | (1<<WGM01);// Timer0 Prescaler 1024 und CTC Modus (S.82)
		OCR0A = 124;				// Ab Zählschritt 125 INT auslösen (S.106)
		TIMSK0 |= (1<<OCIE0A); 		// Erlaube Overflow INT, Timer0
		// Timer1: Flanken zählen
		TCCR1A |= (1<<ICES1) | (1<<CS11);	// Prescale 8, Input Capture Edge Select (S.110)
		TIMSK1 |= (1<<TOIE1) | (1<<ICIE1); 	// OVF und CAPT Int erlauben, Timer1 (S.112)

		sei();					// Interrupts erlauben

		while(1){
		      freq_ausgabe_high1 = (freq1 * korrekturwert);	// Frequenz mit Korrektur berechnen
		  }
	}
