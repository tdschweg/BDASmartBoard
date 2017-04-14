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
#include "ProximityDetectorDEnable.h"

/*
 * Initialisierung: Alle Keyfinder sind Default mässig eingeschaltet
 * FALSE = Keyfinder Disable
 * TRUE = Keyfinder Enable
 */
static bool PL_CONFIG_HAS_KEYFINDER_A = TRUE;
static bool PL_CONFIG_HAS_KEYFINDER_B = TRUE;
static bool PL_CONFIG_HAS_KEYFINDER_C = TRUE;
static bool PL_CONFIG_HAS_KEYFINDER_D = TRUE;

/*
 * Keyfinder Nr
 * 0 = Keyfinder A
 * 1 = Keyfinder B
 * 2 = Keyfinder C
 * 3 = Keyfinder D
 */
static uint8_t Keyfinder_Function_Nr = KEYFINDER_A;

/*
 * Keyfinder Function State
 * 0x08 = Keyfinder on
 * 0x00 = Keyfinder off
 * 0x01 = Keyfinder idle
 */
static uint8_t Keyfinder_Function_State = KEYFINDER_IDLE;


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
 * LED Visualization
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
	/*
	 * Initialisierung: Alle Keyfinder sind Default mässig eingeschaltet
	 * 0 = Keyfinder Disable
	 * 1 = Keyfinder Enable
	 */
	for(;;){
		if(InitButton_GetVal() && LightDetectorEvaluation()){
			PL_CONFIG_HAS_KEYFINDER_A = FALSE;
			PL_CONFIG_HAS_KEYFINDER_B = FALSE;
			PL_CONFIG_HAS_KEYFINDER_C = FALSE;
			PL_CONFIG_HAS_KEYFINDER_D = FALSE;

			while(InitButton_GetVal()){
				//Proximity Detector A
				if(ProximityDetectorA_GetVal() || PL_CONFIG_HAS_KEYFINDER_A==TRUE){
					PL_CONFIG_HAS_KEYFINDER_A = TRUE;
					LEDVisualisation(KEYFINDER_A, TRUE);
				}
				else{
					PL_CONFIG_HAS_KEYFINDER_A = FALSE;
					LEDVisualisation(KEYFINDER_A, FALSE);
				}
				//Proximity Detector B
				if(ProximityDetectorB_GetVal() || PL_CONFIG_HAS_KEYFINDER_B==TRUE){
					PL_CONFIG_HAS_KEYFINDER_B = TRUE;
					LEDVisualisation(KEYFINDER_B, TRUE);
				}
				else{
					PL_CONFIG_HAS_KEYFINDER_B = FALSE;
					LEDVisualisation(KEYFINDER_B, FALSE);
				}
				//Proximity Detector C
				if(ProximityDetectorC_GetVal() || PL_CONFIG_HAS_KEYFINDER_C==TRUE){
					PL_CONFIG_HAS_KEYFINDER_C = TRUE;
					LEDVisualisation(KEYFINDER_C, TRUE);
				}
				else{
					PL_CONFIG_HAS_KEYFINDER_C = FALSE;
					LEDVisualisation(KEYFINDER_C, FALSE);
				}
				//Proximity Detector D
				if(ProximityDetectorD_GetVal() || PL_CONFIG_HAS_KEYFINDER_D==TRUE){
					PL_CONFIG_HAS_KEYFINDER_D = TRUE;
					LEDVisualisation(KEYFINDER_D, TRUE);
				}
				else{
					PL_CONFIG_HAS_KEYFINDER_D = FALSE;
					LEDVisualisation(KEYFINDER_D, FALSE);
				}
			}
		}
		//Go into Low Power Mode

		FRTOS1_vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}


/*
 * Proximity Detector Init
 */
void ProximityDetectorInit(void){
	uint16_t proximity_delay = 1000;
	bool ProximityDetectorAInit = FALSE;
	bool ProximityDetectorBInit = FALSE;
	bool ProximityDetectorCInit = FALSE;
	bool ProximityDetectorDInit = FALSE;

	//Proximity Detector A init
	while(ProximityDetectorAInit==FALSE){
		ProximityDetectorAEnable_SetVal();
		WAIT1_Waitms(proximity_delay);
		//Proximity Detector not init yet
		if(!ProximityDetectorA_GetVal()){
			ProximityDetectorAEnable_ClrVal();
			LEDVisualisation(KEYFINDER_A, ProximityDetectorAInit);
			WAIT1_Waitms(proximity_delay);
		}
		//Proximity Detector init
		else{
			ProximityDetectorAInit = TRUE;
			LEDVisualisation(KEYFINDER_A, ProximityDetectorAInit);
		}
	}







}


/*
 * Proximity Detector Deinit
 */
void ProximityDetectorDeinit(void){
	ProximityDetectorAEnable_ClrVal();
	ProximityDetectorBEnable_ClrVal();
	ProximityDetectorCEnable_ClrVal();
	ProximityDetectorDEnable_ClrVal();
}


/*
* Set Keyfinder Function
* Critical Section
*/
void setKeyfinderFuction(uint8_t Keyfinder_Nr, uint8_t Keyfinder_state){
	EnterCritical();
	Keyfinder_Function_Nr = Keyfinder_Nr;
	Keyfinder_Function_State = Keyfinder_state;
	ExitCritical();
}


/*
* Get Keyfinder Function Keyfinder Nr
*/
uint8_t getKeyfinderFunctionNr(void){
	return Keyfinder_Function_Nr;
}


/*
* Get Keyfinder Function Keyfinder state
*/
uint8_t getKeyfinderFunctionState(void){
	return Keyfinder_Function_State;
}


/*
 * Keyfinder Function Detector Task
 */
static void KeyfinderFunctionDetectorTask(void *pvParameters){
	(void)pvParameters; /* not used */
	for(;;){
		if(PL_CONFIG_HAS_KEYFINDER_A){
			if(!ProximityDetectorA_GetVal()){
				setKeyfinderFuction(KEYFINDER_A, KEYFINDER_ON);
			}
			else{
				setKeyfinderFuction(KEYFINDER_A, KEYFINDER_OFF);
			}
		}
		if(PL_CONFIG_HAS_KEYFINDER_B){

		}
		if(PL_CONFIG_HAS_KEYFINDER_C){

		}
		if(PL_CONFIG_HAS_KEYFINDER_D){

		}

		//Go into Low Power Mode

		FRTOS1_vTaskDelay(250/portTICK_PERIOD_MS);
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
