#include "init.h"

int timer4ctr200 = 0;
int timer4ctr1000 = 0;

void Init_TMR4() {
	T4CON = 0;
	
	T4CONbits.TCKPS = 0b11;	//set prescaler to 1:256
	PR4 = 2700;				//approx 138.24MHz / 256 / 540 = 1kHz
	_T4IP = 1;				//set timer 4 priority to 1
	_T4IF = 0;				//clear interrupt flag
	
	T4CONbits.TON = 1;		//turn on timer 4
	_T4IE = 1;				//enable timer 4 interrupt
}

void __attribute__((interrupt, no_auto_psv)) _T4Interrupt() {
	flags.timer10ms = 1;
	if (timer4ctr200 + 1 == 20) {
		flags.timer200ms = 1;
		timer4ctr200 = 0;
		
		if (timer4ctr1000 + 1 == 5) {
			flags.timer1000ms = 1;
			timer4ctr1000 = 0;
		}
		else
			timer4ctr1000++;
	}
	else {
		timer4ctr200++;
	}
	
	_T4IF = 0;
}
