#ifndef MAIN_H
#define	MAIN_H

//Get rid of those ugly unable to resolve __delay_ms, __delay_us errors that aren't exactly erros but bugs in IDE
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000ULL
#endif

#if 1 // to fix stupid IDE error issues with __delay_ms
#ifndef _delay_ms(x)
#define _delay_ms(x) __delay_ms(x)
#endif
#ifndef _delay_us(x)
#define _delay_us(x) __delay_us(x)
#endif
#endif

#include <xc.h>
#include <stdint.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = ON       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF       // PLL Enable (4x PLL enabled)
#pragma config STVREN = OFF      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)


//Definitions
#define LED         LATAbits.LATA2
#define TRISLED     TRISAbits.TRISA2

#define TOUCH       LATAbits.LATA4
#define TRISTOUCH   TRISAbits.TRISA4
#define ANSELTOUCH  ANSELAbits.ANSA4

void InitializeDevice();
void SetUpCapsense();

#define Timer2_On     (T2CONbits.TMR2ON = 1;)
#define Timer2_Off    (T2CONbits.TMR2ON = 0;)

void InitializeDevice();
void SetUpCapsense();
void SetUpTimer1();
uint8_t FadeInOut(uint8_t current, uint8_t up);




#endif	/* MAIN_H */

