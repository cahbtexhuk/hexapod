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


#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp              */

_FGS(GWRP_OFF & GSS_OFF & GCP_OFF);
_FOSCSEL(FNOSC_FRC & IESO_OFF);
_FOSC(POSCMD_XT & OSCIOFNC_OFF & FCKSM_CSECMD);
_FWDT(FWDTEN_OFF);


/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

/* i.e. uint16_t <variable_name>; */

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int16_t main(void)
{

    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize IO ports and peripherals */
    InitApp();

    /* TODO <INSERT USER APPLICATION CODE HERE> */
// Configure Oscillator to operate the device at 40Mhz
// Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
// Fosc= 8M*32/(2*4)=32Mhz for 8M input clock
	PLLFBD=30;					// M=32
	CLKDIVbits.PLLPOST=1;		// N1=4
	CLKDIVbits.PLLPRE=0;		// N2=2
	OSCTUN=0;					// Tune FRC oscillator, if FRC is used

// Disable Watch Dog Timer
	RCONbits.SWDTEN=0;

// clock switching to incorporate PLL
	__builtin_write_OSCCONH(0x03);		// Initiate Clock Switch to Primary Oscillator with PLL (NOSC=0x03)
	__builtin_write_OSCCONL(OSCCON || 0x01);		// Start clock switching
	while (OSCCONbits.COSC != 0x03);	// Wait for Clock switch to occur
	while(OSCCONbits.LOCK!=1);		// Wait for PLL to lock

    unsigned int x=0;
    
    
    WriteString("Hello!\r\n", 1);
    
    SendByte(1,debugchar);

    LED1++;
    delayUS(2000);    

    while(1)
    {

    }
}
