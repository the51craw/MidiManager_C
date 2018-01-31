MIDI_mapper uses multiple microcontrollers, 1 PIC18F4550 (this project) and 4 PIC16F628 (under the folder ..\..\PIC16F628\MIDI_mapper).

The PIC18F4550 has one of the MIDI inputs and the MIDI output to the chain.

The 4 PIC16F628 adds support for 4 more MIDI inputs that are all communicated on a parallell bus to the PIC18F4550.

The parallell but has eight lines for data, one line for data received flag and one output enable.

The 4 PIC16F628 are responsible for receiving MIDI data, setting the received data pin when data is available, to put the data on the
output pins when and while the select pin is set, and finally to power down it's outputs.

The PIC18F4550 is responsible for receiving 1 MIDI input and to merge all MIDI data streams into one MIDI output.

The PIC18F4550 is also responsible for MIDI error detections, keyboard and display, and for the channel changes and to transpose.
