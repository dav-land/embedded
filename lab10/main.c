#include "msp.h"
#include <stdio.h>
//systick initialization
volatile long *systick_control_reg = (volatile long *) 0xE000E010;  // STCSR
volatile long *systick_reload_value_reg = (volatile long *) 0xE000E014; // STRVR
volatile long *systick_current_value_reg = (volatile long *) 0xE000E018; // STCVR
volatile long *systick_calibration_reg = (volatile long *) 0xE000E01C;  // STCR

volatile long *adc14_control_register_0 = (volatile long *) 0x40012000; // ADCTL0
volatile long *adc14_control_register_1 = (volatile long *) 0x40012004; // ADCTL1
volatile long *adc14_memory_control_register_0 = (volatile long *) 0x40012018; // ADCMEMCTL0
volatile long *adc14_memory_register_0 = (volatile long *) 0x40012098; // ADC14MEM0
volatile long *adc14_interrupt_flag_0_register = (volatile long *) 0x40012144; // ADC14IFGR0
volatile long *adc14_interrupt_enable_0_register = (volatile long *) 0x4001213C; // ADC14IER0
volatile long *adc14_interrupt_enable_1_register = (volatile long *) 0x4001213C; // ADC14IER1

#define ADC14CTL0     *adc14_control_register_0
#define ADC14CTL1     *adc14_control_register_1
#define ADC14MCTL0    *adc14_memory_control_register_0
#define ADC14MEM0     *adc14_memory_register_0
#define ADC14IFGR0    *adc14_interrupt_flag_0_register
#define ADC14IER0     *adc14_interrupt_enable_0_register
#define ADC14IER1     *adc14_interrupt_enable_1_register

long period = 3018000; //one second

int count = 0;

int j, average, timeCount = 0, time = 0, timeCount2 = 0, time2 = 0, time3 = 0,
        time4 = 0, i, upper, lower, f1 = 0, f2 = 0, f3 = 0, f4 = 0, flag = 0,
        raw_distance = 0, raw_distance2, raw_distance3;

//unsigned int IR_distance_calibration_table[10] = { 14300, 14300, 14300, 14000,11950, 10900, 10000, 9800, 9200, 9100 };
void setup();
void readDist();
void avoid();
void readIRSensors();
void move();


void main(void)
{
    setup();
    while (1)
    {
        readDist();
        readIRSensors();
        move();
    }

}


void setup()
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    //Start systick timer
    *systick_reload_value_reg = period;
    *systick_control_reg = 1;

    // Set up the ADC.  These bits are all defined in the MSP432P4xx Technical Reference Manual...
    ADC14CTL0 &= 0xFFFFFFFD; // Note that in this bit mask, only bit 1 is a zero.
    ADC14CTL0 |= 0x00000010;    // ADC14 on
    ADC14CTL0 |= 0x04000000; // Source signal from the sampling timer       ****

    // You must consult the TRM and decide how to set up this register.  The other have been done for you.
    ADC14CTL1 = 0x00000030; // Students must set this one up:  ADC14MEM0, 14-bit, ref on, regular power

    ADC14MCTL0 = 0x0000008E; // 0 to 3.3V, channel A6?? (student must figure this out in Lab 0.4)
    ADC14IER0 = 0;             // no interrupts
    ADC14IER1 = 0;             // no interrupts
    ADC14CTL0 |= 0x00000002; // enable    But doesn't the core have to be on? (bit 4).  Original code.

    //Setup Pin analog input to pin 6.1
    P6SEL0 |= BIT1;
    P6SEL1 |= BIT1;
    P6DIR &= ~BIT1;
    // P6DIR |= BIT0;

    //Pins to read in middle two bump switches
    P4SEL0 = 0;
    P4SEL1 = 0;
    P4DIR &= ~BIT3;
    P4DIR &= ~BIT5;

    //pins 6 and 7 are for motor PWM | 0,1,2 are for led
    P2SEL1 = 0;
    P2SEL0 |= BIT7;
    P2SEL0 |= BIT6;
    P2SEL0 &= ~BIT0;
    P2SEL0 &= ~BIT1;
    P2SEL0 &= ~BIT2;
    P2DIR |= BIT6;
    P2DIR |= BIT7;
    P2DIR |= BIT0;
    P2DIR |= BIT1;
    P2DIR |= BIT2;

    //Set up timer a0 to send a pwm output to pins 2.6 & 2.7
    TIMER_A0->CCTL[0] = 0x0000;    // CCI0 toggle
    TIMER_A0->CCTL[4] = 0x0040;    // CCI0 toggle
    TIMER_A0->CCTL[3] = 0x0040;    // CCI0 toggle
    TIMER_A0->CCR[0] = 3000; // Period will vary with the clock settings.  You can measure it.
    TIMER_A0->EX0 = 0x0007;    // Divide by 1
    TIMER_A0->CCR[4] = 2000;    // CCR1 duty cycle is duty1/period
    TIMER_A0->CCR[3] = 2000;
    TIMER_A0->CTL = 0x0230;    // SMCLK=12MHz, divide by 1, up-down mode

    //These pins control the SLP??
    P3SEL0 = 0;
    P3SEL1 = 0;
    P3DIR |= BIT6;
    P3DIR |= BIT7;
    P3OUT |= BIT6;
    P3OUT |= BIT7;

    //sets up 1.6 and 1.7 as motor DIR pins
    P1SEL0 = 0;
    P1SEL1 = 0;
    P1DIR |= BIT6;
    P1DIR |= BIT7;

    P1OUT &= ~BIT6;
    P1OUT |= BIT7;
    P2OUT |= (BIT6 | BIT7 );
    P1OUT &= ~BIT6;
    P1OUT &= ~BIT7;
    P2OUT |= (BIT6 | BIT7 );

    upper = 25000;
    lower = 15000;

    //NVIC_EnableIRQ(PORT1_IRQn);
    // __enable_irq();
}

