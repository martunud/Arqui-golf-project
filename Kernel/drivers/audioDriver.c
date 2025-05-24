#include "../include/audioDriver.h"

extern void outb(uint16_t port, uint8_t value);
extern uint8_t inb(uint16_t port);

void audio_play(uint32_t nFrequence) {
 	uint32_t div;
 	uint8_t tmp;
 
 	div = 1193180 / nFrequence;
 	outb(0x43, 0xb6);
 	outb(0x42, (uint8_t) (div) );
 	outb(0x42, (uint8_t) (div >> 8));
 
 	tmp = inb(0x61);
  	if (tmp != (tmp | 3)) {
 		outb(0x61, tmp | 3);
 	}
 }
 
 //make it shut up
void audio_stop() {
 	uint8_t tmp = inb(0x61) & 0xFC;
 	outb(0x61, tmp);
 }
 
 //Make a beep
//  void audio_beep(int frequency, int duration) {
//      audio_play(frequency);
//      timer_wait(duration);
//      audio_stop();
//  }