/*
 * Platform_Init.c
 *
 *  Created on: 10.04.2017
 *      Author: Matthias
 */

#include "LED1.h"
#include "LEDA.h"
#include "LEDB.h"
#include "LEDC.h"
#include "LEDD.h"
#include "SmartBoard.h"


void platform_Init(void){
	LED1_Init();
	//Aktivierung LED init
	LEDA_Init();
	LEDB_Init();
	LEDC_Init();
	LEDD_Init();
	//Proximity Detector Init
	ProximityDetectorInit();
}

void platform_Deinit(void){
	//Aktivierung LED deinit
	LED1_Deinit();
	LEDA_Deinit();
	LEDB_Deinit();
	LEDC_Deinit();
	LEDD_Deinit();
	//Proximity Detector Deinit
	ProximityDetectorDeinit();
}



