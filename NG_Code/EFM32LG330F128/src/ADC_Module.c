/******************************************************************************

 *      ADC_Module.c
 *      Description:    This file contains all the LED UI related resources.
 *      Engineer:       Riley Culligan
 *      Date Created:   12/1/2015
 */
/******************************************************************************
 * Includes and defines
 */
#include "../Libraries/includes.h"

#define ADC_NOT_READY   (ADC0->STATUS & 1)
#define ADC_VOLTS       (ADC_DATA*2500/0xffff)
#define ADC0_CH3        0x00210330  //Bat Temp
#define ADC0_CH4        0x00210430  //Vbat
#define ADC0_CH5        0x00210530  //IMON
#define ADC0_CH6        0x00210630  //VMON
#define ADC0_CH7        0x00210730  //VREF
#define ADC_DATA        ADC0->SINGLEDATA
#define CAPTURE_ADC0()  ADC0->CMD = 0x01

/******************************************************************************
 * Global Vars
 */
uint32_t Vref, Battery, Outv, Outi, OutiCalc, BatTemp;

/******************************************************************************
 *              Function Prototypes
*/
//Tasks
void ADTask(void * pvParameters);
//Functions
static uint32_t SingleRead(uint32_t singlectrl);
/****************** SingleRead()
 * Description:     Reads a single AD line
 * Params           singlectrl:     The init sent to ADC0->SINGLECTRL
 * Return           The voltage value on the ADC line.
 * Last Edited:     4/6/2015 RTC
 */
static uint32_t SingleRead(uint32_t singlectrl)
{
    ADC0->SINGLECTRL = singlectrl;
    CAPTURE_ADC0();
    vTaskDelay(5);
    while (ADC_NOT_READY)
    {/* Wait for ADC to capture data*/}
    return (ADC_VOLTS);
}

/****************** ADTask()
 * Description:     Initializes the AD converters, and saves their values for
 *                  debugging.  Testing the safety switch (ADC0_CH5), Battery
 *                  monitor (ADC0_CH7), OUTV_MON (ADC0_CH6), and
 *                  OUTI_MON (ADC0_CH4)
 * Params           none
 * Return           none
 * Last Edited:     4/6/2015 RTC
 */
void ADTask(void * pvParameters)
{
    GPIO->P[4].MODEH |= 5<<4*(13-8);    //Enables the voltage reference
    GPIO->P[4].DOUTSET = 1<<13;
    GPIO->P[4].CTRL = 1;        //output set to "Lowest"
    ADC0->CTRL = 0x030F0323; //g_pADC0->CTRL = 0x030F0323; //x16 oversampling, 16bits resolution
    while (1)
    {

        /***            IMON                ***/
        Outi = SingleRead(ADC0_CH5);   //Convert to mV
        OutiCalc *= 5;
        OutiCalc /= 2;
        /***            OUTV_MON            ***/
        Outv = SingleRead(ADC0_CH6);
        Outv *= 14545;
        Outv /= 4545;
        /***            Voltage Reference   ***/
        Vref = SingleRead(ADC0_CH7);
        /***            Vbat                ***/
        Battery = SingleRead(ADC0_CH4);
        Battery = Battery*7/2;
        xTaskNotify(xDACTask, Battery, eSetValueWithOverwrite);
        /***            Bat_Temp            ***/
        BatTemp = (SingleRead(ADC0_CH3)*Vref/0xffff);
        BatTemp = (BatTemp*(1+10)-Vref*1)/(Vref-BatTemp);
    }
}
