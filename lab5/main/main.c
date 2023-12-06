/****************************************************************************
 * Copyright (C) 2020 by Fabrice Muller                                     *
 *                                                                          *
 * This file is useful for ESP32 Design course.                             *
 *                                                                          *
 ****************************************************************************/

/**
 * @file lab5_main.c
 * @author Fabrice Muller
 * @date 21 Oct. 2020
 * @brief File containing the lab5 of Part 3.
 *
 * @see https://github.com/fmuller-pns/esp32-vscode-project-template
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "driver/gpio.h"

/* FreeRTOS.org includes. */
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "my_helper_fct.h"

static const char *TAG = "APP";

// Green Led
static const gpio_num_t PIN_USER_LED = 2;
// Red Led
static const gpio_num_t PIN_ALARM_LED = 13;
// Push button
static const gpio_num_t PIN_PUSH_BUTTON = 15;

/* Application constants */
static const uint32_t STACK_SIZE = 3000;
static const uint32_t MESS_MAX_LENGTH = 5;

static const uint32_t VALUE_MAX = 100;
static const uint32_t LOW_TRIG = 15;
static const uint32_t HIGH_TRIG = 85;

/* Semaphore Handler */

/* Message Queue */

/* Variable declaration */

/* Type definition */
enum typeInfo
{
	Alarm,
	Monitoring
};

typedef struct
{
	enum typeInfo xInfo;
	int iValue;
} messageType;

/* Task Priority */
const uint32_t TIMER_TASK_PRIORITY = ;
const uint32_t DIVIDER_TASK_PRIORITY = ;
const uint32_t USER_TASK_PRIORITY = ;
const uint32_t SCAN_TASK_PRIORITY = ;
const uint32_t MONITORING_TASK_PRIORITY = ;
const uint32_t OUTPUT_TASK_PRIORITY = ;

/* Task declaration */
void vTaskTimer(void *pvParameters);
void vTaskDivider(void *pvParameters);
void vTaskScan(void *pvParameters);
void vTaskMonitoring(void *pvParameters);
void vTaskOutput(void *pvParameters);
void vTaskUser(void *pvParameters);

/**
 * @brief Starting point function
 *
 */

void app_main(void)
{

	/* GPIO input/output */

	// Configuration of GPIO outputs : Leds
	gpio_config_t config_out;
	config_out.intr_type = GPIO_INTR_DISABLE;
	config_out.mode = GPIO_MODE_OUTPUT;
	config_out.pin_bit_mask = (1ULL << PIN_USER_LED) | (1ULL << PIN_ALARM_LED);
	gpio_config(&config_out);

	/* Install ISR */

	/* Create semaphores */

	/* Create Tasks */
	xTaskCreatePinnedToCore(vTaskTimer, "vTaskTimer", STACK_SIZE, (void *)"vTaskTimer ", TIMER_TASK_PRIORITY, NULL, CORE_0);
	xTaskCreatePinnedToCore(vTaskDivider, "vTaskDivider", STACK_SIZE, (void *)"vTaskDivider ", DIVIDER_TASK_PRIORITY, NULL, CORE_0);
	// inc = xTaskGetHandle("vTaskIncTable");
	xTaskCreatePinnedToCore(vTaskDecTable, "vTaskDecTable", STACK_SIZE, (void *)"vTaskDecTable ", DEC_TABLE_TASK_PRIORITY, NULL, CORE_1);
	// dec = xTaskGetHandle("vTaskDecTable");

	/* to ensure its exit is clean */
	vTaskDelete(NULL);
}

void vTaskScan(void *pvParameters)
{

	/* Sample example */
	iValue = rand() / (RAND_MAX / VALUE_MAX);

	if (iValue < LOW_TRIG)
	{
	}
	else if (iValue > HIGH_TRIG)
	{
	}
	else
	{
	}
}

void vTaskUser(void *pvParameters)
{

	/* Sample example */
	/* Scan keyboard every 50 ms */
	while (car != '@')
	{
		car = getchar();
		if (car != 0xff)
		{
			printf("%c", car);
			break;
		}
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}