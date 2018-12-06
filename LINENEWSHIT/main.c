//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "msp.h"
#include <stdio.h>

int hit_flag = 0;

unsigned int sensor_2_count = 0;
unsigned int sensor_5_count = 0;

int x;

uint32_t current2 = 0;
uint32_t current5 = 0;

volatile long *systick_control_reg = (volatile long *) 0xE000E010; //STCSR
volatile long *systick_reload_value_reg = (volatile long *) 0xE000E014; //STRVR
volatile long *systick_current_value_reg = (volatile long *) 0xE000E018; //STCVR
volatile long *systick_calibration_reg = (volatile long *) 0xE000E01C; //STCR
/*
void TA1_0_IRQHandler()
{
    sensor_2_count++;
    TIMER_A1 -> CCTL[0] &= ~BIT0;
}
*/
/**
 * main.c
 */
void main(void)

{


    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    //the left wheel direction and the right wheel direction
        //set up as GPIO output pins
        P1SEL1 = 0;
        P1SEL0 = 0;
        P1DIR |= BIT6;      //right wheel PWM is pin 2.6
        P1DIR |= BIT7;      //left wheel PWM is pin 2.7
        P1OUT &= ~(BIT6 | BIT7);

        //the left wheel pwm and right wheel pwm
        //set up as GPIO output pins
        P2SEL1 &= ~(BIT6|BIT7);
        P2SEL0 |= (BIT6|BIT7);
        P2DIR |= BIT6;      //right wheel PWM is pin 2.6
        P2DIR |= BIT7;      //left wheel PWM is pin 2.7



        P2DIR |= BIT0 | BIT1 | BIT2;        //leds are outputs
//      P2OUT = 0x00;

        //P2OUT &= ~BIT6;     //turns right wheel off to start
        //P2OUT &= ~BIT7;     //turns left wheel off to start

        //setting up sensors 2 and 5
        P7SEL1 = 0;
        P7SEL0 = 0;

        P5SEL1 = 0;
        P5SEL0 = 0;

        //set up timer, used to measure how long it takes for voltage to drop
        TIMER_A0->CCTL[0] = 0x0000;      // CCI0 toggle, interrupt
        TIMER_A0->CCR[0] =  1000;         // Period will vary with the clock settings.  You can measure it.
        TIMER_A0->EX0 =     0x0007;      // Divide by 8

        //right wheel timer
        TIMER_A0->CCTL[3] = 0x00C0;      // CCR3 toggle, interrupt not enabled
        TIMER_A0->CCR[3] =  200;          // CCR3 duty cycle is duty1/period
        TIMER_A0->CTL =     0x0210;      // SMCLK=12MHz, divide by 1

        //left wheel timer
        TIMER_A0->CCTL[4] = 0x00C0;      // CCR4 toggle/set
        TIMER_A0->CCR[4] =  200;          // CCR4 duty cycle is duty1/period
        TIMER_A0->CTL =     0x0210;      // SMCLK=12MHz, divide by 1


        //bump switch guys

        P4SEL0 = 0x00; // 0000 0000
        P4SEL1 = 0x00; // 0000 0000

        P4DIR &= ~BIT3; // 1.1 is input
        P4DIR &= ~BIT5; // 1.4 is input

        P4IE |= BIT3; //  enable interrupt
        P4IE |= BIT5; //  enable interrupt

        P4IES = 0x00; // toggle on rise

        P4REN |= BIT3; // pull up resistor
        P4REN |= BIT5; // pull up resistor

        NVIC_EnableIRQ(PORT4_IRQn);
        __enable_irq();


        NVIC_EnableIRQ(TA1_0_IRQn);     //enables timer A1
        *systick_reload_value_reg = 301800;

        while(1)
        {
            //P2OUT |= BIT6 | BIT7;
            P7DIR |= BIT1;      //bit 1 corresponds to sensor 2, bit 4 corresponds to bit 5
            P7OUT |= BIT1;      //sets to high output -> 1

            int i;
            for(i=0;i<100;i++);    //allowing time for the sensor output to rise

            P7DIR &= ~BIT1;         //sets sensor 2 to be an input right sensor

            *systick_control_reg = 1;
            current2 = *systick_current_value_reg;
            while(P7IN & BIT1);
            sensor_2_count = current2 - *systick_current_value_reg;
            *systick_control_reg = 0;



            P7DIR |= BIT7;      //bit 1 corresponds to sensor 2, bit 4 corresponds to bit 5
            P7OUT |= BIT7;      //sets to high output -> 1

            for(i=0;i<100;i++);    //allowing time for the sensor output to rise

            P7DIR &= ~BIT7;         //sets sensor 5 to be an input left sensor

            *systick_control_reg = 1;
            current5 = *systick_current_value_reg;
            while(P7IN & BIT7);
            sensor_5_count = current5 - *systick_current_value_reg;
            *systick_control_reg = 0;


            P2OUT &= ~(BIT1 | BIT0 | BIT2);

            TIMER_A0->CCR[4] = 200;           // CCR3 duty cycle is duty1/period
            TIMER_A0->CCR[3] = 200;           // CCR4 duty cycle is duty1/period

            // black
            if ((sensor_2_count < 8000) && (sensor_5_count > 10000))
            {
                P2OUT |= BIT0;
                TIMER_A0->CCR[4] = 100;           // CCR3 duty cycle is duty1/period
                TIMER_A0->CCR[3] = 400;           // CCR4 duty cycle is duty1/period
//                for(i=500;i>0;i--);
//                TIMER_A0->CCR[4] = 200;           // CCR3 duty cycle is duty1/period
//                TIMER_A0->CCR[3] = 200;           // CCR4 duty cycle is duty1/period

                //P2OUT &= ~BIT0;

                //P2OUT |= BIT6 | BIT7;
            }

            else if ((sensor_2_count > 10000) && (sensor_5_count < 8000))
            {
                P2OUT |= BIT2;
                TIMER_A0->CCR[4] =  400;          // CCR3 duty cycle is duty1/period
                TIMER_A0->CCR[3] =  100;          // CCR4 duty cycle is duty1/period
//                for(i=500;i>0;i--);
//                TIMER_A0->CCR[3] =  200;           // CCR3 duty cycle is duty1/period
//                TIMER_A0->CCR[4] =  200;          // CCR3 duty cycle is duty1/period

                //P2OUT &= ~BIT2;

                //P2OUT |= BIT6 | BIT7;
            }

            // orange

            else if ((sensor_2_count > 5000) && (sensor_5_count < 4000))
            {
                P2OUT |= BIT0;
                TIMER_A0->CCR[4] = 100;           // CCR3 duty cycle is duty1/period
                TIMER_A0->CCR[3] = 400;           // CCR4 duty cycle is duty1/period
//                for(i=500;i>0;i--);
//                TIMER_A0->CCR[4] = 200;           // CCR3 duty cycle is duty1/period
//                TIMER_A0->CCR[3] = 200;           // CCR4 duty cycle is duty1/period

                //P2OUT &= ~BIT0;

            }

            else if ((sensor_5_count > 5000) && (sensor_2_count < 4000))
            {
                P2OUT |= BIT2;
                TIMER_A0->CCR[4] =  400;          // CCR3 duty cycle is duty1/period
                TIMER_A0->CCR[3] =  100;          // CCR4 duty cycle is duty1/period
//                for(i=500;i>0;i--);
//                TIMER_A0->CCR[3] =  200;           // CCR3 duty cycle is duty1/period
//                TIMER_A0->CCR[4] =  200;          // CCR3 duty cycle is duty1/period

//                P2OUT &= ~BIT2;

                //P2OUT |= BIT6 | BIT7;
            }

            else
            {
                P2OUT |= BIT1;
            }

            if (hit_flag) {
                P1OUT |= (BIT6 | BIT7);
                for (i=0;i<400000;i++);
                P1OUT &= ~(BIT6 | BIT7);
                TIMER_A0->CCR[4] =  400;
                TIMER_A0->CCR[3] =  100;
                for (i=0;i<100000;i++);
                TIMER_A0->CCR[4] =  300;
                TIMER_A0->CCR[3] =  300;
                for (i=0;i<400000;i++);
                TIMER_A0->CCR[4] =  100;
                TIMER_A0->CCR[3] =  400;
                for (i=0;i<100000;i++);
                TIMER_A0->CCR[4] =  300;
                TIMER_A0->CCR[3] =  300;
                for (i=0;i<400000;i++);
                TIMER_A0->CCR[4] =  100;
                TIMER_A0->CCR[3] =  400;
                for (i=0;i<80000;i++);
                TIMER_A0->CCR[4] =  300;
                TIMER_A0->CCR[3] =  300;
                for (i=0;i<250000;i++);
                TIMER_A0->CCR[4] =  400;
                TIMER_A0->CCR[3] =  100;
                for (i=0;i<80000;i++);
                hit_flag = 0;

            }

        }

}

void PORT4_IRQHandler(void) {
    // yay
    // back up and turn and turn and go
    P4IFG = 0x00;
    hit_flag=1;


}

