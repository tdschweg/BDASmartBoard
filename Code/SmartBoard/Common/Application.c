/*
 * Application.c
 *      Author: Matthias Schwegler
 */
#include "Platform.h"
#include "Platform_Local.h"
#include "Application.h"
#include "FRTOS1.h"
#include "Shell.h"
#include "LED1.h"
#include "RNet_App.h"
#include "Platform_Init.h"
//Keyfinder
#if PL_CONFIG_IS_KEYFINDER
#include "Keyfinder.h"
#endif
//SmartBoard
#if !PL_CONFIG_IS_KEYFINDER
#include "SmartBoard.h"
#endif

void APP_DebugPrint(uint8_t *str) {
 /* dummy */
}

/*
 * Dummy Task
 * Verwendet für FreeRTOS test.
 */
static void led_task(void *param) {
  (void)param;
  for(;;) {
    LED1_Neg();
    vTaskDelay(pdMS_TO_TICKS(1000));
  } /* for */
}

/*
 * Applikation Run
 * Durch App_Run wird die Applikation gestartet, die Plattform und RNET wird initialisiert.
 * Die Task welche das spezifische Verhalten der Platformen beschreibt werden gestartet.
 * Der Scheduler verwaltet die Tasks.
 */
void APP_Run(void) {
  platform_Init();
  /*
   * SmartBoard:
   * Durch die Shell ist es möglich dynamische Veränderungen zur Laufzeit am Programm vorzunehmen.
   * Implementiert wird die Shell in einem eigenen Task, dieser wird nur zur Entwicklung benötigt.
   * Danach wird er deaktiviert.
   */
#if PL_CONFIG_HAS_SHELL
  SHELL_Init();
#endif
#if PL_CONFIG_HAS_RADIO
  RNETA_Init();
#endif

/*
 * Keyfinder:
 * Die zwei Tasks, Radio Task sowie Keyfinder Battery Evaluation Task, sind voneinander unabhängig.
 * Der Task Radio Task ist einzig für den RF-Transceiver, das Senden/Empfangen von Daten Pakete, zuständig.
 * Der zweite Task, Keyfinder Battery Evaluation Task, wertet die Batteriespannung aus.
 */
#if PL_CONFIG_IS_KEYFINDER
  KeyfinderBatEvalutaionInit();
#endif

  /*
   * Durch den Initialization Button Task werden die Schalen/Keyfinder freigeschaltet, welche bei der Initialisierung durch den Initialisierungstaster oder per Default aktiviert werden.
   * Default mässig sind alle Schalen freigeschalten.
   * Die Information, welche Schalen/Keyfinder freigeschaltet sind gibt der Task Initialization Button Task dem Task Keyfinder Function Detector Task weiter.
   * Der Task Keyfinder Function Detector Task wertet die Aktivitäten der Proximity Detectors aus.
   * Wird ein zweimaliges Klopfen in eine Schale detektiert, so wird die Keyfinder Funktion für den zur Schale passenden Keyfinder Aktiviert/Deaktiviert.
   * Diese Information, von welchem Keyfinder die Keyfinder Funktion aktiviert/deaktiviert werden soll, gibt der Task Keyfinder Function Detector Task dem Radio Task weiter.
   * Der Task Radio Task ist einzig für den RF-Transceiver, das Senden/Empfangen von Daten Pakete, zuständig.
   */
#if !PL_CONFIG_IS_KEYFINDER
  InitButtonTaskInit();
  KeyfinderFunctionDetectorTaskInit();
#endif

#if PL_CONFIG_HAS_DUMMY_LED
  if (xTaskCreate(led_task, "Led", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL) != pdPASS) {
    for(;;){} /* error! probably out of memory! */
  }
#endif

  vTaskStartScheduler();
}

