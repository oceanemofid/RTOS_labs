/****************************************************************************
 * Copyright (C) 2020 by Fabrice Muller                                     *
 *                                                                          *
 * This file is useful for ESP32 Design course.                             *
 *                                                                          *
 ****************************************************************************/

/**
 * @file lab3-1_main.c
 * @author Fabrice Muller
 * @date 20 Oct. 2020
 * @brief File containing the lab3-1 of Part 3.
 *
 * @see https://github.com/fmuller-pns/esp32-vscode-project-template
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include "esp_log.h"

/* FreeRTOS.org includes. */
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "my_helper_fct.h"

static const char *TAG = "SEM";

/* Application constants */
#define STACK_SIZE 4096
#define TABLE_SIZE 400

/* Task Priority */
const uint32_t TIMER_TASK_PRIORITY = 5;
const uint32_t INC_TABLE_TASK_PRIORITY = 3;
const uint32_t DEC_TABLE_TASK_PRIORITY = 4;

/* Communications */

/* Tasks */
void vTaskTimer(void *pvParameters);
void vTaskIncTable(void *pvParameters);
void vTaskDecTable(void *pvParameters);

/* Datas */
int Table[TABLE_SIZE];
int ActivationNumber = 0;
int constNumber = 1;

SemaphoreHandle_t xSemIncTab;
SemaphoreHandle_t xSemDecTab;
TaskHandle_t inc, dec;

/* Main function */
void app_main(void)
{

	/* Init Table */
	memset(Table, 0, TABLE_SIZE * sizeof(int));

	/* Create semaphore */
	xSemIncTab = xSemaphoreCreateBinary();
	xSemDecTab = xSemaphoreCreateBinary();

	/* Stop scheduler */
	vTaskSuspendAll();

	/* Create Tasks */
	xTaskCreatePinnedToCore(vTaskTimer, "vTaskTimer", STACK_SIZE, (void *)"vTaskTimer ", TIMER_TASK_PRIORITY, NULL, CORE_0);
	xTaskCreatePinnedToCore(vTaskIncTable, "vTaskIncTable", STACK_SIZE, (void *)"vTaskIncTable ", INC_TABLE_TASK_PRIORITY, NULL, CORE_0);
	inc = xTaskGetHandle("vTaskIncTable");
	xTaskCreatePinnedToCore(vTaskDecTable, "vTaskDecTable", STACK_SIZE, (void *)"vTaskDecTable ", DEC_TABLE_TASK_PRIORITY, NULL, CORE_1);
	dec = xTaskGetHandle("vTaskDecTable");

	/* Continue scheduler */
	xTaskResumeAll();

	/* to ensure its exit is clean */
	vTaskDelete(NULL);
}
/*-----------------------------------------------------------*/

void vTaskTimer(void *pvParameters)
{
	while (1)
	{
		vTaskDelay(pdMS_TO_TICKS(250));
		xTaskNotifyGive(inc);
		xTaskNotifyGive(inc);
		DISPLAY("Task IncTab : give 2 notify");
		// xSemaphoreGive(xSemIncTab);
		xTaskNotifyGive(dec);
		DISPLAY("Task DecTab : give notify");
		// xSemaphoreGive(xSemDecTab);
	}
}

void vTaskIncTable(void *pvParameters)
{
	while (1)
	{
		uint32_t pending_counter_inc = ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		DISPLAY("pending_counter_inc =%d ", pending_counter_inc);
		// xSemaphoreTake(xSemIncTab,portMAX_DELAY);
		DISPLAY("Task IncTable : take notify");
		if (ActivationNumber == 0)
		{
			for (int index = 0; index < TABLE_SIZE; index++)
			{
				Table[index] = Table[index] + constNumber;
			}
			ActivationNumber = 4;
			vTaskDelay(pdMS_TO_TICKS(50));
		}
		else
		{
			ActivationNumber--;
		}
		DISPLAY("Task IncTable is ending");
	}
}

void vTaskDecTable(void *pvParameters)
{
	while (1)
	{
		uint32_t pending_counter_dec = ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		DISPLAY("pending_counter_dec =%d ", pending_counter_dec);
		// xSemaphoreTake(xSemDecTab,portMAX_DELAY);
		DISPLAY("Task DecTable : take notify");
		for (int index = 0; index < TABLE_SIZE; index++)
		{
			Table[index] = Table[index] - 1;
		}
		vTaskDelay(pdMS_TO_TICKS(50));
		DISPLAY("Task DecTable is ending");
	}
}
