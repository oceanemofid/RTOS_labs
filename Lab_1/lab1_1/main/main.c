/****************************************************************************
 * Copyright (C) 2020 by Fabrice Muller                                     *
 *                                                                          *
 * This file is useful for ESP32 Design course.                             *
 *                                                                          *
 ****************************************************************************/

/**
 * @file lab1-1_main.c
 * @author Fabrice Muller
 * @date 6 Oct. 2020
 * @brief File containing the lab1-1 of Part 3.
 *
 * @see https://github.com/fmuller-pns/esp32-vscode-project-template
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "esp_log.h"
#include "soc/rtc_wdt.h"

/* FreeRTOS includes. */
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_freertos_hooks.h"

#include "my_helper_fct.h"

/* Default stack size for tasks */
static const uint32_t STACK_SIZE = 4000;

/* Task Priority */
static const uint32_t T1_PRIO = 5;
static const uint32_t T2_PRIO = 6;


/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		(0x1FFFFF)

/* The task function. */
void vTaskFunction(void *pvParameters);

void app_main(void) {
	
	DISPLAY("Start of app_main task, priority = %d",uxTaskPriorityGet(NULL));
	vTaskSuspendAll();
	/* Create one of the two tasks. */
	xTaskCreate(vTaskFunction,	/* Pointer to the function that implements the task. */
		"Task 1",				/* Text name for the task.  This is to facilitate debugging only. */
		STACK_SIZE,				/* Stack depth  */
		(void*)"Task 1",		/* Pass the text to be printed in as the task parameter. */
		T1_PRIO,				/* Task priority */
		NULL);	
	
						/* We are not using the task handle. */
	xTaskCreate(vTaskFunction,	/* Pointer to the function that implements the task. */
		"Task 2",				/* Text name for the task.  This is to facilitate debugging only. */
		STACK_SIZE,				/* Stack depth  */
		(void*)"Task 2",		/* Pass the text to be printed in as the task parameter. */
		T2_PRIO,				/* Task priority */
		NULL);
	xTaskResumeAll();
	DISPLAY("==== End of creation ====");
	DISPLAY("==== Exit APP_MAIN ====");
	
	/* to ensure its exit is clean */
	vTaskDelete(NULL);
}
/*-----------------------------------------------------------*/

void vTaskFunction(void *pvParameters) {
	char *pcTaskName;
	volatile uint32_t ul;
	const TickType_t xDelay300ms = pdMS_TO_TICKS(300UL);


	/* Task Parameter */
	pcTaskName = (char *)pvParameters;
	
	DISPLAY("Start of %s task, priority = %d",pcTaskName, uxTaskPriorityGet(NULL));

	/* As per most tasks, this task is implemented in an infinite loop. */
	for (;; ) {
		DISPLAY("Run computation of %s", pcTaskName);

		/* Delay for simulating a computation */
		for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++){		
		}
		DISPLAY("Delay of %s", pcTaskName);
		vTaskDelay(xDelay300ms);
		DISPLAY("End of %s", pcTaskName);
	}
}

