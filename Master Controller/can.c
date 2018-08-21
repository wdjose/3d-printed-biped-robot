#include "init.h"

//cont ctrl;	//get accel and accel2 here
//motdata_struct motdata;	//send 

volatile int CAN_buffer[6][4];

unsigned int ecan1MsgBuf[NUM_of_ECAN_BUFFERS][8]
__attribute__((aligned(NUM_of_ECAN_BUFFERS*16)));

unsigned int ecan1RcvMsgBuf[NUM_of_ECAN_BUFFERS][8]
__attribute__((aligned(NUM_of_ECAN_BUFFERS*16)));

void Init_CAN(){
	//configure pins to CAN module
	
	//for master controller
    //CAN_TX to RP39 (RB7)
    //CAN_RX to RPI58 (RC10)
	_RP39R = 0b001110;			//RP39 as CAN1 TX
	_C1RXR = 58;				//RPI58 as CAN1 RX
	
    can1_init();
    can1_inittransmit();
    can1_initreceive();
}

void can1_init(){
    //pin assignments transferred
    
    //configure CAN
    
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

void CAN_Transmit(unsigned int SID, char msg[]){ //length in bytes
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
    ecan1MsgBuf[0][3] = ((msg[0] << 8) & 0xFF00) | (msg[1] & 0xFF);
    ecan1MsgBuf[0][4] = ((msg[2] << 8) & 0xFF00) | (msg[3] & 0xFF);
    ecan1MsgBuf[0][5] = ((msg[4] << 8) & 0xFF00) | (msg[5] & 0xFF);
    ecan1MsgBuf[0][6] = ((msg[6] << 8) & 0xFF00) | (msg[7] & 0xFF);
    
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
		C1RXM1SIDbits.SID = 0xff;       //mask for SID (uses mask 1)
        C1RXM1SIDbits.MIDE = 1;         //only check for std identifier (for mask 1)
		
		C1FMSKSEL1bits.F1MSK = 1;       //filter 1 will use mask 1
        C1RXF1SIDbits.SID = SID_1r;
        C1RXF1SIDbits.EXIDE = 0;
        C1BUFPNT1bits.F1BP = BUF_1;  //filter to use specified buffer
        C1FEN1bits.FLTEN1 = 1;          //enable filter
		
		C1FMSKSEL1bits.F2MSK = 1;       //filter 2 will use mask 2
        C1RXF2SIDbits.SID = SID_2r;
        C1RXF2SIDbits.EXIDE = 0;
        C1BUFPNT1bits.F2BP = BUF_2;  //filter to use specified buffer
        C1FEN1bits.FLTEN2 = 1;          //enable filter
		
		C1FMSKSEL1bits.F3MSK = 1;       //filter 3 will use mask 3
        C1RXF3SIDbits.SID = SID_3r;
        C1RXF3SIDbits.EXIDE = 0;
        C1BUFPNT1bits.F3BP = BUF_3;  //filter to use specified buffer
        C1FEN1bits.FLTEN3 = 1;          //enable filter
		
		C1FMSKSEL1bits.F4MSK = 1;       //filter 4 will use mask 4
        C1RXF4SIDbits.SID = SID_4r;
        C1RXF4SIDbits.EXIDE = 0;
        C1BUFPNT2bits.F4BP = BUF_4;  //filter to use specified buffer
        C1FEN1bits.FLTEN4 = 1;          //enable filter
		
		C1FMSKSEL1bits.F5MSK = 1;       //filter 5 will use mask 5
        C1RXF5SIDbits.SID = SID_5r;
        C1RXF5SIDbits.EXIDE = 0;
        C1BUFPNT2bits.F5BP = BUF_5;  //filter to use specified buffer
        C1FEN1bits.FLTEN5 = 1;          //enable filter
		
		C1FMSKSEL1bits.F6MSK = 1;       //filter 6 will use mask 6
        C1RXF6SIDbits.SID = SID_6r;
        C1RXF6SIDbits.EXIDE = 0;
        C1BUFPNT2bits.F6BP = BUF_6;  //filter to use specified buffer
        C1FEN1bits.FLTEN6 = 1;          //enable filter
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
		if(C1RXFUL1bits.RXFUL9){  //SID_1r
			flags.can_receive1 = 1;
			C1RXFUL1bits.RXFUL9 = 0;
		}
		if(C1RXFUL1bits.RXFUL10){  //SID_2r
			flags.can_receive2 = 1;
			C1RXFUL1bits.RXFUL10 = 0;
		}
		if(C1RXFUL1bits.RXFUL11){  //SID_3r
			flags.can_receive3 = 1;
			C1RXFUL1bits.RXFUL11 = 0;
		}
		if(C1RXFUL1bits.RXFUL12){  //SID_4r
			flags.can_receive4 = 1;
			C1RXFUL1bits.RXFUL12 = 0;
		}
		if(C1RXFUL1bits.RXFUL13){  //SID_5r
			flags.can_receive5 = 1;
			C1RXFUL1bits.RXFUL13 = 0;
		}
		if(C1RXFUL1bits.RXFUL14){  //SID_6r
			flags.can_receive6 = 1;
			C1RXFUL1bits.RXFUL14 = 0;
		}
	}
    _C1IF = 0;
    C1INTFbits.RBOVIF = 0;
    //take note of overflows in receive buffer
    //~1ms delay in between transmissions
    //if there's overflow, message can still be received, but the order can be jumbled
    //if there's still problem and no way to delay 1ms, better to service with two buffers per SID
}

void CAN_process(unsigned int buffer_num) {
	int pos1, pos2, torque1, torque2;
	buffer_num += 8;
	pos1 = ((ecan1RcvMsgBuf[buffer_num][3] >> 8) & 0xFF) << 8;
	pos1 |= (ecan1RcvMsgBuf[buffer_num][3]) & 0xFF;
	pos2 = ((ecan1RcvMsgBuf[buffer_num][4] >> 8) & 0xFF) << 8;
	pos2 |= (ecan1RcvMsgBuf[buffer_num][4]) & 0xFF;
	torque1 = ((ecan1RcvMsgBuf[buffer_num][5] >> 8) & 0xFF) << 8;
	torque1 |= (ecan1RcvMsgBuf[buffer_num][5]) & 0xFF;
	torque2 = ((ecan1RcvMsgBuf[buffer_num][6] >> 8) & 0xFF) << 8;
	torque2 |= (ecan1RcvMsgBuf[buffer_num][6]) & 0xFF;
	CAN_buffer[buffer_num - 9][0] = pos1;
	CAN_buffer[buffer_num - 9][1] = pos2;
	CAN_buffer[buffer_num - 9][2] = torque1;
	CAN_buffer[buffer_num - 9][3] = torque2;
	flags.can_receive |= (1 << (buffer_num - 9));
}
