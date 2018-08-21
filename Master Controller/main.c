#include "init.h"

/*********User editable variables*********/
//these constants are ordered by:
//	0 - right coronal ankle
//	1 - right sagittal ankle
//	2 - right sagittal knee
//	3 - right sagittal hip
//	4 - right coronal hip
//	5 - left coronal ankle
//	6 - left sagittal ankle
//	7 - left sagittal knee
//	8 - left sagittal hip
//	9 - left coronal hip
const int Kp[10] = {16000, 5000, 5000, 5000, 16000, 16000, 5000, 5000, 5000, 16000};
const int Ki[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//const int Ki[10] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5};
//const int Ki[10] = {20, 20, 20, 20, 20, 20, 20, 20, 20, 20};
const int Kd[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const int Kdiv[10] = {20, 20, 20, 20, 20, 20, 20, 20, 20, 20};
/*****End of user editable variables******/

void Init();
void Init_PID();
void sendPacket();

int ctr1, ctr2, requestctr;
char buffer[128];

volatile flags_t flags;
volatile packet_t packet;
volatile uint8_t state;
/*
	State 1: all joints powered down, idle mode
	State 2: joint positions are reset to initial
	State 3: joints are powered up to hold position
	State 4: motion is ongoing
	State 5: motion has been paused
	State 6: motion has just finished
*/
volatile int motion_ctr;

const uint8_t transform[2][6] = {
	{0, 2, 4, 5, 7, 9},
	{1, 2, 3, 6, 7, 8}
};

int main() {
	Init();
	while (1) {
//		UART1_process();
//		UART2_process();
		
		if (flags.can_receive1) {
			CAN_process(1);
			LED11 = ~LED11;
			flags.can_receive1 = 0;
		}
		if (flags.can_receive2) {
			CAN_process(2);
			LED12 = ~LED12;
			flags.can_receive2 = 0;
		}
		if (flags.can_receive3) {
			CAN_process(3);
			LED13 = ~LED13;
			flags.can_receive3 = 0;
		}
		if (flags.can_receive4) {
			CAN_process(4);
			LED14 = ~LED14;
			flags.can_receive4 = 0;
		}
		if (flags.can_receive5) {
			CAN_process(5);
			LED15 = ~LED15;
			flags.can_receive5 = 0;
		}
		if (flags.can_receive6) {
			CAN_process(6);
			LED16 = ~LED16;
			flags.can_receive6 = 0;
		}
		if (flags.can_receive == 0b111111) {
//			sprintf(buffer, "Setpnt: %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d\n", motion[motion_ctr][0], motion[motion_ctr][1], 
//				motion[motion_ctr][2], motion[motion_ctr][3], motion[motion_ctr][4], motion[motion_ctr][5], 
//				motion[motion_ctr][6], motion[motion_ctr][7], motion[motion_ctr][8], motion[motion_ctr][9]);
//			UART1_print(buffer);
//			sprintf(buffer, "Actual: %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d\n", CAN_buffer[0][0], CAN_buffer[0][1], 
//				CAN_buffer[1][1], CAN_buffer[2][1], CAN_buffer[2][0], CAN_buffer[3][0], 
//				CAN_buffer[3][1], CAN_buffer[4][1], CAN_buffer[5][1], CAN_buffer[5][0]);
//			UART1_print(buffer);
//			sprintf(buffer, "Torque: %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d\n", CAN_buffer[0][2], CAN_buffer[0][3], 
//				CAN_buffer[1][3], CAN_buffer[2][3], CAN_buffer[2][2], CAN_buffer[3][2], 
//				CAN_buffer[3][3], CAN_buffer[4][3], CAN_buffer[5][3], CAN_buffer[5][2]);
//			UART1_print(buffer);
			
			flags.can_receive = 0;
		}
		
		if (flags.send_requeststate) {
			packet.Command = 0x0E;
			packet.Options1 = 0b11;
			packet.Options2 = 0b11;
			motion_ctr++;
			for (ctr1 = 0; ctr1 < 6; ctr1++) {
				packet.ID = ctr1 + 1;
				sendPacket(packet);
			}
			
			flags.send_requeststate = 0;
		}
		
		if (flags.timer10ms) {
			if (state == 4) {
//				if (requestctr == 0) {
//					packet.Options1 = 0b11;
//					packet.Options2 = 0b11;
//					requestctr = 0;
//				}
//				else {
					packet.Options1 = 0b01;
					packet.Options2 = 0b01;
//					requestctr++;
//				}
				motion_ctr++;
				for (ctr1 = 0; ctr1 < 6; ctr1++) {
					packet.ID = ctr1 + 1;
					
					packet.Command = 0x02;
					packet.Value1 = motion[motion_ctr][transform[0][ctr1]];
					packet.Value2 = motion[motion_ctr][transform[1][ctr1]];
					sendPacket(packet);
					
//					packet.Command = 0x0F;
//					packet.Value1 = basetorque[motion_ctr][transform[0][ctr1]];
//					packet.Value2 = basetorque[motion_ctr][transform[1][ctr1]];
//					sendPacket(packet);
				}
				if (motion_ctr >= motion_len - 1) {
					LED1 = LED2 = LED3 = LED4 = LED5 = LED6 = 1;
					LED6 = 0;
					state = 6;
				}
			}
			flags.timer10ms = 0;
		}
		if (flags.send_disable) {
			packet.Command = 0x06;
			packet.Options1 = 1;
			packet.Options2 = 1;
			packet.Value1 = 0;
			packet.Value2 = 0;
			for (ctr1 = 0; ctr1 < 6; ctr1++) {
				packet.ID = ctr1 + 1;
				sendPacket(packet);
			}
			flags.send_disable = 0;
		}
		if (flags.send_initializepos) {
			//set setpoint to 0
			packet.Options1 = 1;
			packet.Options2 = 1;
			packet.Value1 = 0;
			packet.Value2 = 0;
			for (ctr1 = 0; ctr1 < 6; ctr1++) {
				packet.ID = ctr1 + 1;
				packet.Command = 0x02;				//set setpoint to 0
				sendPacket(packet);
				packet.Command = 0x04;				//set current position to 0
				sendPacket(packet);
			}
			flags.send_initializepos = 0;
		}
		if (flags.send_enable) {
			packet.Command = 0x06;
			packet.Options1 = 1;
			packet.Options2 = 1;
			packet.Value1 = 1;
			packet.Value2 = 1;
			for (ctr1 = 0; ctr1 < 6; ctr1++) {
				packet.ID = ctr1 + 1;
				sendPacket(packet);
			}
			flags.send_enable = 0;
		}
		
		//Heartbeat
		if (flags.timer100ms) {
			LED10 = ~LED10;
//			UART2_write('g');
//			UART2_write('\n');
			flags.timer100ms = 0;
		}
	}
	return 0;
}

//idle state / emergency stop
void PB1release() {
	//send disable command to all joints
	flags.send_disable = 1;
	
	LED1 = LED2 = LED3 = LED4 = LED5 = LED6 = 1;
	LED1 = 0;
	state = 1;
}

//(re-)initialize joint positions
void PB2release() {
	if (state == 1) {				//ensure previous state is state 1
		//send set joints to 0
		flags.send_initializepos = 1;
		
		LED1 = LED2 = LED3 = LED4 = LED5 = LED6 = 1;
		LED2 = 0;
		state = 2;
	}
}

//power joints to hold position
void PB3release() {
	if (state == 2) {				//ensure previous state is state 2
		//send enable mode
		flags.send_enable = 1;
		
		LED1 = LED2 = LED3 = LED4 = LED5 = LED6 = 1;
		LED3 = 0;
		state = 3;
	}
}

//(re)start motion
void PB4release() {
	if (state == 3) {				//case when motion has just been started
		//start motion
		motion_ctr = -1;
		
		LED1 = LED2 = LED3 = LED4 = LED5 = LED6 = 1;
		LED4 = 0;
		state = 4;
	}
	else if (state == 5) {			//case when motion was paused
		//resume motion
		
		LED1 = LED2 = LED3 = LED4 = LED5 = LED6 = 1;
		LED4 = 0;
		state = 4;
	}
	//ensure previous state is state 3 or 5
	//if state 3: start motion
	//if state 5: resume motion
	//once done with entire motion, set state to state 6
}

//pause motion
void PB5release() {
//	if (state == 4) {				//case when motion is ongoing
//		//pause motion
//		
//		LED1 = LED2 = LED3 = LED4 = LED5 = LED6 = 1;
//		LED5 = 0;
//		state = 5;
//	}
	flags.send_requeststate = 1;
}

void Init() {
	Init_OSC();
	Init_Pins();
	Init_UART1();
	Init_UART2();
	Init_TMR4();
	Init_CN();
	Init_PushButton();
	Init_CAN();
	
	//heartbeat LED
	LED10 = 0;
	
	//status LEDs
	LED1 = 1;
	LED2 = 1;
	LED3 = 1;
	LED4 = 1;
	LED5 = 1;
	LED6 = 1;
	
	//joint LEDs
	LED11 = 1;
	LED12 = 1;
	LED13 = 1;
	LED14 = 1;
	LED15 = 1;
	LED16 = 1;
	
	//motion path LEDs
	LED7 = 0;			//motion 1
	LED8 = 1;			//motion 2
	LED9 = 1;			//motion 3
	
	//initialize state
	LED1 = 0;
	state = 1;
	
	//disable all flags
	flags.timer10ms = 0;
	flags.timer100ms = 0;
	flags.send_disable = 0;
	flags.send_enable = 0;
	flags.send_initializepos = 0;
	flags.send_powerjoints = 0;
	flags.send_requeststate = 0;
	flags.can_receive = 0;
	flags.can_receive1 = 0;
	flags.can_receive2 = 0;
	flags.can_receive3 = 0;
	flags.can_receive4 = 0;
	flags.can_receive5 = 0;
	flags.can_receive6 = 0;
	
	requestctr = 0;
	
	delay_ms(1000);
	//initialize PID constants
	Init_PID();
}

void Init_PID() {
	//Kp
	packet.Command = 0x08;
	packet.Options1 = 1;
	packet.Options2 = 1;
	for (ctr1 = 0; ctr1 < 6; ctr1++) {
		packet.ID = ctr1 + 1;
		packet.Value1 = Kp[transform[0][ctr1]];
		packet.Value2 = Kp[transform[1][ctr1]];
		sendPacket(packet);
	}
	
	//Ki
	packet.Command = 0x09;
	packet.Options1 = 1;
	packet.Options2 = 1;
	for (ctr1 = 0; ctr1 < 6; ctr1++) {
		packet.ID = ctr1 + 1;
		packet.Value1 = Ki[transform[0][ctr1]];
		packet.Value2 = Ki[transform[1][ctr1]];
		sendPacket(packet);
	}
	
	//Kd
	packet.Command = 0x0A;
	packet.Options1 = 1;
	packet.Options2 = 1;
	for (ctr1 = 0; ctr1 < 6; ctr1++) {
		packet.ID = ctr1 + 1;
		packet.Value1 = Kd[transform[0][ctr1]];
		packet.Value2 = Kd[transform[1][ctr1]];
		sendPacket(packet);
	}
	
	//Kdiv
	packet.Command = 0x0B;
	packet.Options1 = 1;
	packet.Options2 = 1;
	for (ctr1 = 0; ctr1 < 6; ctr1++) {
		packet.ID = ctr1 + 1;
		packet.Value1 = Kdiv[transform[0][ctr1]];
		packet.Value2 = Kdiv[transform[1][ctr1]];
		sendPacket(packet);
	}
}

void sendPacket(packet_t p) {
	int i;
	buffer[0] = 0x55;
	buffer[1] = p.ID;
	buffer[2] = p.Command;
	buffer[3] = p.Options1;
	buffer[4] = p.Options2;
	buffer[5] = (uint8_t)((p.Value1 >> 8) & 0xFF);
	buffer[6] = (uint8_t)(p.Value1 & 0xFF);
	buffer[7] = (uint8_t)((p.Value2 >> 8) & 0xFF);
	buffer[8] = (uint8_t)(p.Value2 & 0xFF);
	buffer[9] = 0;
	for (i = 1; i <= 8; i++) {
		unsigned int temp = buffer[i] & 0xFF;
		while (temp) {
			buffer[9] += temp & 1;
			temp >>= 1;
		}
	}
	buffer[10] = 0xAA;
	for (i = 0; i <= 10; i++) {
//		UART1_write(buffer[i]);
//		UART2_write(buffer[i]);
	}
	CAN_Transmit(SELF + p.ID, buffer + 1);
	delay_ms(1);
}
