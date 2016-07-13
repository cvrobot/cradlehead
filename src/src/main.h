#ifndef _MAIN_H
#define _MAIN_H
#ifdef __cplusplus  
       extern "C" {  
#endif  
#define VERSION			"1.0.0"
#include <stdio.h>
#include "digital.h"

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
