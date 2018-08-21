#ifndef CONSTANTS_H
#define	CONSTANTS_H

//controller ID
#define CONTROLLER_ID 0
#define SELF (0xf0 + CONTROLLER_ID)

//frequencies
#define FRC 7372800                 //FRC = 7.3728 MHz +-1%
#define FOSC (FRC/4*150/2)          //FOSC = 138.24 MHz
#define FCY (FOSC/2)                //FCY = 69.12 MHz
#define FBAUD(x) ((FCY/x)/4-1)      //FBAUD(115200) = 149

//led outputs
#define LED1 _LATB12
#define LED2 _LATB13
#define LED3 _LATA10
#define LED4 _LATA7
#define LED5 _LATB14
#define LED6 _LATB15
#define LED7 _LATG6
#define LED8 _LATG7
#define LED9 _LATG8
#define LED10 _LATG9
#define LED11 _LATA12
#define LED12 _LATA11
#define LED13 _LATA0
#define LED14 _LATA1
#define LED15 _LATB0
#define LED16 _LATB1

//pushbuttons
#define PB1 _RC4
#define PB2 _RC5
#define PB3 _RC12
#define PB4 _RC15
#define PB5 _RD8

#endif
