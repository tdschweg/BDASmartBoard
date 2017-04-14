/*
 * SmartBoard.h
 *
 *  Created on: 10.04.2017
 *      Author: Matthias
 */

#ifndef SOURCES_SMARTBOARD_H_
#define SOURCES_SMARTBOARD_H_

#include "PE_Types.h"

/*
 * Light Detector Evaluation
 */
bool LightDetectorEvaluation(void);

/*
 * Power Mode Proximity Detector
 */
void PowerModeProximityDetector(bool state);

/*
 * LED Visualization
 */
void LEDVisualisation(uint8_t LED_Nr, bool state);

/*
 * Create Init Button Task
 */
void InitButtonInit(void);

/*
 * Proximity Detector Init
 */
void ProximityDetectorInit(void);

/*
 * Proximity Detector Deinit
 */
void ProximityDetectorDeinit(void);

/*
 * Set Keyfinder Function
 */
void setKeyfinderFuction(uint8_t Keyfinder_Nr, uint8_t Keyfinder_state);

/*
 * Get Keyfinder Function Keyfinder Nr
 */
uint8_t getKeyfinderFunctionNr(void);

/*
 * Get Keyfinder Function Keyfinder state
 */
uint8_t getKeyfinderFunctionState(void);

/*
 * Create Keyfinder Function Detector Task
 */
void KeyfinderFunctionDetectorInit(void);

#endif /* SOURCES_SMARTBOARD_H_ */
