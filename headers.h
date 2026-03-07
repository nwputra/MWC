#ifndef _HEADERS_H
#define	_HEADERS_H

#include <stdint.h>

typedef union {
	struct {
		unsigned int	relays:3;
		unsigned int	trx:1;
		unsigned int	oper:1;
		unsigned int	nc:1;
		unsigned int	bias:1;
		unsigned int	ptt:1;
	} b;

	uint8_t	c;
} LPFctrl_t;



typedef union {
	struct {
		unsigned int	opersw:1;
		unsigned int	pttline:1;
		unsigned int	autoband:1;
		unsigned int	mode:1;
		unsigned int	temph:1;
		unsigned int	overdrv:1;
		unsigned int	highswr:1;
		unsigned int	bdsel:1;
	} b;

	uint8_t	c;
} AMPstate_t;




// PORTB definitions
//
// FAN is using PB1. DAC is not used here.
// If DAC is used, FAN should be moved somewhere.
//
//
#define		HBEAT		0



//
// PORTD definitions
// Set 7-5 as output, 4 as input
//
//
#define		DOWN_RST	7
#define		UP_AUTO		6
#define		OPER_STBY	5
#define		PDPTT		4			// PD4/T0 is used for PTT sensing not ALC in the build
#define		ALCEN		4
#define		BIASEN		1
#define		TXEN		0



// State Machine
//
#define		STBY		0
#define		OPER		1
#define		TX1		2
#define		TX2		3
#define		RX1		4
#define		RX2		5
#define		PROT1		6
#define		PROT2		7
#define		RESET		8


// Semaphore Flags
//
//
#define		SMTIME		0x01
#define		SMTICK		0x08


// Variables
extern volatile unsigned char	tmp;
extern volatile unsigned char	sema;

extern unsigned char		DSbutton;
extern unsigned char		DSled;

extern const char		ST01[];
extern const char		ST02[];

#endif
