#include "PWMControl.h"

CPWMControl::CPWMControl() : 
m_iState(STATE_PWM_INIT),
m_dCurrentSpeed(0),
m_dTargetSpeed(0),
m_dAcceleration(0),
m_uiCurrentPeriod(0),
m_iDirection(true),
m_uiDistanceToTravel(0),
m_uiRemainingDistance(0),
m_dDistanceForAccel(0),
m_dDistanceForConst(0),
m_bInitialized(false)
{};
	
CPWMControl::CPWMControl(double Speed, double Accel, double Resolution) : 
m_iState(STATE_PWM_INIT),
m_dCurrentSpeed(0),
m_dTargetSpeed(0),
m_uiCurrentPeriod(0),
m_iDirection(true),
m_uiDistanceToTravel(0),
m_uiRemainingDistance(0),
m_dDistanceForAccel(0),
m_dDistanceForConst(0),
m_dResolution(Resolution),
m_bInitialized(false)
{
	m_dAcceleration = Accel/Resolution;
	m_dTargetSpeed = Speed/Resolution;
	PrecalcForStep();
}
CPWMControl::~CPWMControl()
{}
void CPWMControl::Init(double Speed, double Accel, double Resolution)
{
	m_dAcceleration = Accel/Resolution;
	m_dTargetSpeed = Speed/Resolution;
	m_dResolution = Resolution;
	PrecalcForStep();
}
void CPWMControl::SetResolution(double Val)
{
}
void CPWMControl::PrecalcForStep(int Dist)
{
	if (m_dAcceleration != 0 && m_dTargetSpeed != 0)
	{
		m_dDistanceForAccel = (m_dTargetSpeed*m_dTargetSpeed)/(2*m_dAcceleration);
		
		unsigned int dist = static_cast<unsigned int>(Dist);
		
		m_uiDistanceToTravel = dist - 1;
		
		m_dDistanceForConst = m_uiDistanceToTravel - (2*m_dDistanceForAccel);
		if (m_dDistanceForConst<0)
		{
			m_dDistanceForAccel = m_uiDistanceToTravel/2;
			m_dTargetSpeed = sqrt(2*m_dDistanceForAccel*m_dAcceleration);
			m_dDistanceForConst = 0;
		}	
		m_bInitialized = true;
	}
	else
		m_bInitialized = false;
}
void CPWMControl::SetTargetSpeed(double Val)
{
		m_dTargetSpeed = Val/m_dResolution;
		PrecalcForStep();
}
void CPWMControl::SetAcceleration(double Val)
{
		m_dAcceleration = Val/m_dResolution;
		PrecalcForStep();
}
void CPWMControl::MakeStep(int Dist)
{
	if (m_bInitialized)
	{
		PrecalcForStep(Dist);
		m_iState = STATE_PWM_STEP;
		m_iDirection = 1;
		m_uiRemainingDistance = m_uiDistanceToTravel;
	}
	else
		m_iState = STATE_PWM_STANDBY;
}
void CPWMControl::Run()
{
	if (m_bInitialized)
	{
		m_iDirection = 1;
		m_iState = STATE_PWM_RUN;
	}
	else
		m_iState = STATE_PWM_STANDBY;
}
void CPWMControl::Stop()
{
	if (m_bInitialized)
	{
		m_iDirection = -1;
		m_dTargetSpeed = StartVelocity;
	}
}
unsigned int CPWMControl::GetPeriod()
{
	if (m_bInitialized)
	{
		if (m_iDirection == 0)
		{
			if (m_iState == STATE_PWM_STEP)
			{
				if (m_uiRemainingDistance-m_dDistanceForAccel>0)
				{
					m_uiRemainingDistance--;
					return m_uiCurrentPeriod;
				}
				else
				{
					m_iDirection = -1;
					m_dTargetSpeed = StartVelocity;
				}
			}
			else if (m_iState == STATE_PWM_RUN)	
				return m_uiCurrentPeriod;
		}
			
		if (m_dCurrentSpeed == 0.0)
		{
			m_dCurrentSpeed = StartVelocity;
		}
		else
		{
			double SpeedSquare = m_iDirection * 2 * m_dAcceleration + m_dCurrentSpeed*m_dCurrentSpeed;
			m_dCurrentSpeed = SpeedSquare>0?sqrt(SpeedSquare):StartVelocity;
			
			if (m_iDirection == -1)
			{
				if (m_dCurrentSpeed <= m_dTargetSpeed)
				{
					m_dCurrentSpeed = m_dTargetSpeed;
					m_iDirection = 0;
					if (m_dTargetSpeed == StartVelocity)
					{
						m_dTargetSpeed = 0;
						m_iState = STATE_PWM_STANDBY;
						return (m_uiCurrentPeriod = 0);
					}
				}
			}
			else if ((m_iDirection == 1) && (m_dCurrentSpeed >= m_dTargetSpeed))
			{
				m_iDirection = 0;
				m_dCurrentSpeed = m_dTargetSpeed;
			}
		}
		if (m_iState == STATE_PWM_STEP)
			m_uiRemainingDistance--;
		m_uiCurrentPeriod = static_cast<unsigned int>(TimerFrequency/m_dCurrentSpeed);
		return m_uiCurrentPeriod;
	}
	else
		return 0;
}
