#include "init.h"

void Init_PWM() {
	PTCON = 0;
	PTCON2 = 0;
	IOCON1 = 0;
	IOCON2 = 0;
	PWMCON1 = 0;
	PWMCON2 = 0;
	
	//prescaler 1:8
	PTCON2bits.PCLKDIV = 0b011;
	
	//enable immediate updates to duty cycle
	PTCONbits.EIPU = 1;
	PWMCON1bits.IUE = 1;
	PWMCON2bits.IUE = 1;
	
	//approx 138.24MHz / 8 / 10000 = 1.728kHz PWM freq
	PTPER = 10000;
	
	//PWM pin ownership (enable PWM1H1 and PWM1H2, disable PWM1L1, PWM1L2, PWM1L3 and PWM1H3)
	IOCON1bits.PENH = 1;
	IOCON2bits.PENH = 1;
	IOCON3bits.PENH = 0;
	IOCON1bits.PENL = 0;
	IOCON2bits.PENL = 0;
	IOCON3bits.PENL = 0;
	
	//remove PWM fault
	FCLCON1bits.FLTSRC = 0;
	FCLCON2bits.FLTSRC = 0;
	
	//use PDC register for duty cycle
	PWMCON1bits.MDCS = 0;
	PWMCON2bits.MDCS = 0;
	
	//set PWM I/O pin pair to redundant mode
	IOCON1bits.PMOD = 1;
	IOCON2bits.PMOD = 1;
	
	//initally 0/1000 = 0% duty cycle
	PDC1 = 0;
	PDC2 = 0;
	
	//enable PWM
	PTCONbits.PTEN = 1;
}

void Init_PWMInterrupt() {
	SEVTCMP = 0;					//interrupt when PWM master time base = 0
	PTCONbits.SEIEN = 1;			//enable PWM special event interrupt
	
	_PSEMIP = 3;					//set interrupt priority to 3
	_PSEMIF = 0;					//clear interrupt flag
	_PSEMIE = 1;					//enable PWM special event interrupt bit
}

void __attribute__((interrupt, no_auto_psv)) _PWMSpEventMatchInterrupt() {
	Control_loop();
	
    _PSEMIF = 0;
}
