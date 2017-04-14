/*
 * Platform.h
 *
 *  Created on: 21.03.2017
 *      Author: tdschweg
 */

#ifndef SOURCES_PLATFORM_LOCAL_H_
#define SOURCES_PLATFORM_LOCAL_H_

#include <stdint.h>
#include "PE_Types.h"

/*
 * 0 = SmartBoard
 * 1 = Keyfinder
 */
#define PL_CONFIG_IS_KEYFINDER		0

/*
 * Keyfinder Function Progress
 * 1 =_TODO
 * 0 = Done
 */
#define TODO						1
#define DONE						0

/*
 * Proximity Detector State
 */
#define ProximityDetectorState_IDLE	0
#define ProximityDetectorState_1	1
#define ProximityDetectorState_2	2
#define ProximityDetectorState_3	3
#define ProximityDetectorState_4	4
#define Timeout						10

/*
 * Default
 */
#define PL_CONFIG_HAS_DUMMY_LED		0
#define PL_CONFIG_HAS_SEGGER_RTT	0  /* if using extra Segger RTT beside of default I/O */
#define PL_CONFIG_HAS_RADIO			1
#define PL_CONFIG_HAS_SHELL			0

#endif /* SOURCES_PLATFORM_LOCAL_H_ */
