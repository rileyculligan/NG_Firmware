/**
*****************************************************************************
**
**  File  (created by ThunderBench):  main.c v1.2
**
**  Snipet      : main function.
**
**  Functions   : main
**
**  Distribution: The file is distributed “as is,” without any warranty
**                of any kind.
**
*****************************************************************************
*/

/* Includes */
#include "../Libraries/includes.h"

/* defines */
#define MASK_RUN    (1<<2)
#define MEM_SIZE    250
#define PWM_FREQ    200000
#define PWM_PER     configCPU_CLOCK_HZ/PWM_FREQ
#define RUN_DIS()   GPIO->P[0].DOUTCLR = MASK_RUN
#define RUN_EN()    GPIO->P[0].DOUTSET = MASK_RUN
#define PUMP2_EN()  GPIO->P[4].MODEH |=  4<<4*(8-8)
#define PUMP2_DIS() GPIO->P[4].MODEH &= ~0xf<<4*(8-8)
#define PUMP_EN()   GPIO->P[4].DOUTSET = 1<<9
#define PUMP_DIS()  GPIO->P[4].DOUTCLR = 1<<9
#define PUMP_TGL()  GPIO->P[4].DOUTTGL = 1<<9
/******************************************************************************
 *              Typedefs and globals
 */
#define DI_CMU_LFRCOCTRL (0xFE08028)
#define DEADTIME 1

TaskHandle_t xLEDTask = NULL;
TaskHandle_t xPWMTask = NULL;
TaskHandle_t xDACTask = NULL;

UBaseType_t HighWaterMark;
/******************************************************************************
 *              Function Prototypes
*/
//Tasks
void ButtonTask(void * pvParameters);
void PWMTask(void * pvParameters);
void DACTask(void * pvParameters);
//Other Functions
void ChangePWMDC(int32_t dc, CONVERTERSTATETYPE state);
void ButtonPress(void);
void ButtonRelease(void);
/******************************************************************************
*               Function Definitions
*/

/****************** DACTask()
 * Description:     Initializes and controls the DAC and runs the battery charger
 * Params           none
 * Return           none
 * Last Edited:     12/22/2015 RTC
 */
void DACTask(void * pvParameters)
{
    uint32_t bat;
    uint32_t dac = 0;
    uint32_t curr = 0;
    //Init the DAC
    DAC0->CTRL = 0x0130;
    DAC0->CH0CTRL = 0x0001;
    DAC0->CH0DATA = dac;
    while(1)
    {
        xTaskNotifyWait(0, 0xffffffff, &bat, portMAX_DELAY);
        dac = curr*3/10 + bat;  // Calculating Vdac in mv
        dac = (dac*0xfff)/4250; // Convert Vdac to ndac
        DAC0->CH0DATA = dac;
    }
}

/****************** ButtonPress()
 * Description:     Handles what action to take during a button press.  CUrrent code
 *                  is purely used to test the system, not final.
 * Params           none
 * Return           none
 * Date Created:    12/4/15 RTC
 */
void ButtonPress(void)
{
    static int32_t i = 0;
    //xTaskNotifyGive(xPWMTask);
    //xTaskNotifyGive(xLEDTask);
    GPIO->P[4].MODEH |=  4<<4*(8-8);    //PUMP2_EN();
    GPIO->P[4].DOUTSET = 1<<9;          // PUMP_EN();
#if 0
    ChangePWMDC(18, BOOST);
#endif
#if 0
    ChangePWMDC(i, BUCK);
    if (i <= 100)
    {
        i += 4;
    }
    else
    {
        i = 0;
    }
#endif
#if 0
    for (i = 0; i <= 100; i += 4)
    {
        ChangePWMDC(i, BUCK);
        LEDChangePixels(0x80, BLUE);
        vTaskDelay(2000);
        ChangePWMDC(-1, BUCK);
        LEDChangePixels(0x80, BLACK);
        vTaskDelay(2000);
    }
#endif
}

/****************** ButtonRelease()
 * Description:     Handles what action is take during a button release.
 * Params           none
 * Return           none
 * Date Created:    12/4/15 RTC
 */
void ButtonRelease(void)
{
    ChangePWMDC(-1, BOOST);
    PUMP_DIS();
    PUMP2_DIS();
}

/****************** ButtonTask()
 * Description:     Handles button presses.  The button is PAE12.
 * Params           none
 * Return           none
 * Last Edited:     11/16/15 RTC
 */
