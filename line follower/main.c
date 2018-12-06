#include "msp.h"
#include <stdio.h>
//systick initialization
volatile long *systick_control_reg = (volatile long *) 0xE000E010;  // STCSR
volatile long *systick_reload_value_reg = (volatile long *) 0xE000E014; // STRVR
volatile long *systick_current_value_reg = (volatile long *) 0xE000E018; // STCVR
volatile long *systick_calibration_reg = (volatile long *) 0xE000E01C;  // STCR
long period = 3018000; //one second
//int mat[3000][4];
int count = 0;

//when timer_A0 counts up to the period this is triggered
//void TA0_0_IRQHandler()
//{
//    TIMER_A0->CCTL[0] &= ~0x0001;
//   // P3OUT ^=  BIT6;
//
//}
int timeCount = 0, time = 0, timeCount2 = 0, time2 = 0, time3 = 0, time4 = 0, i,
        upper, lower, f1 = 0, f2 = 0, f3 = 0, f4 = 0, flag = 0;
/*
void PORT1_IRQHandler()         // Interrupt handler for port 1
{
    P1IFG = 0x00000000; //reset pending interupt
    flag = 1;
}
*/
/**
 * main.c
 */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    *systick_reload_value_reg = period;
    *systick_control_reg = 1;

    P4SEL0 = 0;
    P4SEL1 = 0;
    P4DIR &= ~BIT3;
    P4DIR &= ~BIT5;


    P2SEL1 = 0;
    P2SEL0 |= BIT7;
    P2SEL0 |= BIT5;
    P2SEL0 &= ~BIT0;
    P2SEL0 &= ~BIT1;
    P2SEL0 &= ~BIT2;

    TIMER_A0->CCTL[0] = 0x0000;    // CCI0 toggle
    TIMER_A0->CCTL[4] = 0x0040;    // CCI0 toggle
    TIMER_A0->CCTL[2] = 0x0040;    // CCI0 toggle
    TIMER_A0->CCR[0] = 3000; // Period will vary with the clock settings.  You can measure it.
    TIMER_A0->EX0 = 0x0007;    // Divide by 1
    TIMER_A0->CCR[4] = 2000;    // CCR1 duty cycle is duty1/period
    TIMER_A0->CCR[2] = 2000;
    TIMER_A0->CTL = 0x0230;    // SMCLK=12MHz, divide by 1, up-down mode
    P3SEL0 = 0;
    P3SEL1 = 0;
    P3DIR |= BIT6;
    P3DIR |= BIT7;
    P3OUT |= BIT6;
    P3OUT |= BIT7;

    P2DIR |= BIT5;
    P2DIR |= BIT7;
    P2DIR |= BIT0;
    P2DIR |= BIT1;
    P2DIR |= BIT2;
    P1SEL0 = 0;
    P1SEL1 = 0;
    P1DIR |= BIT6;
    P1DIR |= BIT7;
    P1OUT &= ~BIT6;
    P1OUT |= BIT7;
    P2OUT |= (BIT5 | BIT7 );
    P1OUT |= BIT5;
    P1OUT |= BIT7;
    P2OUT |= (BIT5 | BIT7 );

    //NVIC_EnableIRQ(PORT1_IRQn);
   // __enable_irq();

    while (1)
    {
        if((P4IN & BIT3) || (P4IN & BIT5))
        {
            flag = 1;
        }

        if (flag)
        {
            P1OUT &= ~BIT5;
            P1OUT &= ~BIT7;
            P2OUT |= (BIT5 | BIT7 );
            TIMER_A0->CCR[4] = 1200;
            TIMER_A0->CCR[2] = 1200;
            for (i = 0; i < 50000; i++)
                ;
            P1OUT |= BIT5;
            TIMER_A0->CCR[4] = 1200;
            TIMER_A0->CCR[2] = 1200;
            for (i = 0; i < 20000; i++)
                ;
            P1OUT |= BIT5;
            P1OUT |= BIT7;
            P2OUT |= (BIT5 | BIT7 );
            for (i = 0; i < 100000; i++)
                ;
            P1OUT &= ~BIT7;
            for (i = 0; i < 20000; i++)
                ;
            P1OUT |= BIT5;
            P1OUT |= BIT7;
            P2OUT |= (BIT5 | BIT7 );
            for (i = 0; i < 100000; i++)
                ;
            P1OUT &= ~BIT7;
            for (i = 0; i < 20000; i++)
                ;
            P1OUT |= BIT5;
            P1OUT |= BIT7;
            P2OUT |= (BIT5 | BIT7 );
            flag = 0;
        }

        //time = 0;
        P7DIR |= BIT2;
        P7OUT |= BIT2;
        P7DIR |= BIT5;
        P7OUT |= BIT5;
        P7DIR |= BIT1;
        P7OUT |= BIT1;
        P7DIR |= BIT6;
        P7OUT |= BIT6;
        for (i = 0; i < 100; i++)
            ;
        P7DIR &= ~BIT2;
        P7DIR &= ~BIT5;
        P7DIR &= ~BIT1;
        P7DIR &= ~BIT6;
        timeCount = *systick_current_value_reg;

        do
        {
            if (!(P7IN & BIT2 ) && (f1 == 0))
            {
                time = timeCount - *systick_current_value_reg;
                f1 = 1;
            }
            if (!(P7IN & BIT5 ) && (f2 == 0))
            {
                time2 = timeCount - *systick_current_value_reg;
                f2 = 1;
            }
            if (!(P7IN & BIT1 ) && (f3 == 0))
            {
                time3 = timeCount - *systick_current_value_reg;
                f3 = 1;
            }
            if (!(P7IN & BIT6 ) && (f4 == 0))
            {
                time4 = timeCount - *systick_current_value_reg;
                f4 = 1;
            }
        }
        while ((f1 == 0) || (f2 == 0) || (f3 == 0) || (f4 == 0));
        f1 = 0;
        f2 = 0;
        f3 = 0;
        f4 = 0;

        if (time < 0)
        {
            time += period;
        }
        if (time2 < 0)
        {
            time2 += period;
        }
        if (time3 < 0)
        {
            time3 += period;
        }
        if (time4 < 0)
        {
            time4 += period;
        }

        upper = 12000;
        lower = 7200;

        if ((time < upper && time > lower) && (time2 < upper && time2 > lower))
        {
            TIMER_A0->CCR[4] = 1200;
            TIMER_A0->CCR[2] = 1200;
            P2OUT &= ~BIT0;
            P2OUT &= ~BIT2;
            P2OUT |= BIT1;    //green = good
        }
        else if (((time2 > upper) || (time2 < lower))
                && !((time4 > upper) || (time4 < lower)))
        {
            TIMER_A0->CCR[4] = 1200;
            TIMER_A0->CCR[2] = 800;
            P2OUT &= ~BIT1;
            P2OUT &= ~BIT2;
            P2OUT |= BIT0;    //red = left
        }
        else if (((time > upper) || (time < lower))
                && !((time3 > upper) || (time3 < lower)))
        {
            TIMER_A0->CCR[4] = 800;
            TIMER_A0->CCR[2] = 1200;

            P2OUT &= ~BIT0;
            P2OUT &= ~BIT1;
            P2OUT |= BIT2; //blue == right
        }
        else if ((time3 > upper) || (time3 < lower))
        {
            TIMER_A0->CCR[4] = 400;
            TIMER_A0->CCR[2] = 1800;
            P2OUT |= BIT2; //blue == right
            P2OUT |= BIT0;    //red = left
            for (i = 0; i < 1000; i++)
                ;

        }
        else if ((time4 > upper) || (time4 < lower))
        {
            TIMER_A0->CCR[4] = 1800;
            TIMER_A0->CCR[2] = 400;
            P2OUT |= BIT0;    //red = left
            P2OUT |= BIT1;    //green = good
            for (i = 0; i < 1000; i++)
                ;
        }

        else
        {
            TIMER_A0->CCR[2] = 1200;
            TIMER_A0->CCR[4] = 1200;
            P2OUT &= ~BIT0;
            P2OUT &= ~BIT2;
            P2OUT |= BIT1; //green = good
        }

    }

}