void readDist()
{
    ADC14CTL0 |= 0x00000001;
    while (ADC14CTL0 & BIT0 )
    {
    }
    raw_distance = ADC14MEM0;
}

void avoid()
{
    TIMER_A0->CCR[4] = 0;
    TIMER_A0->CCR[3] = 1200;
    for (i = 0; i < 100000; i++)
        ;
    TIMER_A0->CCR[4] = 1200;
    TIMER_A0->CCR[3] = 1200;
    for (i = 0; i < 100000; i++)
        ;
    TIMER_A0->CCR[4] = 1200;
    TIMER_A0->CCR[3] = 0;
    for (i = 0; i < 100000; i++)
        ;
    TIMER_A0->CCR[4] = 1200;
    TIMER_A0->CCR[3] = 1200;
    for (i = 0; i < 100000; i++)
        ;
    TIMER_A0->CCR[4] = 1200;
    TIMER_A0->CCR[3] = 0;
    for (i = 0; i < 100000; i++)
        ;
    TIMER_A0->CCR[4] = 1200;
    TIMER_A0->CCR[3] = 1200;
    readIRSensors();
    while ((time < upper && time > lower) && (time2 < upper && time2 > lower)
            && (time3 < upper && time3 > lower)
            && (time4 < upper && time4 > lower))
        readIRSensors();
    TIMER_A0->CCR[4] = 0;
    TIMER_A0->CCR[3] = 1200;
    for (i = 0; i < 100000; i++)
        ;
    TIMER_A0->CCR[4] = 1200;
    TIMER_A0->CCR[3] = 1200;
}

void readIRSensors()
{
    //Set all ir outputs to high
    P7DIR |= BIT2;
    P7OUT |= BIT2;
    P7DIR |= BIT5;
    P7OUT |= BIT5;
    P7DIR |= BIT1;
    P7OUT |= BIT1;
    P7DIR |= BIT6;
    P7OUT |= BIT6;
    //wait for them to be high
    for (i = 0; i < 100; i++)
        ;

    //set to inputs
    P7DIR &= ~BIT2;
    P7DIR &= ~BIT5;
    P7DIR &= ~BIT1;
    P7DIR &= ~BIT6;
    timeCount = *systick_current_value_reg;

    //wait for them to be low and see how long it takes
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
        time += period;
    if (time2 < 0)
        time2 += period;
    if (time3 < 0)
        time3 += period;
    if (time4 < 0)
        time4 += period;

}

void move()
{
    if (raw_distance > 12500)
    {
        avoid();
    }
    else if ((time < upper && time > lower) && (time2 < upper && time2 > lower))
    {
        TIMER_A0->CCR[4] = 1200;
        TIMER_A0->CCR[3] = 1200;
        P2OUT &= ~BIT0;
        P2OUT &= ~BIT2;
        P2OUT |= BIT1;    //green = good
    }
    else if (((time2 > upper) || (time2 < lower))
            && !((time4 > upper) || (time4 < lower)))
    {
        TIMER_A0->CCR[4] = 800;
        TIMER_A0->CCR[3] = 1200;
        P2OUT &= ~BIT1;
        P2OUT &= ~BIT2;
        P2OUT |= BIT0;    //red = left
    }
    else if (((time > upper) || (time < lower))
            && !((time3 > upper) || (time3 < lower)))
    {

        TIMER_A0->CCR[4] = 1200;
        TIMER_A0->CCR[3] = 800;
        P2OUT &= ~BIT0;
        P2OUT &= ~BIT1;
        P2OUT |= BIT2; //blue == right
    }
    else if ((time3 > upper) || (time3 < lower))
    {

        TIMER_A0->CCR[4] = 1800;
        TIMER_A0->CCR[3] = 400;
        P2OUT |= BIT2; //blue == right
        P2OUT |= BIT0;    //red = left
        for (i = 0; i < 1000; i++)
            ;

    }
    else if ((time4 > upper) || (time4 < lower))
    {
        TIMER_A0->CCR[4] = 400;
        TIMER_A0->CCR[3] = 1800;
        P2OUT |= BIT0;    //red = left
        P2OUT |= BIT1;    //green = good
        for (i = 0; i < 1000; i++)
            ;
    }

    else
    {
        TIMER_A0->CCR[3] = 1200;
        TIMER_A0->CCR[4] = 1200;
        P2OUT &= ~BIT0;
        P2OUT &= ~BIT2;
        P2OUT |= BIT1; //green = good
    }

}


