/*
 * big_lcd_font.h
 *
 *  Created on: Jun 13, 2024
 *      Author: Pradoom Rao
 */

#ifndef INC_BIG_LCD_FONT_H_
#define INC_BIG_LCD_FONT_H_
void begin();
int writechar(int col, int row, char c);
void clear(int col, int row);
void writeint(int col, int row, int number, unsigned char digits, int leading);
void LCD_CreateChar(unsigned char location, unsigned char charmap[]);


#endif /* INC_BIG_LCD_FONT_H_ */
