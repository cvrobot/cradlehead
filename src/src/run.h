#ifndef _RUN_H
#define _RUN_H
#ifdef __cplusplus  
       extern "C" {  
#endif  
#include "misc.h"

//#define RUN_ENABLE_IWDG
#define RUN_LSI_FREQ		40000		    // 40 KHz LSI for IWDG

extern uint32_t runMilis;

extern uint16_t runIWDGInit(int ms);
extern void runFeedIWDG(void);
extern void runInit(void);
extern void SysTick_Handler(void);
#ifdef __cplusplus  
        }  
#endif 
#endif
