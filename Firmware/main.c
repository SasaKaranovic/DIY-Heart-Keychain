#include "main.h"

volatile uint8_t currentDC               = 0;
volatile uint8_t CapSensePrevious        = 0;
volatile uint8_t CapTMR0Cnt              = 0;
volatile uint8_t CapSenseSensitivity     = 7;
volatile uint8_t targetRed               = 0;
volatile uint8_t REDCurrent              = 0;
volatile uint8_t heartBeatCount          = 0;
volatile uint8_t delay                   = 0;
volatile uint8_t BlinkEnabled            = 0;
uint8_t fadeSide                         = 0;
uint16_t timeout                         = 0;

void main(void) {
    InitializeDevice();
    SetUpCapsense();
    SetUpTimer1();
    
    while(1) {
        if(!STATUSbits.nTO) {
            CLRWDT();
            heartBeatCount      = 0; 
            LED                 = 0;
            targetRed           = 0;
            currentDC           = 0;
            timeout             = 0;
            delay               = 0;
            timeout             = 0;
            BlinkEnabled        = 0;
            INTCONbits.PEIE     = 1;
        }
        
        if( (delay > 6) &&  (heartBeatCount > 0) && BlinkEnabled) {
            
            if(fadeSide == 1) {
                targetRed++;
            }
            else {
                targetRed--;
            }


            if(targetRed >= 254) {
                fadeSide = 0;
            }

            if(targetRed == 0) {
                fadeSide = 1;
                heartBeatCount--;
            }

            delay = 0;
            timeout = 0;
        }
        
        if(timeout >= 2000 && heartBeatCount == 0) {
            heartBeatCount      = 0; 
            LED                 = 0;
            targetRed           = 0;
            currentDC           = 0;
            delay               = 0;
            BlinkEnabled        = 0;
            INTCONbits.PEIE     = 0;
            SLEEP();
        }
        else if(timeout < 2000) {
            CLRWDT();
        }
    }
    
    return;
}

void interrupt ISRRoutine(void)
{
    //Timer0 interrupt {CAP SENSING timer}
    if(INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
        INTCONbits.TMR0IF   = 0;
        TMR0                = 0;
        
//        LED = ~LED;
        
        return;
    }
    
    //Timer1 Interrupt
    //Use Timer 1 as time base
    if(PIE1bits.TMR1IE && PIR1bits.TMR1IF) {
        INTCONbits.PEIE = 0;
        PIR1bits.TMR1IF = 0;
        
        uint8_t diff;
        
        if(TMR0 < CapSensePrevious) {
            diff = CapSensePrevious - TMR0;
        }
        else { diff = TMR0 - CapSensePrevious; }
        
        if(diff < CapSenseSensitivity && !BlinkEnabled ) {
            heartBeatCount      = 3;
            BlinkEnabled        = 1;
            //CLRWDT();
        }
        else { 
//            heartBeatCount      = 0; 
//            LED                 = 0;
//            BlinkEnabled        = 0;
        }
        
        CapSensePrevious = TMR0;
        //TMR0=0;
        
        INTCONbits.PEIE = 1;
        return;
    }


    //Use Timer2 for pulsing the LED
    if(PIE1bits.TMR2IE && PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0;
        //INTCONbits.PEIE = 0;
        currentDC += 1;
        delay += 1;
        timeout++;
        
        if(heartBeatCount) {
            if(currentDC >= 254) { currentDC = 0; }

            if(currentDC < targetRed) { LED = 1; }
            else { LED = 0; }
        }
        else {
            currentDC   = 0;
            LED         = 0;
        }
        
        //TMR2 = 0;
        //INTCONbits.PEIE = 1;
        return;
    }
}


void InitializeDevice() 
{
    /*
            Configure device for 8MHz
     */
    //SCS<1:0>: System Clock Select bits
    OSCCONbits.SCS          = 0b00;     //00 = Clock determined by FOSC<2:0> in Configuration Word 1.

    //IRCF<3:0>: Internal Oscillator Frequency Select bits
    OSCCONbits.IRCF         = 0b1110;   //1110 = 8MHz HF

    //SPLLEN: Software PLL Enable bit   (If PLLEN in Configuration Word 1 = 0:)
    OSCCONbits.SPLLEN       = 0;        //0 = 4x PLL is disabled

    //Configure pins
    TRISLED                 = 0;
    TRISTOUCH               = 1;
    ANSELTOUCH              = 1;
    LED                     = 0;
    
    OPTION_REGbits.nWPUEN   = 1;
    WPUA                    = 0;

   /*
           Timer2 Setup for heartbeat effect
    */
   //T2OUTPS<3:0>: Timer2 Output Postscaler Select bits
   T2CONbits.T2OUTPS   = 0b0000;   //0000 = 1:1 Postscaler

   //T2CKPS<1:0>: Timer2 Clock Prescale Select bits
   T2CONbits.T2CKPS    = 0b00;     //00 = Prescaler is 1

   //TMR2ON: Timer2 On bit
   T2CONbits.TMR2ON    = 1;        //1 = Timer2 is on
   
   
   //Configure WatchDog Timer
   WDTCONbits.WDTPS = 0b01010;      //01010 = 1:32768 (Interval 1s typ)


    /*
            Configure INTERRUPT CONTROL REGISTER
    */
    PIE1bits.TMR2IE = 1;    //1 = Enables the Timer2 to PR2 match interrupt
    INTCONbits.PEIE = 1;    //1 = Enables all active peripheral interrupts
    INTCONbits.GIE  = 1;    //1 = Enables all active interrupts
}


void SetUpCapsense() 
{
    CPSCON0bits.CPSON       = 1;    //1 = CPS module is enabled
    CPSCON0bits.CPSRM       = 0;    //0 = CPS module is in the low range. Internal oscillator voltage references are used.
    CPSCON0bits.CPSRNG      = 0b01; //10 = Oscillator is in Medium Range. Charge/Discharge Current is nominally 1.2 ?A
    CPSCON1bits.CPSCH       = 0b0011;   //0011 = channel 3, (CPS3) -> RA4

    OPTION_REGbits.TMR0CS   = 1 ;    //0 = Internal instruction cycle clock (FOSC/4)
    OPTION_REGbits.PSA      = 0;    //0 = Prescaler is assigned to the Timer0 module
    CPSCON0bits.T0XCS       = 1;    //1 = Timer0 clock source is the capacitive sensing oscillator

    INTCONbits.T0IE         = 0;    //Timer0 Interrup is disabled
}


void SetUpTimer1() 
{
    /*
            Timer1 Interrupt and Enable
     */
    PIE1bits.TMR1IE     = 1;
    T1CONbits.TMR1CS    = 0b00; //00 =Timer1 clock source is instruction clock (FOSC/4)
    T1CONbits.T1CKPS    = 0b10; //11 = 1:8 Prescale value
    T1CONbits.TMR1ON    = 1;    //1 = Enables Timer1
}


uint8_t FadeInOut(uint8_t current, uint8_t up) {
    uint8_t add = 0;

    if(up == 1) {
        if( current >= 250 ) { add = 1; }
        else if( current >= 200 ) { add = 1; }
        else if (current >= 100) { add = 1; }
        else if (current >= 50)  { add = 1; }
        else { add = 1; }
        return (current+add);
    }
    else {
        if( current >= 200 ) { add = 1; }
        else if (current >= 100) { add = 1; }
        else if (current >= 50)  { add = 1; }
        else { add = 1; }

        return (current-add);
    }
}

