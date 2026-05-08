/*
 * Teig_Maschine-L298N.c
 *
 * Created: 16.05.2025 17:56:29
 * Author : luayy
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>

#define PWM_Pin PD5
#define IN1 PD4
#define IN2 PD6


void adc_init(void);
uint16_t read_adc_once(uint8_t channel);
uint16_t get_adc_value(uint8_t channel);

int main(void)
{
	DDRD |= (1<<PWM_Pin) | (1 << IN1) | (1 << IN2);

	PORTD |= (1 << IN1);   // vorwärts
	PORTD &= ~(1 << IN2);  // rückwärts aus
	
	uint16_t Poti = 0;
	uint16_t Poti_Prozent = 0;
	char buffer[16];
	uint16_t DutyCycle = 0;
	uint8_t ocr_wert;
	uint16_t rpm_schaetzung;
	
	adc_init();
	lcd_init();
	
	//PWM aktivieren
	TCCR0A |= (1<<COM0B1) | (1<<WGM00) | (1<<WGM01);
	//TIMSK0 |= (1<<TOIE0);
	//sei();
	TCCR0B = (1<<CS00);

	while (1)
	{
		Poti = get_adc_value(0); // Kanal 0 = PC0
		Poti_Prozent = Poti/10;
		DutyCycle = (Poti * 100UL) / 1023;
		ocr_wert = (DutyCycle * 255UL) / 100;
		// Beispiel: realistische RPM (z.?B. 0–12.700 bei 100 %)
		rpm_schaetzung = (ocr_wert * 30UL) / 255;
		
		// PWM-Wert auf OCR0B setzen
		OCR0B = ocr_wert;
		
		// LCD Zeile 0: ADC und PWM-Prozent anzeigen
		sprintf(buffer, "ADC:%4uPro:%3u%%", Poti, Poti_Prozent);
		lcd_gotoxy(0, 0);
		lcd_puts(buffer);

		// LCD Zeile 1: RPM anzeigen
		sprintf(buffer, "RPM:%2u  PWM:%3u%%", rpm_schaetzung, DutyCycle);
		lcd_gotoxy(0, 1);
		lcd_puts(buffer);

		_delay_ms(300);
	}
}

void adc_init(void)
{
	ADMUX = (1 << REFS0); // AVCC als Referenz, Kanal 0
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // ADC aktivieren  // Prescaler 128
}

uint16_t read_adc_once(uint8_t channel)
{
	ADMUX = (ADMUX & 0xF0) | (channel & 0x07); // Kanal setzen
	ADCSRA |= (1 << ADSC); // Konvertierung starten
	while (ADCSRA & (1 << ADSC)); // Warten bis fertig
	return ADC; // 10-Bit Ergebnis zurückgeben
}
// Geglättete ADC-Messung (z.?B. Mittelwert aus 10 Werten)
uint16_t get_adc_value(uint8_t channel)
{
	uint32_t summe = 0;
	for (uint8_t i = 0; i < 10; i++)
	{
		summe += read_adc_once(channel);
		_delay_ms(2);
	}
	return summe / 10; // Mittelwert
}