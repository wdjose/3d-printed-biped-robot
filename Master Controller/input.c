#include "init.h"

volatile uint8_t PBstate;

void Init_CN() {
	//enable CN interrupt pins
	_CNIEC4 = 1;			//PB1
	_CNIEC5 = 1;			//PB2
	_CNIEC12 = 1;			//PB3
	_CNIEC15 = 1;			//PB4
	_CNIED8 = 1;			//PB5
	
	//enable CN pullup
	_CNPUC4 = 1;			//PB1
	_CNPUC5 = 1;			//PB2
	_CNPUC12 = 1;			//PB3
	_CNPUC15 = 1;			//PB4
	_CNPUD8 = 1;			//PB5
	
	_CNIF = 0;				//clear bit
	_CNIP = 1;				//set interrupt priority to 5
	_CNIE = 1;				//enable interrupt bit
}

void Init_PushButton() {
	PBstate = 0xFF;
}

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt() {
	delay_ms(1);
	uint8_t pbstate;
	
	pbstate = (PBstate >> 1) & 1;
	if (PB1 != pbstate) {
		if (pbstate) {				//on pushbutton down
			PBstate &= ~(1 << 1);
		}
		else {						//on pushbutton up
			PB1release();
			PBstate |= (1 << 1);
		}
	}
	
	pbstate = (PBstate >> 2) & 1;
	if (PB2 != pbstate) {
		if (pbstate) {				//on pushbutton down
			PBstate &= ~(1 << 2);
		}
		else {						//on pushbutton up
			PB2release();
			PBstate |= (1 << 2);
		}
	}
	
	pbstate = (PBstate >> 3) & 1;
	if (PB3 != pbstate) {
		if (pbstate) {				//on pushbutton down
			PBstate &= ~(1 << 3);
		}
		else {						//on pushbutton up
			PB3release();
			PBstate |= (1 << 3);
		}
	}
	
	pbstate = (PBstate >> 4) & 1;
	if (PB4 != pbstate) {
		if (pbstate) {				//on pushbutton down
			PBstate &= ~(1 << 4);
		}
		else {						//on pushbutton up
			PB4release();
			PBstate |= (1 << 4);
		}
	}
	
	pbstate = (PBstate >> 5) & 1;
	if (PB5 != pbstate) {
		if (pbstate) {				//on pushbutton down
			PBstate &= ~(1 << 5);
		}
		else {						//on pushbutton up
			PB5release();
			PBstate |= (1 << 5);
		}
	}
	
	_CNIF = 0;
}
