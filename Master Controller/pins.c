#include "init.h"

void Init_Pins()
{
	//disable all analog pins
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    ANSELE = 0;
    ANSELG = 0;
	
	//assign all pins as INPUT
	TRISA = 0xFFFF;
	TRISB = 0xFFFF;
	TRISC = 0xFFFF;
	TRISD = 0xFFFF;
	TRISE = 0xFFFF;
	TRISF = 0xFFFF;
	TRISG = 0xFFFF;
	
	//set all pins to LOW by default
	LATA = 0;
	LATB = 0;
	LATC = 0;
	LATD = 0;
	LATE = 0;
	LATF = 0;
	LATG = 0;
	
	//set all LEDs to HIGH
	LED1 = 1;
	LED2 = 1;
	LED3 = 1;
	LED4 = 1;
	LED5 = 1;
	LED6 = 1;
	LED7 = 1;
	LED8 = 1;
	LED9 = 1;
	LED10 = 1;
	LED11 = 1;
	LED12 = 1;
	LED13 = 1;
	LED14 = 1;
	LED15 = 1;
	LED16 = 1;
	
	//assign all LEDs as OUTPUT
	_TRISB12 = 0;
	_TRISB13 = 0;
	_TRISA10 = 0;
	_TRISA7 = 0;
	_TRISB14 = 0;
	_TRISB15 = 0;
	_TRISG6 = 0;
	_TRISG7 = 0;
	_TRISG8 = 0;
	_TRISG9 = 0;
	_TRISA12 = 0;
	_TRISA11 = 0;
	_TRISA0 = 0;
	_TRISA1 = 0;
	_TRISB0 = 0;
	_TRISB1 = 0;
	
	//assign all pushbuttons as INPUT
	_TRISC4 = 1;
	_TRISC5 = 1;
	_TRISC12 = 1;
	_TRISC15 = 1;
	_TRISD8 = 1;
}
