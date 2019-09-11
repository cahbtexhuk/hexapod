/*
 * File:   serial.c
 * Author: ireha
 *
 * Created on 10 September 2019, 23:36
 */
#include "xc.h"
#include "serial.h"
#include "../user.h"
/*
 * Communication module setup
 * UARTs, I2C, SPI
 */

/*
 * UARTs setup
 */
int needDecode = 0;
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

void min_tx_start(uint8_t port)
{
    return;
}
void min_tx_finished(uint8_t port)
{
    return;
}