/**
 * \file
 * \brief This is main application file
 * \author (c) 2013 Erich Styger, http://mcuoneclipse.com/
 * \note MIT License (http://opensource.org/licenses/mit-license.html)
 *
 * This module implements the application part of the program.
 */

#include "Platform.h"
#include "Platform_Local.h"
#if PL_CONFIG_HAS_RADIO
#include "RNet_App.h"
#include "RNetConf.h"
#include "Application.h"
#include "Radio.h"
#include "RStack.h"
#include "RApp.h"
#include "FRTOS1.h"
#include "RPHY.h"
#include "LED1.h"
#if RNET_CONFIG_REMOTE_STDIO
  #include "RStdIO.h"
#endif
#if PL_HAS_REMOTE
  #include "Remote.h"
#endif
#include "Shell.h"
#if !PL_CONFIG_IS_KEYFINDER
#include "SmartBoard.h"
#endif
#if PL_CONFIG_IS_KEYFINDER
#include "Keyfinder.h"
#endif

/*
 * Für die Kommunikation wird der RNET Stack verwendet.
 * Der RNET Stack benötigt Buffer, um zu sendende bzw. empfangende Nachrichten speichern zu können.
 * Hier werden die Buffer von dem Betriebssystem FreeRTOS in Form von Queues zur Verfügung gestellt.
 * Jedes Element in der Queue beinhaltet eine Nachricht, welche in diesem Fall den Payload von 32 Bytes plus ein Overhead Byte, insgesamt also 33 Bytes aufweist.
 * Der RNET Stack sendet bzw. empfängt Daten Blockweise über den SPI-Bus.
 */

static RNWK_ShortAddrType APP_dstAddr = RNWK_ADDR_BROADCAST; /* destination node address */

typedef enum {
  RNETA_NONE,
  RNETA_POWERUP, /* powered up */
  RNETA_TX_RX,
} RNETA_State;

static RNETA_State appState = RNETA_NONE;

RNWK_ShortAddrType RNETA_GetDestAddr(void) {
  return APP_dstAddr;
}

