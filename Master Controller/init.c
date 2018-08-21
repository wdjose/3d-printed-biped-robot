#include "init.h"

void delay_ms(int ms) {
	int ctr;
	while (ms > 0) {
		for (ctr = 9874; ctr > 0; ctr--);
		ms--;
	}
}

void delay_us(int us) {
	int ctr;
	while (us > 0) {
		for (ctr = 9; ctr > 0; ctr--);
		us--;
	}
}