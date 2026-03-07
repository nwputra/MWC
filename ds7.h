#ifndef	_DS7_H
#define	_DS7_H

// 7 Segment Display Controller
// YB1NWP (c) 2026
// version 1.0
//
//

#include "timer.h"
#include "headers.h"

extern unsigned char		DSidx;
extern unsigned char		DSdot;
extern unsigned char		DSbuf[4];

void DSinit( void );
void DSrefr( void );

//==========	--agfedDcb
#define	DGT0	0b10111011
#define	DGT1	0b00000011
#define	DGT2	0b11011001
#define	DGT3	0b11001011
#define	DGT4	0b01100011
#define	DGT5	0b11101010
#define	DGT6	0b11111010
#define	DGT7	0b10000011
#define	DGT8	0b11111011
#define	DGT9	0b11101011
#define	BLNK	0b00000000
#define	DDOT	0b00000100

#endif
