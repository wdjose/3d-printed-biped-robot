#include "init.h"

int main_motorTest() {
	Init_OSC();
	Init_Pins();
	Init_CN();
	Init_PWM();
	Init_TMR4();
	Init_UART();
	
	INA1 = 0;
	INB1 = 1;
	INA2 = 1;
	INB2 = 0;
	PWM1 = 500;
	PWM2 = 500;
	EN1 = 1;
	EN2 = 1;
	LED = 0;
	
	while (1) {
		delay_ms(600 * 2);
		PWM1 = PWM2 = 0;
		delay_ms(400 * 2);
		LED = ~LED;
		
		INA1 = ~INA1;
		INB1 = ~INB1;
		INA2 = ~INA2;
		INB2 = ~INB2;
		PWM1 = PWM2 = 500;
	}
	return 0;
}
