#ifndef	_AMPCTRL_H
#define	_AMPCTRL_H
// Microwave Controller
// YB1NWP - 2026
//
//


// State Machines
//
#define	MWIDLE		0
#define	MWPAUSE		1
#define	MWENDS		2
#define	MWSTART		3
#define	MWWTOPR		4
#define	MWOPER		5

#define	MWCTIM		30




// Variables

extern uint8_t		MWmin;			// Minute timer
extern uint8_t		MWsec;			// Second timer
extern uint8_t		MWton;			// Power duty on time (0-30)
extern uint8_t		MWtmr;			// Timer counter
extern uint8_t		MWstate;		// State


// IO Macros
//
//			
#define	MWRLY		4			// PORTC 4 - Magnetron relay
#define	MWHTR		5			// PORTC 5 - Heater relay
#define	DOOR		4			// PORTB 4 - Door
#define	MWFAN		5			// PORTB 5 - FAN and Table relay


#define MWMG_OFF	PORTC = PORTC & ~(1 << MWRLY);
#define MWMG_ON		PORTC = PORTC |  (1 << MWRLY);

#define MWHT_OFF	PORTC = PORTC & ~(1 << MWHTR);
#define MWHT_ON		PORTC = PORTC |  (1 << MWHTR);

#define MWFAN_OFF	PORTB = PORTB & ~(1 << MWFAN);
#define MWFAN_ON	PORTB = PORTB |  (1 << MWFAN);

#define DOOR_OPEN	(PINB & (1 << DOOR))

void MWfsm( void );

#endif
