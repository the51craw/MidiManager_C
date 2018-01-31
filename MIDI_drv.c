// Description starts on page 237.
// Baud rate calculation on page 242.
// All registers are listed on page 242.
// Reset values arelisted on page 53.

// Baud rate calculation:
// 31250 = FOSC/(64 ([SPBRGH:SPBRG] + 1))
// FOSC/31250 = (64 ([SPBRGH:SPBRG] + 1))
// (FOSC/31250) / 64 = [SPBRGH:SPBRG] + 1
// [SPBRGH:SPBRG] = ((FOSC/31250) / 64) - 1)
// [SPBRGH:SPBRG] = ((20000000/31250) / 64) - 1)
// [SPBRGH:SPBRG] = 0:9
  
// REGISTER 20-1: TXSTA: TRANSMIT STATUS AND CONTROL REGISTER
// R/W-0 R/W-0 R/W-0 R/W-0 R/W-0 R/W-0 R-1 R/W-0
// CSRC TX9 TXEN(1) SYNC SENDB BRGH TRMT TX9D
// bit 7 bit 0
// Legend:
// R = Readable bit W = Writable bit U = Unimplemented bit, read as ‘0’
// -n = Value at POR ‘1’ = Bit is set ‘0’ = Bit is cleared x = Bit is unknown
// bit 7 CSRC: Clock Source Select bit
// Asynchronous mode:
// Don’t care.
// Synchronous mode:
// 1 = Master mode (clock generated internally from BRG)
// 0 = Slave mode (clock from external source)
// bit 6 TX9: 9-Bit Transmit Enable bit
// 1 = Selects 9-bit transmission
// 0 = Selects 8-bit transmission
// bit 5 TXEN: Transmit Enable bit(1)
// 1 = Transmit enabled
// 0 = Transmit disabled
// bit 4 SYNC: EUSART Mode Select bit
// 1 = Synchronous mode
// 0 = Asynchronous mode
// bit 3 SENDB: Send Break Character bit
// Asynchronous mode:
// 1 = Send Sync Break on next transmission (cleared by hardware upon completion)
// 0 = Sync Break transmission completed
// Synchronous mode:
// Don’t care.
// bit 2 BRGH: High Baud Rate Select bit
// Asynchronous mode:
// 1 = High speed
// 0 = Low speed
// Synchronous mode:
// Unused in this mode.
// bit 1 TRMT: Transmit Shift Register Status bit
// 1 = TSR empty
// 0 = TSR full
// bit 0 TX9D: 9th bit of Transmit Data
// Can be address/data bit or a parity bit.
// Note 1: SREN/CREN overrides TXEN in Sync mode with the exception that SREN has no effect in Synchronous
// Slave mode.
// 
// REGISTER 20-2: RCSTA: RECEIVE STATUS AND CONTROL REGISTER
// R/W-0 R/W-0 R/W-0 R/W-0 R/W-0 R-0 R-0 R-x
// SPEN RX9 SREN CREN ADDEN FERR OERR RX9D
// bit 7 bit 0
// Legend:
// R = Readable bit W = Writable bit U = Unimplemented bit, read as ‘0’
// -n = Value at POR ‘1’ = Bit is set ‘0’ = Bit is cleared x = Bit is unknown
// bit 7 SPEN: Serial Port Enable bit
// 1 = Serial port enabled (configures RX/DT and TX/CK pins as serial port pins)
// 0 = Serial port disabled (held in Reset)
// bit 6 RX9: 9-Bit Receive Enable bit
// 1 = Selects 9-bit reception
// 0 = Selects 8-bit reception
// bit 5 SREN: Single Receive Enable bit
// Asynchronous mode:
// Don’t care.
// Synchronous mode – Master:
// 1 = Enables single receive
// 0 = Disables single receive
// This bit is cleared after reception is complete.
// Synchronous mode – Slave:
// Don’t care.
// bit 4 CREN: Continuous Receive Enable bit
// Asynchronous mode:
// 1 = Enables receiver
// 0 = Disables receiver
// Synchronous mode:
// 1 = Enables continuous receive until enable bit CREN is cleared (CREN overrides SREN)
// 0 = Disables continuous receive
// bit 3 ADDEN: Address Detect Enable bit
// Asynchronous mode 9-bit (RX9 = 1):
// 1 = Enables address detection, enables interrupt and loads the receive buffer when RSR<8> is set
// 0 = Disables address detection, all bytes are received and ninth bit can be used as parity bit
// Asynchronous mode 9-bit (RX9 = 0):
// Don’t care.
// bit 2 FERR: Framing Error bit
// 1 = Framing error (can be updated by reading RCREG register and receiving next valid byte)
// 0 = No framing error
// bit 1 OERR: Overrun Error bit
// 1 = Overrun error (can be cleared by clearing bit CREN)
// 0 = No overrun error
// bit 0 RX9D: 9th bit of Received Data
// This can be address/data bit or a parity bit and must be calculated by user firmware.
// 
// REGISTER 20-3: BAUDCON: BAUD RATE CONTROL REGISTER
// R/W-0 R-1 R/W-0 R/W-0 R/W-0 U-0 R/W-0 R/W-0
// ABDOVF RCIDL RXDTP TXCKP BRG16 — WUE ABDEN
// bit 7 bit 0
// Legend:
// R = Readable bit W = Writable bit U = Unimplemented bit, read as ‘0’
// -n = Value at POR ‘1’ = Bit is set ‘0’ = Bit is cleared x = Bit is unknown
// bit 7 ABDOVF: Auto-Baud Acquisition Rollover Status bit
// 1 = A BRG rollover has occurred during Auto-Baud Rate Detect mode (must be cleared in software)
// 0 = No BRG rollover has occurred
// bit 6 RCIDL: Receive Operation Idle Status bit
// 1 = Receive operation is Idle
// 0 = Receive operation is active
// bit 5 RXDTP: Received Data Polarity Select bit
// Asynchronous mode:
// 1 = RX data is inverted
// 0 = RX data received is not inverted
// Synchronous modes:
// 1 = CK clocks are inverted
// 0 = CK clocks are not inverted
// bit 4 TXCKP: Clock and Data Polarity Select bit
// Asynchronous mode:
// 1 = TX data is inverted
// 0 = TX data is not inverted
// Synchronous modes:
// 1 = CK clocks are inverted
// 0 = CK clocks are not inverted
// bit 3 BRG16: 16-Bit Baud Rate Register Enable bit
// 1 = 16-bit Baud Rate Generator – SPBRGH and SPBRG
// 0 = 8-bit Baud Rate Generator – SPBRG only (Compatible mode), SPBRGH value ignored
// bit 2 Unimplemented: Read as ‘0’
// bit 1 WUE: Wake-up Enable bit
// Asynchronous mode:
// 1 = EUSART will continue to sample the RX pin – interrupt generated on falling edge; bit cleared in
// hardware on following rising edge
// 0 = RX pin not monitored or rising edge detected
// Synchronous mode:
// Unused in this mode.
// bit 0 ABDEN: Auto-Baud Detect Enable bit
// Asynchronous mode:
// 1 = Enable baud rate measurement on the next character. Requires reception of a Sync field (55h);
// cleared in hardware upon completion.
// 0 = Baud rate measurement disabled or completed
// Synchronous mode:
// Unused in this mode.

