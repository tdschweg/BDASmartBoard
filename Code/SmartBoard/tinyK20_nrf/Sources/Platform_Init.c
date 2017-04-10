/*
 * Platform_Init.c
 *
 *  Created on: 10.04.2017
 *      Author: Matthias
 */

#include "LED1.h"


void platform_Init(void){
	LED1_Init();

	//proximity init
}

void platform_Deinit(void){
	LED1_Deinit();
}



