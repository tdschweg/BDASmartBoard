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
 * Initialization: All Keyfinder are default enabled
 * FALSE = Keyfinder Disable
 * TRUE  = Keyfinder Enable
 */
static bool PL_CONFIG_HAS_KEYFINDER_A = TRUE;
static bool PL_CONFIG_HAS_KEYFINDER_B = FALSE;
static bool PL_CONFIG_HAS_KEYFINDER_C = FALSE;
static bool PL_CONFIG_HAS_KEYFINDER_D = FALSE;

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
 */
static uint8_t Keyfinder_Function_A_State = KEYFINDER_OFF;
static uint8_t Keyfinder_Function_B_State = KEYFINDER_OFF;
static uint8_t Keyfinder_Function_C_State = KEYFINDER_OFF;
static uint8_t Keyfinder_Function_D_State = KEYFINDER_OFF;

/*
 * Keyfinder Function Progress
 * 1 = In Progress
 * 2 = Done
 */
static bool Keyfinder_Function_Progress = DONE;

/*
 * State Machine for Proximity Detector Variable
 */
//State for State Machine
static uint8_t ProximityDetectorA_State = ProximityDetectorState_IDLE;
static uint8_t ProximityDetectorB_State = ProximityDetectorState_IDLE;
static uint8_t ProximityDetectorC_State = ProximityDetectorState_IDLE;
static uint8_t ProximityDetectorD_State = ProximityDetectorState_IDLE;
//Timeout for State Machine
static uint8_t ProximityDetectorA_Timeout=0;
static uint8_t ProximityDetectorB_Timeout=0;
static uint8_t ProximityDetectorC_Timeout=0;
static uint8_t ProximityDetectorD_Timeout=0;


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
	for(;;){
		if(InitButton_GetVal() && LightDetectorEvaluation()){
			//Deactivate all Proximity Detectors
			PL_CONFIG_HAS_KEYFINDER_A = FALSE;
			PL_CONFIG_HAS_KEYFINDER_B = FALSE;
			PL_CONFIG_HAS_KEYFINDER_C = FALSE;
			PL_CONFIG_HAS_KEYFINDER_D = FALSE;

			//Before Initialization all LED are off
			LEDVisualisation(KEYFINDER_A, FALSE);
			LEDVisualisation(KEYFINDER_B, FALSE);
			LEDVisualisation(KEYFINDER_C, FALSE);
			LEDVisualisation(KEYFINDER_D, FALSE);

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

		//After Initialization all LED are off
		LEDVisualisation(KEYFINDER_A, FALSE);
		LEDVisualisation(KEYFINDER_B, FALSE);
		LEDVisualisation(KEYFINDER_C, FALSE);
		LEDVisualisation(KEYFINDER_D, FALSE);

		//Go into Low Power Mode
		//TODO
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

	//Power Mode on
	PowerModeProximityDetector(TRUE);

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
	PowerModeProximityDetector(FALSE);
	ProximityDetectorAEnable_ClrVal();
	ProximityDetectorBEnable_ClrVal();
	ProximityDetectorCEnable_ClrVal();
	ProximityDetectorDEnable_ClrVal();
}


/*
* Set Keyfinder Function
* Critical Section
*/
void setKeyfinderFuction(uint8_t Keyfinder_Nr, uint8_t KeyfinderFunction_state, bool KeyfinderFunction_progress){
	EnterCritical();
	//Keyfinder Function Keyfinder-Nr
	Keyfinder_Function_Nr = Keyfinder_Nr;
	//Keyfinder Function State
	switch(Keyfinder_Nr){
	case KEYFINDER_A: Keyfinder_Function_A_State = KeyfinderFunction_state; break;
	case KEYFINDER_B: Keyfinder_Function_B_State = KeyfinderFunction_state; break;
	case KEYFINDER_C: Keyfinder_Function_C_State = KeyfinderFunction_state; break;
	case KEYFINDER_D: Keyfinder_Function_D_State = KeyfinderFunction_state; break;
	default: break;
	}
	Keyfinder_Function_Progress = KeyfinderFunction_progress;
	ExitCritical();
}


/*
* Get Keyfinder Function Keyfinder-Nr
*/
uint8_t getKeyfinderFunctionNr(void){
	return Keyfinder_Function_Nr;
}


/*
* Get Keyfinder Function state
*/
uint8_t getKeyfinderFunctionState(uint8_t Keyfinder_Nr){
	switch(Keyfinder_Nr){
	case KEYFINDER_A: return Keyfinder_Function_A_State; break;
	case KEYFINDER_B: return Keyfinder_Function_B_State; break;
	case KEYFINDER_C: return Keyfinder_Function_C_State; break;
	case KEYFINDER_D: return Keyfinder_Function_D_State; break;
	default: break;
	}
}


/*
 * Get Keyfinder Function Progress
 */
bool getKeyfinderFunctionProgress(void){
	return Keyfinder_Function_Progress;
}


/*
 * Proximity Detector
 */
bool ProximityDetector(uint8_t Keyfinder_Nr){
	switch(Keyfinder_Nr){
	case KEYFINDER_A: return !ProximityDetectorA_GetVal(); break;
	case KEYFINDER_B: return !ProximityDetectorB_GetVal(); break;
	case KEYFINDER_C: return !ProximityDetectorC_GetVal(); break;
	case KEYFINDER_D: return !ProximityDetectorD_GetVal(); break;
	default: break;
	}
}


/*
 * Get Proximity Detector State
 */
uint8_t getProximityDetectorState(uint8_t Keyfinder_Nr){
	switch(Keyfinder_Nr){
	case KEYFINDER_A: return ProximityDetectorA_State; break;
	case KEYFINDER_B: return ProximityDetectorB_State; break;
	case KEYFINDER_C: return ProximityDetectorC_State; break;
	case KEYFINDER_D: return ProximityDetectorD_State; break;
	default: break;
	}
}


/*
 * Set Proximity Detector State
 */
void setProximityDetectorState(uint8_t Keyfinder_Nr, uint8_t state){
	switch(Keyfinder_Nr){
	case KEYFINDER_A: ProximityDetectorA_State = state; break;
	case KEYFINDER_B: ProximityDetectorB_State = state; break;
	case KEYFINDER_C: ProximityDetectorC_State = state; break;
	case KEYFINDER_D: ProximityDetectorD_State = state; break;
	default: break;
	}
}


/*
 * Get Proximity Detector Timeout
 */
uint8_t getProximityDetectorTimeout(uint8_t Keyfinder_Nr){
	switch(Keyfinder_Nr){
	case KEYFINDER_A: return ProximityDetectorA_Timeout; break;
	case KEYFINDER_B: return ProximityDetectorB_Timeout; break;
	case KEYFINDER_C: return ProximityDetectorC_Timeout; break;
	case KEYFINDER_D: return ProximityDetectorD_Timeout; break;
	default: break;
	}
}


/*
 * Set Proximity Detector Timeout
 */
void setProximityDetectorTimeout(uint8_t Keyfinder_Nr, uint8_t value){
	switch(Keyfinder_Nr){
	case KEYFINDER_A: ProximityDetectorA_Timeout=value; break;
	case KEYFINDER_B: ProximityDetectorB_Timeout=value; break;
	case KEYFINDER_C: ProximityDetectorC_Timeout=value; break;
	case KEYFINDER_D: ProximityDetectorD_Timeout=value; break;
	default: break;
	}
}


/*
 * Increase Proximity Detector Timeout
 */
void IncreaseProximityDetectorTimeout(uint8_t Keyfinder_Nr){
	switch(Keyfinder_Nr){
	case KEYFINDER_A: ProximityDetectorA_Timeout++; break;
	case KEYFINDER_B: ProximityDetectorB_Timeout++; break;
	case KEYFINDER_C: ProximityDetectorC_Timeout++; break;
	case KEYFINDER_D: ProximityDetectorD_Timeout++; break;
	default: break;
	}
}


/*
 * Proximity Detector Evaluation
 */
void ProximityDetectorEvaluation(uint8_t Keyfinder_Nr){
	switch(getProximityDetectorState(Keyfinder_Nr)){
	//Idle State
	case ProximityDetectorState_IDLE:
		if(ProximityDetector(Keyfinder_Nr)){
			setProximityDetectorState(Keyfinder_Nr, ProximityDetectorState_1);
			setProximityDetectorTimeout(Keyfinder_Nr, 0);
		}
	break;
	//State 1
	case ProximityDetectorState_1:
		if(!ProximityDetector(Keyfinder_Nr)){
			setProximityDetectorState(Keyfinder_Nr, ProximityDetectorState_2);
			setProximityDetectorTimeout(Keyfinder_Nr, 0);
		}
		else{
			IncreaseProximityDetectorTimeout(Keyfinder_Nr);
		}
	break;
	//State 2
	case ProximityDetectorState_2:
		if(ProximityDetector(Keyfinder_Nr)){
			setProximityDetectorState(Keyfinder_Nr, ProximityDetectorState_3);
			setProximityDetectorTimeout(Keyfinder_Nr, 0);
		}
		else{
			IncreaseProximityDetectorTimeout(Keyfinder_Nr);
		}
	break;
	//State 3
	case ProximityDetectorState_3:
		if(!ProximityDetector(Keyfinder_Nr)){
			setProximityDetectorState(Keyfinder_Nr, ProximityDetectorState_IDLE);
			setProximityDetectorTimeout(Keyfinder_Nr, 0);
			//Action, Double Click detected
			if(getKeyfinderFunctionState(Keyfinder_Nr)==KEYFINDER_OFF){
				setKeyfinderFuction(Keyfinder_Nr, KEYFINDER_ON, TODO);
				LEDVisualisation(Keyfinder_Nr, TRUE);
			}
			else if(getKeyfinderFunctionState(Keyfinder_Nr)==KEYFINDER_ON){
				setKeyfinderFuction(Keyfinder_Nr, KEYFINDER_OFF, TODO);
				LEDVisualisation(Keyfinder_Nr, FALSE);
			}
		}
		else{
			IncreaseProximityDetectorTimeout(Keyfinder_Nr);
		}
	break;
	//Default
	default:
		setProximityDetectorState(Keyfinder_Nr, ProximityDetectorState_IDLE);
		setProximityDetectorTimeout(Keyfinder_Nr, 0);
	break;
	}
	//Timeout von 2.5s = 10 * 250ms
	if(getProximityDetectorTimeout(Keyfinder_Nr) > Timeout){
		setProximityDetectorState(Keyfinder_Nr, ProximityDetectorState_IDLE);
		setProximityDetectorTimeout(Keyfinder_Nr, 0);
	}
}


/*
 * Keyfinder Function Detector Task
 */
static void KeyfinderFunctionDetectorTask(void *pvParameters){
	(void)pvParameters; /* not used */
	portTickType xTime;
	for(;;){
		if(PL_CONFIG_HAS_KEYFINDER_A){
			ProximityDetectorEvaluation(KEYFINDER_A);
		}
		if(PL_CONFIG_HAS_KEYFINDER_B){
			ProximityDetectorEvaluation(KEYFINDER_B);
		}
		if(PL_CONFIG_HAS_KEYFINDER_C){
			ProximityDetectorEvaluation(KEYFINDER_C);
		}
		if(PL_CONFIG_HAS_KEYFINDER_D){
			ProximityDetectorEvaluation(KEYFINDER_D);
		}

		//Go into Low Power Mode (tiny and mtch101)
		//TODO
		FRTOS1_vTaskDelay(250/portTICK_PERIOD_MS);
	}
}


/*
 * Create InitButton Task
 */
void InitButtonTaskInit(void){
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
void KeyfinderFunctionDetectorTaskInit(void){
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
