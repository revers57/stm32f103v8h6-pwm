#ifndef __RotationDriver_H
#define __RotationDriver_H

#include <stdlib.h>
#include <math.h>
#include <string>
#include "Timer.h"
#include "Port.h"
#include "LCDIF.h"
#include "DriverControl.h"

const int  STATE_INIT = 0;
const int  STATE_STANDBY = 1;
const int  STATE_RUNNING = 2;
const int  STATE_RETURNING = 3;
const int  STATE_FINISHED = 4;
const int  STATE_MENU = 5;
const int  STATE_ERROR = 6;
const int  STATE_LEFT_SENSOR_SEARCH = 7;
const int  STATE_RIGHT_SENSOR_SEARCH = 8;
const int  STATE_LEFT_SENSOR_DRIVEOFF = 9;
const int  STATE_RIGHT_SENSOR_DRIVEOFF = 10;
const int  STATE_TEST_RUNNING = 11;
const int  STATE_CARET_RETURNING = 12;
const int  STATE_DRUM_RETURNING = 13;
const int  STATE_DRUM_WRAP = 14;
const int  STATE_SKIP_DIST = 15;

const int  STATE_MENU_THICKNESS_SELECT = 1;
const int  STATE_MENU_VELOCITY_SELECT = 2;
const int  STATE_MENU_DRUMDIRECTION_SELECT = 3;
const int  STATE_MENU_MODE_SELECT = 4;
const int  STATE_MENU_DRUMTEST_SELECT = 5;
const int  STATE_MENU_CARETTEST_SELECT = 6;
const int  STATE_MENU_THICKNESS_SETUP = 7;
const int  STATE_MENU_VELOCITY_SETUP = 8;
const int  STATE_MENU_DRUMTEST_SETUP = 9;
const int  STATE_MENU_CARETTEST_SETUP = 10;
const int  STATE_MENU_DRUMDIRECTION_SETUP = 11;
const int  STATE_MENU_MODE_SETUP = 12;

//const int  STATE_MENU_PWMTEST_SELECT  = 3;
//const int  STATE_MENU_INTEST_SELECT   = 4;
//const int  STATE_MENU_OUTTEST_SELECT= 5;
//const int  STATE_MENU_DIST0_SELECT 		= 10;
//const int  STATE_MENU_DIST1_SELECT 		= 11;
//const int  STATE_MENU_SPEED0_SELECT		= 12;
//const int  STATE_MENU_SPEED1_SELECT 	= 13;
//const int  STATE_MENU_ACC0_SELECT 		= 14;
//const int  STATE_MENU_ACC1_SELECT 		= 15;
//const int  STATE_MENU_PWM0_SELECT 		= 16;
//const int  STATE_MENU_PWM1_SELECT 		= 17;
//const int  STATE_MENU_DIST0_SETUP 		= 20;
//const int  STATE_MENU_DIST1_SETUP 		= 21;
//const int  STATE_MENU_SPEED0_SETUP		= 22;
//const int  STATE_MENU_SPEED1_SETUP  	= 23;
//const int  STATE_MENU_ACC0_SETUP 	  	= 24;
//const int  STATE_MENU_ACC1_SETUP 	  	= 25;
//const int  STATE_MENU_PWM0_VIEW 	 	  = 26;
//const int  STATE_MENU_PWM1_VIEW 	  	= 27;
//const int  STATE_MENU_INTEST_VIEW     = 30;

//for Flash memory storage and m_iParameters array
const int  INITIATED = 0;
const int  THICKNESS = 1;
const int  VELOCITY = 2;
const int  DRUM_DIRECTION = 3;
const int  LENCOUNT = 4;
const int  MIDSTOP = 5;

const int  PARAMETERSNUMBER = 6;

const char MENUMARK = 62;

const int  GREETINGDELAY = 3000;//ms

const int  DELAY_KEYRELEASE = 500; //ms

const int  LIMIT = 1000; //total number of possible values for distance, speed and acceleration

const double DISTSTEP = 0.1; //mm
const double SPEEDSTEP = 0.1; //rpm

