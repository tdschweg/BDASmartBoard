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
 * KEYFINDER - Maske
 * Maske: STATE && NR
 *
 * 1 = Keyfinder A
 * 2 = Keyfinder B
 * 3 = Keyfinder C
 * 4 = Keyfinder D
 */
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

#endif /* SOURCES_PLATFORM_H_ */
