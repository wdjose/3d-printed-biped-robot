#include "init.h"
//#include <ecan.h>

//cont ctrl;	//get accel and accel2 here
//motdata_struct motdata;	//send 

unsigned int ecan1MsgBuf[NUM_of_ECAN_BUFFERS][8]
__attribute__((aligned(NUM_of_ECAN_BUFFERS*16)));

unsigned int ecan1RcvMsgBuf[NUM_of_ECAN_BUFFERS][8]
__attribute__((aligned(NUM_of_ECAN_BUFFERS*16)));

void Init_CAN(){
	//configure pins to CAN module
	
	//for joint controller
    //CAN_TX to RP38 (RB6)
    //CAN_RX to RP37 (RB5)
	_RP38R = 0b001110;
	_C1RXR = 37;
	
    can1_init();
    can1_inittransmit();
    can1_initreceive();
}

void can1_init(){
    //pin assignments transferred
    
    //configure CAN
	//LED = 0;
    
    C1CTRL1bits.REQOP = 4;      //config mode
    while(!C1CTRL1bits.OPMODE==4);
    C1FCTRL = 0xC01F;           //no FIFO, 32 buffers (FIO area starts with buffer bits: Receive buffer RB31)
    C1CTRL1bits.CANCKS = 0;     //Fcan = Fp
    
    
    //config 1
    //Fbaud = 0.875Mbps, N=20, Fcan = 70MHz
    //Phase segment1 = 8Tq, PS2 = 6Tq, 5tq propagation delay
    //Phase segment 2 set to be programmable depending on phase segment 1 time
    //synchronization jump width set to 4tq
    C1CFG1 = 0x00C1;
    C1CFG2 = 0x05FC;
    
    //can in normal mode
    C1CTRL1bits.REQOP = 0;  
    while(!C1CTRL1bits.OPMODE==0);
	
	//MSTRPR = 0x20;
}

void can1_inittransmit(){
    C1CTRL1bits.REQOP = 4;      //CAN config mode
    while(!C1CTRL1bits.OPMODE==4);
    
    C1CTRL1bits.WIN = 0;
    //use DMA0 for transmission
    //assign 32x8 word message buffers for ECAN1 in device RAM
    DMA0CONbits.SIZE = 0;
    DMA0CONbits.DIR = 1;        //data transfer from RAM to peripheral
    DMA0CONbits.AMODE = 0x2;    //peripheral indirect addressing mode
    DMA0CONbits.MODE = 0;       //continuous ping pong mode disabled
 
    DMA0REQ = 70;               //assign ECAN1 transmit event for DMA channel 0
    DMA0CNT = 7;            //DMA transfer per ECAN message set to 8 words
    DMA0PAD = (volatile unsigned int)&C1TXD;    //ECAN1 transmit register
    DMA0STAL = (unsigned int) &ecan1MsgBuf;
    DMA0STAH = (unsigned int) &ecan1MsgBuf;
    DMA0CONbits.CHEN = 1;       //enable DMA channel 0
    
    //config message buffer 0 for transmission and assign priority
    C1TR01CONbits.TXEN0 = 1;
    C1TR01CONbits.TX0PRI = 0x3; //highest message priority
    
    //can in normal mode; ready to transmit message
    C1CTRL1bits.REQOP = 0;  
    while(!C1CTRL1bits.OPMODE==0); 
}

void CAN_Transmit(unsigned int SID, int msg[]){ //length in bytes
    can1_inittransmit();

    //SID<10:0> = 0b100 1000 1111 (std identifier bits)
    //IDE = 0b0 (standard message, not extended)
    //SRR = 0b0 (normal msg, not request remote transmission)
    //ecan1MsgBuf[0][0] = 0x123C;
    ecan1MsgBuf[0][0] = (SID<<2) &0x1FFC;
    
    //EID = 0           //extended identifier bits
    ecan1MsgBuf[0][1] = 0x0;
    
    //EID = 0           //extended identifier bits
    //RTR = 0           //normal message (not remote transmission)
    //RB1 = 0           //have to be 0
    //RB0 = 0           //have to be 0
    //DLC = 0b1100      //data length (8 bytes) (max)
    ecan1MsgBuf[0][2] = 8;          //change to len later
    
    //write message data bytes (8 bytes)
    ecan1MsgBuf[0][3] = msg[0];
    ecan1MsgBuf[0][4] = msg[1];
    ecan1MsgBuf[0][5] = msg[2];
    ecan1MsgBuf[0][6] = msg[3];
    
    //request message buffer 0 transmission
    C1TR01CONbits.TXREQ0 = 1;
    
    while(C1TR01CONbits.TXREQ0 == 1); //wait until transmission is complete
    
    //message placed in bus successfully
}

