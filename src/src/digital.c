#include "digital.h"
#include <stdlib.h>

digitalPin *digitalInit(GPIO_TypeDef* port, const uint16_t pin) 
{
	digitalPin *p;
	GPIO_InitTypeDef GPIO_InitStructure;

	p = (digitalPin *)calloc(1, sizeof(digitalPin));
	p->port = port;
	p->pin = pin;

	digitalLo(p);

	GPIO_InitStructure.GPIO_Pin = pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(port, &GPIO_InitStructure);

	return p;
}

void digitalTogg(digitalPin *p) 
{
	if (digitalGet(p)) {
		digitalLo(p);
	}
	else {
		digitalHi(p);
	}
}
