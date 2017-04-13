/*
 * Keyfinder.c
 *
 *  Created on: 10.04.2017
 *      Author: Matthias
 */

#include "Platform.h"
#include "Platform_Local.h"
#include "Keyfinder.h"
#include "FRTOS1.h"
#include "LED1.h"
#include "BAT_KON_LED.h"
#include "ALERT_LED.h"
#include "ALERT_BUZZER.h"
#include "BAT_KON.h"

void KeyfinderAlert(bool state){
	//Keyfinder wird angepingt
	if(state==1){
		LED1_On();
		ALERT_LED_On();
		ALERT_BUZZER_SetVal();
	}
	else{
		LED1_Off();
		ALERT_LED_Off();
		ALERT_BUZZER_ClrVal();
	}
}

static void KeyfinderBatEvaluationTask(void *pvParameters){
	(void)pvParameters; /* not used */
	for(;;){
		//Batteriekontrolle Auswertung
		if(BAT_KON_GetVal()){
			BAT_KON_LED_On();
		}
		else{
			BAT_KON_LED_Off();
		}
		FRTOS1_vTaskDelay(5000/portTICK_PERIOD_MS);
	}
}

void KeyfinderBatEvalutaionInit(void){
	  if (FRTOS1_xTaskCreate(
	        KeyfinderBatEvaluationTask,  /* pointer to the task */
	        "BatEva", /* task name for kernel awareness debugging */
	        configMINIMAL_STACK_SIZE, /* task stack size */
	        (void*)NULL, /* optional task startup argument */
	        tskIDLE_PRIORITY+1,  /* initial priority +2 */
	        (xTaskHandle*)NULL /* optional task handle to create */
	      ) != pdPASS) {
	    /*lint -e527 */
	    for(;;){}; /* error! probably out of memory */
	    /*lint +e527 */
	  }
}





