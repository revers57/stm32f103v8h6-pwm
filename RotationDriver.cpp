#include "RotationDriver.h"

extern CDriverControl DrumDriverCtrl;
extern CDriverControl CaretDriverCtrl;

void CRotationDriver::RestrictionTimerHandler()
{
	
}
CRotationDriver::CRotationDriver() {
				m_iGlobalState  = STATE_INIT;
				m_iMenuState    = STATE_MENU_THICKNESS_SELECT;
				//m_iDriverState0 = STATE_STANDBY;
				//m_iDriverState1 = STATE_STANDBY;
				m_iSeconds      = 0;
				m_bNeedRedraw   = true;
				m_bNeedBlink    = false;
				m_iErrorCode 		= NO_ERROR;
				memset(m_iParameter, 0, PARAMETERSNUMBER*4); 
}

CRotationDriver::~CRotationDriver() {};
	
void CRotationDriver::DisplayUpdate(bool portState[]) {
		switch (m_iGlobalState) {
				case STATE_INIT:
						*m_pLCD << clr;
						*m_pLCD << font(Font8x16)
						<< text_mode(true, true, Center)
						<< "BIGZEE" << line(0)
						<< "PWM Driver" << line(2)
						<< "Version 1.0" << line(6)
						<< update();
						break;
				case STATE_STANDBY:
						*m_pLCD << sReady << line(0)
										<< "Cycles: " << m_iCycleCount << line(2)
										<< "C: " << m_iThreadLength << " m"<< line(4)
										<< "G: " << (int)(m_iParameter[LENCOUNT]) << " m" << line(6)
										<< update();
						break;
				case STATE_RUNNING:
						*m_pLCD << sRunning << line(0)
						        << "Cycles: " << m_iCycleCount << line(2)
										<< "C: " << m_iThreadLength << " m"<< line(4)
										<< "G: " << (int)(m_iParameter[LENCOUNT]) << " m" << line(6)
										<< update();
						break;
				case STATE_RETURNING:
						*m_pLCD << sReturning << line(0)
										<< sWaitFor << line(4)
										<< sSensor1And3 << line(6)
										<< update();
						break;
				case STATE_DRUM_RETURNING:
						*m_pLCD << sReturning << line(0)
										<< sWait << line(4)
										<< sForSensor3 << line(6)
										<< update();
						break;
				case STATE_CARET_RETURNING:
						*m_pLCD << sReturning << line(0)
										<< sWait << line(4)
										<< sForSensor1 << line(6)
										<< update();
						break;
				case STATE_DRUM_WRAP:
						*m_pLCD << sDrum << line(0)
										<< sWrapping << line(4)
										<< update();
						break;
				case STATE_FINISHED:
						*m_pLCD << sFinished << line(0)
										<< sPressOk << line(4)
										<< sToReturn << line(6)
										<< update();
						break;
				case STATE_MENU:
						switch (m_iMenuState) {
								case STATE_MENU_THICKNESS_SELECT:
										*m_pLCD     << sMenu << line(0)
										<< MENUMARK << sThickness << line(2, Left)
										<< ' '      << sSpeed << line(4, Left)
										<< ' '      << sDrumDirection << line(6, Left)
										<< update();
										break;
								case STATE_MENU_VELOCITY_SELECT:
										*m_pLCD << sMenu << line(0)
										<< ' '      << sThickness << line(2, Left)
										<< MENUMARK << sSpeed << line(4, Left)
										<< ' '      << sDrumDirection << line(6, Left)
										<< update();
										break;
								case STATE_MENU_DRUMDIRECTION_SELECT:
										*m_pLCD << sMenu << line(0)
										<< ' '      << sSpeed << line(2, Left)
										<< MENUMARK << sDrumDirection << line(4, Left)
										<< ' '      << sMode << line(6, Left)
										<< update();
										break;
								case STATE_MENU_MODE_SELECT:
										*m_pLCD << sMenu << line(0)
										<< ' '      << sDrumDirection << line(2, Left)
										<< MENUMARK << sMode << line(4, Left)
										<< ' '      << sCaretTest << line(6, Left)
										<< update();
										break;
								case STATE_MENU_CARETTEST_SELECT:
										*m_pLCD << sMenu << line(0)
										<< ' '      << sMode << line(2, Left)
										<< MENUMARK << sCaretTest << line(4, Left)
										<< ' '      << sDrumTest << line(6, Left)
										<< update();
										break;
								case STATE_MENU_DRUMTEST_SELECT:
										*m_pLCD << sMenu << line(0)
										<< ' '      << sDrumDirection << line(2, Left)
										<< ' '      << sCaretTest << line(4, Left)
										<< MENUMARK << sDrumTest << line(6, Left)
										<< update();
										break;
								case STATE_MENU_THICKNESS_SETUP:
										*m_pLCD << precision(1);
										*m_pLCD << "Set Fiber Width" << line(0)
														<< (double)(DISTSTEP * m_iTemp) << sMm << line(2)
														<< sPressUpDown << line(4)
														<< sOrOKMenu << line(6)
														<< update();
										break;
								case STATE_MENU_VELOCITY_SETUP:
										*m_pLCD << precision(1);
										*m_pLCD << "Set Drum Speed" << line(0)
														<< (double)(SPEEDSTEP * m_iTemp) << sRPM << line(2)
														<< sPressUpDown << line(4)
														<< sOrOKMenu << line(6)
														<< update();
										break;
								case STATE_MENU_DRUMDIRECTION_SETUP:
										*m_pLCD << precision(1);
										*m_pLCD << "Set Drum Dir" << line(0)
														<< (m_iTemp ? "Backwards" : "Forward") << line(2)
														<< sPressUpDown << line(4)
														<< sOrOKMenu << line(6)
														<< update();
										break;
								case STATE_MENU_MODE_SETUP:
										*m_pLCD << precision(1);
										*m_pLCD << "Set Mode" << line(0)
														<< (m_iTemp ? "Stop Mid Way" : "No Stop") << line(2)
														<< sPressUpDown << line(4)
														<< sOrOKMenu << line(6)
														<< update();
										break;
								case STATE_MENU_CARETTEST_SETUP:
										*m_pLCD << "Caret Test" << line(0)
														<< sPressUpDown << line(4)
														<< sOrOKMenu << line(6)
														<< update();
										break;
								case STATE_MENU_DRUMTEST_SETUP:
										*m_pLCD << "Drum Test" << line(0)
														<< sPressUpDown << line(4)
														<< sOrOKMenu << line(6)
														<< update();
										break;
							
						}
						break;
				case STATE_ERROR:
						*m_pLCD << sError << center(0);
						switch (m_iErrorCode)
						{
							case ERROR_DRIVERFAULT0:
								*m_pLCD << sErrorFault0;
								break;
							case ERROR_DRIVERFAULT1:
								*m_pLCD << sErrorFault1;
								break;
							case ERROR_LIMIT0:
								*m_pLCD << sErrorLimit0;
								break;
							case ERROR_LIMIT1:
								*m_pLCD << sErrorLimit1;
								break;
							case ERROR_STARTPOS:
								*m_pLCD << sErrorPositioning;
								break;
							default:
								*m_pLCD << sErrorUnknown;
								break;
						}
						*m_pLCD << center(4) << update();
						break;
		}
		m_bNeedRedraw = false;
}

