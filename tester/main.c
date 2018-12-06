#include "msp.h"
int count = 0;

/**
 * main.c
 */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    P2SEL0 = 0;
    P2SEL1 = 0;
    P2DIR &= ~BIT5;

    while (1)
    {
        if (P2IN & BIT5)
            count++;
        while (P2IN & BIT5 )
            ;
    }
}
