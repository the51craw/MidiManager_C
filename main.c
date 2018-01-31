// Uncomment line below to have velocity displayed for test:
//#define DISPLAY_VELOCITY

//////////////////////////////////////////////////////////////////////////////////////////////////////
// MIDI_mapper
//////////////////////////////////////////////////////////////////////////////////////////////////////
//                       _______________
//             MCLR --->|      \_/      |<--- RB7 D7
//       DispD0 RA0 <---|               |<--- RB6 D6
//       DispD1 RA1 <---|               |<--- RB5 D5
//   DispD2/PC0 RA2 <---|               |<--- RB4 D4
//   DispD3/PC1 RA3 <---|               |<--- RB3 D3
//          R/W RA4 <---|               |<--- RB2 D2
//            E RA5 <---|               |<--- RB1 D1
//        Synt+ RE0 <---|               |<--- RB0 D0
//        Synt- RE1 <---|               |<--- VDD
//     Channel+ RE2 <---|               |<--- VSS 
//              VDD --->|               |---> RD7 CS 1
//              VSS --->|               |---> RD6 CS 2
//         Clock in --->|               |<--- RD5 Data ready 4
//           RS RA6 <---|               |<--- RD4 Data ready 3
//     Channel- RC0 --->|               |<--- RC7 MIDI In
//   Transpose+ RC1 --->|               |---> RC6 MIDI Out
//   Transpose- RC2 --->|               |<--- RC5 Data ready 1
//       (N/C) Vusb --->|               |<--- RC4 Data ready 2
//  MIDI in LED RD0 <---|               |---> RD3 CS 3
//  Direct/UR44 RD1 --->|_______________|---> RD2 CS 4
//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
// | LED-| LED+| VSS | VCC | VEE | RS  | R/W | E   | DB7 | DB6 | DB5 | DB4 | DB3 | DB2 | DB1 | DB0 |
// |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
// |  *  |  *  | 1   | 2   | 3   | 4   | 5   | 6   | 7   | 8   | 9   | 10  | 11  | 12  | 13  | 14  |
// |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
// |     |     |     |     |     | RE0 | RE1 | RE2 | VSS | VSS | VDD | VSS | RA3 | RA2 | RA1 | RA0 |
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
// NOTE! Running status can not be allowed to be transmitted, since a message that does not send
// its status byte may follow a message from another input, thus using wrong channel and status.
// Therefore, always add the status byte when missing (i.e. sender uses running status).
//
// PC0/1 are high impedance pulled up/down from the Pach Change Wheel. They work like a 4-phase
// step up/down using e.g. two optical forks, like in a computer mouse using a ball (not the new
// optical ones). They are connected to the inputs via high resistors in orde not to interfere with
// dispolay operations (the outputs should have no problem forcing a 1 or a 0).
//
// Pach numbers are stored in EEPROM per channel.
//
// Now, when the EEPROM updates starts to be frequent, it is time to make them less frequent, skip-
// pin intermediate values, like when changing transpose from 0 to -12 shoul not store each and 
// every time, and especially the pach changes that may be scrolled very fast.
//
// Use the timer function and a counter to get an interval like a few seconds, and then call a new
// function for EEPROM update. The function should only update values that differs from earlier, 
// which means that we have to store a copy in RAM for comparison.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include "p18f4550.h"
#include "config.h"
#include "MIDI_drv.h"
#include "disp_JHD162A_drv.h"
#include "keyboard.h"
#include "main.h"
#include "eeprom_low_level_driver.h"
#include "string.h"

// Each MIDI in message are stored here until complete:
struct _queues
{
    unsigned char data[0x03];
    unsigned char pos;
    unsigned char length;
    unsigned char running_status;
    unsigned char channel;
    char transpose;
};

// Temporary storage for Program Change message with transmit flag:
unsigned char pc[3]; // 0: flag -1 = transmit, 1: pc = 0xcn (n = channel),  2: program (0 - 127)

// RAM copy of EEPROM, used to only periodically update EEPROM
unsigned char eeprom[16];
unsigned char backupEeprom;
unsigned char wheel;
unsigned char synth;
unsigned char seconds;

struct _queues queues[5];

unsigned char data;

