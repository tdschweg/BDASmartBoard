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
  uint8_t keyfinder;
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
* Keyfinder TASK
*/
    case RAPP_MSG_TYPE_PING:
    	*handled = TRUE;
    	val = *data; /* get data value */
#if PL_CONFIG_IS_KEYFINDER
    	keyfinder = val & 0x07;
		alert_state = (val & KEYFINDER_ON) >> 3;

		//Keyfinder Alert
		if(keyfinder == KEYFINDER_NR){
			KeyfinderAlert(alert_state);
		}
		//Schwarm Denken
		else{
			//RAPP_SendPayloadDataBlock(&val, sizeof(val), RAPP_MSG_TYPE_PING, RNWK_ADDR_BROADCAST, RPHY_PACKET_FLAGS_REQ_ACK);
		}
#endif
    return ERR_OK;

    default:
      break;
  } /* switch */
  return ERR_OK;
}

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
 * SmartBoard Radio Task
 */
#if !PL_CONFIG_IS_KEYFINDER
		//Light Detector, Done
		//Proximity power mode
		//Schalen auswertung; Rückgabewert (0=kein, 1, 2, 3, 4) welcher Keyfinder muss angepingt werden
		//an led anzeigen
    	//Initialisierungs Button
    	//Bat Kon
		/*
		cntr++;
    	if (cntr==4) { //with an RTOS 10 ms/100 Hz tick rate, this is every second
    		if(dongle==0){
    			msg = KEYFINDER_B | KEYFINDER_ON;
    			dongle = 1;
    		}
    		else{
    			msg = KEYFINDER_B | KEYFINDER_OFF;
    			dongle = 0;
    		}
    		*/

/*
 *
 */

		Process();

		if(LightDetectorEvaluation() && getKeyfinderFunctionProgress()==TODO){

			msgnr = getKeyfinderFunctionNr();
			msgbefehl=getKeyfinderFunctionState(getKeyfinderFunctionNr());
			msgprogress = getKeyfinderFunctionProgress();

			msg = getKeyfinderFunctionNr() | getKeyfinderFunctionState(getKeyfinderFunctionNr());
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
	//TODO
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