static uint8_t HandleDataRxMessage(RAPP_MSG_Type type, uint8_t size, uint8_t *data, RNWK_ShortAddrType srcAddr, bool *handled, RPHY_PacketDesc *packet) {
#if PL_CONFIG_HAS_SHELL
  uint8_t buf[32];
  CLS1_ConstStdIOTypePtr io = CLS1_GetStdio();
#endif
  uint8_t val;
  uint8_t keyfinder_nr;
  uint8_t CollectiveIntelligence_state;
  uint8_t alert_state;
  
  (void)size;
  (void)packet;
  switch(type) {
    case RAPP_MSG_TYPE_DATA: /* generic data message */
      *handled = TRUE;
      val = *data; /* get data value */
#if PL_CONFIG_HAS_SHELL
      CLS1_SendStr((unsigned char*)"Data: ", io->stdOut);
      CLS1_SendNum8u(val, io->stdOut);
      CLS1_SendStr((unsigned char*)" from addr 0x", io->stdOut);
      buf[0] = '\0';
#if RNWK_SHORT_ADDR_SIZE==1
      UTIL1_strcatNum8Hex(buf, sizeof(buf), srcAddr);
#else
      UTIL1_strcatNum16Hex(buf, sizeof(buf), srcAddr);
#endif
      UTIL1_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
      CLS1_SendStr(buf, io->stdOut);
#endif /* PL_HAS_SHELL */
      return ERR_OK;

/*
* Radio Task, Part Keyfinder
* Wird nun eine Nachricht empfangen so wird als erstes der Type der Nachricht geprüft,
* handelt es sich um den Type RAPP_MSG_TYPE_PING so handelt es sich beim Inhalt der Nachricht um einen Keyfinder Befehl.
* Als nächstes wird der Inhalt der Nachricht ausgewertet, gemäss der Schnittstellenvereinbarung zwischen SmartBoard und Keyfinder.
* Aus der Nachricht kann somit ermittelt werden:
* Welcher Keyfinder gesucht wird und welchen Zustand dieser einnehmen soll.
* Falls es sich beim gesuchten Keyfinder um den Empfänger handeln so wird der, aus der Nachricht evaluierte, Zustand angenommen.
* Falls es sich jedoch nicht um den gesuchten Keyfinder handelt so leitet der Keyfinder die Nachricht einmal weiter, um den gesuchten Keyfinder zu erreichen.
*/
    case RAPP_MSG_TYPE_PING:
    	*handled = TRUE;
    	val = *data; /* get data value */
#if PL_CONFIG_IS_KEYFINDER
    	keyfinder_nr = val & 0x03;
    	CollectiveIntelligence_state = (val & KEYFINDER_ON) >> 2;
		alert_state = (val & KEYFINDER_ON) >> 3;

		/*
		 * Keyfinder Alarm
		 * Der Alarm besteht aus einer LED sowie einem Piezo-Summer. Beide pulsieren mit 0.5 Hz.
		 */
		if(keyfinder_nr == KEYFINDER_NR){
			KeyfinderAlert(alert_state);
		}
		/*
		 * Schwarm Denken
		 * Befindet sich ein Keyfinder ausserhalb der Reichweite des SmartBoard so werden die übrigen aktivierten Keyfinder als Relais verwendet um den gewünschten Keyfinder zu erreichen.
		 * Das Relais, umfunktionierter Keyfinder, sendet die Nachricht für den gesuchten Keyfinder einmal weiter.
		 * Wenn die Nachricht von einem Relais einmal weitergeleitet wurde so verändert das Relais die Nachricht und gibt an, dass die Nachricht einmal weitergeleitet wurde.
		 * Hierbei muss das Relais die Nachricht überschreiben und das Collective Intelligence Bit von 1 auf 0 setzen.
		 */
		else{
			if(CollectiveIntelligence_state == CollectiveIntelligence_ON){
				val = alert_state | CollectiveIntelligence_OFF | keyfinder_nr;
				RAPP_SendPayloadDataBlock(&val, sizeof(val), RAPP_MSG_TYPE_PING, RNWK_ADDR_BROADCAST, RPHY_PACKET_FLAGS_REQ_ACK);
			}
		}
#endif
    return ERR_OK;

    default:
      break;
  } /* switch */
  return ERR_OK;
}

/*
 * Um Daten empfangen zu können wird ein Message Handler Table (Pointer to Function) definiert.
 * Wenn immer nun der Low-Level Radio Driver eine Nachricht empfängt, durchläuft diese den RNET Stack, bis sie zu diesem Message Handler Table gelangt.
 * Der Handler kann nun die Nachricht untersuchen und darauf entsprechend reagieren.
 * Der Handler empfängt die Message Type, Size, Message Payload Data und die Adresse des Senders (sAddr).
 * Als Message Type wird der Type RAPP_MSG_TYPE_PING = 0x50 definiert.
 */

static const RAPP_MsgHandler handlerTable[] = 
{
  HandleDataRxMessage,
  NULL /* sentinel */
};

static void RadioPowerUp(void) {
  /* need to ensure that we wait 100 ms after power-on of the transceiver */
  portTickType xTime;
  
  xTime = FRTOS1_xTaskGetTickCount();
  if (xTime<(100/portTICK_PERIOD_MS)) {
    /* not powered for 100 ms: wait until we can access the radio transceiver */
    xTime = (100/portTICK_PERIOD_MS)-xTime; /* remaining ticks to wait */
    FRTOS1_vTaskDelay(xTime);
  }
  (void)RNET1_PowerUp();
}

/*
 * Der Radio Task beinhaltet eine State Maschine Radio Process, um den RF-Transceiver zu starten, mit den Zuständen:
 * - NONE
 * - POWER UP
 * - TX RX
 * Von dem Zustand NONE, wird der RF-Transceiver in den Zustand POWER UP versetzt.
 * Diese Zustandsänderung von NONE zu POWER UP benötigt eine Wartezeit von 100ms damit das Modul ordnungsgemäss eingeschaltet werden kann.
 * Die Wartezeit von 100ms stammt aus dem Datenblatt des RF-Transceivers (nRF24L01+, 2017).
 * Nach dem Zustand POWER UP gelangt das Modul in den Zustand TX RX, es ist nun bereit Daten zu senden bzw. zu empfangen sowie die Daten auszuwerten.
 */