void nop(void);
void TMR_Interrupt(void);
void check_subprocessor(unsigned char data_ready, unsigned char chip_select, unsigned char queue);
void readEeProm(void);
void menu(void);
void CheckProgramChangeWheel(void);
void ProgramUp(void);
void ProgramDown(void);
void SetProgram(void);
void displaySynth(unsigned char synth);
void welcome(void);
void killNotes(int queue);
void BackupToEeprom(void);

#pragma code lo_vector = 0x18
void interrupt_at_lo_vector(void)
{
  _asm GOTO TMR_Interrupt _endasm
}
#pragma code

#pragma interruptlow TMR_Interrupt
void TMR_Interrupt(void)
{
    unsigned char i;
    static unsigned int count = 0;
    
	INTCON = 0x00;
	TRISC = 0xff;
	// Anything on sub processors?
    if (PORTC & 0x20) check_subprocessor(0x20, 0x80, 1);
    if (PORTC & 0x10) check_subprocessor(0x10, 0x40, 2);
    if (PORTD & 0x10) check_subprocessor(0x10, 0x08, 3);
    if (PORTD & 0x20) check_subprocessor(0x20, 0x04, 4);
    // Anything on main processor?
    if (PIR1 & 0x20) // RCIF
	{
    	data = RCREG;
		if(RCSTA & 0x04) // FERR
		{
			// Framing error! Reset and restart MIDI message:
			RCSTA &= 0xef; // ~CREN
			RCSTA |= 0x10; // CREN
		}
		else if(RCSTA & 0x02) // OERR
		{
			// Overrun error! Reset and restart MIDI message:
			RCSTA &= 0xef;
			RCSTA |= 0x10;
		}
		else
		{
    		// All System Real Time Messages are one byte. Note that those can be sent anytime,
    		// even in the middle of longer messages, and must not impact on those.
    		// If this is a System Real Time Message, just pass it through and do no more:
    		if ((data & 0xf8) == 0xf8)
    		{
                //MIDI_Send(data);
            }
            else
            {
                MIDI_LED_ON;
                //PrintHex(data, 1, 5);
                if (queues[0].pos == 0)
                {
                    // We are expecting a start of message byte here. If not, assume running status.
                    if((data & 0x80) != 0x80)
                    {
                        // This is a data-byte in first byte. Therefore we assume running status is in struct.
                        // Restore status byte and advance pos:
            			queues[0].data[0] = queues[0].running_status;
            			queues[0].pos++;
           		    }
                    else
                    {
            			// This is a status, replace midi channel unless channel is 0xff and store running status
            			// (0xff is mainly for letting instrument channel(s) pass through as is, e.g. for drums
            			// that sends on 2 channels at the same time, 10 and 15):
                        if (queues[0].channel != 0xff)
                        {
                            data = (data & 0xf0) | queues[0].channel;
                        }
            			// Also, store running status:
                        queues[0].running_status = data;
        	            // And, program change and aftertouch are both 2 bytes in length, all other are 3 bytes:
        	            if ((data & 0xf0) == 0xc0 || (data & 0xf0) == 0xd0)
        	            {
        	                queues[0].length = 2;
        	            }
        	            else
        	            {
        	                queues[0].length = 3;
        	            }
                    }
                }
                // If this is the pitch data, transpose as given in queues[0].transpose:
                if (queues[0].pos == (queues[0].length - 2)                                                     // Is first data byte
                    && ((queues[0].running_status & 0xf0) == 0x90 || (queues[0].running_status & 0xf0) == 0x80) // and is note on or note off
                    && queues[0].transpose + (char)data < 128                                                   // and note can be transposed up
                    && queues[0].transpose + (char)data > -1)                                                   // or can be transposed down
                {
                    data = queues[0].transpose + data;
                }
                // Store the byte in the list:
                queues[0].data[queues[0].pos] = data;
                // If the message is complete, send all bytes (actually, ask MIDI_drv to put it in the output queue):
                if (queues[0].pos >= queues[0].length - 1)
                {
                    i = 0;
                    while (i < queues[0].length)
                    {
                        //PrintHex(queues[0].data[i], 2, i * 3);
                        MIDI_Send(queues[0].data[i++]);
                    }

                    // Also send program change if needed:
                    if (pc[0] == -1)
                    {
                        MIDI_Send(pc[1]);
                        MIDI_Send(pc[2]);
                        pc[0] = 0;
                    }
                    
                    queues[0].pos = 0;
                    MIDI_LED_OFF;
                }
                else
                {
                    queues[0].pos += 1;
                }
            }
        }
	}
	count++;
	if (count > 5000)
	{
    	count = 0;
    	seconds++;
    	if (seconds > 10)
    	{
        	backupEeprom = -1;
        	seconds = 0;
        }
    }
    	
	TMR0L = COUNTER_VALUE;
	INTCON = 0xa0;
}

