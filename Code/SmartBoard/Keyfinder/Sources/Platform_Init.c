/*
 * Platform_Init.c
 *
 *  Created on: 10.04.2017
 *      Author: Matthias
 */


#include "LED1.h"
#include "BAT_KON_LED.h"
#include "ALERT_LED.h"

void platform_Init(void){
	LED1_Init();
	BAT_KON_LED_Init();
	ALERT_LED_Init();
}

void platform_Deinit(void){
	LED1_Deinit();
	BAT_KON_LED_Deinit();
	ALERT_LED_Deinit();

}



