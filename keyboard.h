#ifndef KEYBOARD_H_DEFINED
#define KEYBOARD_H_DEFINED

/**
This is the keyboard implementation file for drumpads II.
*/
#define KEYBOARD_TRIS TRISD
#define KEYBOARD_PORT PORTD
#define KEYBOARD_LAT LATD
#define KEYBOARD_COLS 0x0F
#define KEYBOARD_ROWS 0xF0

unsigned char getc(void);

#endif
