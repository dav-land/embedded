#include "msp.h"
//systick initialization
volatile long *systick_control_reg =       (volatile long *)0xE000E010 ;  // STCSR
volatile long *systick_reload_value_reg =  (volatile long *)0xE000E014 ;  // STRVR
volatile long *systick_current_value_reg = (volatile long *)0xE000E018 ;  // STCVR
volatile long *systick_calibration_reg =   (volatile long *)0xE000E01C ;  // STCR
long period = 3018000; //one second

int interuptCount = 0;
void TA0_0_IRQHandler()
{
    TIMER_A0->CCTL[0] &= ~0x0001;
    P2OUT ^=  BIT5;

}
//when the timer gets to the duty cycle this is triggered
void TA0_1_IRQHandler()                 // Interrupt handler for TimerA0.1
{
    TIMER_A0->CCTL[1] &= ~0x0001;
    P2OUT ^= BIT7;

}


/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	*systick_reload_value_reg = period;
	*systick_control_reg = 1;


    P2SEL0 = 0;
    P2SEL1 = 0;


    TIMER_A0->CCTL[0] = 0x0010;    // CCI0 toggle
    TIMER_A0->CCR[0]  = 3028/2;    // Period will vary with the clock settings.  You can measure it.
    TIMER_A0->EX0     = 0x0000;    // Divide by 1
    TIMER_A0->CCTL[1] = 0x0010;    // CCR1 toggle/reset
    TIMER_A0->CCR[1]  = 1000;    // CCR1 duty cycle is duty1/period
    TIMER_A0->CTL     = 0x0210;    // SMCLK=12MHz, divide by 1, up-down mode


	P3SEL0 = 0;
	P3SEL1 = 0;
	P3DIR |= BIT6;
	P3DIR |= BIT7;

	P2DIR |=  BIT5;
	P2DIR |=  BIT7;
	int i;
	P1SEL0 = 0;
	P1SEL1 = 0;
	P1DIR |=  BIT6;
	P1DIR |=  BIT7;
    P1OUT &= ~BIT6;
    P1OUT &= ~BIT7;

	int timeCount = 0,time = 0, timeCount2=0, time2=0;
	NVIC_EnableIRQ(TA0_0_IRQn);
	NVIC_EnableIRQ(TA0_1_IRQn);
	__enable_irq();

	while(1)
	{

	    //time = 0;
	    P7DIR |= BIT2;
	    P7OUT |= BIT2;
	    for(i = 0; i < 100; i++);
	    P7DIR &= ~BIT2;
	    timeCount = *systick_current_value_reg;
	    while(P7IN & BIT2);
	    time = timeCount - *systick_current_value_reg;
	    if(time < 0)
	    {
	        time += period;
	    }

	    P7DIR |= BIT5;
	    P7OUT |= BIT5;
	    for(i = 0; i < 100; i++);
	    P7DIR &= ~BIT5;
	    timeCount2 = *systick_current_value_reg;
	    while(P7IN & BIT5);
	    time2 = timeCount2 - *systick_current_value_reg;
	    if(time2 < 0)
	    {
	        time2 += period;
	    }

	    if(time < 8000 && time2 < 8000)
	    {

	        TIMER_A0->CCR[1] = 2000;
	        TIMER_A0->CCR[0] = 2000;
	    }
	    else if(time > 8000)
	    {
	        TIMER_A0->CCR[0] = 2000;
	        TIMER_A0->CCR[1] = 1500;
	    }
	    else if(time2 > 8000)
	    {
	        TIMER_A0->CCR[1] = 2000;
	        TIMER_A0->CCR[0] = 1500;
	    }


	}
}
