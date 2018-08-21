#ifndef INIT_H
#define	INIT_H

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "constants.h"
#include "can.h"

//control.c
extern volatile long setpoint1;
extern volatile long setpoint2;
extern volatile long basetorque1;
extern volatile long basetorque2;
void Init_Control();
void Control_loop();
extern long Kp1, Ki1, Kd1, Kdiv1;
extern long Kp2, Ki2, Kd2, Kdiv2;
extern long maxPWM1, maxPWM2;
extern long minsetpoint1, maxsetpoint1;
extern long minsetpoint2, maxsetpoint2;

//main.c
typedef struct {
	unsigned timer10ms:1;
	unsigned timer200ms:1;
    unsigned timer1000ms:1;
    unsigned packetrx:1;
    unsigned can_receive:1;
} flags_t;
extern volatile flags_t flags;
typedef struct {
    uint8_t ID;
    uint8_t Command;
    uint8_t Options1;
    uint8_t Options2;
    uint16_t Value1;
    uint16_t Value2;
} packet_t;
extern volatile packet_t packet;

//control.c
extern volatile long signal1;
extern volatile long signal2;

//init.c
void delay_ms(int);

//oscillator.c
void Init_OSC();

//pins.c
void Init_Pins();

//encoder.c
void Init_CN();
void Init_Encoder();
extern volatile long pos1;
extern volatile long pos2;
extern volatile long realpos1;
extern volatile long realpos2;

//pwm.c
void Init_PWM();
void Init_PWMInterrupt();

//timers.c
void Init_TMR4();

//uart.c
void Init_UART();
void UART_puts(char *);
void UART_print(char *);
void UART_write(char);
uint8_t UART_read();
void UART_process();
extern volatile int UART_size;

#endif
