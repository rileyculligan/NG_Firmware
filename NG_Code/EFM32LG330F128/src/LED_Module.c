/******************************************************************************
 *      LED_Module.c
 *      Description:    This file contains all the LED UI related resources.
 *      Engineer:       Riley Culligan
 *      Date Created:   11/30/2015
 */

/******************************************************************************
 * Includes
 */
#include "../Libraries/includes.h"

/******************************************************************************
 * Global Vars
 */
static uint8_t LEDLayerRed = 0x1;
static uint8_t LEDLayerGreen = 0x4;
static uint8_t LEDLayerBlue = 0x2;

/******************************************************************************
 * Functions and tasks
 */
//void LEDTask(void * pvParameters);
static void ChangeLED(uint8_t ledsel, uint8_t redcntrl, uint8_t greencntrl, uint8_t bluecntrl);
//void LEDChangePixels(int pixels, LEDCOLOR color);
//void LEDUpdateLayer(uint8_t pixels, LEDCOLOR color);

/******** LEDChangePixel()
 * Description:     Update the color of specified pixels
 * params:          pixels: specify which pixels' color you want to change
 *                  color:  the color you want to change the pixels to
 * return:          none
 * Last Edited:     11/14/14 RTC
 */
void LEDChangePixels(int pixels, LEDCOLOR color)
{
    //Change red
    xSemaphoreTake(LEDRedMut, portMAX_DELAY);
    LEDLayerRed = (color & RED) ? (LEDLayerRed|pixels) : (LEDLayerRed&~pixels);
    xSemaphoreGive(LEDRedMut);
    //Change green
    xSemaphoreTake(LEDGreenMut, portMAX_DELAY);
    LEDLayerGreen = (color & GREEN) ? (LEDLayerGreen|pixels) : (LEDLayerGreen&~pixels);
    xSemaphoreGive(LEDGreenMut);
    //Change blue
    xSemaphoreTake(LEDBlueMut, portMAX_DELAY);
    LEDLayerBlue = (color & BLUE) ? (LEDLayerBlue|pixels) : (LEDLayerBlue&~pixels);
    xSemaphoreGive(LEDBlueMut);
}
/****************** LEDUpdateLayer()
 * Description:     Update 1 specific subpixel layer (i.e. red, green, blue)
 * Params           pixels: specify which pixels will be on for the given color
 *                  color: color layer you want to change
 * Return           none
 * Last Edited:     11/17/14 RTC
 */
void LEDUpdateLayer(uint8_t pixels, LEDCOLOR color)
{
    switch (color)
    {
    case RED:
        xSemaphoreTake(LEDRedMut, portMAX_DELAY);
            LEDLayerRed = pixels;
        xSemaphoreGive(LEDRedMut);
        break;
    case GREEN:
        xSemaphoreTake(LEDGreenMut, portMAX_DELAY);
            LEDLayerGreen = pixels;
        xSemaphoreGive(LEDGreenMut);
        break;
    case BLUE:
        xSemaphoreTake(LEDBlueMut, portMAX_DELAY);
            LEDLayerBlue = pixels;
        xSemaphoreGive(LEDBlueMut);
        break;
    default:
        //Wrong color given
        break;
    }
}
/****************** LEDTask()
 * Description:     Initializes and PWMs the LEDs
 * Params           none
 * Return           none
 * Last Edited:     11/14/14 RTC
 */
void LEDTask(void * pvParameters) {

    uint32_t i;
    LEDRedMut =   xSemaphoreCreateMutex();
    LEDGreenMut = xSemaphoreCreateMutex();
    LEDBlueMut =  xSemaphoreCreateMutex();
    GPIO->P[2].CTRL = 1;    //g_pGPIO->PC_CTRL = 1;           //Set to "lowest" current, i.e. ~0.1mA
    //GPIO->P[1].MODEH |= 5<<0;      //Here mostly for an example
    GPIO->P[2].MODEL |= 5<<4*1;
    for (i = 0; i <= 11; i++)
    {
        if (i == 3)
        {
            continue;
        }
        else if (i <= 2)
        {
            GPIO->P[2].DOUTSET = 1<<i;
        }
        else
        {
            GPIO->P[2].DOUTCLR = 1<<i;
        }

        if (i <= 7)
        {
            GPIO->P[2].MODEL |= 5<<4*(i);
        }
        else
        {
            GPIO->P[2].MODEH |= 5<<4*(i-8);
        }
    }
    i = GPIO->P[2].MODEL;
    i = GPIO->P[2].MODEH;
    while (1) {
        // 15ms PWM period
        vTaskDelay(5);
        ChangeLED(LEDLayerRed, 1, 0, 0);
        vTaskDelay(5);
        ChangeLED(LEDLayerGreen, 0, 1, 0);
        vTaskDelay(5);
        ChangeLED(LEDLayerBlue, 0, 0, 1);
        if (ulTaskNotifyTake(pdTRUE, 0))
        {
            if (LEDLayerRed < 0x80)
            {
                LEDLayerRed <<= 1;
            }
            else
            {
                LEDLayerRed = 1;
            }
        }
    }
}
/****************** ChangeLED()
 * Description:     Bit blasts to change LEDs and colors.
 * Params           ledsel: Which LEDs are supposed to be on.
 *                  redcntrl: Controls red subpixel
 *                  greencntrl: controls green subpixel
 *                  bluecntrl: controls blue subpixel
 * Return           none
 * Last Edited:     11/14/14 RTC
 */
static void ChangeLED(uint8_t ledsel, uint8_t redcntrl, uint8_t greencntrl, uint8_t bluecntrl)
{
    //Shuts off red, green, and blue
    GPIO->P[2].DOUTSET = 7;
    GPIO->P[2].DOUTCLR = 0xff<<4;
    //Set new LEDs
    GPIO->P[2].DOUTSET = (ledsel & 0xfe)<<3 | (ledsel & 1)<<11;
    GPIO->P[2].DOUTCLR = ((redcntrl<<2) + (greencntrl<<1) + bluecntrl);
}
