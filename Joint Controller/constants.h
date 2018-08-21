#ifndef CONSTANTS_H
#define	CONSTANTS_H

//controller ID
#define CONTROLLER_ID 6
#define SELF (0xf0 + CONTROLLER_ID)

//frequencies
#define FRC 7372800                 //FRC = 7.3728 MHz +-1%
#define FOSC (FRC/4*150/2)          //FOSC = 138.24 MHz
#define FCY (FOSC/2)                //FCY = 69.12 MHz
#define FBAUD(x) ((FCY/x)/4-1)      //FBAUD(115200) = 149

//digital inputs
#define ENCA1 _RA1
#define ENCB1 _RA0
#define ENCA2 _RB10
#define ENCB2 _RB11

//digital outputs
#define INA1 _LATA2
#define INB1 _LATA3
#define INA2 _LATB4
#define INB2 _LATA4
#define EN1 _LATB15
#define EN2 _LATB13
#define LED _LATB1

//PWM duty cycles
#define PWM1 PDC1
#define PWM2 PDC2

#endif
