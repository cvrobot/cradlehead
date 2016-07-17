#ifndef _MAIN_H
#define _MAIN_H
#ifdef __cplusplus  
       extern "C" {  
#endif  
#define VERSION			"1.0.0"
#include <stdio.h>
#include "digital.h"
/*
 * uart 1: connect to host to debug
 * uart 2: connect to SimpleBGC 
 * uart 3: connect ot host or other mather board to receive wofl command and reposne wofl reponse
 * canbus: connect IMU,PCV
 */
#define BOARD_TYPE_1ST                       0  // GCU
#define BOARD_TYPE_2ND                       1  // IMU
#define BOARD_TYPE_3RD                       2  // PCV 

//#define BOARD_TYPE_SELECT                    BOARD_TYPE_1ST
#define BOARD_TYPE_SELECT                    BOARD_TYPE_3RD
				 
#define CAN_FILTER_BASE						 0XF8
#define CAN_FILTER_ID						 (BOARD_TYPE_SELECT+CAN_FILTER_BASE)
#if (BOARD_TYPE_SELECT == BOARD_TYPE_1ST)
#define CAN_SEND_ID							 (BOARD_TYPE_3RD+CAN_FILTER_BASE)
#elif(BOARD_TYPE_SELECT == BOARD_TYPE_2ND)
#define CAN_SEND_ID							 (BOARD_TYPE_1ST+CAN_FILTER_BASE)
#elif(BOARD_TYPE_SELECT == BOARD_TYPE_3RD)
#define CAN_SEND_ID							 (BOARD_TYPE_1ST+CAN_FILTER_BASE)//not used now
#endif

#define GPIO_STATUS_LED_PORT	GPIOB
#define GPIO_STATUS_LED_PIN	GPIO_Pin_3

#define NOP			{__asm volatile ("nop\n\t");}
#define NOPS_4			{NOP; NOP; NOP; NOP;}

extern digitalPin *statusLed;
extern volatile uint32_t minCycles, idleCounter, totalCycles;

extern char printBuf[64];
#ifdef __cplusplus  
        }  
#endif
#endif