void check_subprocessor(unsigned char data_ready, unsigned char chip_select, unsigned char queue)
{
    unsigned char i;
    // Incoming message, turn on LED:
    MIDI_LED_ON;
    // Ask and wait for data:
    LATD |= chip_select;
    if (queue < 2)
    {
        while (PORTC & data_ready);
    }
    else
    {
        while (PORTD & data_ready);
    }
    nop();
    // Fetch data:
    data = PORTB;
    nop();
    // Tell subprocessor that data has been fetched:
    LATD &= ~chip_select;
    /* System messages are all one byte from 0xf8 to 0xff. Those are discarded by the subprocessors.
     * Program change (0xCn) and Channel pressure (0xDn) are both 2 bytes long.
     * All other short messages are 3 bytes long, BUT:
     * When the same type of message is sent again, e.g. note on C, note on D,
     * the first byte may be skipped. That is called Running status.
     * Running status is detected as the first byte in a new message has a 0 in MSb.
     * We cannot handle other types of messages, like system exclusive.
     * If any longer messages are sent, they will pass through without merging, so do not 
     * use more than one sender when sending long messages! This does not work yet!
     * NOTE: we need to add a state for system exclusives since they can contain any data that
     * will confuse the logic here! */
    //PrintHex(data, 1, queues[queue].pos * 3);
    if (queues[queue].pos == 0)
    {
        // This is a first byte in the message, assume length will be 3:
        if((data & 0x80) != 0x80)
        {
            // This is a data-byte in first byte. Therefore we assume running status is in struct.
            // Restore status byte and advance pos:
            queues[queue].data[0] = queues[queue].running_status;
            queues[queue].pos++;
        }
        else
        {
			// This is a status, replace midi channel unless channel is 0xff and store running status
			// (0xff is mainly for letting instrument channel(s) pass through as is, e.g. for drums
			// that sends on 2 channels at the same time, 10 and 15):
            if (queues[queue].channel != 0xff)
            {
                data = (data & 0xf0) | queues[queue].channel;
            }
            // Also, store running status:
            queues[queue].running_status = data;
            // And, program change and aftertouch are both 2 bytes in length, all other is 3 bytes:
            if ((data & 0xf0) == 0xc0 || (data & 0xf0) == 0xd0)
            {
                queues[queue].length = 2;
            }
            else
            {
                queues[queue].length = 3;
            }
        }
    }
    // If this is the pitch data, transpose as given in queues[queue].transpose:
    if (queues[queue].pos == (queues[queue].length - 2)                                                     // Is first data byte
        && ((queues[queue].running_status & 0xf0) == 0x90 || (queues[queue].running_status & 0xf0) == 0x80) // and is note on or note off
        && queues[queue].transpose + (char)data < 128                                                       // and note can be transposed up
        && queues[queue].transpose + (char)data > -1)                                                       // or can be transposed down
    {
        data = queues[queue].transpose + data;
    }
    // Databyte is now ready to be stored in struct's data array:
    queues[queue].data[queues[queue].pos] = data;
    // If message is complete according to length:
    if (queues[queue].pos >= queues[queue].length - 1)
    {
        // Transfer to MIDI driver's output queue:
        i = 0;
        while (i < queues[queue].length)
        {
            //PrintHex(queues[queue].data[i], 2, i * 3);
            MIDI_Send(queues[queue].data[i++]);
        }
        
        // Also send program change if needed:
        if (pc[0] == -1)
        {
            MIDI_Send(pc[1]);
            MIDI_Send(pc[2]);
            pc[0] = 0;
        }
        
        // Reset pos and turn off LED:
        queues[queue].pos = 0;
        MIDI_LED_OFF;
    }
    else
    {
        queues[queue].pos += 1;
    }
}

void nop(void)
{
   	unsigned char i;
	for(i = 0; i < 0x80; i++);
}

void delay(int time)
{
    int i;
    int j;
    int k;
    for (i = 0; i < time; i++)
    {    
        for (k = 0; k < 20; k++)
        {
            MIDI_SendOneByte();
            for (j = 0; j < 100; j++);
        }
    }
}

