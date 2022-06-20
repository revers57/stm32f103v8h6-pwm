#include "main.h"

osMutexId  xGlobalMutexId;
osMutexDef (xGlobalMutex);

CRotationDriver RotationDriver;

CDriverControl DrumDriverCtrl(&port_OutDir1, &port_InDrumStartSensor, &PWMTimer1);
CDriverControl CaretDriverCtrl(&port_OutDir0, &port_InCaretStartSensor, &PWMTimer0);

void CycleCounterHandler()
{
	if (RotationDriver.m_iGlobalState == STATE_RUNNING)
	{
		RotationDriver.m_iCycleCount = RotationDriver.m_iImpCount/ImpsPerDrumRevolution;
		RotationDriver.m_iThreadLength = (RotationDriver.m_iCycleCount * DrumPerimeter)/1000;
		RotationDriver.m_bNeedRedraw = true;
	}
}
void TimerHandler0() {
	CaretDriverCtrl.Handler();
	RotationDriver.m_iImpCount2++;
}

void TimerHandler1() {
	DrumDriverCtrl.Handler();
	RotationDriver.m_iImpCount++;
}

void StopHandler()
{
	if (RotationDriver.m_iGlobalState == STATE_RUNNING)
	{
		RotationDriver.m_iStopSignalCount++;
		if (RotationDriver.m_bStoppingMidWay && RotationDriver.m_iStopSignalCount == 1)
		{
			RotationDriver.Stop();
			port_OutRollerDown.Set(0);
		}
		else if ((RotationDriver.m_bStoppingMidWay && RotationDriver.m_iStopSignalCount == 3) ||
			(!RotationDriver.m_bStoppingMidWay && RotationDriver.m_iStopSignalCount == 1))
		{
			CaretDriverCtrl.SetFastAccel(true);
			DrumDriverCtrl.SetFastAccel(true);
			RotationDriver.Stop();
		}
	}
	if (RotationDriver.m_iGlobalState == STATE_SKIP_DIST)
	{
		if (RotationDriver.m_bStoppingMidWay && RotationDriver.m_iStopSignalCount == 2)
		{
			port_OutRollerDown.Set(1);
			RotationDriver.Stop();
		}

	}
}
void Fault0Handler()
{
	osMutexWait (xGlobalMutexId, osWaitForever);

	RotationDriver.EmergencyStop();
	RotationDriver.m_iGlobalState = STATE_ERROR;
	RotationDriver.m_iErrorCode = ERROR_DRIVERFAULT0;
	RotationDriver.m_bNeedRedraw = true;

	osMutexRelease (xGlobalMutexId);
}
void Fault1Handler()
{
	osMutexWait (xGlobalMutexId, osWaitForever);

	RotationDriver.Stop();
	RotationDriver.m_iGlobalState = STATE_ERROR;
	RotationDriver.m_iErrorCode = ERROR_DRIVERFAULT1;
	RotationDriver.m_bNeedRedraw = true;

	osMutexRelease (xGlobalMutexId);
}

int main() {

	osKernelInitialize();
	osKernelStart();
	SysInit();

	auto i = InitRotationDriverThread();

	while (1) {
	}
}

void SysInit() {

	GPIO_InitTypeDef      GPIO_InitStructure;
	CAN_InitTypeDef       CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	xGlobalMutexId = osMutexCreate (osMutex(xGlobalMutex));
};
