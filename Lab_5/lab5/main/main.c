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
static const gpio_num_t PIN_USER_LED = 18;
// Red Led
static const gpio_num_t PIN_ALARM_LED = 25;
// Push button
static const gpio_num_t PIN_PUSH_BUTTON = 26;

/* Application constants */
static const uint32_t STACK_SIZE = 3000;
static const uint32_t MESS_MAX_LENGTH = 5;

static const uint32_t VALUE_MAX = 100;
static const uint32_t LOW_TRIG = 15;
static const uint32_t HIGH_TRIG = 85;

/* Semaphore Handler */
SemaphoreHandle_t xH;
SemaphoreHandle_t xScanH;
SemaphoreHandle_t xMonitoringH;
SemaphoreHandle_t xSemMutex;

/* Message Queue */
static QueueHandle_t msg_queue;

/* Variable declaration */
uint8_t sampleValue;
bool userMode = false, exitUserMode = false;

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
const uint32_t TIMER_TASK_PRIORITY = 3;
const uint32_t DIVIDER_TASK_PRIORITY = 3;
const uint32_t USER_TASK_PRIORITY = 3;
const uint32_t SCAN_TASK_PRIORITY = 3;
const uint32_t MONITORING_TASK_PRIORITY = 3;
const uint32_t OUTPUT_TASK_PRIORITY = 3;

/* Task declaration */
void vTaskTimer(void *pvParameters);
void vTaskDivider(void *pvParameters);
void vTaskScan(void *pvParameters);
void vTaskMonitoring(void *pvParameters);
void vTaskOutput(void *pvParameters);
void vTaskUser(void *pvParameters);

static void IRAM_ATTR Push_button_isr_handler(void *args);

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

	gpio_config_t config_in;
	config_in.intr_type = GPIO_INTR_NEGEDGE; //  falling  edge  interrupt
	config_in.mode = GPIO_MODE_INPUT;		 // Input  mode (push  button)
	config_in.pull_down_en = false;
	config_in.pull_up_en = true;						// Pull -up  resistor
	config_in.pin_bit_mask = (1ULL << PIN_PUSH_BUTTON); // Pin  number
	gpio_config(&config_in);

	/* Install ISR */
	gpio_install_isr_service(0);
	gpio_isr_handler_add(PIN_PUSH_BUTTON, Push_button_isr_handler, (void *)PIN_PUSH_BUTTON);

	/* Create semaphores */
	xH = xSemaphoreCreateBinary();
	xScanH = xSemaphoreCreateBinary();
	xMonitoringH = xSemaphoreCreateBinary();
	xSemMutex = xSemaphoreCreateMutex();

	/* Create Message Queue */
	msg_queue = xQueueCreate(MESS_MAX_LENGTH, sizeof(messageType));

	/* Stop Scheduler */
	vTaskSuspendAll();

	/* Create Tasks */
	xTaskCreatePinnedToCore(vTaskTimer, "vTaskTimer", STACK_SIZE, (void *)"vTaskTimer ", TIMER_TASK_PRIORITY, NULL, CORE_0);
	xTaskCreatePinnedToCore(vTaskDivider, "vTaskDivider", STACK_SIZE, (void *)"vTaskDivider ", DIVIDER_TASK_PRIORITY, NULL, CORE_0);
	xTaskCreatePinnedToCore(vTaskScan, "vTaskScan", STACK_SIZE, (void *)"vTaskScan", SCAN_TASK_PRIORITY, NULL, CORE_0);
	xTaskCreatePinnedToCore(vTaskMonitoring, "vTaskMonitoring", STACK_SIZE, (void *)"vTaskMonitoring", MONITORING_TASK_PRIORITY, NULL, CORE_1);
	xTaskCreatePinnedToCore(vTaskOutput, "vTaskOutput", STACK_SIZE, (void *)"vTaskOutput", OUTPUT_TASK_PRIORITY, NULL, CORE_0);
	xTaskCreatePinnedToCore(vTaskUser, "vTaskUser", STACK_SIZE, (void *)"vTaskUser", USER_TASK_PRIORITY, NULL, CORE_1);

	/* Continue Scheduler */
	xTaskResumeAll();

	/* to ensure its exit is clean */
	vTaskDelete(NULL);
}

