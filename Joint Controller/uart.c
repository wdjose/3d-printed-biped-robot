#include "init.h"

#define BUFSIZE 128
#define BUFMASK (BUFSIZE - 1)

volatile int UART_size;

volatile int UART_start, UART_end;
uint8_t UART_buffer[BUFSIZE];

void Init_UART() {
	//clear UART receive ring buffer
	UART_start = 0;
	UART_end = 0;
	UART_size = 0;
	
	//map UART1 TXD pin to pin RP40 (RB8 SDO1)
	//RPOR3bits.RP40R = 0b000001;	//disabled to prevent TX pins from interfering with each other
	//map UART1 RXD pin to pin RP41 (RB9 SDI1)
	RPINR18bits.U1RXR = 41;
	
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

void UART_puts(char *s) {
	while (*s)
		UART_write(*s++);
	UART_write('\n');
}

void UART_print(char *s) {
	while (*s)
		UART_write(*s++);
}

void UART_write(char c) {
	while (U1STAbits.UTXBF);
	U1TXREG = c;
}

uint8_t UART_read() {
	if (UART_size) {
		uint8_t ret = UART_buffer[UART_start];
		
		UART_start = (UART_start + 1) & BUFMASK;
		UART_size--;
		
		return ret;
	}
	return 0;
}

void UART_process() {
	if (flags.packetrx)
		return;
	if (UART_size >= 11) {
		if (UART_buffer[UART_start] == 0x55) {
			if (UART_buffer[(UART_start + 10) & BUFMASK] == 0xAA) {
				int checksum = 0;
				int UART_ctr = (UART_start + 8) & BUFMASK;
				while (UART_ctr != UART_start) {
					unsigned int v = UART_buffer[UART_ctr];
					while (v) {
						checksum += v & 1;
						v >>= 1;
					}
					UART_ctr = (UART_ctr - 1) & BUFMASK;
				}
				if (UART_buffer[(UART_start + 9) & BUFMASK] == checksum) {
					packet.ID = UART_buffer[(UART_start + 1) & BUFMASK] & 0xFF;
					packet.Command = UART_buffer[(UART_start + 2) & BUFMASK] & 0xFF;
					packet.Options1 = UART_buffer[(UART_start + 3) & BUFMASK] & 0xFF;
					packet.Options2 = UART_buffer[(UART_start + 4) & BUFMASK] & 0xFF;
					packet.Value1 = (UART_buffer[(UART_start + 5) & BUFMASK] & 0xFF) << 8;
					packet.Value1 |= UART_buffer[(UART_start + 6) & BUFMASK] & 0xFF;
					packet.Value2 = (UART_buffer[(UART_start + 7) & BUFMASK] & 0xFF) << 8;
					packet.Value2 |= UART_buffer[(UART_start + 8) & BUFMASK] & 0xFF;
					
					UART_start = (UART_start + 11) & BUFMASK;
					UART_size -= 11;
					
					if (packet.ID == CONTROLLER_ID)
						flags.packetrx = 1;
					return;
				}
			}
		}
		UART_start = (UART_start + 1) & BUFMASK;
		UART_size--;
	}
}

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt() {
	uint8_t data = U1RXREG;
	int UART_next = (UART_end + 1) & 0b1111111;
	if (UART_next != UART_start) {
		UART_buffer[UART_end] = data;
		UART_end = UART_next;
		UART_size++;
	}
	
    _U1RXIF = 0;
}
