/*
 * File:   protocol.c
 * Author: ireha
 *
 * Created on 10 September 2019, 23:36
 */


#include "xc.h"
#include "protocol.h"
#include "../mechanical/servos.h"

volatile char rxBuff[BUFF_SIZE], rxTemp[BUFF_SIZE],debugchar;
unsigned char rxBuffp;

void LoadCalData(volatile int *bank)
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
void WriteCalData()
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