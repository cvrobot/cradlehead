#ifndef _DIGITAL_H
#define _DIGITAL_H
#ifdef __cplusplus  
       extern "C" {  
#endif  
#include "stm32f10x_gpio.h"

typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} digitalPin;

#define digitalHi(p)    { p->port->BSRR = p->pin; } //set high
#define digitalLo(p)    { p->port->BRR = p->pin; }  //set low
#define digitalGet(p)   ((p->port->ODR & p->pin) != 0) //get io high low

extern digitalPin *digitalInit(GPIO_TypeDef* port, const uint16_t pin);
extern void digitalTogg(digitalPin *p);
#ifdef __cplusplus  
       } 
#endif  
#endif
