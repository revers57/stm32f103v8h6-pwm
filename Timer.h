#ifndef __CPWMTIMER_H__
#define __CPWMTIMER_H__

#include "stm32f10x.h"
#include "stm32f10x_tim.h"

#include "Port.h"
class CTimer
{
public:
		static CTimer* InterruptTable[17];
protected:
	uint8_t TIMNumber;
	uint8_t OCNumber;

	TIM_TimeBaseInitTypeDef Timer;
	TIM_TypeDef *TIM;
	uint16_t Channel;
	uint16_t ChannelFlag;

	uint16_t Period;

	uint32_t BasicFrequency;
	uint32_t Frequency;
	uint16_t Prescaler;

	void (*UserInterruptHandle)();

	void SetInterrupt();
	void Setup();
public:
	CTimer(uint8_t tim_number, 
		uint8_t channel_number, 
		void (*interrupt_handle)(), 
		double seconds);
		
	CTimer( 
		uint8_t tim_number, 
		uint8_t channel_number, 
		uint32_t desirable_freq,
		void (*interrupt_handle)());

	~CTimer();
	void Init();
	void Start();
	void Stop();
	void SetPeriod(uint16_t period);
	void TimerInterruptHandle();
		

	uint16_t GetCounter();
	void SetCounter(uint16_t counter);
};
class CPWMTimer : public CTimer
{
protected:
	CPort* Port;
	uint16_t PulseWidth;
	uint16_t PulseTime;//us
	TIM_OCInitTypeDef PWMConfig;
	TIM_BDTRInitTypeDef BDTRConfig;
public:
	CPWMTimer(
		uint8_t tim_number, 
		uint8_t channel_number, 
		uint16_t pulse_time_us, 
		uint32_t desirable_freq,
		void (*interrupt_handle)());
	void Init();
private:
	void SetPulseWidth(uint16_t pulse_width);
	
};
#endif //__CTIMER_H__
