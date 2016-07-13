#include "config.h"
#include "run.h"
#include "serial.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_rcc.h"
#include <string.h>
#include <math.h>

float p[CONFIG_NUM_PARAMS];

const char *configParameterStrings[] = {
    "CONFIG_VERSION",
    "BAUD_RATE",
    "SERIAL_LOG_PORT",
};

const char *configFormatStrings[] = {
    "%f",	    // CONFIG_VERSION
    "%.0f baud",    // BAUD_RATE
    "%.0f port",		//debug port
};

void configInit(void) {
    float ver;

    configLoadDefault();

    ver = *(float *)FLASH_WRITE_ADDR;

    if (isnan(ver))//判断ver  如果ver为非负数(NAN，值为0xffffffff)，返回1，否则返回0 
		configWriteFlash();//write to flash
    else if (ver >= p[CONFIG_VERSION])
		configReadFlash();
    else if (p[CONFIG_VERSION] > ver)
		configWriteFlash();
}

// recalculate constants with bounds checking
static void configRecalcConst(void) {
    serialSetConstants();
}

int configSetParamByID(int i, float value) {
    int ret = 0;

    if (i >= 0 && i < CONFIG_NUM_PARAMS) {
		p[i] = value;
		configRecalcConst();

		ret = 1;
    }

    return ret;
}

int configSetParam(char *param, float value) {
    int ret = 0;
    int i;

    for (i = 0; i < CONFIG_NUM_PARAMS; i++) {
		if (!strncasecmp(configParameterStrings[i], param, strlen(configParameterStrings[i]))) {
			configSetParamByID(i, value);
			ret = 1;
			break;
		}
    }

    return ret;
}

int configGetId(char *param) {
	int i;

	for (i = 0; i < CONFIG_NUM_PARAMS; i++)
		if (!strncasecmp(configParameterStrings[i], param, strlen(configParameterStrings[i])))
			return i;

	return -1;
}

#if 0
float configGetParam(char *param) {
    int i;

    i = configGetId(param);

    if (i >= 0)
	return p[i];
    else
	//return __float32_nan;
	return NAN;//AXian 找不到__float32_nan这个变量.先这么定义了
}
#endif

void configLoadDefault(void) {
    p[CONFIG_VERSION] = DEFAULT_CONFIG_VERSION;
    p[BAUD_RATE] = DEFAULT_BAUD_RATE;
	p[SERIAL_LOG_PORT] = DEFAULT_SERIAL_PORT;

    configRecalcConst();
}

//write config to flash
int configWriteFlash(void) 
{
    uint16_t prevReloadVal;
    FLASH_Status FLASHStatus;
    uint32_t address;
    int ret = 0;

    prevReloadVal = runIWDGInit(999);

    // Startup HSI clock
    RCC_HSICmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) != SET)
		runFeedIWDG();

    FLASH_Unlock();

    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    if ((FLASHStatus = FLASH_ErasePage(FLASH_WRITE_ADDR)) == FLASH_COMPLETE) 
	{
		address = 0;
		while (FLASHStatus == FLASH_COMPLETE && address < sizeof(p)) 
		{
			if ((FLASHStatus = FLASH_ProgramWord(FLASH_WRITE_ADDR + address, *(uint32_t *)((char *)p + address))) != FLASH_COMPLETE)
				break;

			address += 4;
		}

		ret = 1;
    }

    FLASH_Lock();

    runFeedIWDG();

    // Shutdown HSI clock
    RCC_HSICmd(DISABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == SET)
		;

    runIWDGInit(prevReloadVal);

    return ret;
}

//read config from flash to memory
void configReadFlash(void) {
    memcpy(p, (char *)FLASH_WRITE_ADDR, sizeof(p));

    configRecalcConst();
}