// SPBRGH = 0x00;
// SPBRG = 0x09;
// 
// TXSTA = 0x20;
// 0010 0000
// 
// RCSTA = 0x80;
// 1000 0000
// 
// BAUDCON = 0x00;
// 0000 0000
// 
// TXREG
// data data
// 
// while(PIR1 & 0x10);
// 
// TRMT (TXSTA<1>), shows the status of
// the TSR register. TRMT is a read-only bit which is set
// when the TSR register is empty.
// 
// To set up an Asynchronous Transmission:
// 1. Initialize the SPBRGH:SPBRG registers for the
// appropriate baud rate. 
// [SPBRGH:SPBRG] = 0:9
// 
// Set or clear the BRGH
// and BRG16 bits, as required, to achieve the
// desired baud rate.
// TXSTA<2> = 0 and BAUDCON<3> = 0
// 
// 2. Enable the asynchronous serial port by clearing
// bit SYNC and setting bit SPEN.
// TXSTA<4> = 0 and RCSTA<7> = 1
// 
// 3. If the signal from the TX pin is to be inverted, set
// the TXCKP bit.
// BAUDCON<4> = 0
// 
// 4. If interrupts are desired, set enable bit TXIE.
// 
// 5. If 9-bit transmission is desired, set transmit bit
// TX9. Can be used as address/data bit.
// 
// 6. Enable the transmission by setting bit TXEN
// which will also set bit TXIF.
// TXSTA<5> = 1 then poll PIR1<4>
// 
// 
// 7. If 9-bit transmission is selected, the ninth bit
// should be loaded in bit TX9D.
// 
// 8. Load data to the TXREG register (starts
// transmission).
// 
// 9. If using interrupts, ensure that the GIE and PEIE
// bits in the INTCON register (INTCON<7:6>) are
// set.
/*
 -------------------------------------------------------------------------------------
| TABLE 20-6: REGISTERS ASSOCIATED WITH ASYNCHRONOUS RECEPTION                        |
|-------------------------------------------------------------------------------------|
| Name    | Bit 7    | Bit 6     | Bit 5  | Bit 4  | Bit 3 | Bit 2  | Bit 1  | Bit 0  |
| INTCON  | GIE/GIEH | PEIE/GIEL |(TMR0IE)|(INT0IE)|(RBIE) |(TMR0IF)|(INT0IF)|(RBIF)  |
| PIR1    |(SPPIF(1))|(ADIF)     | RCIF   |(TXIF)  |(SSPIF)|(CCP1IF)|(TMR2IF)|(TMR1IF)|
| PIE1    |(SPPIE(1))|(ADIE)     | RCIE   |(TXIE)  |(SSPIE)|(CCP1IE)|(TMR2IE)|(TMR1IE)|
| IPR1    |(SPPIP(1))|(ADIP)     | RCIP   |(TXIP)  |(SSPIP)|(CCP1IP)|(TMR2IP)|(TMR1IP)|
| RCSTA   | SPEN     | RX9       | SREN   | CREN   | ADDEN | FERR   | OERR   | RX9D   |
|-------------------------------------------------------------------------------------|
| RCREG   | EUSART Receive Register 53                                                |
|-------------------------------------------------------------------------------------|
| TXSTA   |(CSRC)    |(TX9)      |(TXEN)  | SYNC   |(SENDB)| BRGH   |(TRMT)  |(TX9D)  |
| BAUDCON | ABDOVF   | RCIDL     | RXDTP  |(TXCKP) | BRG16 |  (—)   | WUE    | ABDEN  |
|-------------------------------------------------------------------------------------|
| SPBRGH  | EUSART Baud Rate Generator Register High Byte                             |
| SPBRG   | EUSART Baud Rate Generator Register Low Byte                              |
 -------------------------------------------------------------------------------------
INTCON needs to be 1xxx xxxx, set it to 0x80
PIR1 RCIF denotes input available
PIE needs to be xx1x xxxx (reset is 0000 0000 so set it to 0x20)
IPR1 is not in use, we do not activate interrupt priority.
*/
#include <p18f4550.h>
#include "main.h"
#include "MIDI_drv.h"

