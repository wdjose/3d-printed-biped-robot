#include "init.h"

void Init_Pins()
{
	//disable all analog pins
    ANSELA = 0;
    ANSELB = 0;
	
	//set all pins to LOW by default
	LATA = 0;
	LATB = 0;
	
    //set encoders as digital input
	_TRISA1 = 1;			//ENCA1
	_TRISA0 = 1;			//ENCB1
	_TRISB10 = 1;			//ENCA2
	_TRISB11 = 1;			//ENCB2
	
	//set H-bridge outputs as digital output
	_TRISA2 = 0;			//INA1
	_TRISA3 = 0;			//INB1
	_TRISB4 = 0;			//INA2
	_TRISA4 = 0;			//INB2
	
	//set enable outputs as digital output open drain
	_TRISB15 = 0;			//EN1
	_ODCB15 = 1;			//open drain
	_LATB15 = 0;
	_TRISB13 = 0;			//EN2
	_ODCB13 = 1;			//open drain
	_LATB13 = 0;
	
	//set PWM outputs as digital output and PWM
	_TRISB14 = 0;			//PWM1
	_LATB14 = 1;
	_TRISB12 = 0;			//PWM2
	_LATB12 = 1;
	
	//set LED as digital output
	_TRISB1 = 0;			//LED
	_LATB1 = 1;				//LED off default
	
	//set UART pins as digital input/output
	_TRISB9 = 1;			//RX
	_TRISB8 = 1;			//TX
	
	//set CAN pins as digital input
	_TRISB5 = 1;			//CANRX
	_TRISB6 = 1;			//CANTX
}