void main(void)
{
    int i;
    
	// Initializations:
	DISABLE_USB;
	TRISD = 0x32;
	TRISB = 0xff;
	TRISC = 0xff;
	MIDI_Init();
	MIDI_Reset_All();
	lcd_init();
	welcome();
	MIDI_LED_OFF;
    synth = 0;
	backupEeprom = 0;
	seconds = 0;
	pc[0] = 0;
    TRISA |= 0x0c;
    delay(50);
    wheel = (PORTA & 0x0c);

    // Set up interrupts for TIMER0:
    RCON &= ~0x80;
	INTCON2 = 0x00;
	INTCON3 = 0x00;
	PIR1 = 0x00;
	PIR2 = 0x00;
	PIE1 = 0x00;
	IPR2 = 0x00;
	INTCON = 0x00;
	INTCON |= 0x20;
	INTCON |= 0x80;
	
	// Set up TIMER0:
	TMR0L = COUNTER_VALUE;
	T0CON = 0x00;
	T0CON |= 0x40;
	T0CON |= 0x04;
	T0CON |= 0x80;
	
	// Init buffer position pointers:
	for (i = 0; i < 5; i++)
	{
    	queues[i].pos = 0;
    	queues[i].running_status = 0;
    	queues[i].length = 3;
    	queues[i].transpose = 0;
    }
    
    // Standard channels, may be replaced if settings found in EEProm:
    queues[0].channel = 10; // DX7 
    queues[1].channel = 15; // JUNO-106
    queues[2].channel = 11; // WX7
    queues[3].channel = 14; // Drums
    queues[4].channel = 0;  // AUX
    
    readEeProm();
    delay(80);
    displaySynth(0);

    // Alert sub-controllers that we are ready to rock!
    CHIP_SELECT_ON_1;
    CHIP_SELECT_ON_2;
    CHIP_SELECT_ON_3;
    CHIP_SELECT_ON_4;
    delay(1);
    CHIP_SELECT_OFF_1;
    CHIP_SELECT_OFF_2;
    CHIP_SELECT_OFF_3;
    CHIP_SELECT_OFF_4;
    
	while(1)
	{
        MIDI_SendOneByte();
        menu();
        //CheckProgramChangeWheel();
        if (backupEeprom == -1)
        {
            backupEeprom = 0;
            BackupToEeprom();
            
            // Also, restore display that might show a program change message:
            displaySynth(synth);
        }
    }   
}

void menu(void)
{
    static unsigned char direct = 0x00;

    if (DIRECT != direct)
    {
        delay(100);
        if (DIRECT != direct)
        {
            displaySynth(synth);
            direct = DIRECT;
        }
    }
    
    if (SYNTH_PLUS)
    {
        delay_10_ms();
        if (SYNTH_PLUS)
        {
            if (synth < 4)
            {
                displaySynth(++synth);
            }
        }
        while (SYNTH_PLUS);
    }
    if (SYNTH_MINUS)
    {
        delay_10_ms();
        if (SYNTH_MINUS)
        {
            if (synth > 0)
            {
                displaySynth(--synth);
            }
        }
        while (SYNTH_MINUS);
    }
    if (CHANNEL_PLUS)
    {
        delay_10_ms();
        if (CHANNEL_PLUS)
        {
    	    if (queues[synth].channel < 0x0f)
    	    {
        	    killNotes(synth);
        	    queues[synth].channel++;
                //eeprom_write_byte(synth, queues[synth].channel);           
        	    displaySynth(synth);
        	}
    	    if (queues[synth].channel == 0xff)
    	    {
        	    killNotes(synth);
        	    queues[synth].channel = 0x00;
                //eeprom_write_byte(synth, queues[synth].channel);           
        	    displaySynth(synth);
        	}
        }
        while (CHANNEL_PLUS);
    }
    if (CHANNEL_MINUS)
    {
        delay_10_ms();
        if (CHANNEL_MINUS)
        {
    	    if (queues[synth].channel > 0x00 && queues[synth].channel != 0xff)
    	    {
        	    killNotes(synth);
        	    queues[synth].channel--;
                //eeprom_write_byte(synth, queues[synth].channel);           
        	    displaySynth(synth);
        	}
    	    else if (queues[synth].channel == 0x00)
    	    {
        	    killNotes(synth);
        	    queues[synth].channel = 0xff;
                //eeprom_write_byte(synth, queues[synth].channel);           
        	    displaySynth(synth);
        	}
        }
        while (CHANNEL_MINUS);
    }
    if (TRANSPOSE_PLUS)
    {
        delay_10_ms();
        if (TRANSPOSE_PLUS)
        {
    	    if (queues[synth].transpose < 24)
    	    {
        	    killNotes(synth);
        	    queues[synth].transpose++;
                //eeprom_write_byte(synth + 5, queues[synth].transpose);           
        	    displaySynth(synth);
        	}
        }
        while (TRANSPOSE_PLUS);
    }
    if (TRANSPOSE_MINUS)
    {
        delay_10_ms();
        if (TRANSPOSE_MINUS)
        {
    	    if (queues[synth].transpose > -24)
    	    {
        	    killNotes(synth);
        	    queues[synth].transpose--;
                //eeprom_write_byte(synth + 5, queues[synth].transpose);           
        	    displaySynth(synth);
        	}
        }
        while (TRANSPOSE_MINUS);
    }
}

