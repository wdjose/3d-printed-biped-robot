#ifndef __CAN_H__
#define __CAN_H__

#include "constants.h"

//buffers 8 to 14 are automatically receive buffers
#define BUF_0 8
#define BUF_1 9
#define BUF_2 10
#define BUF_3 11
#define BUF_4 12
#define BUF_5 13
#define BUF_6 14

#define SID_0 0xf0
#define SID_1 0xf1
#define SID_2 0xf2
#define SID_3 0xf3
#define SID_4 0xf4
#define SID_5 0xf5
#define SID_6 0xf6

#define SID_0r 0xe0
#define SID_1r 0xe1
#define SID_2r 0xe2
#define SID_3r 0xe3
#define SID_4r 0xe4
#define SID_5r 0xe5
#define SID_6r 0xe6

#define NUM_of_ECAN_BUFFERS 32

void Init_CAN();
void CAN_Transmit(unsigned int, char *);
void CAN_process(unsigned int);
void can1_init();
void can1_inittransmit();
void can1_initreceive();

extern volatile int CAN_buffer[6][4];

extern unsigned int ecan1MsgBuf[NUM_of_ECAN_BUFFERS][8]
__attribute__((aligned(NUM_of_ECAN_BUFFERS*16)));

extern unsigned int ecan1RcvMsgBuf[NUM_of_ECAN_BUFFERS][8]
__attribute__((aligned(NUM_of_ECAN_BUFFERS*16)));

#endif