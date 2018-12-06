#include "msp.h"
#include <stdio.h>

int input = 0;
int high = 0;
long period = 3018000;
long count = 0;

volatile long *systick_control_reg =       (volatile long *)0xE000E010 ;  // STCSR
volatile long *systick_reload_value_reg =  (volatile long *)0xE000E014 ;  // STRVR
volatile long *systick_current_value_reg = (volatile long *)0xE000E018 ;  // STCVR
volatile long *systick_calibration_reg =   (volatile long *)0xE000E01C ;  // STCR

/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer


	*systick_reload_value_reg = period;
	    // Start timer.  Per table 2-54 of the MSP432P401R Technical reference manual.
	*systick_control_reg = 1 ;

	P6SEL0 = 0 ;  // This sets all 8 bits in the "port 2 sel 0 register" to zero.
	P6SEL1 = 0 ;  // Likewise for the sel 1 register.
	P6DIR |= BIT1;
	P6DIR &= ~BIT4;

	while(1){

	    if(*systick_current_value_reg < 200){
	        printf("Count: %d\n", count);
	        count = 0;
	    }
	    if((P6IN & BIT4) && !high){
	        P6OUT |= BIT1;
	        count ++;
	        high = 1;
	    }else if(!(P6IN & BIT4) && high){
	        P6OUT &= ~BIT1;
	        high = 0;
	    }else{
	        P6OUT &= ~BIT1;
	    }

	}
}
