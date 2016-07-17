#include "simplebgc.h"
#include "simplewofl.h"
#include "run.h"
#include "main.h"
#include "cli.h"
#include "can.h"
#include "config.h"
#include "serial.h"
#include "misc.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_dbgmcu.h"
#include <math.h>


uint32_t runMilis;   //systick中断中自加.没有什么控制用途
static uint32_t oldIdleCounter;  //上次main函数中,死循环次数.
float idlePercent;   //空闲时间百分比(在main循环里,什么事情也不做.main死循环运行的时间)

void runFeedIWDG(void) {
#ifdef RUN_ENABLE_IWDG
    IWDG_ReloadCounter();
#endif
}

// setup the hardware independent watchdog
uint16_t runIWDGInit(int ms) 
{
#ifndef RUN_ENABLE_IWDG
    return 0;
#else
	uint16_t prevReloadVal;
	int reloadVal;

	IWDG_ReloadCounter();//feed dog

	DBGMCU_Config(DBGMCU_IWDG_STOP, ENABLE);//stop watch dog when use jtag to debug
	
	// IWDG timeout equal to 10 ms (the timeout may varies due to LSI frequency dispersion)
	// Enable write access to IWDG_PR and IWDG_RLR registers
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	// IWDG counter clock: LSI/4
	IWDG_SetPrescaler(IWDG_Prescaler_4);

	reloadVal = RUN_LSI_FREQ*ms/4000;

	if (reloadVal < 1)
		reloadVal = 1;
	else if (reloadVal > 0xfff)
		reloadVal = 0xfff;

	prevReloadVal = IWDG->RLR;

	IWDG_SetReload(reloadVal);

	// Reload IWDG counter
	IWDG_ReloadCounter();

	// Enable IWDG (the LSI oscillator will be enabled by hardware)
	IWDG_Enable();

	return (prevReloadVal*4000/RUN_LSI_FREQ);
#endif
}

static void runWatchDog(void) 
{
}



//reset and clock control
void runInit(void) {

    SysTick_Config(SystemCoreClock / 1000); // 1ms
    NVIC_SetPriority(SysTick_IRQn, 2);	    // lower priority

    // setup hardware watchdog
    runIWDGInit(20);
}

void SysTick_Handler(void) {
    // reload the hardware watchdog
    runFeedIWDG();

	//计算空闲时间百分比 通过串口发送给上位机  没什么用途
    idlePercent = 100.0f * (idleCounter-oldIdleCounter) * minCycles / totalCycles;
//  空闲时间百分比 = 100 * (本次循环次数 - 上次循环次数) * 最小周期 / 总共周期
    oldIdleCounter = idleCounter;
    totalCycles = 0;

    simplebgc_process_inqueue();//parse input from uart2 and send to uart3
    simplewofl_process_inqueue();//parse input from uart3 and send to uart2
	canProcess();
	cliCheck();    //ascii

    runMilis++;
}

