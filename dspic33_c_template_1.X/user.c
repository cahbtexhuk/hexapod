/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/* Device header file */
#if defined(__XC16__)
    #include <xc.h>
#elif defined(__C30__)
    #if defined(__dsPIC33E__)
    	#include <p33Exxxx.h>
    #elif defined(__dsPIC33F__)
    	#include <p33Fxxxx.h>
    #endif
#endif

#include <stdint.h>          /* For uint16_t definition                       */
#include <stdbool.h>         /* For true/false definition                     */
#include "user.h"            /* variables/params used by user.c               */

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void)
{
    uint8_t x=0;
    /* TODO Initialize User Ports/Peripherals/Project here */
    TRISC=PORTC_SETUP;
    TRISD=PORTD_SETUP;
    TRISF=PORTF_SETUP;
    TRISG=PORTG_SETUP;

    PORTC=0;
    PORTD=0;
    PORTF=0;
    PORTG=0;

    

    for (x=0;x<SERVO_COUNT;x++)
    {
        PWM_Bank[x]=PWM_MID;
    }
    for(x=0;x<BUFF_SIZE;x++)
    {
        rxBuff[x]=0;
    }
    rxBuffp=0;
    needDecode=0;
    delayTick=0;

    /* Setup analog functionality and port direction */

    /* Initialize peripherals */
    UART1_setup();
    UART2_setup();
    OC1_setup();
    OC2_setup();
    OC3_setup();
    T3_setup();
    T2_setup();
    I2C1_setup();
    T4_setup();
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
inline void UpdateOC1(unsigned int value)
{
    OC1RS = value;
}

inline void UpdateOC2(unsigned int value)
{
    OC2RS = value;
}

inline void UpdateOC3(unsigned int value)
{
    OC3RS = value;
}
void Decode (char* str, unsigned int* params)
{
    unsigned int i, cursor, paramNum;
    paramNum=0;
    cursor=0;
    i=0;
    while(str[i]!='\r')
    {
        params[paramNum] = 0;
        while((str[i]!=','))
            {
            params[paramNum] = params[paramNum] * 10 + (str[i] - '0');
            i++;
            if(str[i]=='\r')
                {
                    return;
                }
            }
       paramNum++;
       i++;
    }
}
void ServoShutdown()
{
    WriteString("Servo Shutdown\r\n", 1);
}
void LoadCalData(int *bank)
{
    int x, mAddr;
    char temp;
    //WriteString("Load Calibration\r\n", UART1);
    for (x=0;x<SERVO_COUNT;x++)
    {
        temp=EEPROMReadChar(mAddr);
        mAddr++;
        bank[x]=temp<<8;
        temp=EEPROMReadChar(mAddr);
        mAddr++;
        bank[x]=bank[x]+temp;
    }
}
void WriteCalData(unsigned int state)
{
    WriteString("Write\r\n", 1);
    int x, mAddr;
    mAddr=0;
    for (x=0;x<SERVO_COUNT;x++)
    {
        EEPROMWriteChar(mAddr,PWM_Bank[x]>>8);
        mAddr++;
        EEPROMWriteChar(mAddr,PWM_Bank[x] & 0xff);
        mAddr++;
    }
}
void ResetLegs()
{
    LoadCalData(PWM_Bank);
}
void delayUS(int delay)
{
    delayTick=0;
    T4CONbits.TON = 1;
    while(delayTick<delay);
    T4CONbits.TON = 0;
}

void setServoAngle(unsigned int servo, int angle)
{
    PWM_Bank[servo]=PWM_MID+(angle*15);
}

void moveLegAngle(unsigned int legNo, int coxaAngle, int femurAngle, int tibiaAngle)
{
    int coxa, tibia, femur, coxaServo, tibiaServo, femurServo;
    switch (legNo)
    {
        //set signs
        case LEFT_FRONT:
        {
            coxa=1;
            femur=-1;
            tibia=-1;
            coxaServo=LA_COXA;
            tibiaServo=LA_TIBIA;
            femurServo=LA_FEMUR;
            break;
        }
        case LEFT_MID:
        {
            coxa=1;
            femur=-1;
            tibia=-1;
            coxaServo=LM_COXA;
            tibiaServo=LM_TIBIA;
            femurServo=LM_FEMUR;
            break;
        }
        case LEFT_BACK:
        {
            coxa=1;
            femur=-1;
            tibia=-1;
            coxaServo=LP_COXA;
            tibiaServo=LP_TIBIA;
            femurServo=LP_FEMUR;
            break;
        }
        case RIGHT_FRONT:
        {
            coxa=-1;
            femur=1;
            tibia=1;
            coxaServo=RA_COXA;
            tibiaServo=RA_TIBIA;
            femurServo=RA_FEMUR;
            break;
        }
        case RIGHT_MID:
        {
            coxa=-1;
            femur=1;
            tibia=1;
            coxaServo=RM_COXA;
            tibiaServo=RM_TIBIA;
            femurServo=RM_FEMUR;
            break;
        }
        case RIGHT_BACK:
        {
            coxa=-1;
            femur=1;
            tibia=1;
            coxaServo=RP_COXA;
            tibiaServo=RP_TIBIA;
            femurServo=RP_FEMUR;
            break;
        }    
    }
    setServoAngle(coxaServo, (coxa*coxaAngle));
    setServoAngle(tibiaServo, (tibia*tibiaAngle));
    setServoAngle(femurServo, (femur*femurAngle));
}
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
/*
 * Communication module setup
 * UARTs, I2C, SPI
 */

/*
 * UARTs setup
 */
void UART1_setup()
{
   U1MODEbits.UARTEN = 0;
   U1MODEbits.STSEL = 0;                 	// 1-stop bit
   U1MODEbits.PDSEL0 = 0;               	// No Parity, 8-data bits
   U1MODEbits.PDSEL1 = 0;               	// No Parity, 8-data bits
   U1MODEbits.ABAUD = 0;                	// Auto-Baud Disabled
   U1MODEbits.BRGH = 0;
   U1BRG = BRGVAL;
   U1STAbits.URXISEL0 = 0;
   U1STAbits.URXISEL1 = 0;
   IEC0bits.U1RXIE = 1;                    	 // enable intterupt
   U1MODEbits.UARTEN = 1;               	// Enable UART
   U1STAbits.UTXEN = 1;
}
void UART2_setup()
{
   U2MODEbits.UARTEN = 0;
   U2MODEbits.STSEL = 0;                 	// 1-stop bit
   U2MODEbits.PDSEL0 = 0;               	// No Parity, 8-data bits
   U2MODEbits.PDSEL1 = 0;               	// No Parity, 8-data bits
   U2MODEbits.ABAUD = 0;                	// Auto-Baud Disabled
   U2MODEbits.BRGH = 0;
   U2BRG = BRGVAL;
   U2STAbits.URXISEL0 = 0;
   U2STAbits.URXISEL1 = 0;
   IEC1bits.U2RXIE = 1;                    	 // enable intterupt
   U2MODEbits.UARTEN = 1;               	// Enable UART
   U2STAbits.UTXEN = 1;
}
void I2C1_setup()
{
    I2C1CON = 0; // I2C1 CONFIGURATION
    I2C1STAT = 0;
    I2C1CONbits.I2CEN = 1; // ENABLE
    I2C1BRG = I2CBRGVAL;
    I2C1STATbits.BCL = 0; // CLEAR BUS COLLISION FLAG
    I2C1MSK = 0; // CLEAR MASK
    I2C1ADD = 0; // CLEAR ADDRESS
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

void SendByte(const unsigned char port, char byte)
{
    if(port==1)
    {
        while(U1STAbits.UTXBF);
        U1TXREG=byte;
    }
    if(port==2)
    {
        while(U2STAbits.UTXBF);
        U2TXREG=byte;
    }
}
void WriteString(const char *string, unsigned char port)
{
    while(*string != '\0')
    {
        SendByte(port, *string);
        string++;
    }
}
/*
 * WAIT FOR I2C IDLE STATE
 */
void I2C1WaitForIdle( void )
{
    while( (I2C1CON & 0x001f) | I2C1STATbits.TRSTAT )
        continue;
}
//******************************************************************************************
// START I2C
//******************************************************************************************
void I2C1Start()
{
    I2C1WaitForIdle();
    I2C1CONbits.SEN = 1;
}
//******************************************************************************************
// I2C REPEAT START
//******************************************************************************************
void I2C1Restart()
{
    I2C1WaitForIdle();
    I2C1CONbits.RSEN = 1;
}
//******************************************************************************************
// I2C STOP
//******************************************************************************************
void I2C1Stop()
{
    I2C1WaitForIdle();
    I2C1CONbits.PEN = 1;
}
//******************************************************************************************
// I2C READ char
//******************************************************************************************
char I2C1Read( char pAck )
{
    char lData;
    I2C1WaitForIdle();
    I2C1CONbits.RCEN=1;
    I2C1WaitForIdle();
    lData = I2C1RCV;
    I2C1WaitForIdle();
    if ( pAck )
    I2C1CONbits.ACKDT = 0;
    else
    I2C1CONbits.ACKDT = 1;
    I2C1CONbits.ACKEN = 1; // SEND ACKNOWLEDGE SEQUENCE
    return( lData );
}
//******************************************************************************************
// I2C WRITE DATA
//******************************************************************************************
int I2C1Write( char pData )
{
    I2C1WaitForIdle();
    I2C1TRN = pData;
    return ( !I2C1STATbits.ACKSTAT ); // RETURNS 1 IF TX IS ACKNOWLEDGED
}
/*
 * High-level functions
 */
// ************************************************************************************
// WRITE EEPROM char
// ************************************************************************************
void EEPROMWriteChar( int pAddr, char pData )
{
    I2C1Start(); // START I2C
    I2C1Write( 0xa0 ); // WRITE EEPROM ADDRESS
    I2C1Write( pAddr >> 8 ); // WRITE EEPROM DATA ADDRESS
    I2C1Write( pAddr & 0xff );
    I2C1Write( pData); // WRITE EEPROM DATA
    I2C1Stop(); // START THE DATA ERASE WRITE CYCLE.
    DelayUs(6000);
}
/************************************************************************************
* READ EEPROM DATA
************************************************************************************/
char EEPROMReadChar( int pAddr )
{
    char pData;
    I2C1Start(); // START I2C
    I2C1Write( 0xa0 ); // WRITE EEPROM ADDRESS
    I2C1Write( (pAddr >> 8) ); // WRITE DATA ADDRESS
    I2C1Write(pAddr & 0xff);
    I2C1Restart(); // REPEAT START CONDITION
    I2C1Write(0xa1); // WRITE READ COMMAND
    pData = I2C1Read(0); // READ DATA
    I2C1Stop(); // STOP I2C
    return( pData );
}
/************************************************************************************
 WRITE EEPROM 32 char BUFFER
************************************************************************************/
void EEPROMWritePage32( int pAddr, char *pData )
{
    char pT;

    I2C1Start(); // START I2C
    I2C1Write( 0xa0 ); // WRITE EEPROM COMMAND
    I2C1Write( pAddr >> 8 ); // WRITE DATA ADDRESS START
    I2C1Write( pAddr & 0xff );
    for( pT = 0; pT < 32; pT++ ) // WRITE 32 CHARS OF DATA
    I2C1Write(*pData++);
    I2C1Stop(); // START THE DATA ERASE WRITE CYCLE.
    DelayUs(6000);
}
void LoadMid(int *midData)
{

}
void DelayUs( int pDelay )
{
    while(--pDelay > 0)
    {
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
    }
}
void stepForward(int stepCount)
{
    int x;
    for(x=0;x<stepCount;x++)
    {
        asm("nop");//debug mode, remove later
        //frame 1
        if(PWM_Bank[LA_COXA]==3000)
        {
            moveLegAngle(LEFT_FRONT, 0, 20, 20);
            moveLegAngle(RIGHT_MID, 0, 20, 20);
            moveLegAngle(LEFT_BACK, 0, 20, 20);
        }
        else
        {
            moveLegAngle(LEFT_FRONT, 20, 20, 20);
            moveLegAngle(RIGHT_MID, 20, 20, 20);
            moveLegAngle(LEFT_BACK, 20, 20, 20);
        }
        delayUS(STEP_DELAY);
        //frame 2
        moveLegAngle(RIGHT_FRONT, -20, 0, 0);
        moveLegAngle(LEFT_MID, -20, 0, 0);
        moveLegAngle(RIGHT_BACK, -20, 0, 0);

        moveLegAngle(LEFT_FRONT, 20, 20, 20);
        moveLegAngle(RIGHT_MID, 20, 20, 20);
        moveLegAngle(LEFT_BACK, 20, 20, 20);
        delayUS(STEP_DELAY);
        //frame 3
        moveLegAngle(LEFT_FRONT, 20, 0, 0);
        moveLegAngle(RIGHT_MID, 20, 0, 0);
        moveLegAngle(LEFT_BACK, 20, 0, 0);
        delayUS(STEP_DELAY);
        //frame 4
        moveLegAngle(RIGHT_FRONT, -20, 20, 20);
        moveLegAngle(LEFT_MID, -20, 20, 20);
        moveLegAngle(RIGHT_BACK, -20, 20, 20);
        delayUS(STEP_DELAY);
        //frame 5
        moveLegAngle(LEFT_FRONT, -20, 0, 0);
        moveLegAngle(RIGHT_MID, -20, 0, 0);
        moveLegAngle(LEFT_BACK, -20, 0, 0);
        moveLegAngle(RIGHT_FRONT, 20, 20, 20);
        moveLegAngle(LEFT_MID, 20, 20, 20);
        moveLegAngle(RIGHT_BACK, 20, 20, 20);
        delayUS(STEP_DELAY);
        //frame 6
        moveLegAngle(RIGHT_FRONT, 20, 0, 0);
        moveLegAngle(LEFT_MID, 20, 0, 0);
        moveLegAngle(RIGHT_BACK, 20, 0, 0);
        delayUS(STEP_DELAY);
    }
}

void turnLeft(int stepCount)
{
    asm("nop");//debug
    int x;
    for (x=0;x<stepCount;x++)
    {
        asm("nop");//debug mode, remove later
        //frame 1
        moveLegAngle(LEFT_FRONT, 0, 20, 20);
        moveLegAngle(RIGHT_MID, 0, 20, 20);
        moveLegAngle(LEFT_BACK, 0, 20, 20);
        delayUS(STEP_DELAY);
        //frame 2
        moveLegAngle(LEFT_FRONT, -20, 20, 20);
        moveLegAngle(RIGHT_MID, 20, 20, 20);
        moveLegAngle(LEFT_BACK, -20, 20, 20);
        moveLegAngle(RIGHT_FRONT, -20, 0, 0);
        moveLegAngle(LEFT_MID, 20, 0, 0);
        moveLegAngle(RIGHT_BACK, -20, 0, 0);
        delayUS(STEP_DELAY);
        //frame 3
        moveLegAngle(LEFT_FRONT, -20, 0, 0);
        moveLegAngle(RIGHT_MID, 20, 0, 0);
        moveLegAngle(LEFT_BACK, -20, 0, 0);
        delayUS(STEP_DELAY);
        //frame 4
        moveLegAngle(RIGHT_FRONT, -20, 20, 20);
        moveLegAngle(LEFT_MID, 20, 20, 20);
        moveLegAngle(RIGHT_BACK, -20, 20, 20);
        delayUS(STEP_DELAY);
        //frame 5
        moveLegAngle(LEFT_FRONT, 20, 0, 0);
        moveLegAngle(RIGHT_MID, -20, 0, 0);
        moveLegAngle(LEFT_BACK, 20, 0, 0);
        moveLegAngle(RIGHT_FRONT, 20, 20, 20);
        moveLegAngle(LEFT_MID, -20, 20, 20);
        moveLegAngle(RIGHT_BACK, 20, 20, 20);
        delayUS(STEP_DELAY);
        //frame 6
        moveLegAngle(RIGHT_FRONT, 20, 0, 0);
        moveLegAngle(LEFT_MID, -20, 0, 0);
        moveLegAngle(RIGHT_BACK, 20, 0, 0);
        delayUS(STEP_DELAY);
    }
}

void turnRight(int stepCount)
{
    asm("nop");//debug
    int x;
    for (x=0;x<stepCount;x++)
    {
        asm("nop");//debug mode, remove later
        //frame 1
        moveLegAngle(LEFT_FRONT, 0, 20, 20);
        moveLegAngle(RIGHT_MID, 0, 20, 20);
        moveLegAngle(LEFT_BACK, 0, 20, 20);
        delayUS(STEP_DELAY);
        //frame 2
        moveLegAngle(LEFT_FRONT, 20, 20, 20);
        moveLegAngle(RIGHT_MID, -20, 20, 20);
        moveLegAngle(LEFT_BACK, 20, 20, 20);
        moveLegAngle(RIGHT_FRONT, 20, 0, 0);
        moveLegAngle(LEFT_MID, -20, 0, 0);
        moveLegAngle(RIGHT_BACK, 20, 0, 0);
        delayUS(STEP_DELAY);
        //frame 3
        moveLegAngle(LEFT_FRONT, 20, 0, 0);
        moveLegAngle(RIGHT_MID, -20, 0, 0);
        moveLegAngle(LEFT_BACK, 20, 0, 0);
        delayUS(STEP_DELAY);
        //frame 4
        moveLegAngle(RIGHT_FRONT, 20, 20, 20);
        moveLegAngle(LEFT_MID, -20, 20, 20);
        moveLegAngle(RIGHT_BACK, 20, 20, 20);
        delayUS(STEP_DELAY);
        //frame 5
        moveLegAngle(LEFT_FRONT, -20, 0, 0);
        moveLegAngle(RIGHT_MID, 20, 0, 0);
        moveLegAngle(LEFT_BACK, -20, 0, 0);
        moveLegAngle(RIGHT_FRONT, -20, 20, 20);
        moveLegAngle(LEFT_MID, 20, 20, 20);
        moveLegAngle(RIGHT_BACK, -20, 20, 20);
        delayUS(STEP_DELAY);
        //frame 6
        moveLegAngle(RIGHT_FRONT, -20, 0, 0);
        moveLegAngle(LEFT_MID, 20, 0, 0);
        moveLegAngle(RIGHT_BACK, -20, 0, 0);
        delayUS(STEP_DELAY);
    }
}

void stepBackward(int stepCount)
{
    asm("nop");//debug
    int x;
    for(x=0;x<stepCount;x++)
    {
        asm("nop");//debug mode, remove later
        //frame 1
        if(PWM_Bank[LA_COXA]==3000)
        {
            moveLegAngle(LEFT_FRONT, 0, 20, 20);
            moveLegAngle(RIGHT_MID, 0, 20, 20);
            moveLegAngle(LEFT_BACK, 0, 20, 20);
        }
        else
        {
            moveLegAngle(LEFT_FRONT, -20, 20, 20);
            moveLegAngle(RIGHT_MID, -20, 20, 20);
            moveLegAngle(LEFT_BACK, -20, 20, 20);
        }
        delayUS(STEP_DELAY);
        //frame 2
        moveLegAngle(RIGHT_FRONT, 20, 0, 0);
        moveLegAngle(LEFT_MID, 20, 0, 0);
        moveLegAngle(RIGHT_BACK, 20, 0, 0);

        moveLegAngle(LEFT_FRONT, -20, 20, 20);
        moveLegAngle(RIGHT_MID, -20, 20, 20);
        moveLegAngle(LEFT_BACK, -20, 20, 20);
        delayUS(STEP_DELAY);
        //frame 3
        moveLegAngle(LEFT_FRONT, -20, 0, 0);
        moveLegAngle(RIGHT_MID, -20, 0, 0);
        moveLegAngle(LEFT_BACK, -20, 0, 0);
        delayUS(STEP_DELAY);
        //frame 4
        moveLegAngle(RIGHT_FRONT, 20, 20, 20);
        moveLegAngle(LEFT_MID, 20, 20, 20);
        moveLegAngle(RIGHT_BACK, 20, 20, 20);
        delayUS(STEP_DELAY);
        //frame 5
        moveLegAngle(LEFT_FRONT, 20, 0, 0);
        moveLegAngle(RIGHT_MID, 20, 0, 0);
        moveLegAngle(LEFT_BACK, 20, 0, 0);
        moveLegAngle(RIGHT_FRONT, -20, 20, 20);
        moveLegAngle(LEFT_MID, -20, 20, 20);
        moveLegAngle(RIGHT_BACK, -20, 20, 20);
        delayUS(STEP_DELAY);
        //frame 6
        moveLegAngle(RIGHT_FRONT, -20, 0, 0);
        moveLegAngle(LEFT_MID, -20, 0, 0);
        moveLegAngle(RIGHT_BACK, -20, 0, 0);
        delayUS(STEP_DELAY);
    }
}

void stepLeft(int stepCount)
{
    asm("nop");//debug
}

void stepRight(int stepCount)
{
    asm("nop");//debug
}
