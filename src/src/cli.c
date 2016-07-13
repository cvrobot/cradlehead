#include "cli.h"
#include "getbuildnum.h"
#include "main.h"
#include "serial.h"
#include "run.h"
#include "config.h"
#include "rcc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char version[16];			//current software version
static char cliBuf[3][32];	//cli buf
static int cliBufIndex[3];	//cli buf index
static char tempBuf[64];	//tmp print buf
static int cliTelemetry;	//auto print mode,systick interrupt will call cliCheck

// this table must be sorted by command name
static const cliCommand_t cliCommandTable[] = {
    {"bootloader", "", cliFuncBoot},                      	//reboot
	{"config", "[READ | WRITE | DEFAULT]", cliFuncConfig},	//param list
	{"help", "", cliFuncHelp},                            	//help
	{"set", "LIST | [<PARAMETER> <value>]", cliFuncSet},	//set param
    {"version", "", cliFuncVer}								//show current version
};

#define CLI_N_CMDS (sizeof cliCommandTable / sizeof cliCommandTable[0])


static const char cliHome[] = {0x1b, 0x5b, 0x48, 0x00};
static const char cliClear[] = {0x1b, 0x5b, 0x32, 0x4a, 0x00};
static const char cliClearEOL[] = {0x1b, 0x5b, 0x4b, 0x00};
static const char cliClearEOS[] = {0x1b, 0x5b, 0x4a, 0x00};


void cliUsage(cliCommand_t *cmd) {
    serialPrint("usage: ");
    serialPrint(cmd->name);
    serialWrite(' ');
    serialPrint(cmd->params);
    serialPrint("\r\n");
}

static void cliFuncBoot(void *cmd, char *cmdLine) {
	serialPrint("Rebooting in boot loader mode...\r\n");
	rccReset();
}

static void cliFuncConfig(void *cmd, char *cmdLine) {
	char param[8];

	if (sscanf(cmdLine, "%8s", param) != 1) {
		cliUsage((cliCommand_t *)cmd);
	}
	else if (!strcasecmp(param, "default")) {
		configLoadDefault();
		serialPrint("CONFIG: defaults loaded\r\n");
	}
	else if (!strcasecmp(param, "read")) {
		configReadFlash();
		serialPrint("CONFIG: read flash\r\n");
	}
	else if (!strcasecmp(param, "write")) 
	{
		if (configWriteFlash()) {
			serialPrint("CONFIG: wrote flash\r\n");
		}
		else {
			serialPrint("CONFIG: write flash failed!\r\n");
		}
	}
	else {
		cliUsage((cliCommand_t *)cmd);
	}
}


static void cliFuncHelp(void *cmd, char *cmdLine) {
    int i;

    serialPrint("Available commands:\r\n\n");

    for (i = 0; i < CLI_N_CMDS; i++) {
		serialPrint(cliCommandTable[i].name);
		serialWrite(' ');
		serialPrint(cliCommandTable[i].params);
		serialPrint("\r\n");
    }
}

void cliPrintParam(int i) {
    const char *format = "%-20s = ";

    sprintf(tempBuf, format, configParameterStrings[i]);
    serialPrint(tempBuf);
    sprintf(tempBuf, configFormatStrings[i], p[i]);
    serialPrint(tempBuf);
    serialPrint("\r\n");
}

static void cliFuncSet(void *cmd, char *cmdLine) {
	char param[32];
	float value;
	int i;

	if (sscanf(cmdLine, "%32s", param) != 1) {
		cliUsage((cliCommand_t *)cmd);
	}
	else {
		if (!strcasecmp(param, "list")) {
			for (i = 1; i < CONFIG_NUM_PARAMS; i++)
				cliPrintParam(i);
		}
		else {
			i = configGetId(param);
			if (i < 0) {
				sprintf(tempBuf, "SET: no such parameter '%s'\r\n", param);
				serialPrint(tempBuf);
			} else {
				if (sscanf(cmdLine + strlen(param)+1, "%f", &value) == 1) {
					configSetParamByID(i, value);
					cliPrintParam(i);
				} else {
					cliPrintParam(i);
				}
			}
		}
	}
}

