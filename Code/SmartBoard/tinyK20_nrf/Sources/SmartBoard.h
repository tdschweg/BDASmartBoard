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
void InitButtonTaskInit(void);

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
void setKeyfinderFuction(uint8_t Keyfinder_Nr, uint8_t KeyfinderFunction_state, bool KeyfinderFunction_progress);

/*
 * Get Keyfinder Function Keyfinder-Nr
 */
uint8_t getKeyfinderFunctionNr(void);

/*
 * Get Keyfinder Function state
 */
uint8_t getKeyfinderFunctionState(uint8_t Keyfinder_Nr);

/*
 * Get Keyfinder Function Progress
 */
bool getKeyfinderFunctionProgress(void);

/*
 * Proximity Detector
 */
bool ProximityDetector(uint8_t Keyfinder_Nr);

/*
 * Get Proximity Detector State
 */
uint8_t getProximityDetectorState(uint8_t Keyfinder_Nr);

/*
 * Set Proximity Detector State
 */
void setProximityDetectorState(uint8_t Keyfinder_Nr, uint8_t state);

/*
 * Get Proximity Detector Timeout
 */
uint8_t getProximityDetectorTimeout(uint8_t Keyfinder_Nr);

/*
 * Set Proximity Detector Timeout
 */
void setProximityDetectorTimeout(uint8_t Keyfinder_Nr, uint8_t value);

/*
 * Increase Proximity Detector Timeout
 */
void IncreaseProximityDetectorTimeout(uint8_t Keyfinder_Nr);

/*
 * Proximity Detector Evaluation
 */
void ProximityDetectorEvaluation(uint8_t Keyfinder_Nr);

/*
 * Create Keyfinder Function Detector Task
 */
void KeyfinderFunctionDetectorTaskInit(void);

#endif /* SOURCES_SMARTBOARD_H_ */
