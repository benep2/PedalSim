This program simulates the sustain pedal action while holding down the MIDI NOTEOFF messages,
for use in programs like SetBfree, which don't have this function. Although it creates ALSA MIDI ports,
I use it in conjunction with another program that bridges ALSA to JACK MIDI. I am not a professional
programmer and I created this program using the example from the ALSA MIDI library.

How to compile:
gcc pedalsim.c -o pedalsim -lasound

You need to have the ALSA development library.
