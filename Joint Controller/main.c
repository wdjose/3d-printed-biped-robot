#include "init.h"

#define ACCURACY 10

volatile flags_t flags;
volatile packet_t packet;

int ctr1, ctr2;
char buffer[128];

void sendState(char);

int main() {
	flags.timer10ms = 0;
	flags.timer200ms = 0;
	flags.packetrx = 0;
	flags.can_receive = 0;
	
	Init_OSC();
	Init_Pins();
	Init_Encoder();
	Init_CN();
	Init_PWM();
	Init_TMR4();
//	Init_UART();
	Init_CAN();
	
	Init_Control();
	Init_PWMInterrupt();
	
	
	
	LED = 0;
	int value1, value2;
	while (1) {
		//Command processing
//		UART_process();
		
//		if (flags.can_receive) {
//			delay_ms(50);
//			LED = ~LED;
//			flags.can_receive = 0;
//		}
//		continue;
		
		if (flags.packetrx || flags.can_receive) {
			value1 = (int)packet.Value1;
			value2 = (int)packet.Value2;
			switch (packet.Command) {
				//Reset
				case 0x01:
					asm ("RESET");
					break;

				//Set angle setpoint
				case 0x02:
					if (packet.Options1 & 1) {
						if (value1 < minsetpoint1)
							setpoint1 = minsetpoint1;
						else if (value1 > maxsetpoint1)
							setpoint1 = maxsetpoint1;
						else
							setpoint1 = value1;
					}
					if (packet.Options2 & 1) {
						if (value2 < minsetpoint2)
							setpoint2 = minsetpoint2;
						else if (value2 > maxsetpoint2)
							setpoint2 = maxsetpoint2;
						else
							setpoint2 = value2;
					}
					if ((packet.Options1 & (1 << 1)) || (packet.Options2 & (1 << 1))) {
						sendState(0);
					}
					break;

				//Increment angle setpoint
				case 0x03:
					if (packet.Options1 & 1) {
						if (setpoint1 + value1 < minsetpoint1)
							setpoint1 = minsetpoint1;
						else if (setpoint1 + value1 > maxsetpoint1)
							setpoint1 = maxsetpoint1;
						else
							setpoint1 += value1;
					}
					if (packet.Options2 & 1) {
						if (setpoint2 + value2 < minsetpoint2)
							setpoint2 = minsetpoint2;
						else if (setpoint2 + value2 > maxsetpoint2)
							setpoint2 = maxsetpoint2;
						else
							setpoint2 += value2;
					}
					break;

				//Set current position
				case 0x04:
					if (packet.Options1 & 1)
						pos1 = value1;
					if (packet.Options2 & 1)
						pos2 = value2;
					break;

				//Set angle setpoint to current position
				case 0x05:
					if (packet.Options1 & 1) {
						setpoint1 = pos1;
						if (setpoint1 < minsetpoint1)
							minsetpoint1 = setpoint1;
						else if (setpoint1 > maxsetpoint1)
							maxsetpoint1 = setpoint1;
					}
					if (packet.Options2 & 1) {
						setpoint2 = pos2;
						if (setpoint2 < minsetpoint2)
							minsetpoint2 = setpoint2;
						else if (setpoint2 > maxsetpoint2)
							maxsetpoint2 = setpoint2;
					}
					break;

				//Turn on/off motor control loop
				case 0x06:
					if (packet.Options1 & 1) {
						if (value1 & 1)
							EN1 = 1;
						else
							EN1 = 0;
					}
					if (packet.Options2 & 1) {
						if (value2 & 1)
							EN2 = 1;
						else
							EN2 = 0;
					}
					break;

				//Set maximum PWM duty cycle
				case 0x07:
					if (packet.Options1 & 1) {
						if (value1 <= PTPER)
							maxPWM1 = value1;
						else
							maxPWM1 = PTPER;
					}
					if (packet.Options2 & 1) {
						if (value2 <= PTPER)
							maxPWM2 = value2;
						else
							maxPWM2 = PTPER;
					}
					break;

				//Set Kp constant
				case 0x08:
					if (packet.Options1 & 1)
						Kp1 = value1;
					if (packet.Options2 & 1)
						Kp2 = value2;
					break;

				//Set Ki constant
				case 0x09:
					if (packet.Options1 & 1)
						Ki1 = value1;
					if (packet.Options2 & 1)
						Ki2 = value2;
					break;

				//Set Kd constant
				case 0x0A:
					if (packet.Options1 & 1)
						Kd1 = value1;
					if (packet.Options2 & 1)
						Kd2 = value2;
					break;

				//Set Kdiv constant
				case 0x0B:
					if (packet.Options1 & 1)
						Kdiv1 = value1;
					if (packet.Options2 & 1)
						Kdiv2 = value2;
					break;

				//Set setpoint minimum
				case 0x0C:
					if (packet.Options1 & 1) {
						if (value1 >= -5760 && value1 <= 5760 && value1 <= maxsetpoint1)
							minsetpoint1 = value1;
						if (setpoint1 < minsetpoint1)
							setpoint1 = minsetpoint1;
					}
					if (packet.Options2 & 1)
						if (value2 >= -5760 && value2 <= 5760 && value2 <= maxsetpoint2)
							minsetpoint2 = value2;
						if (setpoint2 < minsetpoint2)
							setpoint2 = minsetpoint2;
					break;

				//Set setpoint maximum
				case 0x0D:
					if (packet.Options1 & 1) {
						if (value1 >= -5760 && value1 <= 5760 && value1 >= minsetpoint1)
							maxsetpoint1 = value1;
						if (setpoint1 > maxsetpoint1)
							setpoint1 = maxsetpoint1;
					}
					if (packet.Options2 & 1)
						if (value2 >= -5760 && value2 <= 5760 && value2 >= minsetpoint2)
							maxsetpoint2 = value2;
						if (setpoint2 > maxsetpoint2)
							setpoint2 = maxsetpoint2;
					break;
				case 0x0E:
					if ((packet.Options1 & (1 << 1)) || (packet.Options2 & (1 << 1))) {
						sendState(1);
					}
					break;
				case 0x0F:
					if (packet.Options1 & 1)
						basetorque1 = value1;
					if (packet.Options2 & 1)
						basetorque2 = value2;
					break;
			}
			flags.packetrx = 0;
			flags.can_receive = 0;
		}

		//Heartbeat
		if (flags.timer200ms) {
			if (EN1 || EN2)
				LED = 0;
			else
				LED = ~LED;
			
			flags.timer200ms = 0;
		}
		if (flags.timer1000ms) {
			
			flags.timer1000ms = 0;
		}
	}
	return 0;
}

void sendState(char real) {
	int msg[4];
	if (real) {
		msg[0] = (int)realpos1;
		msg[1] = (int)realpos2;
	}
	else {
		msg[0] = (int)pos1;
		msg[1] = (int)pos2;
	}

	msg[2] = (int)signal1;
	msg[3] = (int)signal2;

	CAN_Transmit(SID_0r + CONTROLLER_ID, msg);
}
