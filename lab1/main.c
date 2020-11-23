#include <msp430.h> 

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	P1DER &=~ BIT7;
	P1REN |= BIT7;
	P1OUT |= BIT7;

	P2DER &=~ BIT2;
	P2REN |= BIT2;
	P2OUT |= BIT2;

	P1DER |= BIT2;
	P1OUT &=~ BIT2;

	int counter = 0;


	while(1) {

	}

	
	return 0;
}
