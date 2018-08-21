#include "init.h"

void Init_OSC()
{
	//Fosc = 138.24 MHz
	//Fcy = 69.12 MHz
	//generated using MPLAB Code Configuratorr
	
    // CF no clock failure; NOSC FRCPLL; CLKLOCK unlocked; OSWEN Switch is Complete; 
    __builtin_write_OSCCONL((uint8_t) (0x100 & 0x00FF));
    // FRCDIV FRC/1; PLLPRE 4; DOZE 1:8; PLLPOST 1:2; DOZEN disabled; ROI disabled; 
    CLKDIV = 0x3002;
    // TUN Center frequency; 
    OSCTUN = 0x0;
    // ROON disabled; ROSEL disabled; RODIV Base clock value; ROSSLP disabled; 
    REFOCON = 0x0;
    // PLLDIV 148; 
    PLLFBD = 0x94;
    // RND disabled; SATB disabled; SATA disabled; ACCSAT disabled; 
	CORCONbits.RND = 0;
	CORCONbits.SATB = 0;
	CORCONbits.SATA = 0;
	CORCONbits.ACCSAT = 0;
    // WDTO disabled; TRAPR disabled; SLEEP disabled; BOR disabled; CM disabled; SWR disabled; SWDTEN disabled; EXTR disabled; POR disabled; VREGSF disabled; IDLE disabled; IOPUWR disabled; VREGS disabled; 
    RCON = 0x0;
}
