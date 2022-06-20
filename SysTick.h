#ifndef __SYSTICK_H
#define __SYSTICK_H
#include "stm32f10x.h"
#include <stdint.h>

void Delay80Ns();
void Delay40Ns();
void Delay10Ns();
void DelayMs(uint32_t delay);
void DelayUs(uint32_t delay);

#endif // __SYSTICK_H