void CheckProgramChangeWheel(void)
{
    unsigned char program;

    TRISA |= 0x0c;
    delay(1);
    program = (PORTA & 0x0c);
    if (program != wheel)
    {
        switch (wheel)
        {
            case 0x00:
                if (program == 0x08) ProgramUp(); else ProgramDown(); break;
            case 0x08:
                if (program == 0x0c) ProgramUp(); else ProgramDown(); break;
            case 0x0c:
                if (program == 0x04) ProgramUp(); else ProgramDown(); break;
            case 0x04:
                if (program == 0x00) ProgramUp(); else ProgramDown(); break;
        }
        wheel = program;
    }
}

void ProgramUp(void)
{
    unsigned char program = eeprom[queues[synth].channel];
    program++;
    program &= 0x7f;
    eeprom[queues[synth].channel] = program;
    SetProgram();
    PrintLine2("Program up      ");
    PrintNumeric(program, 3, 2, 13);
    delay(10);
}

void ProgramDown(void)
{
    unsigned char program = eeprom[queues[synth].channel];
    program--;
    program &= 0x7f;
    eeprom[queues[synth].channel] = program;
    SetProgram();
    PrintLine2("Program down    ");
    PrintNumeric(program, 3, 2, 13);
    delay(10);
}

void SetProgram(void)
{
    seconds = 0;
    pc[1] = 0xc0 | queues[synth].channel;
    pc[2] = eeprom[queues[synth].channel];
    pc[0] = -1;
}

void killNotes(int queue)
{
    unsigned char channel;
    
    // Do we have a status in data[0]?
    if ((queues[queue].data[0] & 0x80) == 0x80)
    {
        // Fetch channel from data[0]:
        channel = queues[queue].data[0] & 0x0f;
    }
    else
    {
        // Fetch channel from running_status:
        channel = queues[queue].running_status & 0x0f;
    }
    // Wait for channel to be reset in queues:
    while (queues[queue].pos != 0);
    // Send reset all controls:
    MIDI_Send(0xb0 | channel);
    MIDI_Send(0x79);
    MIDI_Send(0x00);
    // Wait for channel to be reset in queues:
    while (queues[queue].pos != 0);
    // Send all notes off:
    MIDI_Send(0xb0 | channel);
    MIDI_Send(0x7b);
    MIDI_Send(0x00);
}

