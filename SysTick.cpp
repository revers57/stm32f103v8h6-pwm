#include "SysTick.h"

#include "cmsis_os.h"

//29.07.20 (grinar) idea for timer is from 
// https://www.keil.com/pack/doc/CMSIS/RTOS/html/group__CMSIS__RTOS__KernelCtrl.html#gad0262e4688e95d1e9038afd9bcc16001

void Delay80Ns()
{
  uint32_t delay = static_cast<uint32_t>(osKernelSysTickMicroSec(1)*(0.08));
  uint32_t tick = osKernelSysTick();
  //while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(delay)); 
	while ((osKernelSysTick() - tick) < delay); //no need to use osKernelSysTickMicroSec() again - by PMA on 10/06/2020
}
void Delay40Ns()
{
  uint32_t delay = static_cast<uint32_t>(osKernelSysTickMicroSec(1)*(0.04));
  uint32_t tick = osKernelSysTick();
  //while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(delay)); 
	while ((osKernelSysTick() - tick) < delay); //no need to use osKernelSysTickMicroSec() again - by PMA on 10/06/2020
}
void Delay10Ns()
{
  uint32_t delay = static_cast<uint32_t>(osKernelSysTickMicroSec(1)*(0.01));
  uint32_t tick = osKernelSysTick();
  //while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(delay)); 
	while ((osKernelSysTick() - tick) < delay); //no need to use osKernelSysTickMicroSec() again - by PMA on 10/06/2020
}
void DelayMs(uint32_t delay)
{
	delay = 1000 * osKernelSysTickMicroSec(delay);
  uint32_t tick = osKernelSysTick();
  //while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(delay));     
	//for (int i = 0; i < 1000; i++)
			while ((osKernelSysTick() - tick) < delay); //no need to use osKernelSysTickMicroSec() again - by PMA on 10/06/2020
}
void DelayUs(uint32_t delay)
{   
	delay = osKernelSysTickMicroSec(delay);
  uint32_t tick = osKernelSysTick();
  while ((osKernelSysTick() - tick) < delay);     
}

