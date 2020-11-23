#include <msp430.h>

int CURRENT_TIMER = 1; // 0 - WT, 1 - TA1
int INTERRUPTS_COUNT = 0;
int LED1_REQ = 0;
int LED2_REQ = 0;
int LED3_REQ = 0;

int CURRENT_BUTTON = 0; //0 - S1, 1 - S2


void startTimerA2() {
    TA2CCR0 = 1023;
    TA2CCTL0 = CCIE; //interrupt by the end of timer
    TA2CTL = TASSEL__ACLK | ID__8 | MC__UP | TACLR;
    P1OUT|=BIT1;
    //frequency - 32768/8 = 4096 Hz
}

void stopTimerA2() {
    TA2CTL = MC__STOP;
    //MC__STOP - Timer A mode control: Stop
    P1OUT&=~BIT1;
}

void startWatchDogTimer() {
    SFRIE1 |= WDTIE; //enable WDT interrupts
    WDTCTL = WDTPW | WDTSSEL_1 | WDTTMSEL | WDTCNTCL | WDTIS_5;
    //WDTPW - WDT Password
    //WDTSSEL_1 - WDT Source: ACLK
    //WDTTMSEL - WDT mode select (interval timer mode)
    //WDTCNTCL - WDT Counter clear
    //WDTIS_5 - WDT interval select: (WDT clock source)/2^13=32768/8192=250ms
    P1OUT|=BIT2;
}

void stopWatchDogTimer() {
    SFRIE1 &= ~WDTIE; //disable WDT interrupts
    WDTCTL = WDTPW | WDTHOLD;
    //WDTHOLD - stop the WDT
    P1OUT&=~BIT2;
}

void turnOnLED(){
    if(LED1_REQ && LED2_REQ && LED3_REQ){
        P1OUT &= ~BIT0;
        P8OUT &= ~BIT2;
        P8OUT &= ~BIT1;
        LED1_REQ = LED2_REQ = LED3_REQ = 0;
    } else {
        if(LED1_REQ){
            P1OUT |= BIT0;
            LED1_REQ = 0;
        }
        if(LED2_REQ){
            P8OUT |= BIT1;
            LED2_REQ = 0;
        }
        if(LED3_REQ){
            P8OUT |= BIT2;
            LED3_REQ = 0;
        }
    }
}

#pragma vector = TIMER2_A0_VECTOR
__interrupt void TIMER_2 (void) {
    if(LED1_REQ || LED2_REQ || LED3_REQ){
        INTERRUPTS_COUNT++;
        if(INTERRUPTS_COUNT == 3) {
            turnOnLED();
            INTERRUPTS_COUNT = 0;
        }
    }
}

#pragma vector = WDT_VECTOR
__interrupt void WDT_interrupt(void) {
    if(LED1_REQ || LED2_REQ || LED3_REQ){
        INTERRUPTS_COUNT++;
        if(INTERRUPTS_COUNT == 3) {
            turnOnLED();
            INTERRUPTS_COUNT = 0;
        }
    }
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void TIMER1(void) {

    if(CURRENT_BUTTON == 0 && !(P1IN & BIT7)) {
        if((P1IN & BIT0) && (P8IN & BIT2) && (P8IN & BIT1)){
            LED1_REQ = 1;
            LED2_REQ = 1;
            LED3_REQ = 1;
        } else if(!(P1IN & BIT0)){
            LED1_REQ = 1;
        } else if(!(P8IN & BIT1)) {
            LED2_REQ = 1;
        } else {
            LED3_REQ = 1;
        }
    }

    if(CURRENT_BUTTON == 1 && !(P2IN & BIT2)){
        if(CURRENT_TIMER == 0) {
            stopWatchDogTimer();
            CURRENT_TIMER = 1;
            startTimerA2();
        } else {
            stopTimerA2();
            CURRENT_TIMER = 0;
            startWatchDogTimer();
        }
    }

    TA1CTL = MC__STOP;

    P2IE |= BIT2;
    P2IFG &= ~BIT2;

    P1IE |= BIT7;
    P1IFG &= ~BIT7;
}

void startTimerA1(){
    P1IE &= ~BIT7;
    P2IE &= ~BIT2;

    TA1CCR0 = 2000;
    TA1CCTL0 = CCIE;
    TA1CTL = TASSEL_1 | ID_0 | MC__UP | TACLR;
}

#pragma vector = PORT1_VECTOR
__interrupt void S1(void)
{
   CURRENT_BUTTON = 0;
   startTimerA1();

}

#pragma vector = PORT2_VECTOR
__interrupt void S2(void)
{
    CURRENT_BUTTON = 1;
    startTimerA1();
}


int main(void){
    WDTCTL = WDTPW | WDTHOLD;

    //S1
    P1DIR &= ~BIT7;
    P1OUT |= BIT7;
    P1REN |= BIT7;
    P1IE |= BIT7;
    P1IES |= BIT7;
    P1IFG &= ~BIT7;

    //S2
    P2DIR &= ~BIT2;
    P2OUT |= BIT2;
    P2REN |= BIT2;
    P2IE |= BIT2;
    P2IES |= BIT2;
    P2IFG &= ~BIT2;

    //LED1
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;

    //LED2
    P8DIR |= BIT1;
    P8OUT &= ~BIT1;

    //LED3
    P8DIR |= BIT2;
    P8OUT &= ~BIT2;

    //TA2 LED
    P1DIR |= BIT1;
    P1OUT &= ~BIT1;

    //WDT LED
    P1DIR |= BIT2;
    P1OUT &= ~BIT2;

    //P1.3/TA0.2
    P1DIR |= BIT3;
    P1OUT &= ~BIT3;
    P1SEL |= BIT3;

    TA0CCR0 = 24575;        //reset out when count to 24575
                            //32768*1,5/2-1
    TA0CCR2 = 4915;         //toggle out when count to 4915
                            //24575/5
    TA0CCTL2 = OUTMOD_2;    //toggle/reset

    TA0EX0 = TAIDEX_0;
    TA0CTL = TASSEL__ACLK | ID_0 | MC__UPDOWN | TACLR;
    //final frequency of timer - 32768 Hz (32768/(1*1))

    startTimerA2();


    __bis_SR_register(GIE);
    __no_operation();
    return 0;
}
