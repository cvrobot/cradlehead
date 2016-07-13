#ifndef _CONFIG_H
#define _CONFIG_H
#ifdef __cplusplus  
       extern "C" {  
#endif  
#define DEFAULT_CONFIG_VERSION		1.0f
#define DEFAULT_BAUD_RATE		115200
#define DEFAULT_SERIAL_PORT		0

#define FLASH_PAGE_SIZE			((uint16_t)0x400)
#define FLASH_WRITE_ADDR		(0x08000000 + (uint32_t)FLASH_PAGE_SIZE * 63)    // use the last KB for storage param

enum configParameters {
    CONFIG_VERSION = 0,
    BAUD_RATE,   /* default baud rate */
	SERIAL_LOG_PORT,   /* default port */
    CONFIG_NUM_PARAMS
};

extern float p[CONFIG_NUM_PARAMS];
extern const char *configParameterStrings[];
extern const char *configFormatStrings[];

extern void configInit(void);
extern int configSetParam(char *param, float value);
extern int configSetParamByID(int i, float value);
extern int configGetId(char *param);
extern float configGetParam(char *param);
extern void configLoadDefault(void);
extern void configReadFlash(void);
extern int configWriteFlash(void);
#ifdef __cplusplus  
       }  
#endif  
#endif
