#pragma once

/* CPWMControl is a descendant of CConveyourControl  
 * for the first Welder machine (grinar) 09.11.2020
 *
 *
 */
#include <math.h>


#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

const int STATE_PWM_INIT           = 0;
const int STATE_PWM_STANDBY        = 1;
const int STATE_PWM_RUN            = 2;
const int STATE_PWM_STEP				   = 3;

const unsigned long TimerFrequency = 2000000;

//const double		Resolution			 = 114.3/4800;//23/09/2020 by PMA
const double		StartVelocity		 = TimerFrequency/(double)0xFFFF;
	
class CPWMControl
{
public:	
	int						m_iState;
	double				m_dCurrentSpeed;
	double				m_dTargetSpeed;
	double				m_dAcceleration;
	unsigned int	m_uiCurrentPeriod;
	int						m_iDirection;

	double				m_uiDistanceToTravel;
	double				m_uiRemainingDistance;
	double				m_dDistanceForAccel;
	double				m_dDistanceForConst;
	double				m_dResolution;//mm per imp
	
	bool 					m_bInitialized;
protected:
private:

//functions
public:
	CPWMControl();
	CPWMControl(double Speed, double Accel, double Resolution);
	~CPWMControl();
	void Init(double Speed, double Accel, double Resolution);
	void Run();
	void Stop();

	void MakeStep(int Dist);

	void SetResolution(double Val);
	void SetTargetSpeed(double Val);
	void SetAcceleration(double Val);

	double GetAccelDist(double TargetSpeed);

	unsigned int GetPeriod();
protected:
private:
	CPWMControl( const CPWMControl &c );
	CPWMControl& operator=( const CPWMControl &c );

	void PrecalcForStep(int Dist = -1);
};