#define BUTTONISPRESSED !(GPIO->P[4].DIN&(1<<12))   //active low button
#define BUTTONDELAY     10  // ms delay for task
void ButtonTask(void *pvParameters)
{
    BUTTONSTATETYPE button1 = BUTOFF;
    //Main button init (PE12)
    GPIO->P[4].MODEH &= ~0xf<<4*(12-8);	//g_pGPIO->PA_MODEL &= ~0xf;
    GPIO->P[4].MODEH |=  0x3<<4*(12-8);	//g_pGPIO->PA_MODEL |=  0x3;
    GPIO->P[4].DOUTSET = 1<<12;			//g_pGPIO->PA_DOUTSET = 1<<0;
    /* Init the Pump MICROA PE8 (active LOW) and MICROB PE9 (active HIGH) */
    PUMP_DIS();
    PUMP2_DIS();
    GPIO->P[4].DOUTCLR = 1<<8;          //Pump init (MICROA, PE8)
    GPIO->P[4].MODEH |= 0x4<<4*(9-8);   //Pump2 init (MICROB, PE9)
    // Loop
    while (1) {
        vTaskDelay(BUTTONDELAY);
        switch (button1) {
        case BUTOFF:
            if (BUTTONISPRESSED)
            {
                button1 = BUTWAIT;
            }
            break;
        case BUTON:
            if (!BUTTONISPRESSED)
            {
                ButtonRelease();
                button1 = BUTOFF;
            }
            else
            {
                /* holding button press action */
            }
            break;
        case BUTWAIT:
            if (BUTTONISPRESSED)
            {
            	//ADD PRESS BUTTON ACTION HERE
                ButtonPress();
                button1 = BUTON;
            }
            else
            {
                button1 = BUTOFF;
            }
            break;
        default:
            // failure of some sort
            break;
        }
    }
}

/****************** ChangePWMDC()
 * Description:     Change the PWM duty cycle
 * Params           dc: 0-100 - percent duty cycle
 *                      -1    - turn off both lines
 *                  state: {BOOST, BUCK}
 * Returns:         None
 * Date Created:    11/19/15 RTC
 */
void ChangePWMDC(int32_t dc, CONVERTERSTATETYPE state)
{
    uint16_t ccvb = (dc*PWM_PER)/100;
    if (state == BOOST)
    {
        ccvb /= 2;
        if (dc == 0)
        {
            TIMER1->CC[0].CCVB = PWM_PER + 1;
            TIMER0->CC[2].CCVB = 0;
            TIMER0->CC[2].CCVB = 0;
        }
        else if ((dc <= 100) && (dc > 0))
        {
            TIMER1->CC[0].CCVB = PWM_PER + 1;
            TIMER0->CC[2].CCVB = ccvb;
            TIMER0->CC[1].CCVB = ccvb+DEADTIME;
        }
        else if (dc == -1)
        {
            TIMER0->CC[1].CCVB = 0;
            TIMER0->CC[1].CCV  = 0;
            TIMER0->CC[2].CCVB = PWM_PER/2 + 1;
            TIMER0->CC[2].CCV  = PWM_PER/2 + 1;
            TIMER1->CC[0].CCVB = 0;
            TIMER1->CC[0].CCV  = 0;
        }
    }
    else if (state == BUCK)
    {
        if ((dc <= 100) && (dc >= 0))
        {
            TIMER1->CC[0].CCVB = ccvb;
            TIMER0->CC[2].CCVB = 0;
        }
        else if (dc == -1)
        {
            TIMER1->CC[0].CCVB = 0;
            TIMER1->CC[0].CCV  = 0;
            TIMER0->CC[2].CCVB = PWM_PER/2 + 1;
            TIMER0->CC[2].CCV  = PWM_PER/2 + 1;
        }
    }
}
/****************** PWMTask()
 * Description:     Initializes and controls the PWM modules.  Runs on HFPERCLK.
 *                  VGATE1: PA1 - TIM0 CC1 #0/#1 - L1 to GND
 *                  VGATE4: PC3 - TIM0 CDTI1 #4  - L1 to OUT
 *                  VGATE2: PB7 - TIM1 CC0 #3    - Safe SW
 *                  VGATE4: PA2 - TIM0 CC2 #0/#1 - L1 to OUT ALTERNATE, must be cut (pin3 on U1)
 * Params           None
 * Date Created:    11/18/15 RTC
 */
