///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ECEN 2440 Simple "blink LED" Demo.
//
// Start Date: 8-24-18
// More work done 8-28 and 8-29-18
//
// Target: Texas Instruments MSP-EXP432P401 dev kit
//
// Author: James Zweighaft, University of Colorado Boulder
//
// This code is design to demonstrate the simplest possible project on the MSP 432 dev board- blink an LED.
//

#include "msp.h"
#include <stdio.h>

long delay ;
long delay_limit_on = 1 ;
long delay_limit_off = 5000 ;
int increasing = 1;

volatile long *systick_control_reg =       (volatile long *)0xE000E010 ;  // STCSR
volatile long *systick_reload_value_reg =  (volatile long *)0xE000E014 ;  // STRVR
volatile long *systick_current_value_reg = (volatile long *)0xE000E018 ;  // STCVR
volatile long *systick_calibration_reg =   (volatile long *)0xE000E01C ;  // STCR




void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;  // See section 17 of the MSP432 Tech Ref manual


	// Title line verifies that the code has started to execute.
	printf("\r\n\n\nMSP 432 Blink LED Demo\n\n") ;


	P2SEL0 = 0 ;  // This sets all 8 bits in the "port 2 sel 0 register" to zero.
	P2SEL1 = 0 ;  // Likewise for the sel 1 register.

	P2DIR = 1 ;  // Configures bit 0 of Port 2 to be an output while all the other bits will be inputs.

	P2DS = 1  ;  // Set bit 0 to be high drive strength which needed to drive an LED.

	//Reload value
	*systick_reload_value_reg = delay_limit_on;
	// Start timer.  Per table 2-54 of the MSP432P401R Technical reference manual.
	*systick_control_reg = 1 ;

	while(1)
	{
	    if(increasing){
	        delay_limit_on += 1;
	        if(delay_limit_on > 512)
	            increasing = 0;
	    }else{
	        delay_limit_on -= 1;
	        if(delay_limit_on < 1)
	            increasing = 1;
	    }


		P2OUT = 1 ;

	    //*systick_reload_value_reg = delay_limit_on ;
		//printf("on\n");
		while(*systick_current_value_reg <= 0){}
		    //do nothiung
		*systick_reload_value_reg = 512- delay_limit_on ;
		// Turn off the LED by clearing P2.  Note that all bits (8? 16? 32?) of this register will be cleared
		// which will have side effects.
		P2OUT = 0 ;

		// Very basic software delay.  Using the systick timer is a better way.



		while( *systick_current_value_reg <= 0){}
		//printf("off\n");
		*systick_reload_value_reg = delay_limit_on ;

	}


}


/////////////////////////////////////////////// end of main.c /////////////////////////////////////////////////////////
