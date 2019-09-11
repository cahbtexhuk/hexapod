/*
 * File:   protocol.c
 * Author: ireha
 *
 * Created on 10 September 2019, 23:36
 */


#include "xc.h"
#include "protocol.h"
#include "../mechanical/servos.h"
#include "min.h"

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

void min_tx_byte(uint8_t port, uint8_t byte)
{
    SendByte(port,byte);
}
uint16_t min_tx_space(uint8_t port)
{
    //implement proper use
    return 64;
}
void min_application_handler(uint8_t min_id, uint8_t *min_payload, uint8_t len_payload, uint8_t port)
{
  // In this simple example application we just echo the frame back when we get one, with the MIN ID
  // one more than the incoming frame.
  //
  // We ignore the port because we have one context, but we could use it to index an array of
  // contexts in a bigger application.
//  Serial.print("MIN frame with ID ");
//  Serial.print(min_id);
//  Serial.print(" received at ");
//  Serial.println(millis());
//  min_id++;
//  // The frame echoed back doesn't go through the transport protocol: it's send back directly
//  // as a datagram (and could be lost if there were noise on the serial line).
//  min_send_frame(&min_ctx, min_id, min_payload, len_payload);
    
    // DO SHIT HERE
}