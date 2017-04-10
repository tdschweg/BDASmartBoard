/*
 * Platform.h
 *
 *  Created on: 21.03.2017
 *      Author: tdschweg
 */

#ifndef SOURCES_PLATFORM_H_
#define SOURCES_PLATFORM_H_

#include <stdint.h>
/*
 * 0 = SmartBoard
 * 1 = Keyfinder
 */
#define PL_CONFIG_IS_KEYFINDER	  1

/*
 * KEYFINDER - Maske
 * Maske: STATE && NR
 */
#if PL_CONFIG_IS_KEYFINDER
/*
 * 1 = Keyfinder A
 * 2 = Keyfinder B
 * 3 = Keyfinder C
 * 4 = Keyfinder D
 */
#define KEYFINDER_NR			  1
#endif
#define KEYFINDER_A	  			  1
#define KEYFINDER_B	  			  2
#define KEYFINDER_C	  			  3
#define KEYFINDER_D	  			  4
/*
 * 0x80 = Keyfinder on
 * 0x00 = Keyfinder off
 */
#define KEYFINDER_ON			  8
#define KEYFINDER_OFF			  0


#define PL_CONFIG_HAS_DUMMY_LED   0
#define PL_CONFIG_HAS_SEGGER_RTT  0  /* if using extra Segger RTT beside of default I/O */
#define PL_CONFIG_HAS_RADIO       1
#define PL_CONFIG_HAS_SHELL       0

#endif /* SOURCES_PLATFORM_H_ */
