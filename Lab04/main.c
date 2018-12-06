#include "msp.h"
#include <stdio.h>



// Define the locations for some of the ADC registers.  This is a parallel method that works side by side
   // with the "CMSIS" method of defines that involve pointers and
   // structures, which I feel is unduely complicated.
   // The following defines the hardware address's of a subset of the ADC setup registers.
   // Reference:  MSP432P401R Data Sheet, table 6-31 "Precsion ADC Registers" (page 107 of the pdf file)
   // Also see: section 22.3 "ADC Registers" of the MSP432P4xx Technical reference manual.
   volatile long *adc14_control_register_0 =          (volatile long *)0x40012000 ;  // ADCTL0
   volatile long *adc14_control_register_1 =          (volatile long *)0x40012004 ;  // ADCTL1
   volatile long *adc14_memory_control_register_0 =   (volatile long *)0x40012018 ;  // ADCMEMCTL0
   volatile long *adc14_memory_register_0 =           (volatile long *)0x40012098 ;  // ADC14MEM0
   volatile long *adc14_interrupt_flag_0_register =   (volatile long *)0x40012144 ;  // ADC14IFGR0
   volatile long *adc14_interrupt_enable_0_register = (volatile long *)0x4001213C ;  // ADC14IER0
   volatile long *adc14_interrupt_enable_1_register = (volatile long *)0x4001213C ;  // ADC14IER1

// These "defines" allows us to write code using the exact same names that appear in the data sheet, like we can
// for the port setup registers.  Due to legacy and portability issues, the TI compiler sometimes uses
// different methods of defining address's in code.
#define ADC14CTL0     *adc14_control_register_0
#define ADC14CTL1     *adc14_control_register_1
#define ADC14MCTL0    *adc14_memory_control_register_0
#define ADC14MEM0     *adc14_memory_register_0
#define ADC14IFGR0    *adc14_interrupt_flag_0_register
#define ADC14IER0     *adc14_interrupt_enable_0_register
#define ADC14IER1     *adc14_interrupt_enable_1_register

   long period = 3018000/1000;

   volatile long *systick_control_reg =       (volatile long *)0xE000E010 ;  // STCSR
   volatile long *systick_reload_value_reg =  (volatile long *)0xE000E014 ;  // STRVR
   volatile long *systick_current_value_reg = (volatile long *)0xE000E018 ;  // STCVR
   volatile long *systick_calibration_reg =   (volatile long *)0xE000E01C ;  // STCR



int adc[10], count = 0, i;
float voltages[10], percent[10];

/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	for(i = 0; i < 10 ; i++){
	    adc[i] = 0;
	}

	*systick_reload_value_reg = period;
	        // Start timer.  Per table 2-54 of the MSP432P401R Technical reference manual.
	*systick_control_reg = 1 ;

	// Diagnostic printing...
    printf("Set up ADC for input from channel 6\n") ;

    // Set up the ADC.  These bits are all defined in the MSP432P4xx Technical Reference Manual...
    ADC14CTL0 &= 0xFFFFFFFD ;    // Note that in this bit mask, only bit 1 is a zero.
    ADC14CTL0 |= 0x00000010 ;    // ADC14 on
    ADC14CTL0 |= 0x04000000 ;    // Source signal from the sampling timer       ****

    // You must consult the TRM and decide how to set up this register.  The other have been done for you.
    ADC14CTL1 =  0x00000030 ;    // Students must set this one up:  ADC14MEM0, 14-bit, ref on, regular power

    ADC14MCTL0 = 0x00000086 ;    // 0 to 3.3V, channel A6?? (student must figure this out in Lab 0.4)
    ADC14IER0 =  0 ;             // no interrupts
    ADC14IER1 =  0 ;             // no interrupts
    ADC14CTL0 |= 0x00000002 ;    // enable    But doesn't the core have to be on? (bit 4).  Original code.

    P2DIR  |=  0x10 ;              // P2.4 output (bit 4 is set)
    P2SEL0 |=  0x10 ;              // P2.4 Timer0A functions (choose PWM as the source)
    P2SEL1 &= ~0x10 ;              // P2.4 Timer0A functions

    TIMER_A0->CCTL[0] = 0x0080;    // CCI0 toggle
    TIMER_A0->CCR[0] =  0x0010;    // Period will vary with the clock settings.  You can measure it.
    TIMER_A0->EX0 =     0x0000;    // Divide by 1
    TIMER_A0->CCTL[1] = 0x0040;    // CCR1 toggle/reset
    TIMER_A0->CCR[1] =  0x0001;    // CCR1 duty cycle is duty1/period
    TIMER_A0->CTL =     0x0230;    // SMCLK=12MHz, divide by 1, up-down mode


    //Setup Pin analog input to pin 4.7
    P4SEL0 |= BIT7;
    P4SEL1 |= BIT7;
    P4DIR &= ~BIT7;
    P4DIR |= BIT0;

	    while(1)
	    {
	        //while(*systick_current_value_reg >200){}
	        ADC14CTL0 |= 0x00000001;
	        while(ADC14CTL0 & BIT0){}

	        if(count > 9)
	            count = 0;
	        adc[count] = ADC14MEM0;
	        percent[count] = (float)adc[count]/16383;
	        TIMER_A0->CCR[1] = TIMER_A0->CCR[0]*percent[count];
	        voltages[count] = (float)(adc[count])/16383.0*3.3;
	      //  printf("Value: %5d, Voltage: %f\n",adc[count], voltages[count]);
	        count ++;
	    }
}
