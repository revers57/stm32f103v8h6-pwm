#include "Port.h"
#include "SysTick.h"
#include "stm32f10x.h"
#include "stm32f10x_exti.h"
CPort* CPort::InterruptTable[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
extern "C" void EXTI0_IRQHandler(void)
{
	CPort::InterruptTable[0]->TimerInterruptHandle();
}
extern "C" void EXTI1_IRQHandler(void)//14.09.2020 (grinar)
{
	CPort::InterruptTable[1]->TimerInterruptHandle();
}
extern "C" void EXTI2_IRQHandler(void)//14.09.2020 (grinar)
{
	CPort::InterruptTable[2]->TimerInterruptHandle();
}
extern "C" void EXTI3_IRQHandler(void)//14.09.2020 (grinar)
{
	CPort::InterruptTable[3]->TimerInterruptHandle();
}
extern "C" void EXTI4_IRQHandler(void)//14.09.2020 (grinar)
{
	CPort::InterruptTable[4]->TimerInterruptHandle();
}
extern "C" void EXTI9_5_IRQHandler(void)//14.09.2020 (grinar)
{
	for (int i = 5; i <= 9; i++)
	{
		CPort* port = CPort::InterruptTable[i];
		if (port!=0) port->TimerInterruptHandle();
	}
}
extern "C" void EXTI15_10_IRQHandler(void)//14.09.2020 (grinar)
{
	for (int i = 10; i <= 15; i++)
	{
		CPort* port = CPort::InterruptTable[i];
		if (port!=0) port->TimerInterruptHandle();
	}
}
const int BUTTONRELEASE  = 500;
void CPort::SetMode()
{
	if (DirOut && IsAF)
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	if (DirOut && !IsAF)
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	if (!DirOut)
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
}
uint8_t CPort::GetPortSource()
{
	if (Port == GPIOA)
		return GPIO_PortSourceGPIOA;
	if (Port == GPIOB)
		return GPIO_PortSourceGPIOB;
	if (Port == GPIOC)
		return GPIO_PortSourceGPIOC;
	if (Port == GPIOD)
		return GPIO_PortSourceGPIOD;
	if (Port == GPIOE)
		return GPIO_PortSourceGPIOE;
	if (Port == GPIOF)
		return GPIO_PortSourceGPIOF;
	return 0;
}
CPort::CPort(GPIO_TypeDef *port, uint8_t pin, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed)
{
	Port = port;
	PinNumber = pin;
	IsAF = (mode==GPIO_Mode_AF_OD || mode==GPIO_Mode_AF_PP);
	DirOut = (mode==GPIO_Mode_Out_PP || mode==GPIO_Mode_AF_PP);
	
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = mode;
	GPIO_InitStruct.GPIO_Pin = static_cast<uint16_t>(1<<PinNumber);
	GPIO_InitStruct.GPIO_Speed = speed;
}
CPort::CPort(GPIO_TypeDef *port, uint8_t pin, bool dir_out, bool is_af, GPIOSpeed_TypeDef speed)
{
	Port = port;
	PinNumber = pin;
	IsAF = is_af;
	DirOut = dir_out;
	
	GPIO_StructInit(&GPIO_InitStruct);
	SetMode();
	GPIO_InitStruct.GPIO_Pin = static_cast<uint16_t>(1<<PinNumber);
	GPIO_InitStruct.GPIO_Speed = speed;
}
CPort::~CPort(){}
	/*call after Init()*/

	//we cannot set interrupt for the same line on different gpios!!!(grinar)
void CPort::ConfigInterrupt(void (*interrupt_handle)())
{
	EXTI_Line = GPIO_InitStruct.GPIO_Pin;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_EXTILineConfig(GetPortSource(), PinNumber);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  DISABLE);
	
	InterruptTable[PinNumber] = this;
	if (PinNumber<=4)
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn + PinNumber;
	else if (PinNumber<=9)
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	else if (PinNumber<=15)
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	UserInterruptHandle = interrupt_handle;
}
void CPort::Init()
{
	GPIO_Init(Port, &GPIO_InitStruct);
}
void CPort::SetDirection(bool dir_out)
{
	DirOut = dir_out;
	SetMode();
	GPIO_Init(Port, &GPIO_InitStruct);
}
bool CPort::Get()
{
	return GPIO_ReadInputDataBit(Port, GPIO_InitStruct.GPIO_Pin) == Bit_SET ? true : false;
}
void CPort::Set(bool state)
{
	GPIO_WriteBit(Port, GPIO_InitStruct.GPIO_Pin, state?Bit_SET:Bit_RESET);
}
bool CPort::EqualTo(bool High)
{
	bool res = false;
	if (High)
	{
		if (!Get()) {
			for (int i = 0; i<500; i++) __NOP();
			if (!Get()) res = true;
		}
	}
	else
	{
		if (Get()) {
			for (int i = 0; i<500; i++) __NOP();
			if (Get()) res = true;
		}
	}	
	return res;
}
void CPort::TimerInterruptHandle()
{
	if(EXTI_GetITStatus(EXTI_Line) != RESET)
  {
      UserInterruptHandle();
  }
  EXTI_ClearITPendingBit(EXTI_Line);
}
