#include <time.h>

static unsigned long ticks = 0;

void timer_handler() {
	ticks++;
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}

void timer_wait(int ms) {
    unsigned long start = ticks_elapsed();
    unsigned long wait_ticks = (ms * 18 + 999) / 1000;
    while (ticks_elapsed() - start < wait_ticks) {
        // bucle vacío
    }
}