void vTaskScan(void *pvParameters)
{
	while (1)
	{
		xSemaphoreTake(xScanH, portMAX_DELAY);
		/* Sample example */
		uint8_t iValue = rand() / (RAND_MAX / VALUE_MAX);
		// DISPLAY("Scan value = %d", iValue);
		messageType msg;

		if (iValue < LOW_TRIG)
		{
			msg.xInfo = Alarm;
			msg.iValue = iValue;
			xQueueSend(msg_queue, &msg, 0);
			// DISPLAY("Scan send msg alarm low");
		}
		else if (iValue > HIGH_TRIG)
		{
			msg.xInfo = Alarm;
			msg.iValue = iValue;
			xQueueSend(msg_queue, &msg, 0);
			// DISPLAY("Scan send msg alarm high");
		}
		else
		{
			xSemaphoreTake(xSemMutex, portMAX_DELAY);
			// DISPLAY("Scan updating sampleValue");
			sampleValue = iValue;
			xSemaphoreGive(xSemMutex);
		}
	}
}

void vTaskTimer(void *pvParameters)
{
	while (1)
	{
		TickType_t xLastWakeTime = xTaskGetTickCount();
		COMPUTE_IN_TIME_MS(10);
		// Period = 300 ms
		vTaskDelayUntil(&xLastWakeTime, portMAX_DELAY);
		xSemaphoreGive(xH);
		// DISPLAY("Timer H");
	}
}

void vTaskDivider(void *pvParameters)
{

	uint8_t cpt = 0;

	while (1)
	{
		xSemaphoreTake(xH, portMAX_DELAY);
		if (cpt % 5 == 0)
			xSemaphoreGive(xScanH);
		else if (cpt % 18 == 0)
			xSemaphoreGive(xMonitoringH);
		cpt++;
	}
}

void vTaskMonitoring(void *pvParameters)
{
	while (1)
	{
		xSemaphoreTake(xMonitoringH, portMAX_DELAY);
		xSemaphoreTake(xSemMutex, portMAX_DELAY);
		messageType msg;
		msg.xInfo = Monitoring;
		msg.iValue = sampleValue;
		xQueueSend(msg_queue, &msg, 0);
		// DISPLAY("Monitoring send msg");
		xSemaphoreGive(xSemMutex);
	}
}

void vTaskOutput(void *pvParameters)
{
	messageType msg;
	while (1)
	{
		xQueueReceive(msg_queue, &msg, 10000);
		if (!userMode)
		{
			if (msg.xInfo == Alarm)
			{
				DISPLAY("Alarm: value=%d", msg.iValue);
				gpio_set_level(PIN_ALARM_LED, 1);
			}
			else
			{
				DISPLAY("Monitoring: value=%d", msg.iValue);
				gpio_set_level(PIN_ALARM_LED, 0);
			}
		}
	}
}

void vTaskUser(void *pvParameters)
{

	while (1)
	{
		char car = 0;
		while (car != '@')
		{
			car = getchar();
			if (car != 0xff)
			{
				printf("%c", car);
				if (!userMode)
				{
					userMode = true;
					DISPLAY("Start user mode\n");
					gpio_set_level(PIN_USER_LED, 1);
				}
			}
			if (exitUserMode)
			{
				exitUserMode = false;
				break;
			}
			vTaskDelay(pdMS_TO_TICKS(50));
		}
		userMode = false;
		DISPLAY("\nEnd user mode");
		gpio_set_level(PIN_USER_LED, 0);
	}
}

static void IRAM_ATTR Push_button_isr_handler(void *args)
{
	exitUserMode = true;
}