#define TIMER0_START()   TIMER0->CMD = 1
#define TIMER0_STOP()    TIMER0->CMD = 0
#define TIMER1_START()   TIMER1->CMD = 1
#define TIMER1_STOP()    TIMER1->CMD = 0
void PWMTask(void * pvParameters)
{
    uint16_t dc = 0;
    /*** Setup TIMER0 for VGATE1 and VGATE4 for BOOST mode ***/
    TIMER0->TOPB = PWM_PER/2;
    TIMER0->TOP  = PWM_PER/2;
    TIMER0->CTRL |= 2;          // UP-DOWN COUNT mode (so we can manually create the deadtime)
    TIMER0->ROUTE = 1<<16       //Using position 1 for TIMER0 CC1 (PA1)
            | 1<<1              //enables CC1
            | 1<<2;             //enables CC2
    /** Setup Primary VGATE1 (CC1) **/
    TIMER0->CC[1].CTRL = 3<<0   //Enable PWM mode
            | 1<<4              //initial value is 1.
            | 1<<2;             //inverts output
    GPIO->P[0].MODEL |= 4<<4*1; //Enable output on PA1
    GPIO->P[0].CTRL   = 2;      //Sets drive current to HIGH
    /** Setup Secondary VGATE4 (CC2) **/
    TIMER0->CC[2].CTRL = 3<<0   //Enables PWM mode
            | 1<<4;             //Initial value is 1
    GPIO->P[0].MODEL |= 4<<4*2; //Enable output on PA2
    ChangePWMDC(-1, BOOST);
    /*** Setup TIMER1 for VGATE 2 for BUCK mode ***/
    TIMER1->TOPB = PWM_PER;
    TIMER1->TOP  = PWM_PER;
    TIMER1->CTRL = 0;           //UP COUNT mode, since we don't have to worry about deadtime
    TIMER1->ROUTE = 3<<16       //Using position 3 for TIMER1 CC0 (PB7)
            | 1<<0;             //enables CC0
    /** Setup Primary VGATE2 (CC0) **/
    TIMER1->CC[0].CTRL = 3<<0   //Enable PWM mode
            | 1<<4              //initial value is 1.
            | 1<<2;             //inverts output
    GPIO->P[1].MODEL |= 5<<4*7; //Enable output on PB7
    GPIO->P[1].CTRL   = 2;      //Sets drive current to HIGH
    ChangePWMDC(-1, BUCK);
    /** Start the Timer **/
    TIMER0_START();
    TIMER1_START();
    while(1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        for(dc = 0; dc <= PWM_PER; dc++)
        {
            TIMER1->CC[0].CCVB = dc;
            vTaskDelay(1000);
        }
        PUMP_DIS();
        PUMP2_DIS();
    }
}

/****************** main()
 * Description:     Creates all tasks, semaphores, mutexes, and queues then
 *                  begins the FreeRTOS scheduler.
 * Params           None
 * Return           None
 * Last Edited:     11/17/14 RTC
 */
int main(void)
{
    int dummy = CMU->HFRCOCTRL;
    CMU->HFRCOCTRL = (CMU->HFRCOCTRL & ~(0x7ff)) | 5<<8
            | ((DEVINFO->HFRCOCAL1 & _DEVINFO_HFRCOCAL1_BAND28_MASK) >> _DEVINFO_HFRCOCAL1_BAND28_SHIFT);
    dummy = CMU->HFRCOCTRL;
    CMU->HFPERCLKEN0 |= (1 << 13)   //Enable GPIO clock
             | (1<<16)              //Enable ADC clock
             | (1<<17)              //Enable DAC clock
             | (1<<5 )              //Enable Timer0 (Timer1 is <<6)
             | (1<<6 );             //Enable Timer1
    xTaskCreate(ButtonTask, "Buttn", 175, NULL, 4, NULL);
    xTaskCreate(LEDTask, "LED", 128, NULL, 1, &xLEDTask);
    xTaskCreate(PWMTask, "PWM", 128, NULL, 5, &xPWMTask);
    xTaskCreate(ADTask, "ADC", 128, NULL, 2, NULL);
//    xTaskCreate(DACTask, "DAC", 128, NULL, 3, &xDACTask);
    PuffSem = xSemaphoreCreateBinary();
    vTaskStartScheduler();  //Begin kernel
    while (dummy)
    {}

  /* Never reach this statement. */
  return (0);
}
