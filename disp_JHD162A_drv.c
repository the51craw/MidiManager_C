#include "p18f4550.h"
#include <string.h>
#include "disp_JHD162A_drv.h"

// #pragma warning(disable:2058)

////////////////////////////////////////////////////////////////
// Driver internals
////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Green display:
// |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
// | LED-| LED+| VSS | VCC | VEE | RS  | R/W | E   | DB7 | DB6 | DB5 | DB4 | DB3 | DB2 | DB1 | DB0 | <-- Is this correct order of DB0 - DB7???
// |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
// |  *  |  *  | 1   | 2   | 3   | 4   | 5   | 6   | 7   | 8   | 9   | 10  | 11  | 12  | 13  | 14  |
// |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
// |     |     |     |     |     | RE0 | RE1 | RE2 | VSS | VSS | VDD | VSS | RA3 | RA2 | RA1 | RA0 |
// |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
//
// Blue display:
// |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
// | VSS | VCC | VEE | RS  | R/W | E   | DB0 | DB1 | DB2 | DB3 | DB4 | DB5 | DB6 | DB7 | LED+| LED-|
// |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
// | 1   | 2   | 3   | 4   | 5   | 6   | 7   | 8   | 9   | 10  | 11  | 12  | 13  | 14  | 15  | 16  |
// |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
// |     |     |     | RE0 | RE1 | RE2 | VSS | VSS | VDD | VSS | RA3 | RA2 | RA1 | RA0 |     |     |
// |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
//
// * = not numbered on the LCD I use in this project.
//
// LED-/+ will light the display while VEE is for contrast, use a trimpot.
// VSS +5V
// VCC Gnd
// RS Register select
// R/W Read or write 5V = read, 0volt = write
// E Enable, activity on negative flank
//
// Function set. We use 4-bit data transfer, 2-line display 5*8 dots display.
// Send: RS = 0, R/W = 0, DB7= 0, DB6 = 0, DB5 = 1, DB4 = 0, DB3 = 1, DB2 = 0, DB1 = 0, DB0 = 0
// Thus DB7 thru 4 needs to be hardwired to 0, 0, 1, 0.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

// Display control port and pins.
// Port is PORTC and pins 0-2 are in use.
#define DISPLAY_CONTROL_PORT PORTA
#define DISPLAY_CONTROL_TRIS TRISA
#define DISPLAY_CONTROL_LAT  LATA
#define DISPLAY_RS_PIN 0x40
#define DISPLAY_RW_PIN 0x10
#define DISPLAY_E_PIN  0x20
#define DISPLAY_CONTROL_PINS (DISPLAY_RS_PIN | DISPLAY_RW_PIN | DISPLAY_E_PIN)

// Display data/command port is PORTA, low nibble
#define DISPLAY_DATA_PORT PORTA
#define DISPLAY_DATA_TRIS TRISA
#define DISPLAY_DATA_LAT LATA
#define DISPLAY_DATA_PINS 0x0f

// R/W Read/Write      0 = write, 1 = read
// RS  Register Select 0 = control(command) register, 1 = data register
// E   Enable          0 = disable, 1 = enable (actually, clocks in data/command on negative flank)
// Pin manipulation macros:
#define SET_RS_PIN                 DISPLAY_CONTROL_LAT  |= DISPLAY_RS_PIN
#define RESET_RS_PIN               DISPLAY_CONTROL_LAT  &= ~DISPLAY_RS_PIN
#define SET_RW_PIN                 DISPLAY_CONTROL_LAT |= DISPLAY_RW_PIN
#define RESET_RW_PIN               DISPLAY_CONTROL_LAT &= ~DISPLAY_RW_PIN
#define SET_E_PIN                  DISPLAY_CONTROL_LAT |= DISPLAY_E_PIN
#define RESET_E_PIN                DISPLAY_CONTROL_LAT &= ~DISPLAY_E_PIN

