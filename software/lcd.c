#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

// LCD RS ? PD7
#define LCD_RS_PORT PORTD
#define LCD_RS_DDR  DDRD
#define LCD_RS_PIN  PD7

// LCD E ? PB0
#define LCD_EN_PORT PORTB
#define LCD_EN_DDR  DDRB
#define LCD_EN_PIN  PB0

// LCD D4–D7 ? PB1–PB4
#define LCD_DATA_PORT PORTB
#define LCD_DATA_DDR  DDRB
#define LCD_D4_PIN PB1
#define LCD_D5_PIN PB2
#define LCD_D6_PIN PB3
#define LCD_D7_PIN PB4

void lcd_enable(void) {
	LCD_EN_PORT |= (1 << LCD_EN_PIN);
	_delay_us(1);
	LCD_EN_PORT &= ~(1 << LCD_EN_PIN);
	_delay_us(100);
}

void lcd_send_nibble(uint8_t data) {
	// nur High Nibble relevant
	if (data & 0x10) LCD_DATA_PORT |= (1 << LCD_D4_PIN); else LCD_DATA_PORT &= ~(1 << LCD_D4_PIN);
	if (data & 0x20) LCD_DATA_PORT |= (1 << LCD_D5_PIN); else LCD_DATA_PORT &= ~(1 << LCD_D5_PIN);
	if (data & 0x40) LCD_DATA_PORT |= (1 << LCD_D6_PIN); else LCD_DATA_PORT &= ~(1 << LCD_D6_PIN);
	if (data & 0x80) LCD_DATA_PORT |= (1 << LCD_D7_PIN); else LCD_DATA_PORT &= ~(1 << LCD_D7_PIN);
	lcd_enable();
}

void lcd_cmd(uint8_t cmd) {
	LCD_RS_PORT &= ~(1 << LCD_RS_PIN); // RS = 0 für Befehle
	lcd_send_nibble(cmd & 0xF0);
	lcd_send_nibble((cmd << 4) & 0xF0);
	_delay_ms(2);
}

void lcd_data(uint8_t data) {
	LCD_RS_PORT |= (1 << LCD_RS_PIN); // RS = 1 für Daten
	lcd_send_nibble(data & 0xF0);
	lcd_send_nibble((data << 4) & 0xF0);
	_delay_ms(2);
}

void lcd_init(void) {
	// RS (PD7)
	LCD_RS_DDR |= (1 << LCD_RS_PIN);
	// EN + D4–D7 (PB0–PB4)
	LCD_EN_DDR |= (1 << LCD_EN_PIN);
	LCD_DATA_DDR |= (1 << LCD_D4_PIN) | (1 << LCD_D5_PIN) | (1 << LCD_D6_PIN) | (1 << LCD_D7_PIN);

	_delay_ms(20);
	lcd_cmd(0x33);
	lcd_cmd(0x32);
	lcd_cmd(0x28); // 4-bit, 2 Zeilen, 5x8 Font
	lcd_cmd(0x0C); // Display AN, Cursor AUS
	lcd_cmd(0x06); // Cursor INKREMENT
	lcd_cmd(0x01); // Löschen
	_delay_ms(2);
}

void lcd_gotoxy(uint8_t x, uint8_t y) {
	uint8_t addr[] = {0x00, 0x40};
	lcd_cmd(0x80 + addr[y] + x);
}

void lcd_puts(const char* str) {
	while (*str) {
		lcd_data(*str++);
	}
}
