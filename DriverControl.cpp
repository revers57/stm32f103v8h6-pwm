#include "DriverControl.h"
#include "SysTick.h"
#include "RotationDriver.h"
CDriverControl::CDriverControl(CPort* dir, CPort* limit0, CPWMTimer* timer) : 
m_pDir(dir),
m_pStartLimit(limit0),
m_pTimer(timer)
{}
	
CDriverControl::~CDriverControl()
{}
	
void CDriverControl::Init(double RunningSpeed, double IdleSpeed, double Accel, double FastAccel, double Resolution, bool Dir)
{
		m_dRunningSpeed = RunningSpeed;
		m_dIdleSpeed = IdleSpeed;
		m_dAcceleration = Accel;
		m_dFastAcceleration = FastAccel;
		m_cPWMControl.Init(RunningSpeed, Accel, Resolution);
		m_iState = STATE_STANDBY;
		m_bActionDir = Dir;
}

void CDriverControl::Run(bool Dir)
{
		m_bActionDir = Dir;
		m_iState = STATE_RUNNING;
		m_pDir->Set(m_bActionDir);
		m_iDistAfterSensor = 0;
		m_bSensorFound = false;
		m_cPWMControl.SetTargetSpeed(m_dRunningSpeed);
		m_cPWMControl.SetAcceleration(m_dAcceleration);
		m_cPWMControl.Run();
		m_pTimer->Start();
}
void CDriverControl::RunSingle(bool Dir)
{
		m_bActionDir = Dir;
		m_iState = STATE_RUNNING;
		m_pDir->Set(m_bActionDir);
		m_iDistAfterSensor = 0;
		m_bSensorFound = false;
		m_cPWMControl.SetTargetSpeed(m_dIdleSpeed);
		m_cPWMControl.SetAcceleration(m_dFastAcceleration);
		m_cPWMControl.Run();
		m_pTimer->Start();
}
void CDriverControl::TestRun(bool Dir)
{
		m_iState = STATE_TEST_RUNNING;
		m_pDir->Set(Dir);
		m_iDistAfterSensor = 0;
		m_bSensorFound = false;
		m_cPWMControl.SetTargetSpeed(m_dIdleSpeed);
		m_cPWMControl.SetAcceleration(m_dFastAcceleration);
		m_cPWMControl.Run();
		m_pTimer->Start();
}
void CDriverControl::Step(bool Dir, int Dist, bool Wait)
{
		m_iState = STATE_RUNNING;
		m_pDir->Set(Dir);
		m_iDistAfterSensor = 0;
		m_bSensorFound = false;
		m_cPWMControl.SetTargetSpeed(m_dIdleSpeed);
		m_cPWMControl.SetAcceleration(m_dAcceleration);
		m_cPWMControl.MakeStep(Dist);
		m_pTimer->Start();
}
void CDriverControl::Step(bool Dir, double Dist, bool Wait)
{
		m_iState = STATE_RUNNING;
		m_pDir->Set(Dir);
		m_iDistAfterSensor = 0;
		m_bSensorFound = false;
		m_cPWMControl.SetTargetSpeed(m_dIdleSpeed);
		m_cPWMControl.SetAcceleration(m_dAcceleration);
		m_cPWMControl.MakeStep(Dist);
		m_pTimer->Start();
}
void CDriverControl::Stop(bool Wait)
{
		m_cPWMControl.Stop();
		if (Wait)
			while (m_cPWMControl.m_iState == STATE_PWM_RUN) DelayMs (1);
}

void CDriverControl::Return(bool Wait)
{
		m_iState = STATE_RETURNING;
		m_pDir->Set(!m_bActionDir);
		m_iDistAfterSensor = 0;
		m_bSensorFound = false;
		m_cPWMControl.SetTargetSpeed(m_dIdleSpeed);
		m_cPWMControl.SetAcceleration(m_dFastAcceleration);
		m_cPWMControl.Run();
		m_pTimer->Start();
		if (Wait)
			while (m_cPWMControl.m_iState == STATE_PWM_RUN) DelayMs (1);
}

void CDriverControl::Wait()
{
		while (m_cPWMControl.m_iState != STATE_PWM_STANDBY && m_cPWMControl.m_iState != STATE_PWM_INIT) DelayMs (1);
}

void CDriverControl::Handler()
{
	switch (m_iState)
	{
		case STATE_STANDBY:
			m_pTimer->Stop();
			m_cPWMControl.m_iState = STATE_PWM_STANDBY;
			return;
		case STATE_RETURNING:
			{
				uint16_t period = m_cPWMControl.GetPeriod();
				if (m_bSensorFound && period == 0)
				{
						m_pDir->Set(m_bActionDir);
						m_cPWMControl.SetTargetSpeed(m_dIdleSpeed);
						m_cPWMControl.MakeStep(m_iDistAfterSensor + DistToSensor);
						period = m_cPWMControl.GetPeriod();
						m_iState = STATE_LEFT_SENSOR_DRIVEOFF;
				}
				else if (m_bSensorFound)
				{
					m_iDistAfterSensor++;
				}
				else if (m_pStartLimit->EqualTo(ON))
				{
					m_cPWMControl.Stop();
					m_bSensorFound = true;
					m_iDistAfterSensor++;
				}
				m_pTimer->SetPeriod(period);
			}
			return;
		case STATE_TEST_RUNNING:
		case STATE_RUNNING:
			{
				uint16_t period = m_cPWMControl.GetPeriod();
				if (period == 0)
				{
					m_pTimer->Stop();
					m_iState = STATE_FINISHED;
					return;
				}
				/*if (m_pStartLimit->EqualTo(ON))
				{
					m_pTimer->Stop();
					m_iState = STATE_ERROR;
					m_iErrorCode = ERROR_LIMIT0;
					m_cPWMControl.m_iState = STATE_PWM_STANDBY;
					return;
				}*/
				/*if (m_bSensorFound && period == 0)
				{
						m_pDir->Set(!m_pDir->Get());
						m_cPWMControl.MakeStep(m_iDistAfterSensor + DistToSensor);
						period = m_cPWMControl.GetPeriod();
						m_iState = STATE_LEFT_SENSOR_DRIVEOFF;
				}
				else if (m_bSensorFound)
				{
					m_iDistAfterSensor++;
				}
				if (m_pStartLimit->EqualTo(ON))
				{
					m_bSensorFound = true;
					m_iDistAfterSensor++;
				}*/
				m_pTimer->SetPeriod(period);
			}
			return;
		case STATE_LEFT_SENSOR_DRIVEOFF:
				uint16_t period = m_cPWMControl.GetPeriod();
				if (period == 0)
				{
					m_pTimer->Stop();
					m_iState = STATE_STANDBY;
					return;
				}
				else m_pTimer->SetPeriod(period);
				/*if (m_pStartLimit->EqualTo(ON))//error
				{
					m_pTimer->Stop();
					m_iState = STATE_ERROR;
					m_iErrorCode = ERROR_LIMIT0;
					m_cPWMControl.m_iState = STATE_PWM_STANDBY;
					return;
				}
				if (m_pEndLimit->EqualTo(ON))//error
				{
					m_pTimer->Stop();
					m_iState = STATE_ERROR;
					m_iErrorCode = ERROR_LIMIT1;
					m_cPWMControl.m_iState = STATE_PWM_STANDBY;
					return;
				}*/
	}
}