static void Process(void) {
  for(;;) {
    switch(appState) {
    case RNETA_NONE:
      appState = RNETA_POWERUP;
      continue;
      
    case RNETA_POWERUP:
      RadioPowerUp();
      appState = RNETA_TX_RX;
      break;
      
    case RNETA_TX_RX:
      (void)RNET1_Process();
      break;
  
    default:
      break;
    } /* switch */
    break; /* break for loop */
  } /* for */
}


static void Init(void) {
  if (RAPP_SetThisNodeAddr(RNWK_ADDR_BROADCAST)!=ERR_OK) { /* set a default address */
    APP_DebugPrint((unsigned char*)"ERR: Failed setting node address\r\n");
  }
}

static portTASK_FUNCTION(RadioTask, pvParameters) {
	uint8_t msg;
	uint8_t msgnr, msgbefehl, msgprogress;
	(void)pvParameters; /* not used */
	Init();
	appState = RNETA_NONE;
	for(;;) {

/*
 * Keyfinder Radio Task
 */
#if PL_CONFIG_IS_KEYFINDER
		Process(); /* process radio in/out queues */
#endif

/*
 * Radio Task
 * Der Task Radio Task ist einzig für den RF-Transceiver, das Senden/Empfangen von Daten Pakete, zuständig.
 * Er wird periodisch all 500ms aufgerufen.
 * Als erstes wird die Knoten Adresse festgelegt.
 * Es wird eine Broadcast Node Address 0xFF als Default definiert.
 * Das SmartBoard sowie die Keyfinder haben dieselbe Knoten Adresse.
 * Dies ist die Voraussetzung für das Implementieren eines Schwarm Denkens.
 * Durch die State Maschine Radio Process wird der RF-Transceiver initialisiert. Der Zustand RX TX der State Maschine Process Radio wird periodisch aufgerufen.
 * In diesem Zustand werden gespeicherte Daten gesendet bzw. empfangen und ausgewertet.
 */
#if !PL_CONFIG_IS_KEYFINDER

		/*
		 * Radio Task, Part SmartBoard
		 * Wenn das Keyfinder Function Progress Flag gesetzt ist, sofern Licht Detektor Licht detektiert, wird der aktivierte Keyfinder angepingt.
		 * Hierbei wird dem Keyfinder mitgeteilt welchen Zustand er einnehmen soll.
		 * Konnte der Keyfinder angepingt werden, so wir das Keyfinder Function Progress Flag zurück gesetzt.
		 */
		Process();

		if(LightDetectorEvaluation() && getKeyfinderFunctionProgress()==TODO){

			msgnr = getKeyfinderFunctionNr();
			msgbefehl=getKeyfinderFunctionState(getKeyfinderFunctionNr());
			msgprogress = getKeyfinderFunctionProgress();

			msg = getKeyfinderFunctionState(getKeyfinderFunctionNr()) | CollectiveIntelligence_ON | getKeyfinderFunctionNr();
			RAPP_SendPayloadDataBlock(&msg, sizeof(msg), RAPP_MSG_TYPE_PING, RNWK_ADDR_BROADCAST, RPHY_PACKET_FLAGS_REQ_ACK);
			setKeyfinderFuction(getKeyfinderFunctionNr(), getKeyfinderFunctionState(getKeyfinderFunctionNr()), DONE);

			if(msgbefehl==KEYFINDER_ON){
				LED1_On();
			}
			else{
				LED1_Off();
			}
		}
#endif
    //Go into Low Power Mode
    FRTOS1_vTaskDelay(500/portTICK_PERIOD_MS);
	}
}

void RNETA_Deinit(void) {
  RNET1_Deinit();
}

