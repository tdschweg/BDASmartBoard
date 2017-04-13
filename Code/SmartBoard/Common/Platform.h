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
 * Maske: STATE | NR
 *
 * 0 = Keyfinder A
 * 1 = Keyfinder B
 * 2 = Keyfinder C
 * 3 = Keyfinder D
 */
#define KEYFINDER_A	  			  0
#define KEYFINDER_B	  			  1
#define KEYFINDER_C	  			  2
#define KEYFINDER_D	  			  3
/*
 * STATE:
 * 0x80 = Keyfinder on
 * 0x00 = Keyfinder off
 */
#define KEYFINDER_ON			  8
#define KEYFINDER_OFF			  0

#endif /* SOURCES_PLATFORM_H_ */
