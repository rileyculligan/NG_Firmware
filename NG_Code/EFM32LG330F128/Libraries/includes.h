/*
 * includes.h
 *
 *  Created on: Nov 30, 2015
 *      Author: riley.culligan
 */

/******************************************************************************
 * Includes
 */
#include <stdint.h>
#include <stdbool.h>
#include <FreeRTOS.h>
#include <EFM32LG330F128.h>
#include "mpu_wrappers.h"
#include "portmacro.h"
#include "portable.h"
#include "queue.h"
#include "semphr.h"
#include "StackMacros.h"
#include "task.h"
#include "list.h"
#include "../Libraries/LED_Module.h"
#include "../Libraries/ADC_Module.h"
#include "datatypes.h"


/******************************************************************************
 * RTOS elements
 */
//Semaphores/mutexes
SemaphoreHandle_t PuffSem;
SemaphoreHandle_t LEDSem;
SemaphoreHandle_t LEDRedMut;
SemaphoreHandle_t LEDGreenMut;
SemaphoreHandle_t LEDBlueMut;

//Task Handles
extern TaskHandle_t xPWMTask;
extern TaskHandle_t xDACTask;
extern TaskHandle_t xLEDTask;