void can1_initreceive(){    
    
    C1CTRL1bits.REQOP = 4;      //CAN config mode
    while(!C1CTRL1bits.OPMODE==4);
    
    C1CTRL1bits.WIN = 1;
    
    //uses DMA1
    DMA1CONbits.SIZE = 0;
    DMA1CONbits.DIR = 0;        //reads from peripheral address, writes to RAM address
    DMA1CONbits.AMODE = 2;      //peripheral indirect addressing mode
    DMA1CONbits.MODE = 0;
    DMA1REQ = 34;
    DMA1CNT = 7;                //8 words to be accepted
    DMA1PAD = (volatile unsigned int)&C1RXD;
    DMA1STAL = (unsigned int) &ecan1RcvMsgBuf;
    DMA1STAH = (unsigned int) &ecan1RcvMsgBuf;
    DMA1CONbits.CHEN = 1;
    
    //if(SELF!=DASH_SID){
        C1FMSKSEL1bits.F1MSK = 1;       //filter 1 will use mask 1
        C1RXM1SIDbits.SID = 0xff;       //mask for SID
        C1RXF1SIDbits.SID = SELF;
        C1RXM1SIDbits.MIDE = 1;         //only check for std identifier
        C1RXF1SIDbits.EXIDE = 0;
        C1BUFPNT1bits.F1BP = BUF_0;  //filter to use specified buffer
        C1FEN1bits.FLTEN1 = 1;          //enable filter
    //}
    
    //enable interrupt
    IEC2bits.C1IE = 1;
    _C1IP = 0x05;
    C1INTEbits.RBIE = 1;
    
    //clear window bit to access ECAN control registers
    C1CTRL1bits.WIN = 0;
    
    C1CTRL1bits.REQOP = 0;      //CAN normal mode
    while(!C1CTRL1bits.OPMODE==0);
}

unsigned int buf[20];
void __attribute__((interrupt, no_auto_psv))_C1Interrupt(void){
    _C1IF = 0;
	
	if(C1INTFbits.RBIF == 1){
		C1INTFbits.RBIF = 0;
		if(C1RXFUL1bits.RXFUL8){  //front buf
			if (((ecan1RcvMsgBuf[BUF_0][3] >> 8) & 0xFF) == CONTROLLER_ID)
				CAN_process();
			C1RXFUL1bits.RXFUL8 = 0;
		}
	}
    _C1IF = 0;
    C1INTFbits.RBOVIF = 0;
    //take note of overflows in receive buffer
    //~1ms delay in between transmissions
    //if there's overflow, message can still be received, but the order can be jumbled
    //if there's still problem and no way to delay 1ms, better to service with two buffers per SID
}

void CAN_process(){
	packet.ID = (ecan1RcvMsgBuf[BUF_0][3] >> 8) & 0xFF;
	packet.Command = (ecan1RcvMsgBuf[BUF_0][3]) & 0xFF;
	packet.Options1 = (ecan1RcvMsgBuf[BUF_0][4] >> 8) & 0xFF;
	packet.Options2 = (ecan1RcvMsgBuf[BUF_0][4]) & 0xFF;
	packet.Value1 = ((ecan1RcvMsgBuf[BUF_0][5] >> 8) & 0xFF) << 8;
	packet.Value1 |= (ecan1RcvMsgBuf[BUF_0][5]) & 0xFF;
	packet.Value2 = ((ecan1RcvMsgBuf[BUF_0][6] >> 8) & 0xFF) << 8;
	packet.Value2 |= (ecan1RcvMsgBuf[BUF_0][6]) & 0xFF;
	flags.can_receive = 1;
}