//unsigned char in_buff[0x10];
#pragma udata buffer_section
static unsigned char out_buff[0x5ff];
#pragma udata
//unsigned char ib_put_pos;
//unsigned char ib_get_pos;
unsigned char * ob_put_pos;
unsigned char * ob_get_pos;
//unsigned char ob_put_pos;
//unsigned char ob_get_pos;
//unsigned char byte_cnt;

void MIDI_Send(unsigned char MIDI_data);
void MIDI_KeyOn(unsigned char channel, unsigned char key_number, unsigned char velocity);
void MIDI_KeyOff(unsigned char channel, unsigned char key_number, unsigned char velocity);

void MIDI_Init(void)
{
	TRISC |= 0xc0;
	SPBRGH = 0x00;
#ifdef CLOCK_FREQUENCY_20_MHZ
	SPBRG = 0x09;
#endif
#ifdef CLOCK_FREQUENCY_48_MHZ
	SPBRG = 0x17;
#endif

	TXSTA = 0x20;
	RCSTA = 0x90;
	BAUDCON = 0x00;
	PIE1 = 0x00;
	PIR1 = 0x00;
	RCON = 0x80;
	ob_put_pos = &out_buff[0];
	ob_get_pos = &out_buff[0];
}

// Reset all sounds, controllers and buffers:
void MIDI_Reset_All(void)
{
	unsigned char sound;
	unsigned char channel;
	ob_put_pos = & out_buff[0];
	ob_get_pos = & out_buff[0];
	
	for(channel = 0; channel < (unsigned char)16; channel++)
	{
		while((PIR1 & 0x10) != (unsigned int)0x10);
		TXREG = 0xb0 | channel;
		while((PIR1 & 0x10) != (unsigned int)0x10);
		TXREG = 0x78;
		while((PIR1 & 0x10) != (unsigned int)0x10);
		TXREG = 0x00;
		while((PIR1 & 0x10) != (unsigned int)0x10);
		TXREG = 0xb0 | channel;
		while((PIR1 & 0x10) != (unsigned int)0x10);
		TXREG = 0x79;
		while((PIR1 & 0x10) != (unsigned int)0x10);
		TXREG = 0x00;
	}	
}
	
// Put a key-on event on the output buffer.
void MIDI_KeyOn(unsigned char channel, unsigned char key_number, unsigned char velocity)
{
	MIDI_Send(0x90 | (channel & 0x0f));
	MIDI_Send(key_number & 0x7f);
	MIDI_Send(velocity & 0x7f);
}

// Put a key-off event on the output buffer.
void MIDI_KeyOff(unsigned char channel, unsigned char key_number, unsigned char velocity)
{
	MIDI_Send(0x80 | (channel & 0x0f));
	MIDI_Send(key_number & 0x7f);
	MIDI_Send(velocity & 0x7f);
}

// Call to send one byte of data (only adds to output buffer).
void MIDI_Send(unsigned char MIDI_data)
{
	// Don't send here, just buffer up:
	// buf_ptr[5] = 10;
	ob_put_pos[0] = MIDI_data;
	ob_put_pos++;
	if (ob_put_pos >= &out_buff[0] + 0x600)
	{
	    ob_put_pos = &out_buff[0];
	}
}

// Call this in main each iteration to send one byte from buffer if available and Tx ready.
void MIDI_SendOneByte(void)
{
    // Send if buffer is not empty:
	if(ob_get_pos != ob_put_pos)
	{
		// Send if Tx is ready:
		if(PIR1 & 0x10)
		{
			TXREG = ob_get_pos[0];
			ob_get_pos++;
        	if (ob_get_pos >= &out_buff[0] + 0x600)
        	{
        	    ob_get_pos = &out_buff[0];
        	}
		}
	}
}

