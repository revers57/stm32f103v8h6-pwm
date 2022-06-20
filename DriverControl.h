#pragma once
#include "Port.h"
#include "Timer.h"
#include "PWMControl.h"

#define FORWARD true
#define BACKWARDS false
class CDriverControl
{
	CPort* m_pDir;
	CPort* m_pStartLimit;
	CPWMTimer* m_pTimer;
	
	double m_dRunningSpeed;//mm
	double m_dIdleSpeed;//mm
	double m_dAcceleration;//mm
	double m_dFastAcceleration;//mm
	
public:
	int m_iDistAfterSensor;
	bool m_bSensorFound;

	bool m_bActionDir;

	CDriverControl(CPort* dir, CPort* limit0, CPWMTimer* timer);
	~CDriverControl();

	void Init(double RunningSpeed, double IdleSpeed, double Accel, double FastAccel, double Resolution, bool Dir);

	void Step(bool Dir, int Dist, bool Wait = true);//imp
	void Step(bool Dir, double Dist, bool Wait = true);//mm

	void TestRun(bool Dir);
	void Run(bool Dir);
	void RunSingle(bool Dir);
	void Stop(bool Wait = true);

	void Return(bool Wait);

	void Wait();

	CPWMControl m_cPWMControl;
	int m_iState;
	int m_iErrorCode;
	void Handler();
	
	void SetFastAccel(bool val)
	{
		m_cPWMControl.SetAcceleration(val ? m_dFastAcceleration : m_dAcceleration);
	}
};