const double CaretAcceleration = 1; //mm/s2
const double CaretFastAcceleration = 50; //mm/s2
const double DrumFastAcceleration = 300; //mm/s2
const int DistToSensor = 0;
const double FindHomeTimeRestraint = 2; //sec
const int ImpsPerDrumRevolution = 800000;//imps
const double CaretDriverResolution = 859.0 / 264004;//82.0/25500;//mm/imps
const double DrumPerimeter = 3330.0;//(3330.0/(5.0*M_PI))* DISTSTEP * M_PI;
const double CaretReturnVelocity = 20;//mm/s
const double DrumReturnVelocity = 200;//m/s
/////////////////////////////// "################";
const char sReady[] = "Ready";
const char sPressMenu[] = "Press 'Menu'";
const char sForSetup[] = "for Setup";
const char sRunning[] = "Running";
const char sReturning[] = "Returning";
const char sDrum[] = "Drum";
const char sMode[] = "Mode";
const char sWrapping[] = "Wrapping";
const char sFinished[] = "Finished";
const char sPressOk[] = "Press 'OK'";
const char sToReturn[] = "To Return";
/////////////////////////////// "################";
const char sThickness[] = "Fiber Width";
const char sSpeed[] = "Rotation Speed";
const char sDrumDirection[] = "Drum Direction";
const char sCaretTest[] = "Caret Test";
const char sDrumTest[] = "Drum Test";
const char sError[] = "Error";
const char sErrorLimit0[] = "Limit 0";
const char sErrorLimit1[] = "Limit 1";
const char sErrorFault0[] = "Driver Fault 0";
const char sErrorFault1[] = "Driver Fault 1";
const char sErrorPositioning[] = "Start Position";
const char sErrorUnknown[] = "Unknown";
const char sWait[] = "Wait";
const char sWaitFor[] = "Wait for";
const char sForSensor1[] = "for Sensor #1";
const char sForSensor2[] = "for Sensor #2";
const char sForSensor3[] = "for Sensor #3";
const char sSensor1And3[] = "Sensor #1 & #3";
const char sMenu[] = "Menu";
/////////////////////////////// "################";

const char sPressUpDown[] = "Press Up/Down";
const char sOrOKMenu[] = "or OK/Menu";
const char sPWMTest[] = "PWM Test";
const char sSetup[] = "Setup";
const char sMm[] = " mm";
const char sMms[] = " mm/s";
const char sMms2[] = " mm/s2";
const char sRPM[] = " rpm";

//Error codes
const int  NO_ERROR = -1;
const int  ERROR_DRIVERFAULT0 = 0xA0;
const int  ERROR_DRIVERFAULT1 = 0xA1;
const int  ERROR_LIMIT0 = 0xB0;
const int  ERROR_LIMIT1 = 0xB1;
const int  ERROR_STARTPOS = 0xC0;
class CRotationDriver {
public:
	LCDIF* m_pLCD;

	CFlashMemory*   m_pFlashMemory;

	int            	m_iGlobalState;          // Global FSM
	int            	m_iPrevState;
	int            	m_iMenuState;
	int            	m_iSeconds;
	int            	m_iTemp;
	int            	m_iErrorCode;
	bool           	m_bNeedRedraw;
	bool			m_bNeedBlink;

	int            	m_iParameter[PARAMETERSNUMBER];

	bool			m_bCountSecondsToStart;
	int            	m_iStartSeconds;
	int            	m_iRestrictionSeconds;

	int 			m_iImpCount;
	int 			m_iImpCount2;
	int 			m_iCycleCount;
	int				m_iThreadLength;//m

	CDriverControl* m_pDrumDriver;
	CDriverControl* m_pCaretDriver;
	CTimer* 		m_pRestrictionTimer;

	bool			m_bStoppingMidWay;
	int 			m_iStopSignalCount;

public:
	CRotationDriver();
	~CRotationDriver();

	void DisplayUpdate(bool portState[]);
	void SaveParameters();
	void LoadParameters();

	bool GoHome(int TimeRestriction);//sec
	void GoHome();
	void Run();
	void Stop();
	void EmergencyStop();

	bool IsRunning();
	bool IsReturning();

	void RestrictionTimerHandler();

private:
	CRotationDriver(const CRotationDriver& c);
	CRotationDriver& operator=(const CRotationDriver& c);

};
#endif