static void cliXFuncVer(int serial,void *cmd, char *cmdLine) {
    sprintf(tempBuf, "WOFL ver %s\r\n", version);
    serialXPrint(serial,tempBuf);
}
static void cliFuncVer(void *cmd, char *cmdLine) {
	cliXFuncVer(0, cmd, cmdLine);
}

static int cliCommandComp(const void *c1, const void *c2) {
    const cliCommand_t *cmd1 = c1, *cmd2 = c2;

    return strncasecmp(cmd1->name, cmd2->name, strlen(cmd2->name));
}

static cliCommand_t *cliCommandGet(char *name) {
    cliCommand_t target = {name, NULL};

    return bsearch(&target, cliCommandTable, CLI_N_CMDS, sizeof cliCommandTable[0], cliCommandComp);
}

static void cliXPrompt(int serial) {
    serialXPrint(serial,"\r\n> ");
    memset(cliBuf[serial], 0, 32);
    cliBufIndex[serial] = 0;
}
//static void cliPrompt(void){
//	cliXPrompt(0);
//}

void cliXCheck(int serial) {
	cliCommand_t *cmd;

	if (cliTelemetry && !(runMilis % cliTelemetry)) 
	{

		serialXPrint(serial,cliHome);
		sprintf(tempBuf, "Telemetry @ %d Hz\r\n\n", 1000/cliTelemetry);
		serialXPrint(serial,tempBuf);
		serialXPrint(serial,"\n> ");
		serialXPrint(serial,cliBuf[serial]);
		serialXPrint(serial,cliClearEOL);
	}

	while (serialXAvailable(serial))
	{
		char c = serialXRead(serial);

		cliBuf[serial][cliBufIndex[serial]++] = c;
		if (cliBufIndex[serial] == 32) {
			cliBufIndex[serial]--;
			c = '\n';
		}

		// EOL
		if (cliBufIndex[serial] && (c == '\n' || c == '\r')) 
		{
			if (cliBufIndex[serial] > 1) 
			{
				serialXPrint(serial,"\r\n");
				serialXPrint(serial,cliClearEOS);
				cliBuf[serial][cliBufIndex[serial]] = 0;

				cmd = cliCommandGet(cliBuf[serial]);

				if (cmd)
					cmd->cmdFunc(cmd, cliBuf[serial] + strlen(cmd->name));
				else
					serialXPrint(serial,"Command not found");

			}

			cliXPrompt(serial);
		}
		// interrupt
		else if (c == CLI_INTR) {
			cliXPrompt(serial);
		}
		// backspace
		else if (c == CLI_BS) {
			if (cliBufIndex[serial] > 1) {
				cliBuf[serial][cliBufIndex[serial]-2] = 0;
				serialXPrint(serial,"\r> ");
				serialXPrint(serial,cliBuf[serial]);
				serialXWrite(serial,' ');
				serialXPrint(serial,"\r> ");
				serialXPrint(serial,cliBuf[serial]);
				cliBufIndex[serial] -= 2;
			}
			else {
				cliBufIndex[serial] -= 1;
			}
		}
		// out of range character
		else if (c < 32 || c > 126) {
			serialXWrite(serial,CLI_BELL);
			cliBufIndex[serial] -= 1;
		}
		else {
			serialXWrite(serial,c);
		}
	}
}
void cliCheck(void){
	cliXCheck(0);
}
//print some info to serial
void cliXInit(int serial) {

    serialXPrint(serial,cliHome);
    serialXPrint(serial,cliClear);
    sprintf(version, "%s.%d,port:%d", VERSION, getBuildNumber(),(serial));

    cliXFuncVer(serial, 0, 0);
    serialXPrint(serial,"\r\nCLI ready.\r\n");

    cliXPrompt(serial);
}
void cliInit(void){
	cliXInit(0);
}
