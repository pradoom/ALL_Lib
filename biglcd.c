/*
 * biglcd.c
 *
 *  Created on: Jun 13, 2024
 *      Author: Pradoom Rao
 */


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LCD_CreateChar(unsigned char location, unsigned char charmap[]) {
    location &= 0x7; // We only have 8 locations 0-7
    lcd_send_cmd(0x40 | (location << 3));
    for (int i = 0; i < 8; i++) {
    	lcd_send_data(charmap[i]);
    }
const unsigned char gBigFontShapeTable[] = {
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
const unsigned char gBigFontAsciiTable[] = {
    // Ascii to shape mappings...
    32, 32, 32, 32, 32, 32,    // 0x20 space
    32, 0, 32, 32, 4, 32,      // 0x21 !
    32, 32, 32, 32, 32, 32,    // 0x22 "
    // Add remaining mappings here...
    0, 1, 2, 3, 4, 5,          // 0x30 0
    1, 2, 32, 32, 5, 32,       // 0x31 1
    6, 6, 2, 3, 7, 7,          // 0x32 2
    6, 6, 2, 7, 7, 5,          // 0x33 3
    3, 4, 2, 32, 32, 5,        // 0x34 4
    255, 6, 6, 7, 7, 5,        // 0x35 5
    0, 6, 6, 3, 7, 5,          // 0x36 6
    1, 1, 2, 32, 0, 32,        // 0x37 7
    0, 6, 2, 3, 7, 5,          // 0x38 8
    0, 6, 2, 32, 32, 5,        // 0x39 9
    // Add remaining mappings here...
    32, 32, 32, 32, 32, 32,    // 0x20 space
    32, 0, 32, 32, 4, 32,      // 0x21 !
    32, 32, 32, 32, 32, 32,    // 0x22 "
    // Add remaining mappings here...
    0, 1, 2, 3, 4, 5,          // 0x30 0
    1, 2, 32, 32, 5, 32,       // 0x31 1
    6, 6, 2, 3, 7, 7,          // 0x32 2
    6, 6, 2, 7, 7, 5,          // 0x33 3
    3, 4, 2, 32, 32, 5,        // 0x34 4
    255, 6, 6, 7, 7, 5,        // 0x35 5
    0, 6, 6, 3, 7, 5,          // 0x36 6
    1, 1, 2, 32, 0, 32,        // 0x37 7
    0, 6, 2, 3, 7, 5,          // 0x38 8
    0, 6, 2, 32, 32, 5,        // 0x39 9
};

// Separate table for wide characters
const unsigned char gBigFontAsciiTableWide[] = {
    // Add wide character mappings here...
    0, 1, 3, 1, 2, 3, 32, 32, 32, 5,    // 0x4D M 5-wide
    0, 3, 32, 2, 32, 3, 32, 2, 5, 32,   // 0x4E N 4-wide
    0, 1, 2, 32, 32, 3, 4, 3, 4, 32,    // 0x51 Q 4-wide
    3, 32, 32, 5, 32, 32, 3, 5, 32, 32, // 0x56 V 4-wide
    0, 32, 32, 32, 2, 3, 4, 0, 4, 5,    // 0x57 W 5-wide
};

void begin() {
    unsigned char customCharDef[8];
    for (unsigned char ii = 0; ii < 8; ii++) {
        for (int jj = 0; jj < 8; jj++) {
            customCharDef[jj] = gBigFontShapeTable[ii * 8 + jj];
        }
        LCD_CreateChar(ii, customCharDef);
    }
}

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
            lcd_put_cur(col, row);
            for (int i = 0; i < 3; i++) {
                int b = gBigFontAsciiTable[offset + i];
                lcd_send_data(b);
            }
            lcd_put_cur(col, row + 1);
            offset += 3;
            for (int i = 0; i < 3; i++) {
                int b = gBigFontAsciiTable[offset + i];
                lcd_send_data(b);
            }
            break;
    }

    if (specialCase) {
        offset = 10 * specialIndex;
        lcd_put_cur(col, row);
        for (int i = 0; i < charWidth; i++) {
            unsigned char b = gBigFontAsciiTableWide[offset + i];
            lcd_send_data(b);
        }
        lcd_put_cur(col, row + 1);
        offset += 5;
        for (int i = 0; i < charWidth; i++) {
            unsigned char b = gBigFontAsciiTableWide[offset + i];
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

void writeint(int col, int row, int number, unsigned char digits, int leading) {
    int isNegative = 0;
    if (number < 0) {
        isNegative = 1;
        number = abs(number);
    }

    unsigned char numString[digits];
    unsigned char index = digits - 1;
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
}
