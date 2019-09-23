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

// Function prototypes
void __interrupt () interrupt_handler(void);
void sendchar(unsigned char outchar);
unsigned char recvchar(void);
void configure_uart(void);

// Test delays chosen randomly
unsigned char delays[10] = {48, 90, 30, 49, 44, 16, 28, 85, 25, 15};
unsigned char delay_index = 0;

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
    
    TRISA = 0x00; // RA0 as output
    
    RA0 = 0; RA1 = 0; RA2 = 0;
    
    configure_uart();
    GIE = 1;
    
    while(1)
    {
        unsigned char message[6] = "\0";

        for (unsigned char i=0; i<5; ++i)
        {
            message[i] = recvchar();
        }
        message[5] = '\0';
        
        // imprecise delay period
        for (unsigned int i=0; i<delays[delay_index]; ++i)
        {
            for (unsigned int j=0; j<50000; ++j);
        }
        
        ++delay_index;
        if (delay_index >= 10)
            delay_index = 0;
        
        // Echo message
        for (unsigned char i=0; i<5; ++i)
        {
            sendchar(message[i]);
        }
    }
}

/* UART functions */
void configure_uart(void)
{
    SPEN = 1; SYNC = 0; TXEN = 1;     // Enable transmit side of UART  
    BRG16 = 1; BRGH=1; SPBRG = 42;    // Config UART for 115200 Baud / 20 MHz clock
    CREN = 1;                         // Enable receive side of UART  
}

void sendchar(unsigned char outchar)
{
    while(TXIF == 0);
    TXREG = outchar;
}

unsigned char recvchar(void)
{
    while (RCIF == 0);
    return RCREG;
}
