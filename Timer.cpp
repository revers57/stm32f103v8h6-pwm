#include "Timer.h"

CTimer* CTimer::InterruptTable[17];

extern "C" void TIM1_UP_IRQHandler(void){

	CTimer* Timer = CPWMTimer::InterruptTable[0];
	Timer->TimerInterruptHandle();
}
extern "C" void TIM2_IRQHandler(void){

	CTimer* Timer = CPWMTimer::InterruptTable[1];
	Timer->TimerInterruptHandle();
}

extern "C" void TIM3_IRQHandler(void){

	CTimer* Timer = CPWMTimer::InterruptTable[2];
	Timer->TimerInterruptHandle();
}
void CTimer::Setup()
{
	switch (TIMNumber)
	{
		case 1:
			TIM = TIM1;
			break;
		case 2:
			TIM = TIM2;
			break;
		case 3:
			TIM = TIM3;
			break;
	}
	
	switch (OCNumber)
	{
		case 1:
			Channel = TIM_IT_CC1;
			ChannelFlag = TIM_FLAG_CC1;
			break;
		case 2:
			Channel = TIM_IT_CC2;
			ChannelFlag = TIM_FLAG_CC2;
			break;
		case 3:
			Channel = TIM_IT_CC3;
			ChannelFlag = TIM_FLAG_CC3;
		case 4:
			Channel = TIM_IT_CC4;
			ChannelFlag = TIM_FLAG_CC4;
			break;
	}
	
	TIM_TimeBaseStructInit(&Timer);
	Timer.TIM_Prescaler = Prescaler - 1;
	Timer.TIM_Period = Period - 1;
	Timer.TIM_ClockDivision = 0;
	Timer.TIM_RepetitionCounter = 0;
	Timer.TIM_CounterMode = TIM_CounterMode_Up;
}
//by grinar on 12/08/2020
CTimer::CTimer(uint8_t tim_number, uint8_t channel_number, void (*interrupt_handle)(), double seconds)
	:BasicFrequency(72000000)
{
	uint32_t period = static_cast<uint32_t>(static_cast<double>(BasicFrequency)*seconds);
	uint32_t pres = 1;
	while (period > 0xFFFF)
	{
		pres *= 2;
		period /= 2;
	}
	Prescaler = pres;
	Frequency = BasicFrequency/Prescaler;
	Period = period;
	
	UserInterruptHandle = interrupt_handle;
	TIMNumber = tim_number;
	OCNumber = channel_number;
	Setup();
}
CTimer::CTimer(uint8_t tim_number, uint8_t channel_number, uint32_t desirable_freq, void (*interrupt_handle)())
	:BasicFrequency(72000000)
{
	Frequency = desirable_freq;
	Prescaler = BasicFrequency/Frequency;
	Frequency = BasicFrequency/Prescaler;
	Period = 0xFFFF;
	
	UserInterruptHandle = interrupt_handle;
	TIMNumber = tim_number;
	OCNumber = channel_number;
	Setup();
}
CPWMTimer::CPWMTimer(uint8_t tim_number, uint8_t channel_number, uint16_t pulse_time_us, uint32_t desirable_freq, void (*interrupt_handle)())
	:CTimer(tim_number, channel_number, desirable_freq, interrupt_handle)
{
	PulseTime = pulse_time_us;
	PulseWidth = Frequency/1000000*PulseTime;
	
	TIM_OCStructInit(&PWMConfig);
	PWMConfig.TIM_OCMode = TIM_OCMode_PWM1;
  PWMConfig.TIM_OutputState = TIM_OutputState_Enable;
  PWMConfig.TIM_OutputNState = TIM_OutputNState_Enable;
  PWMConfig.TIM_Pulse = PulseWidth;
  PWMConfig.TIM_OCPolarity = TIM_OCPolarity_High;
  PWMConfig.TIM_OCNPolarity = TIM_OCNPolarity_High;
  PWMConfig.TIM_OCIdleState = TIM_OCIdleState_Reset;
  PWMConfig.TIM_OCNIdleState = TIM_OCIdleState_Reset;
  
	if (TIM == TIM1)
	{
		TIM_BDTRStructInit(&BDTRConfig);
		BDTRConfig.TIM_OSSRState = TIM_OSSRState_Enable;
		BDTRConfig.TIM_OSSIState = TIM_OSSIState_Enable;
		BDTRConfig.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
		BDTRConfig.TIM_DeadTime = 0;
		BDTRConfig.TIM_Break = TIM_Break_Disable;
		BDTRConfig.TIM_BreakPolarity = TIM_BreakPolarity_High;
		BDTRConfig.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	}
}
CTimer::~CTimer(){}
void CTimer::Init()
{
	TIM_TimeBaseInit(TIM, &Timer);
	SetInterrupt();
	
}
void CTimer::SetInterrupt()
{
	if (UserInterruptHandle!=nullptr)
	{
		switch (TIMNumber)
		{
			case 1:
				InterruptTable[0] = this;
				NVIC_EnableIRQ(TIM1_UP_IRQn);	
				break;
			case 2:
				InterruptTable[1] = this;
				NVIC_EnableIRQ(TIM2_IRQn);	
				break;
			case 3:
				InterruptTable[2] = this;
				NVIC_EnableIRQ(TIM3_IRQn);	
				break;
			default:
				break;
		}
		TIM_ITConfig (TIM, TIM_IT_Update, ENABLE);
	}
}
void CPWMTimer::Init()
{
	//GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, DISABLE);
	TIM_Cmd(TIM, DISABLE);
	
	TIM_TimeBaseInit(TIM, &Timer);
	
	if (OCNumber == 1)
		TIM_OC1Init(TIM, &PWMConfig); 
	if (OCNumber == 2)
		TIM_OC2Init(TIM, &PWMConfig); 
	if (OCNumber == 3)
		TIM_OC3Init(TIM, &PWMConfig); 
	if (OCNumber == 4)
		TIM_OC4Init(TIM, &PWMConfig); 
	if (TIM == TIM1) TIM_BDTRConfig(TIM, &BDTRConfig);
	SetInterrupt();
	SetCounter(0);
}
void CTimer::Start()
{
	//SetCounter(0);
	TIM_Cmd(TIM, ENABLE);
	TIM_CtrlPWMOutputs(TIM, ENABLE);
}
void CTimer::Stop()
{
	TIM_Cmd(TIM, DISABLE);
	TIM_CtrlPWMOutputs(TIM, DISABLE);
}
void CTimer::SetPeriod(uint16_t period)
{
	Period = period;
	TIM_SetAutoreload(TIM, Period - 1);
}
uint16_t CTimer::GetCounter()
{
	return TIM_GetCounter(TIM);
}
void CTimer::SetCounter(uint16_t counter)
{
	TIM_SetCounter(TIM, counter);
}
void CPWMTimer::SetPulseWidth(uint16_t pulse_width)
{
	PulseWidth = pulse_width;
	TIM_SetCompare1(TIM, pulse_width);
} 
void CTimer::TimerInterruptHandle()
{
	//if (TIM_GetITStatus(TIM, Channel) != RESET)
		if (TIM_GetITStatus(TIM, TIM_IT_Update) != RESET)
	{
		/* Reset flag */
		TIM_ClearITPendingBit(TIM, TIM_IT_Update);

		UserInterruptHandle();
		
		if (TIM_GetFlagStatus(TIM, TIM_FLAG_Update) != RESET)
		{
			TIM_ClearFlag(TIM, TIM_FLAG_Update);
		}
	}
}
