#include "main.h"
#include "can.h"
#include "config.h"
#include "run.h"
#include "serial.h"

CanTxMsg TxMessage;
CanRxMsg RxMessage;

void canSend(uint8_t *pBuf, uint16_t len,uint16_t can_id)
{
    uint8_t err;
    uint8_t cur_len,TransmitMailbox,i;
    
    while(len)
    {
        cur_len = (len>8)?8:len;
        TxMessage.DLC = cur_len;
        TxMessage.ExtId = can_id;
				TxMessage.RTR = CAN_RTR_DATA;
				TxMessage.IDE = CAN_ID_EXT;
    
        for(i = 0; i < cur_len; i++)
        {
            TxMessage.Data[i] = pBuf[i];
    	}
       
	    TransmitMailbox = CAN_Transmit(CAN_CAN, &TxMessage);
        
	    while((CAN_TransmitStatus(CAN_CAN, TransmitMailbox) != CANTXOK));
        len -= cur_len;
        pBuf += cur_len;
    }
    
}
void canProcess(void){
	//TODO: process input data,it will be called at itr
}
void canInit(uint16_t filter_id) {
    GPIO_InitTypeDef GPIO_InitStructure;
    CAN_InitTypeDef CAN_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
    // Configure CAN pin: RX
    GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(CAN_PORT, &GPIO_InitStructure);

    // Configure CAN pin: TX
    GPIO_InitStructure.GPIO_Pin = CAN_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CAN_PORT, &GPIO_InitStructure);

    // CAN register init
    CAN_DeInit(CAN_CAN);
    CAN_StructInit(&CAN_InitStructure);

    // CAN cell init
    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = ENABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = ENABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;

    // CAN Baudrate = 1MBps
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
    CAN_InitStructure.CAN_Prescaler = 4;
    CAN_Init(CAN_CAN, &CAN_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //Filter Init
    CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000<<3;
	CAN_FilterInitStructure.CAN_FilterIdLow = filter_id<<3;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000<<3; //1: check; 0:ignore
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFFFF<<3;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	//CAN_ITConfig(CAN_CAN, CAN_IT_FMP0|CAN_IT_TME, ENABLE);
	CAN_ITConfig(CAN_CAN, CAN_IT_FMP0, ENABLE);
}

void USB_LP_CAN1_RX0_IRQHandler(void) {

    CanRxMsg RxMessage;
	serialPrint("USB_LP_CAN1_RX0_IRQHandler\r\n");
    memset(&RxMessage, 0, sizeof(RxMessage));
    CAN_Receive(CAN_CAN,  CAN_FIFO0, &RxMessage);
    
    if(RxMessage.IDE==CAN_ID_EXT)
    {
        uint8_t i;

        for(i = 0; i < RxMessage.DLC; i++)
        {
        	//read data to rx fifo
        	serialWrite(RxMessage.Data[i]);
            //#if (CARD_TYPE_SELECT == CARD_TYPE_MASTER)
    		//Card_Communicate_Slave_Recv(RxMessage.Data[i]);
            //#else
            //Card_Communicate_Master_Recv(RxMessage.Data[i]);
            //#endif
        }    
    }

}

void canSetConstants(void) {
}

void CAN1_RX1_IRQHandler(void) {
}

void USB_HP_CAN1_TX_IRQHandler(void)  {
	CAN_ClearITPendingBit(CAN_CAN, CAN_IT_TME);
	serialPrint("USB_HP_CAN1_TX_IRQHandler\r\n");
}
