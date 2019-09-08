/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

/* TODO Application specific user parameters used in user.c may go here */

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

/* TODO User level functions prototypes (i.e. InitApp) go here */
#define PIC_CLK 32000000 // CLOCK SPEED
#define FCY (PIC_CLK/2) // INSTRUCTION SPEED

#define BAUDRATE          9600
#define BRGVAL          ((FCY/BAUDRATE)/16)-1

#define I2CCLOCK    400000
#define I2CBRGVAL   ((FCY/I2CCLOCK)-(FCY/1111111))

#define LED1 _RD3
#define LED2 _RD4

#define PORTC_SETUP 0b0000000000000000
#define PORTD_SETUP 0b0000000000000000
#define PORTF_SETUP 0b0000000000010100
#define PORTG_SETUP 0b0000001111001100
#define PORTB_SETUP 0b0000000000010000

#define PWM1 _RD0 // USED BY PWM BLOCK
#define PWM2 _RD1
#define PWM3 _RD2

#define MUX_A0 _RD10 // PWM BLOCK MULTIPLEXOR PINS
#define MUX_A1 _RD9
#define MUX_A2 _RD8


#define PWM_RANGE 1000
#define PWM_MID 3000
#define PWM_MIN 2500
#define PWM_MAX 3500
#define OFFSET 10
#define SERVO_COUNT 24
#define START_T3 T3CONbits.TON = 1
#define STOP_T3 T3CONbits.TON = 0

#define BUFF_SIZE 128
#define PARAM_COUNT 3
#define PARAM_SIZE 4

#define EEPROM_ADDR 0xa0

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

extern void UART1_setup();
extern void UART2_setup();
extern void OC1_setup();
extern void OC2_setup();
extern void OC3_setup();
extern void T3_setup();
extern void T2_setup();
extern void T4_setup();
extern void I2C1_setup();
extern void IC7_setup();

void InitApp(void);         /* I/O and Peripheral Initialization */
#define CALIB_MID 0
#define CALIB_MAX 48
#define CALIB_MIN 96

extern void I2C1WaitForIdle( void );
extern void I2C1Start();
extern void I2C1Restart();
extern void I2C1Stop();
extern char I2C1Read( char pAck );
extern int I2C1Write( char pData );
extern void EEPROMWriteChar( int pAddr, char pData );
extern char EEPROMReadChar( int pAddr );
extern void EEPROMWritePage32( int pAddr, char *pData );
void LoadMid(int *midData);
extern void DelayUs( int pDelay );
void stepForward(int stepCount);
void stepBackward(int stepCount);
void stepLeft(int stepCount);
void stepRight(int stepCount);
void turnLeft(int stepCount);
void turnRight(int stepCount);
volatile int PWM_Bank[SERVO_COUNT], PWM_Pointer;
int PWM_MidPos[SERVO_COUNT], ServoCollapsed[SERVO_COUNT];
int servoValue, servoChannel, x, needDecode;
int delayTick;

void SendByte(unsigned char port, const char byte);
void WriteString(const char *string, unsigned char port);


volatile char rxBuff[BUFF_SIZE], rxTemp[BUFF_SIZE],debugchar;
unsigned char rxBuffp;
volatile unsigned int Params[PARAM_COUNT];