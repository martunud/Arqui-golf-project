#ifndef AUDIO_DRIVER_H
#define AUDIO_DRIVER_H

#define AUDIO_FREQUENCY 440

void audio_play();

void audio_stop();

void audio_beep(int frequency, int duration);
#endif