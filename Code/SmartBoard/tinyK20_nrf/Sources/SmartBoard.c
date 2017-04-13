/*
 * SmartBoard.c
 *
 *  Created on: 10.04.2017
 *      Author: Matthias
 */

#include "Platform.h"
#include "Platform_Local.h"
#include "SmartBoard.h"
#include "FRTOS1.h"
#include "InitButton.h"
#include "LEDA.h"
#include "LEDB.h"
#include "LEDC.h"
#include "LEDD.h"
#include "LightDetector.h"
#include "BAT_KON.h"
#include "ProximityDetectorA.h"
#include "ProximityDetectorB.h"
#include "ProximityDetectorC.h"
#include "ProximityDetectorD.h"
#include "PowerMode.h"
#include "ProximityDetectorAEnable.h"
#include "ProximityDetectorBEnable.h"
#include "ProximityDetectorCEnable.h"


/*
 * Light Detector Evaluation
 */
bool LightDetectorEvaluation(void){
	return LightDetector_GetVal();
}


/*
 * Power Mode Proximity Detector
 */
void PowerModeProximityDetector(bool state){
	PowerMode_PutVal(state);
}


/*
 *
 */
void LEDVisualisation(uint8_t LED_Nr, bool state){
	switch(LED_Nr){
	case KEYFINDER_A: LEDA_Put(state); break;
	case KEYFINDER_B: LEDB_Put(state); break;
	case KEYFINDER_C: LEDC_Put(state); break;
	case KEYFINDER_D: LEDD_Put(state); break;
	default: break;
	}
}


/*
 * Init Button Task
 */
static void InitButtonTask(void *pvParameters){
	(void)pvParameters; /* not used */
	for(;;){
		LEDB_Neg();
		FRTOS1_vTaskDelay(5000/portTICK_PERIOD_MS);
	}
}


/*
 * Keyfinder Function Detector Task
 */
static void KeyfinderFunctionDetectorTask(void *pvParameters){
	(void)pvParameters; /* not used */
	for(;;){
		LEDA_Neg();
		FRTOS1_vTaskDelay(5000/portTICK_PERIOD_MS);
	}
}


/*
 * Battery Evaluation Task
 */
static void BatteryEvaluationTask(void *pvParameters){
	(void)pvParameters; /* not used */
	for(;;){
		LEDC_Neg();
		FRTOS1_vTaskDelay(5000/portTICK_PERIOD_MS);
	}
}


/*
 * Create InitButton Task
 */
void InitButtonInit(void){
	  if (FRTOS1_xTaskCreate(
	        InitButtonTask,  /* pointer to the task */
	        "InitBtn", /* task name for kernel awareness debugging */
	        configMINIMAL_STACK_SIZE, /* task stack size */
	        (void*)NULL, /* optional task startup argument */
	        tskIDLE_PRIORITY+3,  /* initial priority +2 */
	        (xTaskHandle*)NULL /* optional task handle to create */
	      ) != pdPASS) {
	    /*lint -e527 */
	    for(;;){}; /* error! probably out of memory */
	    /*lint +e527 */
	  }
}


/*
 * Create Keyfinder Function Detector Task
 */
void KeyfinderFunctionDetectorInit(void){
	  if (FRTOS1_xTaskCreate(
	        KeyfinderFunctionDetectorTask,  /* pointer to the task */
	        "KeyFktDet", /* task name for kernel awareness debugging */
	        configMINIMAL_STACK_SIZE, /* task stack size */
	        (void*)NULL, /* optional task startup argument */
	        tskIDLE_PRIORITY+2,  /* initial priority +2 */
	        (xTaskHandle*)NULL /* optional task handle to create */
	      ) != pdPASS) {
	    /*lint -e527 */
	    for(;;){}; /* error! probably out of memory */
	    /*lint +e527 */
	  }
}


/*
 * Create Battery Evaluation Task
 */
void BatteryEvaluationInit(void){
	  if (FRTOS1_xTaskCreate(
			BatteryEvaluationTask,  /* pointer to the task */
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
