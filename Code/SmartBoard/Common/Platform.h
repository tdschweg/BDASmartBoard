/*
 * Platform.h
 *
 *  Created on: 21.03.2017
 *      Author: Matthias Schwegler
 */

#ifndef SOURCES_PLATFORM_H_
#define SOURCES_PLATFORM_H_

#include <stdint.h>

/*
 * Schnittstellenbeschreibung zwischnen SmartBoard und Keyfinder.
 * Das Datenpaket zwischen SmartBoard und Keyfinder beinhaltet den Zustand welcher der Keyfinder einnhemen soll,
 * das Collective Intelligence Bit für das Schwam Denken sowie die Keyfinder Nr des anzupingenden Keyfinders.
 *
 * KEYFINDER - Maske:
 * Maske: STATE | Collective Intelligence | NR
 *
 * 0 = Keyfinder A
 * 1 = Keyfinder B
 * 2 = Keyfinder C
 * 3 = Keyfinder D
 */
#define KEYFINDER_A	  				0
#define KEYFINDER_B					1
#define KEYFINDER_C					2
#define KEYFINDER_D					3

/*
 * STATE:
 * 0x08 = Keyfinder on
 * 0x00 = Keyfinder off
 */
#define KEYFINDER_ON				8
#define KEYFINDER_OFF				0

/*
 * Collective Intelligence
 * 0x04 = Collective Intelligence on
 * 0x00 = Collective Intelligence off
 */
#define CollectiveIntelligence_ON	4
#define CollectiveIntelligence_OFF	0

#endif /* SOURCES_PLATFORM_H_ */