// Command definitions for lcd_wrcmd(void)
#define CLEAR_DISPLAY 0x01
#define CURSOR_HOME 0x02
#define INC_SHIFT_DISPLAY 0x07
#define DEC_SHIFT_DISPLAY 0x05
#define INC_DO_NOT_SHIFT_DISPLAY 0x06
#define DEC_DO_NOT_SHIFT_DISPLAY 0x04
#define ON_CURSOR_ON_BLINK_ON 0x0F
#define OFF_CURSOR_ON_BLINK_ON 0x0B
#define ON_CURSOR_OFF_BLINK_ON 0x0D
#define OFF_CURSOR_OFF_BLINK_ON 0x09
#define ON_CURSOR_ON_BLINK_OFF 0x0E
#define OFF_CURSOR_ON_BLINK_OFF 0x0A
#define ON_CURSOR_OFF_BLINK_OFF 0x0C
#define OFF_CURSOR_OFF_BLINK_OFF 0x08
#define CURSOR_SHIFT_LEFT 0x18
#define CURSOR_SHIFT_RIGHT 0x1C
#define CURSOR_NO_SHIFT 0x10
#define SYSTEM_SET8BITS_2LIN_5X10 0x3C
#define SYSTEM_SET8BITS_2LIN_5X7 0x38
#define SYSTEM_SET8BITS_1LIN_5X10 0x34
#define SYSTEM_SET8BITS_1LIN_5X7 0x30
#define SYSTEM_SET4BITS_2LIN_5X10 0x2C
#define SYSTEM_SET4BITS_2LIN_5X7 0x28
#define SYSTEM_SET4BITS_1LIN_5X10 0x24
#define SYSTEM_SET4BITS_1LIN_5X7 0x20
#define SET_CG_RAM_ADDRESS 0x40 //Or on address as last 6 bits
#define SET_DD_RAM_ADDRESS 0x80 //Or on address as last 7 bits

// Internal function prototypes
void soft_nop(void);
void lcd_wrcmd(unsigned char lcdcmd);
void lcd_wrdta(unsigned char lcddta);
void lcd_init(void);
unsigned char lcd_checkbusy(void);
void print_line_rom(const rom far char * p);
void print_line_ram(char * p);
void my_atoi(char * dest, int length, int value);
void my_atox(char * dest, unsigned char value);
void my_strcpy(char * dest, char * source);
void pause(unsigned char p);

char digits[32];
char four_bit_protocol;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// Exported functions
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

void PrintLine1(const rom far char * p)
{
	pause(1);
    lcd_wrcmd(SET_DD_RAM_ADDRESS + 0x00);
	print_line_rom(p);
}

void PrintLine2(const rom far char * p)
{
	pause(1);
	lcd_wrcmd(SET_DD_RAM_ADDRESS + 0x40);
	print_line_rom(p);
}

void PrintLine1Ram(far char * p)
{
	pause(1);
	lcd_wrcmd(SET_DD_RAM_ADDRESS + 0x00);
	p[16] = 0;
	print_line_ram(p);
}

void PrintLine2Ram(far char * p)
{
	pause(1);
	lcd_wrcmd(SET_DD_RAM_ADDRESS + 0x40);
	p[16] = 0;
	print_line_ram(p);
}

void PrintNumeric(int value, int length, int line, int pos)
{
	pause(1);
	my_atoi(digits, length, value);
	if (line == 2)
	{
		pos += 0x40;
	}
	lcd_wrcmd(SET_DD_RAM_ADDRESS + pos);
	print_line_ram(digits);
}

void PrintHex(unsigned char value, int line, int pos)
{
	pause(1);
	my_atox(digits, value);
	if (line == 2)
	{
		pos += 0x40;
	}
	lcd_wrcmd(SET_DD_RAM_ADDRESS + pos);
	print_line_ram(digits);
}

////////////////////////////////////////////////////////////////
// Clear display:
////////////////////////////////////////////////////////////////

