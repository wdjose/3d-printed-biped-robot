#include "init.h"

long Kp1 = 5000;
long Kd1 = 1000;
long Ki1 = 1;

long Kp2 = 5000;
long Kd2 = 1000;
long Ki2 = 1;

long Kdiv1 = 100;
long Kdiv2 = 100;

long maxPWM1, maxPWM2;
long minsetpoint1 = -5760, maxsetpoint1 = 5760;
long minsetpoint2 = -5760, maxsetpoint2 = 5760;

volatile long basetorque1, basetorque2;
volatile long setpoint1, setpoint2;
volatile long error1, error2;
volatile long sumerror1, sumerror2;
volatile long prevpos1, prevpos2;
volatile long currpos1, currpos2;
volatile long signal1, signal2;

void Init_Control() {
	//brake motors
	INA1 = 0;
	INB1 = 0;
	INA2 = 0;
	INB2 = 0;
	
	//set PWM duty cycle to 0/10000
	PWM1 = 0;
	PWM2 = 0;
	
	//disable motors
	EN1 = 0;
	EN2 = 0;
	
	maxPWM1 = PTPER;
	maxPWM2 = PTPER;
	
	sumerror1 = 0;
	sumerror2 = 0;
	prevpos1 = 0;
	prevpos2 = 0;
	
	setpoint1 = 0;
	setpoint2 = 0;
	
	basetorque1 = 0;
	basetorque2 = 0;
	
	pos1 = 0;
	pos2 = 0;
}

void Control_loop() {
	if (EN1) {
		currpos1 = pos1;
		error1 = currpos1 - setpoint1;
		sumerror1 += error1;
		signal1 = error1 * Kp1 / Kdiv1  + sumerror1 * Ki1 / Kdiv1 + (currpos1 - prevpos1) * Kd1 / Kdiv1 + basetorque1;
		prevpos1 = currpos1;
		if (signal1 < 0) {
			signal1 = -signal1;
			if (signal1 > maxPWM1)
				signal1 = maxPWM1;
			INA1 = 0;
			INB1 = 1;
			PWM1 = signal1;
		}
		else {
			if (signal1 > maxPWM1)
				signal1 = maxPWM1;
			INA1 = 1;
			INB1 = 0;
			PWM1 = signal1;
		}
	}
	
	if (EN2) {
		currpos2 = pos2;
		error2 = pos2 - setpoint2;
		sumerror2 += error2;
		signal2 = error2 * Kp2 / Kdiv2  + sumerror2 * Ki2 / Kdiv2 + (currpos2 - prevpos2) * Kd2 / Kdiv2 + basetorque2;
		prevpos2 = currpos2;
		if (signal2 < 0) {
			signal2 = -signal2;
			if (signal2 > maxPWM2)
				signal2 = maxPWM2;
			INA2 = 0;
			INB2 = 1;
			PWM2 = signal2;
		}
		else {
			if (signal2 > maxPWM2)
				signal2 = maxPWM2;
			INA2 = 1;
			INB2 = 0;
			PWM2 = signal2;
		}
	}
}
