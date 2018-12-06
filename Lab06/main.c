#include "msp.h"
int count = 0;

//systick initialization
volatile long *systick_control_reg =       (volatile long *)0xE000E010 ;  // STCSR
volatile long *systick_reload_value_reg =  (volatile long *)0xE000E014 ;  // STRVR
volatile long *systick_current_value_reg = (volatile long *)0xE000E018 ;  // STCVR
volatile long *systick_calibration_reg =   (volatile long *)0xE000E01C ;  // STCR
long period = 3018000*2;
/*
void TA0_0_IRQHandler()
{
    TIMER_A0->CCTL[0] &= ~0x0001;
    count++;
    if(count > 3)
    {
        count = 0;
    }
}
*/
/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	*systick_reload_value_reg = period;
	*systick_control_reg = 1;

	P3SEL0 = 0;
	P3SEL1 = 0;
	P3DIR |= BIT6;
	P3DIR |= BIT7;

	P2SEL0 = 0;
	P2SEL1 = 0;
	P2DIR |=  BIT6;
	P2DIR |=  BIT7;

	P1SEL0 = 0;
	P1SEL1 = 0;
	P1DIR |=  BIT6;
	P1DIR |=  BIT7;
/*
	//sets up the timer
	TIMER_A0->CCTL[0] = 0x0010;    // CCI0 toggle
	TIMER_A0->CCR[0]  = 3028*1000;      //.5 hz
	TIMER_A0->EX0     = 0x0000;    // Divide by 1
	TIMER_A0->CCTL[1] = 0x0010;    // CCR1 toggle/reset
	TIMER_A0->CCR[1]  = 100;    // CCR1 duty cycle is duty1/period
	TIMER_A0->CTL     = 0x0210;    // SMCLK=12MHz, divide by 1, up-down mode

	NVIC_EnableIRQ(TA0_0_IRQn);
	__enable_irq();
*/

	P2OUT = 0;
	while(1)
	{
	    P1OUT |= BIT7;  //Left wheel forward
	    P2OUT |= BIT7;
	    while(*systick_current_value_reg > 0) {}

	    P1OUT &= ~BIT7;  //Left wheel backwards
	    P2OUT |= BIT7;
	    while(*systick_current_value_reg > 0) {}

	    P1OUT &= ~BIT7; //stops left wheel
	    P2OUT &= ~BIT7;

	    P1OUT |= BIT6; //right wheel forwards
	    P2OUT |= BIT6;
	    while(*systick_current_value_reg > 0) {}

	    P1OUT &= ~BIT6; //right wheel backwards
	    P2OUT |= BIT6;
	    while(*systick_current_value_reg > 0) {}

	    P1OUT &= ~BIT6; //stops right wheel
	    P2OUT &= ~BIT6;

	}

}
