/*
 * LCD1602.c
 *
 *  Created on: 21-Jan-2020
 *      Author: Controllerstech
 */



#include "stm32f1xx_hal.h"
//#include "LCD1602.h"
#include<stdio.h>
#include<stdlib.h>
/*********** Define the LCD PINS below ****************/

#define RS_Pin GPIO_PIN_1
#define RS_GPIO_Port GPIOA
#define RW_Pin GPIO_PIN_2
#define RW_GPIO_Port GPIOA
#define EN_Pin GPIO_PIN_3
#define EN_GPIO_Port GPIOA
#define D4_Pin GPIO_PIN_4
#define D4_GPIO_Port GPIOA
#define D5_Pin GPIO_PIN_5
#define D5_GPIO_Port GPIOA
#define D6_Pin GPIO_PIN_6
#define D6_GPIO_Port GPIOA
#define D7_Pin GPIO_PIN_7
#define D7_GPIO_Port GPIOA

/****************** define the timer handler below  **************/
#define timer htim1


extern TIM_HandleTypeDef timer;
void delay (uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&timer, 0);
	while (__HAL_TIM_GET_COUNTER(&timer) < us);
}

/****************************************************************************************************************************************************************/

void send_to_lcd (char data, int rs)
{
	HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, rs);  // rs = 1 for data, rs=0 for command

	/* write the data to the respective pin */
	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, ((data>>3)&0x01));
	HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, ((data>>2)&0x01));
	HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, ((data>>1)&0x01));
	HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, ((data>>0)&0x01));

	/* Toggle EN PIN to send the data
	 * if the HCLK > 100 MHz, use the  20 us delay
	 * if the LCD still doesn't work, increase the delay to 50, 80 or 100..
	 */
	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 1);
	delay (20);
	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 0);
	delay (20);
}

void lcd_send_cmd (char cmd)
{
    char datatosend;

    /* send upper nibble first */
    datatosend = ((cmd>>4)&0x0f);
    send_to_lcd(datatosend,0);  // RS must be 0 while sending command

    /* send Lower Nibble */
    datatosend = ((cmd)&0x0f);
	send_to_lcd(datatosend, 0);
}

void lcd_send_data (char data)
{
	char datatosend;

	/* send higher nibble */
	datatosend = ((data>>4)&0x0f);
	send_to_lcd(datatosend, 1);  // rs =1 for sending data

	/* send Lower nibble */
	datatosend = ((data)&0x0f);
	send_to_lcd(datatosend, 1);
}

void lcd_clear (void)
{
	lcd_send_cmd(0x01);
	HAL_Delay(2);
}

void lcd_put_cur(int row, int col)
{
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }

    lcd_send_cmd (col);
}


void lcd_init (void)
{
	// 4 bit initialisation
	HAL_Delay(50);  // wait for >40ms
	lcd_send_cmd (0x30);
	HAL_Delay(5);  // wait for >4.1ms
	lcd_send_cmd (0x30);
	HAL_Delay(1);  // wait for >100us
	lcd_send_cmd (0x30);
	HAL_Delay(10);
	lcd_send_cmd (0x20);  // 4bit mode
	HAL_Delay(10);

  // dislay initialisation
	lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(1);
	lcd_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	HAL_Delay(1);
	lcd_send_cmd (0x01);  // clear display
	HAL_Delay(1);
	HAL_Delay(1);
	lcd_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(1);
	lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
	HAL_Delay(1);
}

