#ifndef LCD_H
#define LCD_H

void lcd_init(void);
void lcd_cmd(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_gotoxy(uint8_t x, uint8_t y);
void lcd_puts(const char* str);

#endif
