#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "esp_log.h"

/* FreeRTOS.org includes. */
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "my_helper_fct.h"

static const uint32_t T1_PRIO = 2;
static const uint32_t T2_PRIO = 2;
static const uint32_t T3_PRIO = 3;


static const uint32_t STACK_SIZE = 4000;

QueueHandle_t xQueue1;

/**
 * @brief Starting point function
 * 
 */ 

void Task1(void *pvParameters);
void Task2(void *pvParameters);
void Task3(void *pvParameters);

void app_main(void) {

	xQueue1 = xQueueCreate(5, sizeof(uint32_t));

	vTaskSuspendAll();
	xTaskCreatePinnedToCore(Task1,	"Task 1", STACK_SIZE,	(void*)"Task 1", T1_PRIO,	NULL,CORE_1);
	xTaskCreatePinnedToCore(Task2,	"Task 2", STACK_SIZE,	(void*)"Task 2", T2_PRIO,	NULL,CORE_0);	
	xTaskCreatePinnedToCore(Task3,	"Task 3", STACK_SIZE,	(void*)"Task 3", T3_PRIO,	NULL,CORE_0);		

	xTaskResumeAll();
	DISPLAY("Inside of main");
	vTaskDelete(NULL);
	
}

void Task1(void *pvParameters) {

	TickType_t xDelayTime = xTaskGetTickCount();
	uint32_t lSentValue = 50;
	
	while(1){
	uint32_t result = xQueueSend(xQueue1, &lSentValue, 0);
	if (result == pdTRUE){
		DISPLAY("Task1 : Good message");
	}
	else {
		DISPLAY("Task1 : Bad message");
	}
	COMPUTE_IN_TIME_MS(40);
	vTaskDelayUntil(&xDelayTime, pdMS_TO_TICKS(500));
	}

}

void Task2(void *pvParameters) {

	uint32_t lReceivedValue = 0;
	BaseType_t xStatus;
	while(1){
		xStatus = xQueueReceive(xQueue1, &lReceivedValue, portMAX_DELAY);
		DISPLAY("Task2 running, Value = %d", lReceivedValue);
		COMPUTE_IN_TIME_MS(30);
	}

}

void Task3(void *pvParameters) {

	while(1){
	DISPLAY("Task3 ready");
	vTaskDelay(pdMS_TO_TICKS(100));
	DISPLAY("Task3 running");
	COMPUTE_IN_TIME_MS(20);
	}

}