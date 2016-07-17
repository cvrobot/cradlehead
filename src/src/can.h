/*
    This file is part of AutoQuad ESC32.

    AutoQuad ESC32 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AutoQuad ESC32 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with AutoQuad ESC32.  If not, see <http://www.gnu.org/licenses/>.

    Copyright � 2011-2014  Bill Nesbitt
*/

#ifndef _can_h
#define _can_h
#ifdef __cplusplus  
       extern "C" {  
#endif 
#include "stm32f10x_can.h"

#define CAN_CAN		CAN1
#define CAN_PORT	GPIOA
#define CAN_RX_PIN	GPIO_Pin_11
#define CAN_TX_PIN	GPIO_Pin_12

#define CAN_UUID	0x1FFFF7E8

// types
enum {
    CAN_TYPE_ESC = 1,
    CAN_TYPE_SERVO,
    CAN_TYPE_SENSOR,
    CAN_TYPE_SWITCH,
    CAN_TYPE_OSD,
    CAN_TYPE_UART,
    CAN_TYPE_HUB,
    CAN_TYPE_NUM
};

// commands
enum {
    CAN_CMD_DISARM = 1,
    CAN_CMD_ARM,
    CAN_CMD_START,
    CAN_CMD_STOP,
    CAN_CMD_SETPOINT10,
    CAN_CMD_SETPOINT12,
    CAN_CMD_SETPOINT16,
    CAN_CMD_RPM,
    CAN_CMD_CFG_READ,
    CAN_CMD_CFG_WRITE,
    CAN_CMD_CFG_DEFAULT,
    CAN_CMD_TELEM_RATE,
    CAN_CMD_TELEM_VALUE,
    CAN_CMD_BEEP,
    CAN_CMD_POS,
    CAN_CMD_USER_DEFINED,
    CAN_CMD_RESET,
    CAN_CMD_STREAM,
    CAN_CMD_ON,
    CAN_CMD_OFF
};


extern void canInit(uint16_t filter_id);
extern void canProcess(void);
extern void canSend(uint8_t *pBuf, uint16_t len,uint16_t can_id);
extern void canSetConstants(void);
#ifdef __cplusplus  
        }  
#endif
#endif
