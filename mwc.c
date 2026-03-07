// Microwave Controller
// YB1NWP (c) 2026
// version 1.0
//
//

#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <util/delay.h>

#include "headers.h"
#include "globals.h"
#include "timer.h"
#include "keyboard.h"
#include "mwc.h"
#include "ds7.h"


/* Labels/Text arrays */
const char	ST01[]  PROGMEM = "    ";
const char	ST02[]  PROGMEM = "END ";
const char	ST03[]  PROGMEM = {DGT0, DGT1, DGT2, DGT3 };


uint8_t		MWmin;			// Minute timer
uint8_t		MWsec;			// Second timer
uint8_t		MWton;			// Power duty on time (0-30)
uint8_t		MWtmr;			// Timer counter
uint8_t		MWstate;		// State


int main( )
{
	// Port directions
	// 0 - input
	// 1 - output
	//
	DDRB	= 0x00;				// PORT B input for keyboard
	PORTB	= 0x1F;				// Initial data, all HIGH
	DDRC	= 0x3F;				// PORT C as outputs for Display common
	PORTC	= 0x00;				// Initial state is LOW
	DDRD	= 0xFF;				// PORT as outputs for 7-segment display
	PORTD	= 0xFF;				// Initial state is HIGH


	// Variables
	//
	//
	sema		= SMTIME;
	DSidx 		= 0;			// Digit index



	// Initialize Hardware systems
	// 
	TMtmr2init();
	KBinit();


	// Disable interrupts
	cli();

	// INT0 setup
	EICRA   = EICRA | (1<<ISC01);			// Interrupt on Falling edge of INT0
	EIFR	= EIFR  | (1<<INTF0);			// Clear INT0 flag
	EIMSK	= EIMSK | (1<<INT0);			// Enable external interrupt INT0

	// Enable Watchdog
        wdt_enable( WDTO_500MS );			// Make aggressive watchdog (500mS)                     

	// Enable interrupts
	sei();


	while( 1 )
	{
		if( DOOR_OPEN )
		{
			// Turn off Magnetron and Heater
			// if door is opened
			MWMG_OFF;
			MWHT_OFF;

			// State is now paused
			MWstate = MWPAUSE;
		}

		if( sema & SMTICK )
		{
			sema &= ~SMTICK;

			// Display Scan every 5mS
			DSrefr();
		}

		if( sema & SMTIME )
		{
			sema &= ~SMTIME;

			// Call applications
			MWfsm();
		}

		// If everything is OK, clear WDT
		wdt_reset();
	}
}


void MWfsm( void )
{
	// Check if Magnetron is operating

	switch( MWstate )
	{
		case MWIDLE:	// Idle
				break;

		case MWPAUSE:	// Paused
				// No change
				break;

		case MWENDS:	// Ended
				MWstate = MWIDLE;
				break;

		case MWSTART:	// Start operating
				// Turn on FAN and Table and wait a while
				MWstate 	= MWWTOPR;
				TMtimer[0] 	= 2;
				MWFAN_ON;
				break;

		case MWWTOPR:	// Waiting for FAN
				if( TMtimer[0] == 0 )
				{
					MWstate = MWOPER;
				}
				break;

		case MWOPER:	// Operating
				// Decrement timer
				//
				if( MWsec )
				{
					--MWsec;
				}
				else
				{
					// Check if still time
					if( MWmin )
					{
						MWmin--;
						MWsec = 59;
					}
				}
		
				// Check if Microwave operating time
				if( MWmin == 0 && MWsec == 0 )
				{
					// Ended
					//
					MWMG_OFF;
					MWstate = MWENDS;
				}
				else
				{
					// Still time to operate
					//
					if( MWtmr >= MWton )
					{
						// End of ON duty time
						MWMG_OFF;
					}
					else
					{
						// At ON period
						MWMG_ON;
					}

					if( ++MWtmr >= MWCTIM )
					{
						// Cycle time elapsed
						// Reset duty counter
						//
						MWtmr = 0;
					}
				}
				break;
	}
}

