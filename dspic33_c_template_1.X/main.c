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

    unsigned int x=0;
    LED1++;
    delayUS(2000);    

    while(1)
    {

    }
}
