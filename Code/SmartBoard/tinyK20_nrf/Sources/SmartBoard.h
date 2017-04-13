/*
 * SmartBoard.h
 *
 *  Created on: 10.04.2017
 *      Author: Matthias
 */

#ifndef SOURCES_SMARTBOARD_H_
#define SOURCES_SMARTBOARD_H_

#include "PE_Types.h"

bool LightDetectorEvaluation(void);

void PowerModeProximityDetector(bool state);

//uint8_t PoolEvaluation(void);

void LEDVisualisation(uint8_t LED_Nr, bool state);

void InitButtonInit(void);

void KeyfinderFunctionDetectorInit(void);

void BatteryEvaluationInit(void);

#endif /* SOURCES_SMARTBOARD_H_ */
