#ifndef MIDI_H_DEFINED
#define MIDI_H_DEFINED

void MIDI_Init(void);
void MIDI_Reset_All(void);
void Check_MIDI_In(void);
void MIDI_Send(unsigned char data);
void MIDI_SendOneByte(void);
#define MIDI_IN 

#endif
