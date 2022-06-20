#ifndef __CPORT_H__
#define __CPORT_H__
extern "C" {
#include "stm32f10x_gpio.h"
}

#ifndef ON
	#define ON true
#endif

#ifndef OFF
	#define OFF false
#endif

#ifndef IN
	#define IN false
#endif

#ifndef OUT
	#define OUT true
#endif
	
#ifndef AF
	#define AF true
#endif
#define DEFAULT_SPEED GPIO_Speed_50MHz

class CPort
{
public:
	static CPort* InterruptTable[17];
private:
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_TypeDef* Port;
	uint32_t EXTI_Line;
	uint32_t PinNumber;
	void (*UserInterruptHandle)();

	bool IsAF;
	bool DirOut;
	
	void SetMode();
	uint8_t GetPortSource();
public:
	
	CPort(GPIO_TypeDef *port, uint8_t pin, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed = DEFAULT_SPEED);
	CPort(GPIO_TypeDef *port, uint8_t pin, bool dir_out, bool is_af = false, GPIOSpeed_TypeDef speed = DEFAULT_SPEED);
	~CPort();
	void ConfigInterrupt(void (*interrupt_handle)());
	void Init();
	void SetDirection(bool mode);
	bool Get();
	void Set(bool state);
	bool EqualTo(bool High);
	void TimerInterruptHandle();
};
#endif //__CPORT_H__
