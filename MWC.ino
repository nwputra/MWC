/* Fix MWmin and MWsec left initialized when Pause/Reset pressed
   Add TMhour
   Rewrite DSclock now to display clock
   Add MWSTME for setting clock
    - Dot is set to alwasy on during set time
    - Clock is set to 12:00 at power on
    - Clock is disabled (TMsec bit 7 is set)
    - Clock will be enabled after setting (TMsec bit 7 is cleared)
    - Clock runs and not part of MWFSM
  Use multi column to apply all keypad function

  Create DSbldot for dot treatment. Freeing ISR from doing the dots.
  
*/
// Semaphore Flags
//
//
#define	SMTIME		0x01
#define SMDOTS    0x02
#define	SMDSCN		0x04
#define	SMDOOR		0x08


// State Machines
//
#define KBIDLE          0
#define KBIN            1


#define MWIDLE          0
#define MWPAUSE         1
#define MWENDS          2
#define MWSTART         3
#define MWWTOPR         4
#define MWOPER          5
#define MWPWRS          6
#define MWRDY           7
#define MWTMRS          8
#define MWSTME          9

#define MWMAXST         9
#define MWCTIM          30

// Display
/*
//==========  --bcDdefga  PORTD
#define CHR0  0b11011101
#define CHR1  0b11000000
#define CHR2  0b10011011
#define CHR3  0b11010011
#define CHR4  0b11000110
#define CHR5  0b01010111
#define CHR6  0b01011111
#define CHR7  0b11000001
#define CHR8  0b11011111
#define CHR9  0b11010111
#define BLNK  0b00000000
#define DDOT  0b00100000
#define CHRD  0b11011010
#define CHRE  0b00011111
#define CHRH  0b01001111
#define CHRI  0b11000000
#define CHRL  0b00011100
#define CHRM  0b01001011
#define CHRN  0b01001010
*/


//==========	--agfedDcb	PORTD
#define CHR0	0b10111011
#define CHR1	0b00000011
#define CHR2	0b11011001
#define CHR3	0b11001011
#define CHR4	0b01100011
#define CHR5	0b11101010
#define CHR6	0b11111010
#define CHR7	0b10000011
#define CHR8	0b11111011
#define CHR9	0b11101011
#define BLNK	0b00000000
#define DDOT	0b00000100
#define CHRD  0b01011011
#define CHRE  0b11111000
#define CHRH  0b01110011
#define CHRI  0b00000011
#define CHRL  0b00111000
#define CHRM  0b11010010
#define CHRN  0b01010010

/*
//==========  --dgcbaDfe  PORTD
#define CHR0  0b10111011
#define CHR1  0b00110000
#define CHR2  0b11011001
#define CHR3  0b11111000
#define CHR4  0b01110010
#define CHR5  0b11101010
#define CHR6  0b11101011
#define CHR7  0b00111000
#define CHR8  0b11111011
#define CHR9  0b11111010
#define BLNK  0b00000000
#define DDOT  0b00000100
#define CHRD  0b11110001
#define CHRE  0b11001011
#define CHRH  0b01110011
#define CHRI  0b00110000
#define CHRL  0b10000011
#define CHRM  0b01101001
#define CHRN  0b01100001
*/

#define DGT0	0 // C1 -> PC0
#define DGT1	2 // C2 -> PC2
#define DGT2	3 // C3 -> PC3
#define DGT3	1 // C4 -> PC1


// IO Macros
//
//
#define PCDGT0		DGT0			// PORTC 0 - C1
#define PCDGT1		DGT1			// PORTC 1 - C4
#define PCDGT2		DGT2			// PORTC 2 - C2
#define PCDGT3		DGT3			// PORTC 3 - C3
#define MWRLY           4                       // PORTC 4 - Magnetron relay
#define MWHTR           5                       // PORTC 5 - Heater relay

#define PBSCN0		0			// PORTB 0 - Keyscan line
#define PBSCN1		1			// PORTB 1 - Keyscan line
#define PBSCN2		2			// PORTB 2 - Keyscan line
#define PBSCN3		3			// PORTB 3 - Keyscan line
#define DOOR            4                       // PORTB 4 - Door
#define MWFAN           5                       // PORTB 5 - FAN and Table relay


#define MWMG_OFF        PORTC = PORTC & ~(1 << MWRLY);
#define MWMG_ON         PORTC = PORTC |  (1 << MWRLY);

