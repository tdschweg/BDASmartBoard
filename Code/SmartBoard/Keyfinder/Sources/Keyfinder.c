/*
 * Keyfinder.c
 *
 *  Created on: 10.04.2017
 *      Author: Matthias
 */

#include "Platform.h"
#include "Platform_Local.h"
#include "LED1.h"
#include "BAT_KON_LED.h"
#include "ALERT_LED.h"
#include "ALERT_BUZZER.h"
#include "BAT_KON.h"

void Keyfinderpingen(int keyfinder, int state){
	//Keyfinder wird angepingt
	if(keyfinder == KEYFINDER_NR){
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
}

void KeyfinderBatAuswertung(void){
	//Batteriekontrolle Auswertung
	if(BAT_KON_GetVal()){
		BAT_KON_LED_On();
	}
	else{
		BAT_KON_LED_Off();
	}
}


