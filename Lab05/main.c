/**
 * Author: Davis Landry, Daniel Levin
 * Lab 05 Interupts
 */

#include "msp.h"
#include <stdio.h>

//variable to hold what pin is in and what pin to output to
static int isP1 = 1, pin = 0;
static int interuptCount = 0, mainCount = 0;

//systick initialization
volatile long *systick_control_reg =       (volatile long *)0xE000E010 ;  // STCSR
volatile long *systick_reload_value_reg =  (volatile long *)0xE000E014 ;  // STRVR
volatile long *systick_current_value_reg = (volatile long *)0xE000E018 ;  // STCVR
volatile long *systick_calibration_reg =   (volatile long *)0xE000E01C ;  // STCR
long period = 3018000/4;

#define CTLREG *systick_control_reg
#define RELVAL *systick_reload_value_reg
#define CURVAL *systick_current_value_reg
#define CALREG *systick_calibration_reg

//when timer_A0 counts up to the period this is triggered
void TA0_0_IRQHandler()
{
    TIMER_A0->CCTL[0] &= ~0x0001;
    P2OUT ^=  BIT6;

}
//when the timer gets to the duty cycle this is triggered
void TA0_1_IRQHandler()                 // Interrupt handler for TimerA0.1
{
    TIMER_A0->CCTL[1] &= ~0x0001;
 //   P2OUT ^= BIT7;

}

//when one of the side buttons is pressed this is triggered
void PORT1_IRQHandler()         // Interrupt handler for port 1
{
    //determin if it is in 1.1 or 1.4
    if(P1IFG & BIT1)
    {
        isP1 = 1;
    }
    else
    {
        isP1 = 0;

    }
    P1IFG = 0x00000000; //reset pending interupt
    /*
    P2OUT &= ~BIT(pin); //clear output bit
    if(isP1) //increment or decrement pin to show correct color
    {
        pin ++;
    }
    else
    {
        pin --;
    }
    if(pin < 0 ){
        pin = 2;
    }
    if(pin > 2){
        pin = 0;
    }
    //output pin to correct color
    P2OUT |= BIT(pin);
*/
    interuptCount++;

}



/**
 * main.c
 */
void main()
    {
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	RELVAL = period;
	// Start timer.  Per table 2-54 of the MSP432P401R Technical reference manual.


//Sets up all of our 2.* pins as outputs
	P2SEL0 = 0;
	P2SEL1 = 0;
	P2DIR |=  BIT5;
	P2DIR |=  BIT6;
	P2DIR |=  BIT7;
	P2DIR |=  BIT0;
	P2DIR |=  BIT1;
	P2DIR |=  BIT2;
//sets up 1.1 and 1.4 as interupt inputs
	P1SEL0 = 0;
	P1SEL1 = 0;
	P1DIR &= ~BIT1;
	P1DIR &= ~BIT4;
	P1IE |= BIT1;
	P1IE |= BIT4;
	P1REN |= BIT1;
	P1REN |= BIT4;
	P1IES &= ~BIT1;
	P1IES &= ~BIT4;
//sets up the timer
	TIMER_A0->CCTL[0] = 0x0010;    // CCI0 toggle
	TIMER_A0->CCR[0]  = 3028/2;    // Period will vary with the clock settings.  You can measure it.
	TIMER_A0->EX0     = 0x0000;    // Divide by 1
	TIMER_A0->CCTL[1] = 0x0010;    // CCR1 toggle/reset
	TIMER_A0->CCR[1]  = 100;    // CCR1 duty cycle is duty1/period
	TIMER_A0->CTL     = 0x0210;    // SMCLK=12MHz, divide by 1, up-down mode
//enables the different interupts
   //NVIC_EnableIRQ(TA0_0_IRQn);
   //NVIC_EnableIRQ(TA0_1_IRQn);
    NVIC_EnableIRQ(PORT1_IRQn);
	__enable_irq();

	CTLREG = 1 ;
	P2OUT = 0;
	while(1)
	{
	    while(CURVAL > 0 ){}
	    mainCount ++;
	    if(isP1)
	    {
	        P2OUT &= ~BIT(pin);
	    }
	    else if(!isP1)
	    {
	        P2OUT &= ~BIT(pin);
	        pin++;
	        if(pin < 0 ){
	            pin = 2;
	        }
	        if(pin > 2){
	            pin = 0;
	        }
	        //output pin to correct color
	        P2OUT |= BIT(pin);
	    }

	    /*P2OUT |= BIT5;
	    while(CURVAL > 0){}
	    P2OUT &= ~BIT5;
	    while(CURVAL > 0 ){}
	    RELVAL = period;*/
	}
}
