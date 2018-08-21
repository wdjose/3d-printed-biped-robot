#include "init.h"

#define BUFSIZE 128
#define BUFMASK (BUFSIZE - 1)

volatile int UART2_size;
volatile int UART2_start, UART2_end;
uint8_t UART2_buffer[BUFSIZE];

void Init_UART2() {
	UART2_start = 0;
	UART2_end = 0;
	UART2_size = 0;
	
	//map UART2 TXD pin to pin RP40 (RB8)
	RPOR3bits.RP40R = 0b000011;
	//map UART2 RXD pin to pin RPI61 (RC13)
	RPINR19bits.U2RXR = 61;
	
	U2MODEbits.STSEL = 0;			//1 stop bit
	U2MODEbits.PDSEL = 0;			//no parity, 8 data bits
	U2MODEbits.ABAUD = 0;			//auto baud disabled
	U2MODEbits.BRGH = 1;			//high speed mode (4 clocks per bit)
	U2BRG = FBAUD(115200);			//baud rate set to 115200
	U2STAbits.URXISEL = 0;			//interrupt after one rx done
	
	_U2RXIF = 0;					//clear uart receive interrupt flag
	_U2RXIP = 4;					//set uart receive interrupt priority to 4
	_U2RXIE = 1;					//enable uart rx interrupt
	
	U2MODEbits.UARTEN = 1;			//enable uart
	U2STAbits.UTXEN = 1;			//enable uart transmissions
	delay_ms(1);					//delay 1ms to ensure tx is ready
}

void UART2_puts(char *s) {
	while (*s)
		UART2_write(*s++);
	UART2_write('\n');
}

void UART2_print(char *s) {
	while (*s)
		UART2_write(*s++);
}

void UART2_write(char c) {
	while (U2STAbits.UTXBF);
	U2TXREG = c;
//	delay_us(100);
}

uint8_t UART2_read() {
	if (UART2_size) {
		uint8_t ret = UART2_buffer[UART2_start];
		
		UART2_start = (UART2_start + 1) & BUFMASK;
		UART2_size--;
		
		return ret;
	}
	return 0;
}

void UART2_process() {
	
}

void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt() {
	uint8_t data = U2RXREG;
	int UART2_next = (UART2_end + 1) & 0b1111111;
	if (UART2_next != UART2_start) {
		UART2_buffer[UART2_end] = data;
		UART2_end = UART2_next;
		UART2_size++;
	}
	
    _U2RXIF = 0;
}