#define MWHT_OFF        PORTC = PORTC & ~(1 << MWHTR);
#define MWHT_ON         PORTC = PORTC |  (1 << MWHTR);

#define MWLAMP_OFF      PORTB = PORTB & ~(1 << MWFAN);
#define MWLAMP_ON       PORTB = PORTB |  (1 << MWFAN);

#define DOOR_OPEN       ((~PINB) & (1 << DOOR))




// Main Process
// Control
volatile unsigned char	tmp;
volatile unsigned char  sema;

// OS FSM
unsigned char		OSstate;
unsigned char		POSTst;


// Display
unsigned char		DSidx;
volatile unsigned char	DSdot;
unsigned char		DSbuf[4];


// Timer
volatile unsigned char	OSALDIS;
volatile unsigned char	OSALNUM;
volatile unsigned char	OSSTATE;
volatile unsigned char	OSALCTR;
volatile unsigned char	TMcnt;
volatile unsigned char	TMtimer[1];
volatile unsigned int	TMtick;
volatile unsigned int	TMdelay;
volatile unsigned char	TMpre;
unsigned char		TMsec;
unsigned char		TMmin;
unsigned char   TMhour;

uint8_t         MWmin;                  // Minute timer
uint8_t         MWsec;                  // Second timer
uint8_t         MWton;                  // Power duty on time (0-30)
uint8_t         MWtmr;                  // Timer counter
uint8_t         MWstate;                // State


unsigned char    KBcol;
unsigned char    KBrow;
unsigned int     KBxinp;
unsigned int     KBxprev;
unsigned int     KBxpres;
unsigned int     KBxbit;
unsigned char    KBidx;
unsigned char    KBch;
unsigned char    KBstate;





const char	LKUP[]	PROGMEM = {CHR0, CHR1, CHR2, CHR3, CHR4, CHR5, CHR6, CHR7, CHR8, CHR9};
const char  KBmap[] PROGMEM = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
const char  DSHP[]  PROGMEM = {CHRH, CHRH, CHR6, CHRI}; // C1-C4-C2-C3
const char  DSMP[]  PROGMEM = {BLNK, CHRM, CHRD, CHRE}; // C1-C4-C2-C3
const char  DSLP[]  PROGMEM = {BLNK, CHRL, BLNK, CHR0}; // C1-C4-C2-C3
const char  DSEND[] PROGMEM = {BLNK, CHRE, CHRD, CHRN}; // C1-C4-C2-C3


ISR(TIMER2_COMPA_vect)
{
  /*
    // Increment TMcnt and let it overflowed (255 -> 256)
    // We just need the condition of bit 0
    //
    if ( ++TMcnt & 0x01 )
    {
    // 2mS period here ...
    OCR2A = 62;
    }
    else
    {
    OCR2A = 61;
    }
  */
  // 16MHz / 64 * 250 counts
  // set OCR2A = 250-1
  //
  // OCR2A = 249;

  // Periodic 5mS timer
  /*
  if ( TMtick > 0 )
  {
    if ( --TMtick == 0 )
    {
      // Timer expires, set flag
      TMtick = 5;
      sema |= SMTICK;		// Inform application
    }
  }
  */


  // Refresh speed every 20mS
  if ( --OSALCTR == 0 )
  {
    // 20mS Timer expires, set flag
    OSALCTR = 20;
    sema |= SMDOTS;     // Inform application

    if ( --TMpre == 0 )
    {
      TMpre = 50;		    // Prescale to 1 Second
      sema |= SMTIME;		// Inform application

      if ( TMtimer[0] )
      {
        // Timer 0
        if ( --TMtimer[0] == 0 )
        {
          // Nothing
        }
      }
    }
  }
  sema |= SMDSCN;
}

