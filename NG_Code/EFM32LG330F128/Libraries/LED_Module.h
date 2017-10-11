/******************************************************************************
 *      LED_Module.h
 *      Description:    This file contains the function includes for LED_Module
 *      Engineer:       Riley Culligan
 *      Date Created:   11/30/2015
 */

/******************************************************************************
 * Typedefs
 */
#include "datatypes.h"

/******************************************************************************
 * Functions
 */
#ifndef LIBRARIES_LED_MODULE_H_
#define LIBRARIES_LED_MODULE_H_

void LEDChangePixels(int pixels, LEDCOLOR color);
void LEDUpdateLayer(uint8_t pixels, LEDCOLOR color);
void LEDTask(void * pvParameters);

#endif /* LIBRARIES_LED_MODULE_H_ */
