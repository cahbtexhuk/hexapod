/*
 * File:   oc.c
 * Author: ireha
 *
 * Created on 10 September 2019, 23:39
 */


#include "xc.h"
#include "oc.h"

volatile int PWM_Bank[SERVO_COUNT], PWM_Pointer;
int PWM_MidPos[SERVO_COUNT], ServoCollapsed[SERVO_COUNT];
int servoValue, servoChannel, x;

/*
 * Timers setup
 */
void T3_setup()
{
    T3CONbits.TON = 0; // Disable Timer
    T3CONbits.TCS = 0; // Select internal instruction cycle clock
    T3CONbits.TGATE = 0; // Disable Gated Timer mode
    T3CONbits.TCKPS = 0b01; // Select 1:8 Prescaler, 2 MHz
    TMR3 = 0x00; // Clear timer register
    PR3 = 5000; // Load the period value
    IPC2bits.T3IP = 0x03; // Set Timer1 Interrupt Priority Level
    IFS0bits.T3IF = 0; // Clear Timer1 Interrupt Flag
    IEC0bits.T3IE = 1;
    T3CONbits.TON = 1; // Start Timer
}
void T2_setup()
{
    T2CONbits.TON = 0; // Disable Timer
    T2CONbits.TCS = 0; // Select internal instruction cycle clock
    T2CONbits.TGATE = 0; // Disable Gated Timer mode
    T2CONbits.TCKPS = 0b01; // Select 1:8 Prescaler, 2 MHz
    TMR2 = 0x00; // Clear timer register
    PR2 = 50000; // Load the period value
    IPC1bits.T2IP = 0x04; // Set Timer1 Interrupt Priority Level
    IFS0bits.T2IF = 0; // Clear Timer1 Interrupt Flag
    IEC0bits.T2IE = 1;
    T2CONbits.TON = 1; // Start Timer
}
void T4_setup()
{
    T4CONbits.TON = 0; // Disable Timer
    T4CONbits.TCS = 0; // Select internal instruction cycle clock
    T4CONbits.TGATE = 0; // Disable Gated Timer mode
    T4CONbits.TCKPS = 0b10; // Select 1:64 Prescaler, 250 KHz
    TMR4 = 0x00; // Clear timer register
    PR4 = 250; // Load the period value
    IPC6bits.T4IP = 0x05; // Set Timer1 Interrupt Priority Level
    IFS1bits.T4IF = 0; // Clear Timer1 Interrupt Flag
    IEC1bits.T4IE = 1;
    T4CONbits.TON = 0; // Start Timer
}
void T5_setup()
{
    T5CONbits.TON = 0; // Disable Timer
    T5CONbits.TCS = 0; // Select internal instruction cycle clock
    T5CONbits.TGATE = 0; // Disable Gated Timer mode
    T5CONbits.TCKPS = 0b10; // Select 1:64 Prescaler, 250 KHz
    TMR5 = 0x00; // Clear timer register
    PR5 = 25000; // Load the period value
    IPC7bits.T5IP = 0x06; // Set Timer1 Interrupt Priority Level
    IFS1bits.T5IF = 0; // Clear Timer1 Interrupt Flag
    IEC1bits.T5IE = 1;
    T5CONbits.TON = 1; // Start Timer
}
/*
 * Output Compare modules setup
 */
void OC1_setup()
{
    OC1CONbits.OCM = 0b000; // Disable Output Compare Module
    OC1CONbits.OCTSEL = 1; // Select Timer 2 as output compare time base
    OC1R = OFFSET; // Load the Compare Register Value for rising edge
    OC1RS = PWM_MID + OFFSET; // Load the Compare Register Value for falling edge
    OC1CONbits.OCM = 0b101; // Select the Output Compare mode
}
void OC2_setup()
{
    OC2CONbits.OCM = 0b000; // Disable Output Compare Module
    OC2CONbits.OCTSEL = 1; // Select Timer 2 as output compare time base
    OC2R = OFFSET; // Load the Compare Register Value for rising edge
    OC2RS = PWM_MID + OFFSET; // Load the Compare Register Value for falling edge
    OC2CONbits.OCM = 0b101; // Select the Output Compare mode
}
void OC3_setup()
{
    OC3CONbits.OCM = 0b000; // Disable Output Compare Module
    OC3CONbits.OCTSEL = 1; // Select Timer 2 as output compare time base
    OC3R = OFFSET; // Load the Compare Register Value for rising edge
    OC3RS = PWM_MID + OFFSET; // Load the Compare Register Value for falling edge
    OC3CONbits.OCM = 0b101; // Select the Output Compare mode
}


void IC7_setup()
{
    IC7CONbits.ICM = 0b00; // Disable Input Capture 1 module
    IC7CONbits.ICTMR = 1; // Select Timer2 as the IC1 time base
    IC7CONbits.ICI = 0b00; // Interrupt on every capture event
    
    IPC5bits.IC7IP = 4; // Setup IC1 interrupt priority level
    IFS1bits.IC7IF = 0; // Clear IC1 Interrupt Status Flag
    IEC1bits.IC7IE = 1; // Enable IC1 interrupt

    IC7CONbits.ICM = 0b011; // Generate capture on every rising edge
}
void SwitchBank()
{
    if (PWM_Pointer==7)
        PWM_Pointer=0;
    else
        PWM_Pointer++;
    STOP_T3;
    UpdateOC1(PWM_Bank[PWM_Pointer]);
    UpdateOC2(PWM_Bank[PWM_Pointer+8]);
    UpdateOC3(PWM_Bank[PWM_Pointer+16]);

    if (PWM_Pointer & 0x0001)
        MUX_A0 = 1;
    else
        MUX_A0 = 0;

    if (PWM_Pointer & 0x0002)
        MUX_A1 = 1;
    else
        MUX_A1 = 0;

    if (PWM_Pointer & 0x0004)
        MUX_A2 = 1;
    else
        MUX_A2 = 0;
    START_T3;
}