void TMtmr2init()
{
  TCCR2A	 = (1 << WGM21);	// Configure timer 2 as CTC (WGM22:WGM21:WGM20 = 010)
  // TCCR2B	 = (1 << CS22) | (1 << CS21);		// Timer at CLKio/256 (CS22:CS21:CS20 = 110)
  TCCR2B     = (1 << CS22);                 // Timer at CLKio/64  (CS22:CS21:CS20 = 100)
  TIMSK2	 |= (1 << OCIE2A);			          // Enable CTC Timer 2A interrupt
  OCR2A	    = 249;					                // Set CTC compare value
  //
  // !!!!! OCR2A count !!!!!
  // 1. Timer increments on each clock pulse
  // 2. When TCNT2 reaches the value in OCR2 (eq. 249), the compare match occurs
  // 3. On then NEXT clock cycle :
  //    3a. Timer reset back to 0
  //    3b. Interrupt flag is set
  //

  // 20MHz / 256 = 78125 Hz = 12.8 uS
  // 1mS = 1000uS = 78.125 * 12.8uS
  // Use 78 * 12.8uS =  998.4uS period x 7
  // And 79 * 12.8uS = 1011.2uS period x 1
  // Result in exact 8mS
  // Use 77 and 78 as value
  //

  // 16MHz / 256 = 62500 Hz = 16uS
  // 1mS = 1000uS = 62.5 * 16uS
  // Use alternate 62 and 63 to make exact 2mS interval
  // 63 * 16uS = 1008 uS
  // 62 * 16uS =  992 uS
  // -------------------+
  //             2000 uS
  // Use 62 and 61 as value
  //

  // 16MHz / 64 = 250 KHz = 4uS
  // 1mS = 1000uS = 250 *4uS
  // Use 250-1 = 249 as count value
  //
  sei();						// Enable global interrupts
  TMcnt	= 0;
  OSALCTR = 20;					// 20mS interval
  TMdelay = 0;					// ...
  TMtick	= 5;					// ...
  TMpre	= 50;					// 1 S interval timer
}


void DSshowtime( uint8_t mnt, uint8_t sec )
{
  // Update display
  DSbuf[DGT0] = pgm_read_byte_near( LKUP + (sec % 10) );
  DSbuf[DGT1] = pgm_read_byte_near( LKUP + (sec / 10) );
  DSbuf[DGT2] = pgm_read_byte_near( LKUP + (mnt % 10) );
  DSbuf[DGT3] = pgm_read_byte_near( LKUP + (mnt / 10) );
}


void setup()
{
  // Port directions
  // 0 - input
  // 1 - output
  //
  PORTC = 0x00;         // Initial state is LOW
  DDRC	= 0x3F;					// PORT C as outputs for Display common
  PORTD = 0xFF;         // Initial state is HIGH
  DDRD	= 0xFF;					// PORT as outputs for 7-segment display
  PORTB = 0x1F;         // Initial state is HIGH except for output
  DDRB	= 0x20;					// PORTB 5 as output, 0-4 inputs
  
  // Variables
  //
  //
  MWstate = MWIDLE;
  DSidx	  = 0;  					// Digit index
  TMhour  = 12;
  TMmin	  = 34;
  TMsec   = 0x80;         // Mark clock as unset
  sema    = 0x00;
  
  // Keyboard initialization
  //
  KBstate = KBIDLE;
  KBcol   = 0;
  KBrow   = 0;
  KBxprev = 0;
  KBxinp  = 0;

  // Timer initialization
  //
  TMtmr2init();

  
  // Serial for debugging only
  //
  // Serial.begin( 115200 );
}




void DSbldot( void )
{
  switch ( MWstate )
  {
    case MWIDLE  :
      // Check if clock has been set
      // TMsec >= 128
      //
      if ( TMsec & 0x80 )
      {
        DSdot = 0x03;
        break;
      }
    case MWSTART :
    case MWWTOPR :
    case MWOPER  :
      if ( TMpre < 25 )
      {
        DSdot = 0x00;    // Blink dots
      }
      else
      {
        DSdot = 0x03;
      }
      break;

    case MWTMRS  :
    case MWPAUSE :
    case MWSTME  :
      if ( TMpre < 25 )
      {
        DSdot = 0x01;    // Toggle upper/lower dots
      }
      else
      {
        DSdot = 0x02;
      }
      break;

    case MWPWRS :
    default :
      DSdot = 0x00;
  }
}


void loop()
{
  // 1mS period ..
  //
  if ( sema & SMDSCN )
  {
    sema &= ~SMDSCN;

    // Check Door
    if( DOOR_OPEN )
    {
      if( sema & SMDOOR )
      {
        // Door is already open, No change
      }
      else
      {
        // Door is opened
        switch ( MWstate )
        {
          case MWWTOPR :
          case MWOPER  :
            // Turn off Magnetron and Heater
            MWMG_OFF;
            MWHT_OFF;
            MWstate = MWPAUSE;
            break;

          default:
            // Turn on Lamp
            MWLAMP_ON;
        }

        // Set flag
        sema = sema | SMDOOR;
      }
    }
    else
    {
      if( sema & SMDOOR )
      {
        // Door is closed
        // Turn off Lamp
        MWLAMP_OFF;
        
        // Clear flag
        sema = sema & ~SMDOOR;
      }
      else
      {
        // Door is already closed, No change
      }
    }
    
    // Display Scan every 1mS
    DSrefr();
  }

  // 20mS period
  //
  if ( sema & SMDOTS )
  {
    sema &= ~SMDOTS;

    // Dot treatment
    DSbldot();
  }

  // 1S period
  if ( sema & SMTIME )
  {
    sema &= ~SMTIME;

    // Call applications
    DSclock();
    MWfsm();
  }
}


