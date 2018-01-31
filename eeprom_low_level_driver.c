/*
This is the EEPROM low-level driver header file
*/
#include "eeprom_low_level_driver.h"
#include "p18f4550.h"
#include "disp_JHD162A_drv.h"

void eeprom_write_byte(unsigned char address, unsigned char data)
{
	EEADR = address;		// Set EEPROM address
	EEDATA = data;			// Set EEPROM data
	EECON1 &= ~0x40;		// Access EEPROM/RAM (not config) memory
	EECON1 &= ~0x80;		// Point to data (not RAM) memory
	EECON1 |= 0x04;			// Enable writing to EEPROM
	INTCON &= ~0x80;		// Disable interrupts
	EECON2 = 0x55;			// Specifically required hocus-pocus
	EECON2 = 0xAA;			// Ditto
	EECON1 |= 0x02;			// Do the write
	while (EECON1 & 0x02);	// Wait for write to complete
	delay_10_ms();
	INTCON |= 0x80;			// Enable interrupts
	EECON1 &= ~0x04;		// Disable EEPROM writes
}

unsigned char eeprom_read_byte(unsigned char address)
{
	EEADR = address;		// Set EEPROM address
	EECON1 &= ~0x40;		// Access EEPROM/RAM (not config) memory
	EECON1 &= ~0x80;		// Point to data (not RAM) memory
	EECON1 |= 0x01;			// Initiate read
	return EEDATA;			// Get the result
}
