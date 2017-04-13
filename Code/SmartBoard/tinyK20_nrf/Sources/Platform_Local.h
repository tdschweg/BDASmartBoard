/*
 * Platform.h
 *
 *  Created on: 21.03.2017
 *      Author: tdschweg
 */

#ifndef SOURCES_PLATFORM_LOCAL_H_
#define SOURCES_PLATFORM_LOCAL_H_

#include <stdint.h>

/*
 * 0 = SmartBoard
 * 1 = Keyfinder
 */
#define PL_CONFIG_IS_KEYFINDER	  0

#define KEYFINDER_NONE			  4

/*
 * Initialisierung: Alle Keyfinder sind Default mässig eingeschaltet
 * 0 = Keyfinder Disable
 * 1 = Keyfinder Enable
 */
#define PL_CONFIG_HAS_KEYFINDER_A  1
#define PL_CONFIG_HAS_KEYFINDER_B  1
#define PL_CONFIG_HAS_KEYFINDER_C  1
#define PL_CONFIG_HAS_KEYFINDER_D  1

/*
 * Default
 */
#define PL_CONFIG_HAS_DUMMY_LED   0
#define PL_CONFIG_HAS_SEGGER_RTT  0  /* if using extra Segger RTT beside of default I/O */
#define PL_CONFIG_HAS_RADIO       1
#define PL_CONFIG_HAS_SHELL       0

#endif /* SOURCES_PLATFORM_LOCAL_H_ */