void Clr(void)
{
	lcd_wrcmd(CLEAR_DISPLAY);
	lcd_wrcmd(CURSOR_HOME);
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// Internal driver functions:
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// Atoi replacement
////////////////////////////////////////////////////////////////

int power(int i)
{
	int result = 1;

	while (--i)
	{
		result *= 10;
	}

	return result;
}

void my_atoi(char * dest, int length, int value)
{
	int v = value;
	int temp;
	int l = length;
	int pos = 0;

	if (l < 0)
	{
		l = -l;
		if (v < 0)
		{
			dest[pos++] = '-';
			v = -v;
		}
		else
		{
			dest[pos++] = ' ';
		}
	}

	while (l)
	{
		temp = v / power(l);
		dest[pos++] = 0x30 + temp;
		v = v - temp * power(l);
		l--;
	}
	dest[pos] = 0;
}

void my_atox(char * dest, unsigned char value)
{
	unsigned char v;
    
    v = 0x30 + ((value & 0xf0) >> 4);
    if (v > '9') v += ('A' - '9' - 1);
    dest[0] = v;
    v = 0x30 + (value & 0x0f);
    if (v > '9') v += ('A' - '9' - 1);
    dest[1] = v;
	dest[2] = 0;
}

void my_strcpy(char * dest, char * source)
{
	int i = 0;
	while (i < 16 && ((dest[i] = source[i]) != 0)) i++;
}

void print_line_ram(char * p)
{
	int i = 16;
	
	pause(1);

	while (i-- && *p)
	{
		lcd_wrdta(*p++);
	}
}

void print_line_rom(const rom far char * p)
{
	int i = 16;
	
	pause(1);

	while (i-- && *p)
	{
		lcd_wrdta(*p++);
	}
}

////////////////////////////////////////////////////////////////
// Pauses and wait while busy
////////////////////////////////////////////////////////////////

void pause(unsigned char p)
{
	unsigned char i;
	unsigned char j;

	for(i = 0; i < p; i++)
	{
		for(j = 0; j < 0xFE; j++)
		{
			j = j;
		}
	}
}

void delay_64_us(void)
{
    soft_nop();
}

void delay_10_ms(void)
{
	unsigned char i;
	for (i = 0; i < (unsigned int)200; i++)
	{
		soft_nop();
	}
}

void soft_nop(void)
{
	unsigned char i;
	for(i = 0; i < 0x80; i++);
}

unsigned char lcd_checkbusy(void) // Check busy on bit 7
{
	unsigned char lcdstate;
	DISPLAY_DATA_TRIS |= DISPLAY_DATA_PINS;			// Set data port to input
	DISPLAY_CONTROL_TRIS &= ~DISPLAY_CONTROL_PINS;	// Set command port to output
	RESET_E_PIN;									// Should always be low except when pulsing in data/command
	RESET_RS_PIN;            						// Set command mode
	SET_RW_PIN;
	soft_nop();
	SET_E_PIN;
	soft_nop();
	lcdstate = DISPLAY_DATA_LAT;      				// Read status bit
	soft_nop();
	RESET_E_PIN;
	soft_nop();
	return(lcdstate & 0x80);
}

////////////////////////////////////////////////////////////////
// Read and write commands
////////////////////////////////////////////////////////////////

void lcd_wrcmd(unsigned char lcdcmd) // Write command in lcdcmd
{
    //while(lcd_checkbusy());
	DISPLAY_DATA_TRIS &= ~DISPLAY_DATA_PINS;		// Set data/command port to output
	DISPLAY_CONTROL_TRIS &= ~DISPLAY_CONTROL_PINS;
	RESET_E_PIN;									// Should always be low except when pulsing in data/command
	RESET_RS_PIN;									// Set command mode
	RESET_RW_PIN;									// Set write mode
	if(four_bit_protocol != 0x00)
	{
		soft_nop();
		SET_E_PIN;									// Enable chip
		soft_nop();
		DISPLAY_DATA_PORT &= ~DISPLAY_DATA_PINS;    // Clear the data part of the port (in case of 4-bit transfer in order not to touch the other ones)
		DISPLAY_DATA_PORT |= (lcdcmd & 0xf0) >> 4;	// Put data on port
		soft_nop();
		RESET_E_PIN;								// Disable chip (latching in data)
		soft_nop();
		SET_E_PIN;								    // Enable chip
		soft_nop();
		DISPLAY_DATA_PORT &= ~DISPLAY_DATA_PINS;    // Clear the data part of the port (in case of 4-bit transfer in order not to touch the other ones)
		DISPLAY_DATA_PORT |= lcdcmd & 0x0f;	        // Put data on port
		soft_nop();
		SET_E_PIN;									// Enable chip
		soft_nop();
		RESET_E_PIN;								// Disable chip (latching in data)
	}
	else
	{
		soft_nop();
		SET_E_PIN;									// Enable chip
		soft_nop();
		DISPLAY_DATA_PORT &= ~DISPLAY_DATA_PINS;    // Clear the data part of the port (in case of 4-bit transfer in order not to touch the other ones)
		DISPLAY_DATA_PORT |= lcdcmd;				// Put data on port
		soft_nop();
		RESET_E_PIN;								// Disable chip (latching in data)
	}
	soft_nop();
} 

void lcd_wrdta(unsigned char lcddta)				// Write data in lcddta
{
	//while(lcd_checkbusy());
	DISPLAY_DATA_TRIS &= ~DISPLAY_DATA_PINS;		// Set data/command port to output
	DISPLAY_CONTROL_TRIS &= ~DISPLAY_CONTROL_PINS;
	RESET_E_PIN;									// Should always be low except when pulsing in data/command
	SET_RS_PIN;										// Set data mode
	RESET_RW_PIN;									// Set write mode
	if(four_bit_protocol != 0x00)
	{
		soft_nop();
		SET_E_PIN;									// Enable chip
		soft_nop();
		DISPLAY_DATA_PORT &= ~DISPLAY_DATA_PINS;    // Clear the data part of the port (in case of 4-bit transfer in order not to touch the other ones)
		DISPLAY_DATA_PORT |= (lcddta & 0xf0) >> 4;	// Put data on port
		soft_nop();
		RESET_E_PIN;								// Enable chip
		soft_nop();
		SET_E_PIN;								    // Disable chip (latching in data)
		soft_nop();
		DISPLAY_DATA_PORT &= ~DISPLAY_DATA_PINS;    // Clear the data part of the port (in case of 4-bit transfer in order not to touch the other ones)
		DISPLAY_DATA_PORT |= lcddta & 0x0f;	        // Put data on port
		soft_nop();
		SET_E_PIN;									// Enable chip
		soft_nop();
		RESET_E_PIN;								// Disable chip (latching in data)
	}
	else
	{
		soft_nop();
		SET_E_PIN;									// Enable chip
		soft_nop();
		DISPLAY_DATA_PORT &= ~DISPLAY_DATA_PINS;    // Clear the data part of the port (in case of 4-bit transfer in order not to touch the other ones)
		DISPLAY_DATA_PORT |= lcddta;					// Put data on port
		soft_nop();
		RESET_E_PIN;								// Disable chip (latching in data)
	}
	soft_nop();
} 

////////////////////////////////////////////////////////////////
// Initialization
////////////////////////////////////////////////////////////////

void lcd_init(void)
{
    int i;
    four_bit_protocol = 0;                  // This will change after init if DISPLAY_DATA_PINS = 0x0f
    
    // The following is specific when PORTA is used for IO:
	SPPCON = 0x00;                          // Disable Streaming Parallel Port
	CCP1CON = 0x00;                         // Disable Capture/Compare/PWM (ECCP) module
	ADCON1 |= 0x0f;                         // Disable analog inputs to make PORTA digital
	CMCON = 0x07;                           // Disable comparators inputs to make PORTA digital
	
	DISPLAY_CONTROL_TRIS &= ~DISPLAY_CONTROL_PINS;
	if(DISPLAY_DATA_PINS == 0x0f)
	{
        delay_10_ms();
        delay_10_ms();
        delay_10_ms();
        lcd_wrcmd(0x03);
        delay_10_ms();
        lcd_wrcmd(0x03);
        delay_10_ms();
        lcd_wrcmd(0x03);
        delay_10_ms();
        lcd_wrcmd(0x02);
        delay_10_ms();
        four_bit_protocol = 0x01;
		lcd_wrcmd(SYSTEM_SET4BITS_2LIN_5X7); // Set interface data length 4 bits (bit5=0), number of display lines (bit4=1), font type (bit3=1) 5*11/5*8
	}
	else
	{
		lcd_wrcmd(SYSTEM_SET8BITS_2LIN_5X7); // Set interface data length 8 bits (bit5=1), number of display lines (bit4=1), font type (bit3=0) 5*11/5*8
	}
	lcd_wrcmd(ON_CURSOR_OFF_BLINK_OFF); // Set Display (bit3=1), Display on (bit2=1), Cursor off (bit1=0), Blinking off (bit0=0)
	lcd_wrcmd(INC_DO_NOT_SHIFT_DISPLAY); // Set mode (bit2=1), Increment on (bit1=1), Shift off (bit0=0)
	lcd_wrcmd(CURSOR_NO_SHIFT);
	lcd_wrcmd(CLEAR_DISPLAY); // Clear display (bit0=1)
	lcd_wrcmd(CURSOR_HOME);
	//while(1);
}

