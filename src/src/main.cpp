
#include "main.h"
#include "rcc.h"
#include "config.h"
#include "serial.h"
#include "run.h"
#include "cli.h"
#include "simplebgc.h"
#include "simplewofl.h"
#include "can.h"

digitalPin *statusLed;

volatile uint32_t minCycles, idleCounter,      totalCycles;
volatile uint8_t state;
char printBuf[64];
__asm void nop(void);

int main(void) 
{

	rccInit();     //reset and clock control

	statusLed = digitalInit(GPIO_STATUS_LED_PORT, GPIO_STATUS_LED_PIN);

	configInit();  // load default config
	serialInit();  //serial init
  canInit(CAN_FILTER_ID);
	runInit();
	cliInit();     //serial client for debug
	simplebgc_init();//serial bor sbgc protocol
	simplewofl_init();//serial for wingoflight protocol

	//LED init
	digitalHi(statusLed);

	// self calibrating idle timer loop
	{
		volatile unsigned long cycles;
		volatile unsigned int *DWT_CYCCNT  = (volatile unsigned int *)0xE0001004;//当前PC采样器周期计数寄存器
		volatile unsigned int *DWT_CONTROL = (volatile unsigned int *)0xE0001000;
		volatile unsigned int *SCB_DEMCR   = (volatile unsigned int *)0xE000EDFC;

		*SCB_DEMCR = *SCB_DEMCR | 0x01000000;
		*DWT_CONTROL = *DWT_CONTROL | 1;	// enable the counter

		minCycles = 0xffff;

		while (1) 
		{
			idleCounter++;
			//NOPS_4;
			nop();

			cycles = *DWT_CYCCNT;
			*DWT_CYCCNT = 0;		    // reset the counter

			// record shortest number of instructions for loop
			totalCycles += cycles;
			if (cycles < minCycles)
				minCycles = cycles;
		}
	}
}

void HardFault_Handler(void) {
    //FET_PANIC;
    while (1)
	;
}

void MemManage_Handler(void) {
    //FET_PANIC;
    while (1)
	;
}

void BusFault_Handler(void) {
    //FET_PANIC;
    while (1)
	;
}

void UsageFault_Handler(void) {
    //FET_PANIC;
    while (1)
	;
}

void reset_wait(void) {
    //FET_PANIC;
    while (1)
	;
}

__asm void nop(void)
{
	PUSH {lr}
	nop
	nop
	nop
	nop
	POP {PC}
}