void lcd_send_string (char *str)
{
	while (*str) lcd_send_data (*str++);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void LCD_CreateChar(uint8_t location, uint8_t charmap[]) {
//    location &= 0x7; // We only have 8 locations 0-7
//    lcd_send_cmd(0x40 | (location << 3));
//    for (int i = 0; i < 8; i++) {
//    	lcd_send_data(charmap[i]);
 //   }
const uint8_t gBigFontShapeTable[] = {
    // Shapes...
    0x07, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, // LT
    0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, // UB
    0x1C, 0x1E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, // RT
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0F, 0x07, // LL
    0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, // LB
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1E, 0x1C, // LR
    0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x1F, 0x1F, // UMB
    0x1F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F  // LMB
};

// Ascii table
const uint8_t	gBigFontAsciiTable[]  	=	{

	32,	32,	32,	32,	32,	32,		//	0x20	space
	32,	0,	32,	32,	4,	32,		//	0x21	!
	32,	32,	32,	32,	32,	32,		//	0x22
	32,	32,	32,	32,	32,	32,		//	0x23
	32,	32,	32,	32,	32,	32,		//	0x24
	32,	32,	32,	32,	32,	32,		//	0x25
	32,	32,	32,	32,	32,	32,		//	0x26
	32,	32,	32,	32,	32,	32,		//	0x27
	32,	32,	32,	32,	32,	32,		//	0x28
	32,	32,	32,	32,	32,	32,		//	0x29
	32,	32,	32,	32,	32,	32,		//	0x2A
	32,	32,	32,	32,	32,	32,		//	0x2B
	32,	32,	32,	32,	32,	32,		//	0x2C
	32,	32,	32,	32,	32,	32,		//	0x2D
	32,	32,	32,	32,	4,	32,		//	0x2E	. (period)
	32,	32,	32,	32,	32,	32,		//	0x2F

	0,	1,	2,	3,	4,	5,		//	0x30	0
	1,	2,	32,	32,	5,	32,		//	0x31	1
	6,	6,	2,	3,	7,	7,		//	0x32	2
	6,	6,	2,	7,	7,	5,		//	0x33	3
	3,	4,	2,	32,	32,	5,		//	0x34	4
	255,6,	6,	7,	7,	5,		//	0x35	5
//	0,	6,	6,	7,	7,	5,		//	0x35	5
	0,	6,	6,	3,	7,	5,		//	0x36	6
	1,	1,	2,	32,	0,	32,		//	0x37	7
	0,	6,	2,	3,	7,	5,		//	0x38	8
	0,	6,	2,	32,	32,	5,		//	0x39	9
	32,	32,	32,	32,	32,	32,		//	0x3A
	32,	32,	32,	32,	32,	32,		//	0x3B
	32,	32,	32,	32,	32,	32,		//	0x3C
	32,	32,	32,	32,	32,	32,		//	0x3D
	32,	32,	32,	32,	32,	32,		//	0x3E
	1,	6,	2,	254,7,	32,		//	0x3F	?
	32,	32,	32,	32,	32,	32,		//	0x40	@
	0,	6,	2,	255,254,255,	//	0x41	A
	255,6,	5,	255,7,	2,		//	0x42	B
	0,	1,	1,	3,	4,	4,		//	0x43	C
	255,1,	2,	255,4,	5,		//	0x44	D
	255,6,	6,	255,7,	7,		//	0x45	E
	255,6,	6,	255,32,	32,		//	0x46	F
	0,	1,	1,	3,	4,	2,		//	0x47	G
	255,4,	255,255,254,255,	//	0x48	H
	1,	255,1,	4,	255,4,		//	0x49	I
	32,	32,	255,4,	4,	5,		//	0x4A	J
	255,4,	5,	255,254,2,		//	0x4B	K
	255,32,	32,	255,4,	4,		//	0x4C	L
	32,	32,	32,	32,	32,	32,		//	0x4D	M	place holder
	32,	32,	32,	32,	32,	32,		//	0x4E	N	place holder
	0,	1,	2,	3,	4,	5,		//	0x4F	O	(same as zero)
	0,	6,	2,	3,	32,	32,		//	0x50	P
	32,	32,	32,	32,	32,	32,		//	0x51	Q
	0,	6,	5,	3,	32,	2,		//	0x52	R
	0,	6,	6,	7,	7,	5,		//	0x35	S	(same as 5)
	1,	2,	1,	32,	5,	32,		//	0x54	T
	2,	32,	2,	3,	4,	5,		//	0x55	U
	32,	32,	32,	32,	32,	32,		//	0x56	V	place holder
	32,	32,	32,	32,	32,	32,		//	0x57	W	place holder
	3,	4,	5,	0,	32,	2,		//	0x58	X
	3,	4,	5,	32,	5,	32,		//	0x59	Y
	1,	6,	5,	0,	7,	4,		//	0x5A	Z
	0

};

// Separate table for wide characters
const uint8_t gBigFontAsciiTableWide[] = {
    // Add wide character mappings here...
    0, 1, 3, 1, 2, 3, 32, 32, 32, 5,    // 0x4D M 5-wide
    0, 3, 32, 2, 32, 3, 32, 2, 5, 32,   // 0x4E N 4-wide
    0, 1, 2, 32, 32, 3, 4, 3, 4, 32,    // 0x51 Q 4-wide
    3, 32, 32, 5, 32, 32, 3, 5, 32, 32, // 0x56 V 4-wide
    0, 32, 32, 32, 2, 3, 4, 0, 4, 5,    // 0x57 W 5-wide
};

void begin() {
    uint8_t customCharDef[8];
    for (uint8_t ii = 0; ii < 8; ii++) {
        for (int jj = 0; jj < 8; jj++) {
            customCharDef[jj] = gBigFontShapeTable[ii * 8 + jj];
        }
        LCD_CreateChar(ii, customCharDef);
    }
}
char buff[20];
int writechar(int col, int row, char c) {
    int offset;
    int charWidth;
    int specialCase = 1;
    int specialIndex;

    if (c == ' ') {
        return 2;
    } else if (c < ' ') {
        return 0;
    }

    if (c >= 'A') {
        c = c & 0x5F; // force to upper case
    }

    switch (c) {
        case 'M': charWidth = 5; specialIndex = 0; break;
        case 'N': charWidth = 4; specialIndex = 1; break;
        case 'Q': charWidth = 4; specialIndex = 2; break;
        case 'V': charWidth = 4; specialIndex = 3; break;
        case 'W': charWidth = 5; specialIndex = 4; break;
        default:
            charWidth = 3;
            specialCase = 0;
            offset = 6 * (c - ' ');
            lcd_put_cur(row, col);
            for (int i = 0; i < 3; i++) {
                int b = gBigFontAsciiTable[offset + i];
                lcd_send_data(b);

            }
            lcd_put_cur(row + 1, col);
            offset += 3;
            for (int i = 0; i < 3; i++) {
                //int b = gBigFontAsciiTable[offset + i];
            	int b = gBigFontAsciiTable[offset + i];
                lcd_send_data(b);
               // sprintf(buff,"Data :%d",offset);
               // lcd_send_string(buff);
            }

            break;
    }

    if (specialCase) {
        offset = 10 * specialIndex;
        lcd_put_cur(col, row);
        for (int i = 0; i < charWidth; i++) {
            uint8_t b = gBigFontAsciiTableWide[offset + i];
            lcd_send_data(b);
        }
        lcd_put_cur(col, row + 1);
        offset += 5;
        for (int i = 0; i < charWidth; i++) {
            uint8_t b = gBigFontAsciiTableWide[offset + i];
            lcd_send_data(b);
        }
    }

    return charWidth + 1;
}

void clear(int col, int row) {
	lcd_put_cur(col, row);
	lcd_send_data(' ');
	lcd_send_data(' ');
	lcd_send_data(' ');
    lcd_put_cur(col, row + 1);
    lcd_send_data(' ');
    lcd_send_data(' ');
    lcd_send_data(' ');
}

void writeint(int col, int row, int number, uint8_t digits, int leading) {
    int isNegative = 0;
    if (number < 0) {
        isNegative = 1;
        number = abs(number);
    }

    uint8_t numString[digits];
    uint8_t index = digits - 1;
    while (index) {
        numString[index] = number % 10;
        number /= 10;
        index--;
    }
    numString[0] = number % 10;

    for (int i = 0; i < digits; i++) {
        if (!leading && numString[i] == 0 && i != digits - 1) {
            writechar(col, row, ' ');
        } else {
            writechar(col, row, numString[i] + '0');
            leading = 1;
        }
        col += 4;
    }

    if (isNegative) {
        writechar(col, row, '-');
    }
}