void displaySynth(unsigned char synth)
{
    char transpose;
    
    switch (synth)
    {
        case 0:
            if (DIRECT)
            {
       	        PrintLine1("DX7       Direct");
       	    }
       	    else
            {
       	        PrintLine1("DX7         UR44");
       	    }
       	break;
        case 1:
            if (DIRECT)
            {
       	        PrintLine1("JUNO-106  Direct");
       	    }
       	    else
            {
       	        PrintLine1("JUNO-106    UR44");
       	    }
       	break;
        case 2:
            if (DIRECT)
            {
       	        PrintLine1("WX7       Direct");
       	    }
       	    else
            {
       	        PrintLine1("WX7         UR44");
       	    }
       	break;
        case 3:
            if (DIRECT)
            {
       	        PrintLine1("Drumpads  Direct");
       	    }
       	    else
            {
       	        PrintLine1("Drumpads    UR44");
       	    }
       	break;
        case 4:
            if (DIRECT)
            {
       	        PrintLine1("AUX       Direct");
       	    }
       	    else
            {
       	        PrintLine1("AUX         UR44");
       	    }
       	break;
        
    }
    transpose = queues[synth].transpose;
    if (transpose & 0x80)
    {
        if (queues[synth].channel == 0xff)
        {
            PrintLine2("ch.-- x-pose -  ");
        }
        else
        {
       	    PrintLine2("ch.   x-pose -  ");
        }
   	    transpose = 0 - transpose;
   	}
   	else
    {
        if (queues[synth].channel == 0xff)
        {
            PrintLine2("ch.--  x-pose   ");
        }
        else
        {
            PrintLine2("ch.    x-pose   ");
        }
   	}
   	if (queues[synth].channel != 0xff)
   	{
   	    PrintNumeric(queues[synth].channel + 1, 2, 2, 3);
   	}
   	PrintNumeric(transpose, 2, 2, 14);
}

void readEeProm(void)
{
    unsigned char i;
    unsigned char data;
    char transpose;
    
    for (i = 0; i < 5; i++)
    {
        MIDI_SendOneByte();
        data = eeprom_read_byte(i);
        if (data < 0x10 || data == 0xff)
        {
            queues[i].channel = data;
        }
        else
        {
            eeprom_write_byte(i, queues[i].channel);
        }
        MIDI_SendOneByte();
        transpose = eeprom_read_byte(i + 5);
        if (transpose < 25 && transpose > -25)
        {
            queues[i].transpose = transpose;
        }
        else
        {
            eeprom_write_byte(i + 5, (unsigned char)queues[i + 5].transpose);
        }
    }
    for (i = 0; i < 16; i++)
    {
        MIDI_SendOneByte();
        eeprom[i] = eeprom_read_byte(i + 16);
        if (eeprom[i] < 0x80)
        {
            MIDI_Send(0xc0 | i);
            MIDI_Send(eeprom[i]);
        }
    }
    MIDI_SendOneByte();
}

void BackupToEeprom(void)
{
    unsigned char i;
    unsigned char data;
    
    MIDI_LED_ON;
    for (i = 0; i < 5; i++)
    {
        MIDI_SendOneByte();
        data = eeprom_read_byte(i);
        if (data < 0x10 || data == 0xff)
        {
            if (queues[i].channel != data)
            {
                eeprom_write_byte(i, queues[i].channel);
            }
        }
        MIDI_SendOneByte();
        data = eeprom_read_byte(i + 5);
        if (data < 25 && data > -25)
        {
            if (queues[i].transpose != data)
            {
                eeprom_write_byte(i + 5, (unsigned char)queues[i + 5].data);
            }
        }
    }
    for (i = 0; i < 16; i++)
    {
        MIDI_SendOneByte();
        data = eeprom_read_byte(i + 16);
        if (data != eeprom[i])
        {
            eeprom_write_byte(i + 16, eeprom[i]);
        }
    }
    MIDI_SendOneByte();
    MIDI_LED_OFF;
}

void welcome(void)
{
    unsigned char l1[40] = "                 MIDI                  ";
    unsigned char l2[40] = "                Manager                ";
    int i;
    delay(10);
    PrintLine1("                ");
    PrintLine2("                ");
    delay(4);
    PrintLine1("                ");
    PrintLine2("               m");
    delay(8);
    PrintLine1("I-              ");
    PrintLine2("              ma");
    delay(12);
    PrintLine1("DI-             ");
    PrintLine2("             man");
    delay(16);
    PrintLine1("IDI-            ");
    PrintLine2("            mana");
    delay(20);
    PrintLine1("MIDI-           ");
    PrintLine2("           manag");
    delay(24);
    PrintLine1(" MIDI-          ");
    PrintLine2("          manage");
    delay(28);
    PrintLine1("  MIDI-         ");
    PrintLine2("         manager");
    delay(32);
    PrintLine1("   MIDI-        ");
    PrintLine2("        manager ");
    delay(36);
    PrintLine1("    MIDI-       ");
    PrintLine2("       manager  ");
    delay(40);
    PrintLine1("     MIDI-      ");
    PrintLine2("      manager   ");
    delay(44);
    PrintLine1("      MIDI-     ");
    PrintLine2("     manager    ");
    delay(500);
    Clr();
}
