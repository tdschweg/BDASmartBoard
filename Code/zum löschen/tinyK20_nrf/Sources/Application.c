/*
 * Application.c
 *      Author: Erich Styger
 */
#include "Platform.h"
#include "Application.h"
#include "FRTOS1.h"
#include "Shell.h"
#include "LED1.h"
#include "RNet_App.h"

void APP_DebugPrint(uint8_t *str) {
 /* dummy */
}

static void led_task(void *param) {
  (void)param;
  for(;;) {
    LED1_Neg();
    vTaskDelay(pdMS_TO_TICKS(1000));
  } /* for */
}

void APP_Run(void) {
#if PL_CONFIG_HAS_SHELL
  SHELL_Init();
#endif
#if PL_CONFIG_HAS_RADIO
  RNETA_Init();
#endif
#if PL_CONFIG_HAS_DUMMY_LED
  if (xTaskCreate(led_task, "Led", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL) != pdPASS) {
    for(;;){} /* error! probably out of memory */
  }
#endif
  vTaskStartScheduler();
}

