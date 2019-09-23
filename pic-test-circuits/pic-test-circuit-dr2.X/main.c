/*
 * File:   main.c
 * Author: seitzaj
 *
 * Created on October 31, 2018, 2:51 PM
 */

#include <xc.h>

// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator: High-speed crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR21V   // Brown-out Reset Selection bit (Brown-out Reset set to 2.1V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#define TMR_START 65505

// Function prototypes
void __interrupt () interrupt_handler(void);
void Timer_CCP_init(void);

void main(void) {
    // Disable analog inputs
    ANSEL = 0;
    ANSELH = 0;
    
    // Enable pull-up resistors on PORTB
    TRISB0 = 0x01; // Set RB0 as input
    nRBPU = 0; 
    
    // Enable PORTB interrupt-on-change
    IOCB0 = 1;
    RBIF = 0;
    RBIE = 1;
    
    Timer_CCP_init();
    
    GIE = 1;

    RB1 = 0;
    
    while(1);
}

void Timer_CCP_init(void)
{
	TMR1GE=0;   TMR1ON = 1; 		//Enable TIMER1 (See Fig. 6-1 TIMER1 Block Diagram in PIC16F887 Data Sheet)
	TMR1CS = 0;  					//Select internal clock whose frequency is Fosc/4, where Fosc = 20 MHz
	T1CKPS1 = 0; T1CKPS0 = 0; 		//Set prescale to divide by 1 yielding a clock tick period of 0.2 microsecond
    
	CCP1M3 = 0;CCP1M2 = 0;CCP1M1 = 1;CCP1M0 = 0; 	// Set to Compare Mode, toggle on match
	TRISC2 = 0;                     //Make CCP1 pin an output.
	
	CCP1IF = 0;
    
    CCPR1 = TMR_START;
    CCP1IE = 1;
    
	PEIE = 1;                       // Enable timer interrupts
}

unsigned char half_period;

void interrupt_handler()
{ 
    if ((RBIF == 1)){ // Check for interrupt
        half_period = TMR1 - TMR_START;
        CCPR1 = TMR_START;
        
        // Accept 125 kHz (4 us half-period = 20 ticks) 
        //     to 250 kHz (2 us half-period = 10 ticks)
        RB1 = ((10 <= half_period) && (half_period <= 20));
        RBIF = 0;  // Clear interrupt
    }  
    
    if ((CCP1IF == 1)){    
        // Timer overflow is an invalid clock (too slow)
        CCPR1 = TMR_START;
        
        RB1 = 0;
        CCP1IF = 0;
    }
}