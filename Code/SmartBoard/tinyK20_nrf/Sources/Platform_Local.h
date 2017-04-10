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

#if PL_CONFIG_IS_KEYFINDER
/*
 * 1 = Keyfinder A
 * 2 = Keyfinder B
 * 3 = Keyfinder C
 * 4 = Keyfinder D
 */
#define KEYFINDER_NR			  1
#endif


#define PL_CONFIG_HAS_DUMMY_LED   0
#define PL_CONFIG_HAS_SEGGER_RTT  0  /* if using extra Segger RTT beside of default I/O */
#define PL_CONFIG_HAS_RADIO       1
#define PL_CONFIG_HAS_SHELL       0

#endif /* SOURCES_PLATFORM_LOCAL_H_ */
