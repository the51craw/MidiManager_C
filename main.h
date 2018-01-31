#ifndef MAIN_H_DEFINED
#define MAIN_H_DEFINED
// Uncomment only the define below that corresponds to the clock in use.
//#define CLOCK_FREQUENCY_20_MHZ
//#define COUNTER_VALUE 122
#define CLOCK_FREQUENCY_48_MHZ
#define COUNTER_VALUE 200
#define DATA_READY_1 PORTC & 0x20        // RC5
#define DATA_READY_2 PORTC & 0x10        // RC4
#define DATA_READY_3 PORTD & 0x10        // RD4
#define DATA_READY_4 PORTD & 0x20        // RD5
#define DIRECT (PORTD & 0x02)            // RD1
#define CHIP_SELECT_ON_1 LATD |= 0x80    // RD7
#define CHIP_SELECT_ON_2 LATD |= 0x40    // RD6
#define CHIP_SELECT_ON_3 LATD |= 0x08    // RD4
#define CHIP_SELECT_ON_4 LATD |= 0x04    // RD3
#define CHIP_SELECT_OFF_1 LATD &= ~0x80
#define CHIP_SELECT_OFF_2 LATD &= ~0x40
#define CHIP_SELECT_OFF_3 LATD &= ~0x08
#define CHIP_SELECT_OFF_4 LATD &= ~0x04
#define MIDI_LED_ON LATD |= 0x01
#define MIDI_LED_OFF LATD &= ~0x01
#define SYNTH_PLUS !(PORTE & 0x01)       // RE0
#define SYNTH_MINUS !(PORTE & 0x02)      // RE1
#define CHANNEL_PLUS !(PORTE & 0x04)     // RE2
#define CHANNEL_MINUS !(PORTC & 0x01)    // RC0
#define TRANSPOSE_PLUS !(PORTC & 0x02)   // RC1
#define TRANSPOSE_MINUS !(PORTC & 0x04)  // RC2
#define DISABLE_USB UCON &= ~0x08; UCFG |= 0x08
#endif
