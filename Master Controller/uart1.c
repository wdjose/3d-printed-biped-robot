#include "init.h"

#define BUFSIZE 128
#define BUFMASK (BUFSIZE - 1)

volatile int UART1_size;
volatile int UART1_start, UART1_end;
uint8_t UART1_buffer[BUFSIZE];

void Init_UART1() {
	UART1_start = 0;
	UART1_end = 0;
	UART1_size = 0;
	
	//map UART1 TXD pin to pin RP49 (RC1)
	RPOR5bits.RP49R = 0b000001;
	//map UART1 TXD pin to pin RP20 (RA4 SDO1)
	RPOR0bits.RP20R = 0b000001;
	//map UART1 RXD pin to pin RP41 (RC0)
	RPINR18bits.U1RXR = 48;
	
	U1MODEbits.STSEL = 0;			//1 stop bit
	U1MODEbits.PDSEL = 0;			//no parity, 8 data bits
	U1MODEbits.ABAUD = 0;			//auto baud disabled
	U1MODEbits.BRGH = 1;			//high speed mode (4 clocks per bit)
	U1BRG = FBAUD(115200);			//baud rate set to 115200
	U1STAbits.URXISEL = 0;			//interrupt after one rx done
	
	_U1RXIF = 0;					//clear uart receive interrupt flag
	_U1RXIP = 4;					//set uart receive interrupt priority to 4
	_U1RXIE = 1;					//enable uart rx interrupt
	
	U1MODEbits.UARTEN = 1;			//enable uart
	U1STAbits.UTXEN = 1;			//enable uart transmissions
	delay_ms(1);					//delay 1ms to ensure tx is ready
}

void UART1_puts(char *s) {
	while (*s)
		UART1_write(*s++);
	UART1_write('\n');
}

void UART1_print(char *s) {
	while (*s)
		UART1_write(*s++);
}

void UART1_write(char c) {
	while (U1STAbits.UTXBF);
	U1TXREG = c;
}

uint8_t UART1_read() {
	if (UART1_size) {
		uint8_t ret = UART1_buffer[UART1_start];
		
		UART1_start = (UART1_start + 1) & BUFMASK;
		UART1_size--;
		
		return ret;
	}
	return 0;
}

void UART1_process() {
	
}

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt() {
	uint8_t data = U1RXREG;
	int UART1_next = (UART1_end + 1) & 0b1111111;
	if (UART1_next != UART1_start) {
		UART1_buffer[UART1_end] = data;
		UART1_end = UART1_next;
		UART1_size++;
	}
	
    _U1RXIF = 0;
}
