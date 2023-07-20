#ifndef ST7789H2_H
#define ST7789H2_H

#define WIDTH  240
#define HEIGHT 240

void WriteData(uint8_t data);
void WriteCmd(uint8_t data);
void lcd_set_startaddr(uint16_t x_start, uint16_t x_end, uint16_t y_start,  uint16_t y_end);

#endif