void DSprint( const char PROGMEM * ptr )
{
  for ( tmp = 0; tmp < 4; tmp++ )
  {
    DSbuf[tmp] = pgm_read_byte_near( ptr + tmp );
  }
}


void DSclock()
{
  // Display clock here  ...

  if ( TMsec < 128 )
  {
    // Clock has been set
    //
    if ( ++TMsec > 59 )
    {
      TMsec = 0;
      if ( ++TMmin > 59 )
      {
        TMmin = 0;
        if ( ++TMhour > 12 )
        {
          TMhour = 1;
        }
      }
    }
  }

  // Display time
  if ( MWstate == MWIDLE )
  {
    DSshowtime( TMhour, TMmin );
  }
}



void DSrefr( void )
{
  // Disable display
  PORTC = PORTC & 0xF0;

  if ( DSidx == 4 )
  {
    // Dots
    // Disable all segments but dots
    PORTD = DDOT;

    // Check which dot is active
    if ( DSdot & 0x01 )
    {
      // Upper dot (bit 0)
      PORTC = PORTC | (1 << DGT0);
    }

    if ( DSdot & 0x02 )
    {
      // Lower dot (bit 2)
      PORTC = PORTC | (1 << DGT1);
    }
    // Continue to Keyscan
    DSidx++;
  }
  else if ( DSidx == 5 )
  {
    // Keypad column scan
    // Off the digits
    PORTD = 0x00;
    PORTC = (PORTC & 0xF0) | (1 << KBcol);

    // Read keystatus at lower 4 bits of PORTB
    // Active LOW -> convert to active HIGH
    //

    // Mask the input matrix
    KBxinp &= ~(0x0F << (KBcol * 4));

    // Read current input
    KBrow   = (~PINB) & 0x0F;
    KBxinp |= (KBrow << (KBcol * 4));

    if ( ++KBcol == 4 )
    {
      // Reset column index
      KBcol = 0;

      if ( KBxinp )
      {
        // Find what is newly pressed
        KBxpres = KBxinp & ~KBxprev;
      }
      else
      {
        // Nothing pressed
        KBxprev = 0;
      }

      // If any new key pressed
      while ( KBxpres )
      {
        // Find the lowest bit
        KBxbit = KBxpres & -KBxpres;

        // Count the trailing zero
        KBidx = __builtin_ctz( KBxbit );

        KBch    = pgm_read_byte_near( KBmap + KBidx );
        KBmake();

        // Clear the lowest bit
        KBxpres ^= KBxbit;

        // KBstate = KBIN;
        KBxprev = KBxinp;
      }
    }

    // Reset digit index
    DSidx = 0;
  }
  else
  {
    // Update Data bits
    PORTD = DSbuf[ DSidx ];

    // Enable display
    //
    PORTC = PORTC | (1 << DSidx);

    // Next digit
    DSidx++;
  }
}





void MWfsm( void )
{
  // Main State Machine
  if ( MWstate > MWMAXST ) MWstate = MWIDLE;


  switch ( MWstate )
  {
    case MWIDLE:    // Idle
      break;

    case MWSTART:   // Start operating
      // Turn on FAN and Table and wait a while
      MWstate         = MWWTOPR;
      TMtimer[0]      = 2;
      MWLAMP_ON;

      // Display timer
      DSshowtime( MWmin, MWsec );
      break;

    case MWWTOPR:   // Waiting for FAN
      if ( TMtimer[0] == 0 )
      {
        MWstate = MWOPER;
      }
      break;

    case MWOPER:    // Operating
      // Decrement timer
      //
      if ( MWsec )
      {
        --MWsec;
      }
      else
      {
        // Check if still time
        if ( MWmin )
        {
          MWmin--;
          MWsec = 59;
        }
      }

      // Display timer
      DSshowtime( MWmin, MWsec );


      // Check if Microwave operating time
      if ( MWmin == 0 && MWsec == 0 )
      {
        // Ended
        //
        MWMG_OFF;
        MWLAMP_OFF;
        MWstate = MWENDS;
        DSprint( DSEND );
      }
      else
      {
        // Still time to operate
        //
        if ( ++MWtmr >= MWton )
        {
          // End of ON duty time
          MWMG_OFF;
        }
        else
        {
          // At ON period
          MWMG_ON;
        }

        if ( MWtmr >= MWCTIM )
        {
          // Cycle time elapsed
          // Reset duty counter
          //
          MWtmr = 0;
        }
      }
      break;

    case MWSTME:    // Set time
      // No change
      break;

    case MWPWRS:    // Micro Power Select
      // No change
      break;

    case MWPAUSE:   // Paused
      // No change
      break;

    case MWENDS:    // Ended
      // No change
      break;

    default:        // Nochange
      break;
  }
}