void RNETA_Init(void) {
  RNET1_Init(); /* initialize stack */
  if (RAPP_SetMessageHandlerTable(handlerTable)!=ERR_OK) { /* assign application message handler */
    APP_DebugPrint((unsigned char*)"ERR: failed setting message handler!\r\n");
  }
  if (FRTOS1_xTaskCreate(
        RadioTask,  /* pointer to the task */
        "Radio", /* task name for kernel awareness debugging */
        configMINIMAL_STACK_SIZE+200, /* task stack size */
        (void*)NULL, /* optional task startup argument */
        tskIDLE_PRIORITY+1,  /* initial priority +2 */
        (xTaskHandle*)NULL /* optional task handle to create */
      ) != pdPASS) {
    /*lint -e527 */
    for(;;){}; /* error! probably out of memory */
    /*lint +e527 */
  }
}

#if PL_CONFIG_HAS_SHELL
static uint8_t PrintStatus(const CLS1_StdIOType *io) {
  uint8_t buf[32];
  
  CLS1_SendStatusStr((unsigned char*)"radio", (unsigned char*)"\r\n", io->stdOut);
  
  UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"0x");
#if RNWK_SHORT_ADDR_SIZE==1
  UTIL1_strcatNum8Hex(buf, sizeof(buf), APP_dstAddr);
#else
  UTIL1_strcatNum16Hex(buf, sizeof(buf), APP_dstAddr);
#endif
  UTIL1_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
  CLS1_SendStatusStr((unsigned char*)"  dest addr", buf, io->stdOut);
  
  return ERR_OK;
}

static void PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr((unsigned char*)"rapp", (unsigned char*)"Group of application commands\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  help", (unsigned char*)"Shows radio help or status\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  saddr 0x<addr>", (unsigned char*)"Set source node address\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  daddr 0x<addr>", (unsigned char*)"Set destination node address\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  send val <val>", (unsigned char*)"Set a value to the destination node\r\n", io->stdOut);
}

uint8_t RNETA_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
  uint8_t res = ERR_OK;
  const uint8_t *p;
  uint16_t val16;
  uint8_t val8;

  if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, (char*)"rapp help")==0) {
    PrintHelp(io);
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, (char*)"rapp status")==0) {
    *handled = TRUE;
    return PrintStatus(io);
  } else if (UTIL1_strncmp((char*)cmd, (char*)"rapp saddr", sizeof("rapp saddr")-1)==0) {
    p = cmd + sizeof("rapp saddr")-1;
    *handled = TRUE;
    if (UTIL1_ScanHex16uNumber(&p, &val16)==ERR_OK) {
      (void)RNWK_SetThisNodeAddr((RNWK_ShortAddrType)val16);
    } else {
      CLS1_SendStr((unsigned char*)"ERR: wrong address\r\n", io->stdErr);
      return ERR_FAILED;
    }
  } else if (UTIL1_strncmp((char*)cmd, (char*)"rapp send val", sizeof("rapp send val")-1)==0) {
    p = cmd + sizeof("rapp send val")-1;
    *handled = TRUE;
    if (UTIL1_ScanDecimal8uNumber(&p, &val8)==ERR_OK) {
      (void)RAPP_SendPayloadDataBlock(&val8, sizeof(val8), (uint8_t)RAPP_MSG_TYPE_DATA, APP_dstAddr, RPHY_PACKET_FLAGS_NONE); /* only send low byte */
    } else {
      CLS1_SendStr((unsigned char*)"ERR: wrong number format\r\n", io->stdErr);
      return ERR_FAILED;
    }
  } else if (UTIL1_strncmp((char*)cmd, (char*)"rapp daddr", sizeof("rapp daddr")-1)==0) {
    p = cmd + sizeof("rapp daddr")-1;
    *handled = TRUE;
    if (UTIL1_ScanHex16uNumber(&p, &val16)==ERR_OK) {
      APP_dstAddr = val16;
    } else {
      CLS1_SendStr((unsigned char*)"ERR: wrong address\r\n", io->stdErr);
      return ERR_FAILED;
    }
  }
  return res;
}
#endif /* PL_HAS_SHELL */

#endif /* PL_HAS_RADIO */
