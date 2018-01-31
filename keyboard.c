/**
This is the keyboard implementation file for drumpads II.
*/
#include "p18f4550.h"
#include "keyboard.h"
#include "disp_JHD162A_drv.h"

#define KEYBOARD_TRIS TRISD
#define KEYBOARD_PORT PORTD
#define KEYBOARD_LAT LATD
#define KEYBOARD_COLS 0x0F
#define KEYBOARD_ROWS 0xF0
#define SET_KEYBOARD_PORT KEYBOARD_TRIS = KEYBOARD_COLS //ooooiiii

//unsigned char check_row(unsigned char row, unsigned char r);

#pragma idata keys
static unsigned char matrix[4][4] =
{
	'1', '2', '3', 'A', 
	'4', '5', '6', 'B', 
	'7', '8', '9', 'C', 
	'*', '0', '#', 'D', 
};							

#pragma code

unsigned char getc(void)
{
	// Note: col is the port bit to mask for, while c is the col number 0 - 3
	// Note: row is the port bit to test, while r is the row number 0 - 3
	static unsigned char col = 0x01; // Input column bits
	static unsigned char row = 0x10; // Output row bits
	static unsigned char c = 0;
	static unsigned char r = 0;
	SET_KEYBOARD_PORT;

	// Output a one on the current row:
	KEYBOARD_PORT = row;

	// Check current column for a depressed key (a one in the col bit):
	if (KEYBOARD_PORT & col)
	{
		delay_10_ms();
		if (KEYBOARD_PORT & col)
		{
			// Wait for key to be released, otherwise we will interfere with the display driver!
			while (KEYBOARD_PORT & col);
			delay_10_ms();
			return matrix[r][c];
		}
	}
	col *= 2;

	c++;
	if (c > (unsigned char)3)
	{
		col = 0x01;
		c = 0;
		row *= 2;
		r++;
		if (r > (unsigned char)3)
		{
			row = 0x10;
			r = 0;
		}
	}

	return ' ';
}
/*
unsigned char getc(void)
{
	unsigned char row = 0x10;
	unsigned char value;
	unsigned char r;

	SET_KEYBOARD_PORT;
	for (r = (unsigned char)0; r < (unsigned char)4; r++)
	{
		value = check_row(row, r);
		if (value > (unsigned char)0)
		{
			return value;
		}
		row *= 2;
	}
	return 0;
}

unsigned char check_row(unsigned char row, unsigned char r)
{
	unsigned char col = 0x01;
	unsigned char c;

	#define CODE_ADDR_UPPER 0x0100    
	unsigned char CODE_ADDR_HIGH;   
	unsigned char CODE_ADDR_LOW;   
	unsigned char WORD_EVEN;   
	unsigned char WORD_ODD;   

	KEYBOARD_PORT = row;
	for (c = (unsigned char)0; c < (unsigned char)4; c++)
	{
		if (KEYBOARD_PORT & col)
		{
			delay_10_ms(void);
			if (KEYBOARD_PORT & col)
			{
				// Wait for key to be released, otherwise we will interfere with the display driver!
				while (KEYBOARD_PORT & col);
				delay_10_ms(void);
				return matrix[r][c];
			}
		}
		col *= 2;
	}
*/
/*
_asm
	MOVLW CODE_ADDR_UPPER    
	MOVWF TBLPTRU, 1    
	MOVLW CODE_ADDR_HIGH   
	MOVWF TBLPTRH, 1   
	MOVLW CODE_ADDR_LOW   
	MOVWF TBLPTRL, 1    
READ_WORD:   
	TBLRDPOSTINC    
	MOVF TABLAT, 0, ACCESS
	MOVWF WORD_EVEN, 1   
	TBLRDPOSTINC    
	MOVF TABLAT, 0, ACCESS    
	MOVWF WORD_ODD, 1   
_endasm
*/
//	return 0;
//}