void CRotationDriver::SaveParameters()
{
		m_pFlashMemory->ErasePage0();
		m_pFlashMemory->StoreValues(0, m_iParameter, PARAMETERSNUMBER);
}

void CRotationDriver::LoadParameters()
{
		memset(m_iParameter, 0, 4*PARAMETERSNUMBER);
		m_pFlashMemory->RecallValues(0, m_iParameter, PARAMETERSNUMBER);
}

bool CRotationDriver::GoHome(int TimeRestriction)
{
		CaretDriverCtrl.Return(BACKWARDS);
		m_iStartSeconds = 0;
		m_iRestrictionSeconds = TimeRestriction;
		m_bCountSecondsToStart = true;
		while (m_bCountSecondsToStart);
		if (CaretDriverCtrl.m_iState == STATE_STANDBY)
		{
			//int CaretReturnDist = CaretDriverCtrl.m_iDistAfterSensor + DistToSensor;
			//CaretDriverCtrl.Step(FORWARD, CaretReturnDist);
			return true;
		}
		return false;
};
void CRotationDriver::GoHome()
{
		DrumDriverCtrl.Return(!m_iParameter[DRUM_DIRECTION]);
		
		CaretDriverCtrl.Return(BACKWARDS);
		
	
		//int DrumReturnDist = CaretDriverCtrl.m_iDistAfterSensor + DistToSensor;
		//DrumDriverCtrl.Step(FORWARD, DrumReturnDist);
	
		//int CaretReturnDist = CaretDriverCtrl.m_iDistAfterSensor + DistToSensor;
		//CaretDriverCtrl.Step(FORWARD, CaretReturnDist);
};
void CRotationDriver::Run()
{
	CaretDriverCtrl.Run(FORWARD);
	DrumDriverCtrl.Run(m_iParameter[DRUM_DIRECTION]);
};
bool CRotationDriver::IsRunning()
{
	if (CaretDriverCtrl.m_iState == STATE_RUNNING || DrumDriverCtrl.m_iState == STATE_RUNNING)
	{
		return true;
	}
	return false;
};
bool CRotationDriver::IsReturning()
{
	if ((CaretDriverCtrl.m_iState == STATE_RETURNING) || (DrumDriverCtrl.m_iState == STATE_RETURNING) ||
		(CaretDriverCtrl.m_iState == STATE_LEFT_SENSOR_DRIVEOFF) || 
		(DrumDriverCtrl.m_iState == STATE_LEFT_SENSOR_DRIVEOFF))
	{
		return true;
	}
	return false;
};
void CRotationDriver::Stop()
{
	if (CaretDriverCtrl.m_iState == STATE_RUNNING)
	{
		CaretDriverCtrl.Stop(false);
		DrumDriverCtrl.Stop(false);
	}
};
void CRotationDriver::EmergencyStop()
{
		CaretDriverCtrl.Stop(false);
		DrumDriverCtrl.Stop(false);
};