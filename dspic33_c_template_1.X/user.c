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
#include "comms/protocol.h"
#include "comms/min.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

unsigned int delayTick, millis;
struct min_context uart1;

volatile unsigned int Params[PARAM_COUNT];
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
    //void min_init_context(struct min_context *self, uint8_t port);
    min_init_context(&uart1,1);
}
void minDecode (uint8_t *str, uint32_t params)
{
//    unsigned int i, cursor, paramNum;
//    paramNum=0;
//    cursor=0;
//    i=0;
//    while(str[i]!='\r')
//    {
//        params[paramNum] = 0;
//        while((str[i]!=','))
//            {
//            params[paramNum] = params[paramNum] * 10 + (str[i] - '0');
//            i++;
//            if(str[i]=='\r')
//                {
//                    return;
//                }
//            }
//       paramNum++;
//       i++;
//    }
    //void min_poll(struct min_context *self, uint8_t *buf, uint32_t buf_len)
    min_poll(&uart1,str,params);
}
void ServoShutdown()
{
    //WriteString("Servo Shutdown\r\n", 1);
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

uint32_t min_time_ms()
{
    return delayTick;
}