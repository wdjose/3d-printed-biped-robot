#include "init.h"

int enc_incr[4][4] = {
	{0, 1, -1, 0},
	{-1, 0, 0, 1},
	{1, 0, 0, -1},
	{0, -1, 1, 0}
};

volatile int state1, state2;
volatile long pos1, pos2;
volatile long realpos1, realpos2;

void Init_CN() {
	//enable CN interrupt pins
	_CNIEA1 = 1;			//ENCA1
	_CNIEA0 = 1;			//ENCB1
	_CNIEB10 = 1;			//ENCA2
	_CNIEB11 = 1;			//ENCB2
	
	//enable CN pullup
	_CNPUA1 = 1;			//ENCA1
	_CNPUA0 = 1;			//ENCB1
	_CNPUB10 = 1;			//ENCA2
	_CNPUB11 = 1;			//ENCB2
	
	_CNIF = 0;				//clear bit
	_CNIP = 5;				//set interrupt priority to 5
	_CNIE = 1;				//enable interrupt bit
}

void Init_Encoder() {
	//get initial states and set position to 0
	state1 = (ENCA1 << 1) | ENCB1;
	pos1 = 0;
	realpos1 = 0;
	
	state2 = (ENCA2 << 1) | ENCB2;
	pos2 = 0;
	realpos2 = 0;
}

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt() {
	//increment pos1 or pos2 depending on previous and current encoder states
	int next1, next2;
	
	next1 = (ENCA1 << 1) | ENCB1;
	pos1 += enc_incr[state1][next1];
	realpos1 += enc_incr[state1][next1];
	state1 = next1;
	
	next2 = (ENCA2 << 1) | ENCB2;
	pos2 += enc_incr[state2][next2];
	realpos2 += enc_incr[state2][next2];
	state2 = next2;
	
	_CNIF = 0;
}
