#ifndef DISP_JHD162A_H_DEFINED
#define DISP_JHD162A_H_DEFINED

/**
Print one argument line to buffer.
*/
void PrintLine1(const rom far char * p);
void PrintLine2(const rom far char * p);

/**
Print one RAM line to buffer.
*/
void PrintLine1Ram(far char * p);
void PrintLine2Ram(far char * p);

/**
Print an integer as base 10 or hex.
value = value to print.
length = number of digits. negative to reserve space for minus sign.
line = line to print on, 1 or 2.
pos = position on line, 1 to 16.
*/
void PrintNumeric(int value, int length, int line, int pos);
void PrintHex(unsigned char value, int line, int pos);

/**
Clear display and set current position to 0:
*/
void Clr(void);
void lcd_init(void);
void delay_50_us(void);
void delay_10_ms(void);

#endif
