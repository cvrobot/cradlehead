#include "serial.h"
#include "config.h"
#include "misc.h"
#include <stdio.h>
#include <stdlib.h>

#include "stm32f10x_dma.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"

#define SERIAL_UART		USART1
//#define SERIAL_FLOW_CONTROL	USART_HardwareFlowControl_RTS_CTS
#define SERIAL_FLOW_CONTROL	USART_HardwareFlowControl_None
#define SERIAL_UART_PORT	GPIOA
#define SERIAL_UART_TX_PIN	GPIO_Pin_9
#define SERIAL_UART_RX_PIN	GPIO_Pin_10
#define SERIAL_TX_DMA		DMA1_Channel4
#define SERIAL_RX_DMA		DMA1_Channel5

#define SERIAL_MIN_BAUD		9600
#define SERIAL_MAX_BAUD		921600


#define SERIAL2_UART		USART2
//#define SERIAL_FLOW_CONTROL	USART_HardwareFlowControl_RTS_CTS
#define SERIAL2_FLOW_CONTROL	USART_HardwareFlowControl_None
#define SERIAL2_UART_PORT	GPIOA
#define SERIAL2_UART_TX_PIN	GPIO_Pin_2
#define SERIAL2_UART_RX_PIN	GPIO_Pin_3
#define SERIAL2_TX_DMA		DMA1_Channel7
#define SERIAL2_RX_DMA		DMA1_Channel6
#define SERIAL2_MIN_BAUD		9600
#define SERIAL2_MAX_BAUD		921600


#define SERIAL3_UART		USART3
//#define SERIAL_FLOW_CONTROL	USART_HardwareFlowControl_RTS_CTS
#define SERIAL3_FLOW_CONTROL	USART_HardwareFlowControl_None
#define SERIAL3_UART_PORT	GPIOB
#define SERIAL3_UART_TX_PIN	GPIO_Pin_10
#define SERIAL3_UART_RX_PIN	GPIO_Pin_11
#define SERIAL3_TX_DMA		DMA1_Channel2
#define SERIAL3_RX_DMA		DMA1_Channel3

#define SERIAL3_MIN_BAUD		9600
#define SERIAL3_MAX_BAUD		921600

#define SERIAL_TX_BUFSIZE	4096
#define SERIAL_RX_BUFSIZE	256
typedef struct {
    volatile unsigned char txBuf[SERIAL_TX_BUFSIZE];
    unsigned int txHead, txTransfing,txTail;
    volatile unsigned char rxBuf[SERIAL_RX_BUFSIZE];
    volatile unsigned int rxHead, rxTail;
    unsigned int rxPos;
	//gpio pin 
	uint16_t gpioRxPin,gpioTxPin;
	GPIO_TypeDef* gpioUartPort;
	//nvic dma Tx interrupt 
	int serialTxNvicCh;
	DMA_Channel_TypeDef *serialRxDma,*serialTxDma;

	USART_TypeDef *uartPort;

} serialPort_t;
serialPort_t serialPort[3];
static int serialLogPort=0;

void serialXStartTxDMA(int serial) {
    serialPort_t *s = &serialPort[serial];

    s->serialTxDma->CMAR = (uint32_t)&s->txBuf[s->txTail];
    if (s->txHead > s->txTail) {
		s->serialTxDma->CNDTR = s->txHead - s->txTail;
		s->txTail = s->txHead;
    }
    else {
		s->serialTxDma->CNDTR = SERIAL_TX_BUFSIZE - s->txTail;
		s->txTail = 0;
    }

    DMA_Cmd(s->serialTxDma, ENABLE);
}
void serialStartTxDMA() {
	serialXStartTxDMA(serialLogPort);
}

void serialXWrite(int serial,uint8_t ch) {
    serialPort_t *s = &serialPort[serial];

    s->txBuf[s->txHead] = ch;
    s->txHead = (s->txHead + 1) % SERIAL_TX_BUFSIZE;

    if (!(s->serialTxDma->CCR & 1))
		serialXStartTxDMA(serial);
}
void serialWrite(uint8_t ch) {
	serialXWrite(serialLogPort,ch);
}

unsigned short serialXAvailable(int serial) {
	serialPort_t *s = &serialPort[serial];
    return (s->serialRxDma->CNDTR != s->rxPos);
}
unsigned short serialAvailable(void) {
	return serialXAvailable(serialLogPort);
}

// only call after a affirmative return from serialAvailable()
unsigned char serialXRead(int serial) {
    serialPort_t *s = &serialPort[serial];
    uint8_t ch;

    ch = s->rxBuf[SERIAL_RX_BUFSIZE - s->rxPos];
    if (--s->rxPos == 0)
		s->rxPos = SERIAL_RX_BUFSIZE;

    return ch;
}
unsigned char serialRead(void){
	return serialXRead(serialLogPort);
}


