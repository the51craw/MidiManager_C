#ifndef EEPROM_H_DEFINED
#define EEPROM_H_DEFINED

/*
This is the EEPROM low-level driver declaration file
*/

void eeprom_write_byte(unsigned char address, unsigned char data);
unsigned char eeprom_read_byte(unsigned char address);

#endif
