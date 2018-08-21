#ifndef INIT_H
#define	INIT_H

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "constants.h"
#include "can.h"

//main.c
typedef struct {
	unsigned timer10ms:1;
	unsigned timer100ms:1;
    unsigned send_disable:1;
    unsigned send_initializepos:1;
    unsigned send_powerjoints:1;
    unsigned send_enable:1;
    unsigned send_requeststate:1;
    unsigned can_receive1:1;
    unsigned can_receive2:1;
    unsigned can_receive3:1;
    unsigned can_receive4:1;
    unsigned can_receive5:1;
    unsigned can_receive6:1;
    unsigned can_receive:6;
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
extern volatile uint8_t state;
extern char buffer[];
void PB1release();
void PB2release();
void PB3release();
void PB4release();
void PB5release();
void Init();

//motion.c
extern const int motion_len;
extern const __prog__ int __attribute__((space(prog))) motion[][10];
extern const __prog__ int __attribute__((space(prog))) basetorque[][10];

//init.c
void delay_ms(int);

//oscillator.c
void Init_OSC();

//pins.c
void Init_Pins();

//timers.c
void Init_TMR4();

//uart1.c
void Init_UART1();
void UART1_puts(char *);
void UART1_print(char *);
void UART1_write(char);
uint8_t UART1_read();
void UART1_process();
extern volatile int UART1_size;

//uart2.c
void Init_UART2();
void UART2_puts(char *);
void UART2_print(char *);
void UART2_write(char);
uint8_t UART2_read();
void UART2_process();
extern volatile int UART2_size;

//input.c
void Init_CN();
void Init_PushButton();

#endif