void serialXPrint(int serial, const char *str) {
    while (*str)
		serialXWrite(serial,*(str++));
}
void serialPrint(const char *str) {
	serialXPrint(serialLogPort,str);
}
static void serialStructInit(int serial){
	serialPort_t *s = &serialPort[serial];
	
	switch(serial){
		case 0:
			// alternate function push-pull
    		s->gpioTxPin = SERIAL_UART_TX_PIN;
			s->gpioRxPin = SERIAL_UART_RX_PIN;
			s->gpioUartPort = SERIAL_UART_PORT;
			s->serialTxNvicCh = DMA1_Channel4_IRQn;
			s->uartPort = SERIAL_UART;
			s->serialRxDma = SERIAL_RX_DMA;
			s->serialTxDma = SERIAL_TX_DMA;
			break;
		case 1:
    		s->gpioTxPin = SERIAL2_UART_TX_PIN;
			s->gpioRxPin = SERIAL2_UART_RX_PIN;
			s->gpioUartPort = SERIAL2_UART_PORT;
			s->serialTxNvicCh = DMA1_Channel7_IRQn;
			s->uartPort = SERIAL2_UART;
			s->serialRxDma = SERIAL2_RX_DMA;
			s->serialTxDma = SERIAL2_TX_DMA;
			break;
		case 2:
    		s->gpioTxPin = SERIAL3_UART_TX_PIN;
			s->gpioRxPin = SERIAL3_UART_RX_PIN;
			s->gpioUartPort = SERIAL3_UART_PORT;
			s->serialTxNvicCh = DMA1_Channel2_IRQn;
			s->uartPort = SERIAL3_UART;
			s->serialRxDma = SERIAL3_RX_DMA;
			s->serialTxDma = SERIAL3_TX_DMA;
			break;
	}
		//point for serial usage
    s->rxHead = s->rxTail = 0;
    s->txHead = s->txTail = s->txTransfing =0;
    s->rxPos = SERIAL_RX_BUFSIZE;
}

static void serialPortOpen(int serial,int baud) {
		USART_InitTypeDef USART_InitStructure;
		serialPort_t *s = &serialPort[serial];

    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = SERIAL_FLOW_CONTROL;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(s->uartPort, &USART_InitStructure);
}

static void serialPinInit(int serial){
	GPIO_InitTypeDef GPIO_InitStructure;
	serialPort_t *s = &serialPort[serial];

	GPIO_InitStructure.GPIO_Pin = s->gpioTxPin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(s->gpioUartPort, &GPIO_InitStructure);

    // input floating w/ pull ups
    GPIO_InitStructure.GPIO_Pin = s->gpioRxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(s->gpioUartPort, &GPIO_InitStructure);
}

static void serialNvicInit(int serial){
	NVIC_InitTypeDef NVIC_InitStructure;
	serialPort_t *s = &serialPort[serial];

    // Enable the DMA1_Channelx global Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = s->serialTxNvicCh;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

static void serialDmaInit(int serial){
	DMA_InitTypeDef DMA_InitStructure;
	serialPort_t *s = &serialPort[serial];

    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	//dma rx
	DMA_DeInit(s->serialRxDma);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)s->uartPort + 0x04;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)s->rxBuf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_BufferSize = SERIAL_RX_BUFSIZE;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_Init(s->serialRxDma, &DMA_InitStructure);
	DMA_Cmd(s->serialRxDma, ENABLE);

	USART_DMACmd(s->uartPort, USART_DMAReq_Rx, ENABLE);

	//dma tx
    DMA_DeInit(s->serialTxDma);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)s->uartPort + 0x04;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_Init(s->serialTxDma, &DMA_InitStructure);
    DMA_ITConfig(s->serialTxDma, DMA_IT_TC, ENABLE);
	s->serialTxDma->CNDTR = 0;
	USART_DMACmd(s->uartPort, USART_DMAReq_Tx, ENABLE);
    USART_Cmd(s->uartPort, ENABLE);
}

void serialInit(void) {
    int i;
    
	for(i= 0;i<3;i++){
		serialStructInit(i);
		serialPinInit(i);
		serialPortOpen(i,p[BAUD_RATE]);

		serialNvicInit(i);
		serialDmaInit(i);
	}
}

// USART1 tx DMA done IRQ 
void DMA1_Channel4_IRQHandler(void) {
    DMA_ClearITPendingBit(DMA1_IT_TC4);
    DMA_Cmd(SERIAL_TX_DMA, DISABLE);

    if (serialPort[0].txHead != serialPort[0].txTail)
		serialXStartTxDMA(0);
}

// USART2 tx DMA done IRQ
void DMA1_Channel7_IRQHandler(void) {
    DMA_ClearITPendingBit(DMA1_IT_TC7);
    DMA_Cmd(SERIAL2_TX_DMA, DISABLE);

    if (serialPort[1].txHead != serialPort[1].txTail)
		serialXStartTxDMA(1);
}
// USART3 tx DMA IRQ
void DMA1_Channel2_IRQHandler(void) {
    DMA_ClearITPendingBit(DMA1_IT_TC2);
    DMA_Cmd(SERIAL3_TX_DMA, DISABLE);

    if (serialPort[2].txHead != serialPort[2].txTail)
		serialXStartTxDMA(2);
}

void serialSetConstants(void) {
    p[BAUD_RATE] = (int)p[BAUD_RATE];
	p[SERIAL_LOG_PORT] = (int)p[SERIAL_LOG_PORT];
    if (p[BAUD_RATE] < SERIAL_MIN_BAUD)
		p[BAUD_RATE] = SERIAL_MIN_BAUD;
    else if (p[BAUD_RATE] > SERIAL_MAX_BAUD)
		p[BAUD_RATE] = SERIAL_MAX_BAUD;

		serialLogPort = (int)(p[SERIAL_LOG_PORT]);
    //serialPortOpen(p[SERIAL_DEBUG_PORT],p[BAUD_RATE]);
}