void KBmake( void )
{
  switch ( KBch )
  {
    case '0' :  // Micro Power
      switch ( MWstate )
      {
        case MWIDLE  :  // Select High Power
          MWstate = MWPWRS;

        case MWPWRS  :  //
          MWton = 25;
          DSprint( DSHP );
      }
      break;

    case '1' :  // Micro Power
      switch ( MWstate )
      {
        case MWIDLE  :  // Select Medium Power
          MWstate = MWPWRS;

        case MWPWRS  :  //
          MWton = 15;
          DSprint( DSMP );
      }
      break;

    case '2' :  // Micro Power
      switch ( MWstate )
      {
        case MWIDLE  :  // Select Low Power
          MWstate = MWPWRS;

        case MWPWRS  :  //
          MWton = 5;
          DSprint( DSLP );
      }
      break;

    case '3' :  // 10 Min
      switch ( MWstate )
      {
        case MWPWRS  :  //
          MWstate = MWTMRS;

        case MWTMRS  :  //
          MWmin += 10;
          if ( MWmin >= 60 )
          {
            MWmin -= 60;
          }
          // Display time
          DSshowtime( MWmin, MWsec );
          break;

        case MWSTME  :  //
          if ( ++TMhour > 12 ) TMhour = 1;
          DSshowtime( TMhour, TMmin );
          break;
      }
      break;

    case '8' :  // 1 Min
      switch ( MWstate )
      {
        case MWPWRS  :  //
          MWstate = MWTMRS;

        case MWTMRS  :  //
          MWmin += 1;
          if ( MWmin >= 60 )
          {
            MWmin -= 60;
          }
          // Display time
          DSshowtime( MWmin, MWsec );
          break;

        case MWSTME  :  //
          TMmin += 10;
          if ( TMmin >= 60 ) TMmin -= 60;
          DSshowtime( TMhour, TMmin );
          break;
      }
      break;

    case '9' :  // 10 Sec
      switch ( MWstate )
      {
        case MWPWRS  :  //
          MWstate = MWTMRS;

        case MWTMRS  :  //
          MWsec += 10;
          if ( MWsec >= 60 )
          {
            MWsec -= 60;
          }
          // Display time
          DSshowtime( MWmin, MWsec );
          break;

        case MWSTME  :  //
          TMmin += 1;
          if ( TMmin >= 60 ) TMmin -= 60;
          DSshowtime( TMhour, TMmin );
          break;
      }
      break;

    case 'F' :  // Set time
      switch ( MWstate )
      {
        case MWIDLE  :  //
          MWstate = MWSTME;
          break;

        case MWSTME  :  //
          MWstate = MWIDLE;
          TMsec = 0;
          break;
      }
      break;

    case '4' :  // Pause / Reset
      // Turn off everything
      //
      MWMG_OFF;
      MWHT_OFF;
      MWLAMP_OFF;

      switch ( MWstate )
      {
        case MWWTOPR :
        case MWOPER  :
          MWstate = MWPAUSE;
          break;

        default      :
          // Reset on duty timer
          MWton   = 0;
          MWmin   = 0;
          MWsec   = 0;

          // Back to clock display
          MWstate = MWIDLE;
          DSshowtime( TMhour, TMmin );
          break;
      }
      break;

    case '5' :  // Start
      // Ignore if door is still open
      if( DOOR_OPEN ) break;
      
      switch ( MWstate )
      {
        case MWTMRS  :  // Runs only when ready
          // Set initial duty timer
          MWtmr   = 0;

        case MWPAUSE :  // Continue from last
          MWstate = MWSTART;
          break;
      }
      break;
  }
}
