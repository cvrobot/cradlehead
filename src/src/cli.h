#ifndef _CLI_H
#define _CLI_H
#ifdef __cplusplus  
       extern "C" {  
#endif  
#define CLI_INTR    3	    // interrupt
#define CLI_BELL    7	    // bellz
#define CLI_BS	    8	    // backspace

typedef struct {
    char *name;
    char *params;
    void (*cmdFunc)(void *cmd, char *cmdLine);
} cliCommand_t;

extern char version[16];

extern void cliInit(void);
extern void cliCheck(void);
extern void cliFuncArm(void *cmd, char *cmdLine);
extern void cliFuncBeep(void *cmd, char *cmdLine);
extern void cliFuncBinary(void *cmd, char *cmdLine);
extern void cliFuncBoot(void *cmd, char *cmdLine);
extern void cliFuncConfig(void *cmd, char *cmdLine);
extern void cliFuncDisarm(void *cmd, char *cmdLine);
extern void cliFuncDuty(void *cmd, char *cmdLine);
extern void cliFuncHelp(void *cmd, char *cmdLine);
extern void cliFuncInput(void *cmd, char *cmdLine);
extern void cliFuncMode(void *cmd, char *cmdLine);
extern void cliFuncPos(void *cmd, char *cmdLine);
extern void cliFuncPwm(void *cmd, char *cmdLine);
extern void cliFuncRpm(void *cmd, char *cmdLine);
extern void cliFuncSet(void *cmd, char *cmdLine);
extern void cliFuncStart(void *cmd, char *cmdLine);
extern void cliFuncStatus(void *cmd, char *cmdLine);
extern void cliFuncStop(void *cmd, char *cmdLine);
extern void cliFuncTelemetry(void *cmd, char *cmdLine);
extern void cliFuncVer(void *cmd, char *cmdLine);
#ifdef __cplusplus  
       }  
#endif  
#endif
