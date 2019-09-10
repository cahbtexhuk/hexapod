/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

/* TODO Application specific user parameters used in user.c may go here */
#include "comms/protocol.h"
#include "mechanical/servos.h"
/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

/* TODO User level functions prototypes (i.e. InitApp) go here */
#define PIC_CLK 32000000 // CLOCK SPEED
#define FCY (PIC_CLK/2) // INSTRUCTION SPEED



#define LED1 _RD3
#define LED2 _RD4

#define PORTC_SETUP 0b0000000000000000
#define PORTD_SETUP 0b0000000000000000
#define PORTF_SETUP 0b0000000000010100
#define PORTG_SETUP 0b0000001111001100
#define PORTB_SETUP 0b0000000000010000

#define SERVO_COUNT 24



/*
 * Params[x] definitions
 */
#define COMMAND Params[0]

/*
 * Legs and leg joints definitions. Servo to joint mapping
 *
 * Leg definitions
 *
 */

#define LEFT_FRONT 1
#define LEFT_MID 2
#define LEFT_BACK 3
#define RIGHT_FRONT 4
#define RIGHT_MID 5
#define RIGHT_BACK 6

/*
 * Leg joints
 */
#define LA_COXA 19
#define LM_COXA 2
#define LP_COXA 10
#define RA_COXA 23
#define RM_COXA 4
#define RP_COXA 12

#define LA_FEMUR 18
#define LM_FEMUR 1
#define LP_FEMUR 9
#define RA_FEMUR 20
#define RM_FEMUR 5
#define RP_FEMUR 13

#define LA_TIBIA 17
#define LM_TIBIA 0
#define LP_TIBIA 8
#define RA_TIBIA 21
#define RM_TIBIA 6
#define RP_TIBIA 14

#define STEP_DELAY 100


void OC1_setup();
void OC2_setup();
void OC3_setup();
void T3_setup();
void T2_setup();
void T4_setup();
void I2C1_setup();
void IC7_setup();

void InitApp(void);         /* I/O and Peripheral Initialization */




void stepForward(int stepCount);
void stepBackward(int stepCount);
void stepLeft(int stepCount);
void stepRight(int stepCount);
void turnLeft(int stepCount);
void turnRight(int stepCount);



extern void DelayUs( int pDelay );

extern int PWM_MidPos[], ServoCollapsed[];
extern int servoValue, servoChannel, x;
extern int delayTick;
extern volatile char rxBuff[], rxTemp[],debugchar;
extern unsigned char rxBuffp;
extern volatile unsigned int Params[];