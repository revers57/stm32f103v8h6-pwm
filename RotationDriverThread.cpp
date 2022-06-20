#include "RotationDriverThread.h"

extern CRotationDriver RotationDriver;
extern CDriverControl DrumDriverCtrl;
extern CDriverControl CaretDriverCtrl;

int InitRotationDriverThread() {
	RotationDriver.m_pFlashMemory = &FlashMemory;
	RotationDriver.m_pLCD = &LCD;
	RotationDriver.m_pLCD->InitLowLevel();

	port_InDrumStartSensor.Init();
	port_InDrumStartSensor.ConfigInterrupt(CycleCounterHandler);//!!!
	//port_InLeftSensor1.Init();
	port_InCaretStartSensor.Init();
	port_InCaretStopSensor.Init();
	port_InCaretStopSensor.ConfigInterrupt(StopHandler);//!!!
	//port_InDriverFault0.Init();
	//port_InDriverFault1.Init();
	//port_InDriverFault0.ConfigInterrupt(Fault0Handler);
	//port_InDriverFault1.ConfigInterrupt(Fault1Handler);
	port_InButtonMenu.Init();
	port_InButtonOK.Init();
	port_InButtonUp.Init();
	port_InButtonDown.Init();
	port_OutDir0.Init();
	port_OutDir1.Init();
	port_OutRollerDown.Init();
	//port_OutDriveEnable1.Init();
	port_OutLED.Init();
	port_Step0.Init();
	port_Step1.Init();
	port_InButtonHome.Init();
	port_InButtonStart.Init();
	port_InButtonWrap.Init();
	//port_InButtonHome.ConfigInterrupt(GoHomeHandler);

	timerSystemTick.Init();
	PWMTimer0.Init();
	PWMTimer1.Init();

	RotationDriverThreadId = osThreadCreate(osThread(RotationDriverThread), NULL);

	if (!RotationDriverThreadId)
		return(-1);
	return(0);
}
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
void SetupDrivers()
{
	double DrumCaretDistRatio = DrumPerimeter / (double)(RotationDriver.m_iParameter[THICKNESS] * DISTSTEP);
	double DrumVelocity = RotationDriver.m_iParameter[VELOCITY] * SPEEDSTEP;
	DrumVelocity = (DrumVelocity * DrumPerimeter) / 60.0; //converting rpm to mm/sec
	double CaretVelocity = (DrumVelocity / DrumCaretDistRatio);
	double DrumAcceleration = CaretAcceleration * DrumCaretDistRatio;

	double DrumResolution = DrumPerimeter / ImpsPerDrumRevolution;
	double CaretResolution = CaretDriverResolution;

	DrumDriverCtrl.Init(DrumVelocity, DrumReturnVelocity, DrumAcceleration, DrumFastAcceleration, DrumResolution, RotationDriver.m_iParameter[DRUM_DIRECTION]);
	CaretDriverCtrl.Init(CaretVelocity, CaretReturnVelocity, CaretAcceleration, CaretFastAcceleration, CaretResolution, FORWARD);
}
void RotationDriverThread(void const* argument) {
	//load configuration
	RotationDriver.LoadParameters();
	if (RotationDriver.m_iParameter[INITIATED] != 1)
	{ //first run
		RotationDriver.m_iParameter[INITIATED] = 1;
		RotationDriver.m_iParameter[THICKNESS] = 400; // * 0.1 mm
		RotationDriver.m_iParameter[VELOCITY] = 50; // * 0.1 rpm
		RotationDriver.m_iParameter[DRUM_DIRECTION] = false; // * 0.1 rpm
		RotationDriver.m_iParameter[LENCOUNT] = 0;
		RotationDriver.m_iParameter[MIDSTOP] = 0;
		RotationDriver.SaveParameters();
	}
	if (RotationDriver.m_iParameter[MIDSTOP] < 0 || RotationDriver.m_iParameter[MIDSTOP] > 1)
		RotationDriver.m_iParameter[MIDSTOP] = 0;
	//for (int i = 1; i < PARAMETERSNUMBER; i++)
	//		if (RotationDriver.m_iParameter[i] < 0 || RotationDriver.m_iParameter[i] > LIMIT) RotationDriver.m_iParameter[i] = 20; //default
	//RotationDriver.SaveParameters();

	SetupDrivers();

	timerSystemTick.Start();

	port_OutLED.Set(ON);

	while (1) { //Main Loop
		switch (RotationDriver.m_iGlobalState) {
		case STATE_INIT:
			RotationDriver.DisplayUpdate(abInputState);
			DelayMs(GREETINGDELAY);
			RotationDriver.m_iGlobalState = STATE_STANDBY;
			RotationDriver.m_iMenuState = STATE_MENU_THICKNESS_SELECT;
			RotationDriver.m_iSeconds = 0;
			RotationDriver.m_bNeedRedraw = true;
			RotationDriver.m_bNeedBlink = false;
			RotationDriver.m_iErrorCode = NO_ERROR;
			//port_OutRollerDown.Set(true);
			//port_OutDriveEnable1.Set(true);
			RotationDriver.m_iCycleCount = 0;
			RotationDriver.m_iThreadLength = 0;
			RotationDriver.m_iImpCount = 0;
			RotationDriver.m_iImpCount2 = 0;
			break;
		case STATE_STANDBY:
			osMutexWait(xGlobalMutexId, osWaitForever);
			RotationDriver.m_iImpCount2 = 0;
			if (RotationDriver.m_iGlobalState != STATE_ERROR)
			{
				RotationDriver.m_iPrevState = STATE_STANDBY;
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, &port_InButtonMenu);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_RUNNING, &port_InButtonStart);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_RETURNING, &port_InButtonHome);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_DRUM_WRAP, &port_InButtonWrap);
				if (RotationDriver.m_iGlobalState == STATE_RUNNING)
				{
					RotationDriver.Run();
					/*else
					{
							RotationDriver.EmergencyStop();
							RotationDriver.m_iGlobalState = STATE_ERROR;
							RotationDriver.m_iErrorCode = ERROR_STARTPOS;
							RotationDriver.m_bNeedRedraw = true;
					}*/
				}
				if (RotationDriver.m_iGlobalState == STATE_RETURNING)
				{
					//RotationDriver.GoHome();
					RotationDriver.m_iGlobalState = STATE_CARET_RETURNING;
					CaretDriverCtrl.Return(false);
				}
				if (RotationDriver.m_iGlobalState == STATE_DRUM_WRAP)
				{
					DrumDriverCtrl.Step(RotationDriver.m_iParameter[DRUM_DIRECTION], ImpsPerDrumRevolution, false);
				}
			}
			osMutexRelease(xGlobalMutexId);
			break;
		case STATE_MENU:
			switch (RotationDriver.m_iMenuState) {
			case STATE_MENU_THICKNESS_SELECT:
				SwitchStateWithButton(RotationDriver.m_iGlobalState, RotationDriver.m_iPrevState, &port_InButtonMenu);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_THICKNESS_SETUP, &port_InButtonOK);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_THICKNESS_SETUP, &port_InButtonUp);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_VELOCITY_SELECT, &port_InButtonDown);
				break;
			case STATE_MENU_VELOCITY_SELECT:
				SwitchStateWithButton(RotationDriver.m_iGlobalState, RotationDriver.m_iPrevState, &port_InButtonMenu);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_VELOCITY_SETUP, &port_InButtonOK);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_THICKNESS_SELECT, &port_InButtonUp);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_DRUMDIRECTION_SELECT, &port_InButtonDown);
				break;
			case STATE_MENU_DRUMDIRECTION_SELECT:
				SwitchStateWithButton(RotationDriver.m_iGlobalState, RotationDriver.m_iPrevState, &port_InButtonMenu);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_DRUMDIRECTION_SETUP, &port_InButtonOK);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_VELOCITY_SELECT, &port_InButtonUp);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_MODE_SELECT, &port_InButtonDown);
				break;
			case STATE_MENU_MODE_SELECT:
				SwitchStateWithButton(RotationDriver.m_iGlobalState, RotationDriver.m_iPrevState, &port_InButtonMenu);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_MODE_SETUP, &port_InButtonOK);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_DRUMDIRECTION_SETUP, &port_InButtonUp);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_CARETTEST_SELECT, &port_InButtonDown);
				break;
			case STATE_MENU_CARETTEST_SELECT:
				SwitchStateWithButton(RotationDriver.m_iGlobalState, RotationDriver.m_iPrevState, &port_InButtonMenu);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_CARETTEST_SETUP, &port_InButtonOK);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_MODE_SELECT, &port_InButtonUp);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_DRUMTEST_SELECT, &port_InButtonDown);
				break;
			case STATE_MENU_DRUMTEST_SELECT:
				SwitchStateWithButton(RotationDriver.m_iGlobalState, RotationDriver.m_iPrevState, &port_InButtonMenu);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_DRUMTEST_SETUP, &port_InButtonOK);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_CARETTEST_SELECT, &port_InButtonUp);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_THICKNESS_SETUP, &port_InButtonDown);
				break;
			case STATE_MENU_THICKNESS_SETUP:
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_THICKNESS_SELECT, &port_InButtonMenu);
				if (CheckButton(&port_InButtonUp) && RotationDriver.m_iTemp < LIMIT)
				{
					RotationDriver.m_iTemp++;
					RotationDriver.m_bNeedRedraw = true;
				}
				if (CheckButton(&port_InButtonDown) && RotationDriver.m_iTemp > 0)
				{
					RotationDriver.m_iTemp--;
					RotationDriver.m_bNeedRedraw = true;
				}
				if (CheckButton(&port_InButtonOK)) {
					RotationDriver.m_iParameter[THICKNESS] = RotationDriver.m_iTemp;
					RotationDriver.SaveParameters();
					SetupDrivers();
					RotationDriver.m_iMenuState = STATE_MENU_THICKNESS_SELECT;
				}
				break;
			case STATE_MENU_VELOCITY_SETUP:
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_VELOCITY_SELECT, &port_InButtonMenu);
				if (CheckButton(&port_InButtonUp) && RotationDriver.m_iTemp < LIMIT)
				{
					RotationDriver.m_iTemp++;
					RotationDriver.m_bNeedRedraw = true;
				}
				if (CheckButton(&port_InButtonDown) && RotationDriver.m_iTemp > 0)
				{
					RotationDriver.m_iTemp--;
					RotationDriver.m_bNeedRedraw = true;
				}
				if (CheckButton(&port_InButtonOK)) {
					RotationDriver.m_iParameter[VELOCITY] = RotationDriver.m_iTemp;
					RotationDriver.SaveParameters();
					SetupDrivers();
					RotationDriver.m_iMenuState = STATE_MENU_VELOCITY_SELECT;
				}
				break;
			case STATE_MENU_DRUMDIRECTION_SETUP:
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_DRUMDIRECTION_SELECT, &port_InButtonMenu);
				if (CheckButton(&port_InButtonUp) && RotationDriver.m_iTemp < 1)
				{
					RotationDriver.m_iTemp++;
					RotationDriver.m_bNeedRedraw = true;
				}
				if (CheckButton(&port_InButtonDown) && RotationDriver.m_iTemp > 0)
				{
					RotationDriver.m_iTemp--;
					RotationDriver.m_bNeedRedraw = true;
				}
				if (CheckButton(&port_InButtonOK)) {
					RotationDriver.m_iParameter[DRUM_DIRECTION] = RotationDriver.m_iTemp;
					RotationDriver.SaveParameters();
					SetupDrivers();
					RotationDriver.m_iMenuState = STATE_MENU_DRUMDIRECTION_SELECT;
				}
				break;
			case STATE_MENU_MODE_SETUP:
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_MODE_SELECT, &port_InButtonMenu);
				if (CheckButton(&port_InButtonUp) && RotationDriver.m_iTemp < 1)
				{
					RotationDriver.m_iTemp++;
					RotationDriver.m_bNeedRedraw = true;
				}
				if (CheckButton(&port_InButtonDown) && RotationDriver.m_iTemp > 0)
				{
					RotationDriver.m_iTemp--;
					RotationDriver.m_bNeedRedraw = true;
				}
				if (CheckButton(&port_InButtonOK)) {
					RotationDriver.m_iParameter[MIDSTOP] = RotationDriver.m_iTemp;
					RotationDriver.SaveParameters();
					RotationDriver.m_iMenuState = STATE_MENU_MODE_SELECT;
				}
				break;
			case STATE_MENU_DRUMTEST_SETUP:
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_DRUMTEST_SELECT, &port_InButtonMenu);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_DRUMTEST_SELECT, &port_InButtonOK);

				if (DrumDriverCtrl.m_iState == STATE_TEST_RUNNING)
				{
					if (!port_InButtonUp.Get() && !port_InButtonDown.Get())
						DrumDriverCtrl.Stop();
					if (port_InButtonUp.Get() && port_InButtonDown.Get())
						DrumDriverCtrl.Stop();
				}

				if (!port_InButtonUp.Get() && DrumDriverCtrl.m_iState != STATE_TEST_RUNNING)
					DrumDriverCtrl.TestRun(FORWARD);
				else if (!port_InButtonDown.Get() && DrumDriverCtrl.m_iState != STATE_TEST_RUNNING)
					DrumDriverCtrl.TestRun(BACKWARDS);
				break;
			case STATE_MENU_CARETTEST_SETUP:
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_CARETTEST_SELECT, &port_InButtonMenu);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, RotationDriver.m_iMenuState, STATE_MENU_CARETTEST_SELECT, &port_InButtonOK);
				if (CaretDriverCtrl.m_iState == STATE_TEST_RUNNING)
				{
					if (!port_InButtonUp.Get() && !port_InButtonDown.Get())
						CaretDriverCtrl.Stop();
					if (port_InButtonUp.Get() && port_InButtonDown.Get())
						CaretDriverCtrl.Stop();
				}

				if (!port_InButtonUp.Get() && CaretDriverCtrl.m_iState != STATE_TEST_RUNNING)
					CaretDriverCtrl.TestRun(FORWARD);
				else if (!port_InButtonDown.Get() && CaretDriverCtrl.m_iState != STATE_TEST_RUNNING)
					CaretDriverCtrl.TestRun(BACKWARDS);
				break;
			}
			break;
		case STATE_RUNNING:
			SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_STANDBY, &port_InButtonStart);
			if (RotationDriver.m_iGlobalState == STATE_STANDBY)
			{
				RotationDriver.Stop();
				while (RotationDriver.IsRunning());
			}
			else if (!RotationDriver.IsRunning() && RotationDriver.m_iGlobalState != STATE_ERROR)
			{
				if (RotationDriver.m_bStoppingMidWay && RotationDriver.m_iStopSignalCount == 1)
				{
					RotationDriver.m_iGlobalState = STATE_SKIP_DIST;
					CaretDriverCtrl.RunSingle(FORWARD);
				}
				else
					RotationDriver.m_iGlobalState = STATE_FINISHED;
			}
			break;
		case STATE_FINISHED:
			osMutexWait(xGlobalMutexId, osWaitForever);
			if (RotationDriver.m_iGlobalState != STATE_ERROR)
			{
				RotationDriver.m_iPrevState = STATE_FINISHED;
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_MENU, &port_InButtonMenu);
				SwitchStateWithButton(RotationDriver.m_iGlobalState, STATE_RETURNING, &port_InButtonHome);
				if (RotationDriver.m_iGlobalState == STATE_RETURNING)
				{
					//RotationDriver.GoHome();
					RotationDriver.m_iGlobalState = STATE_CARET_RETURNING;
					CaretDriverCtrl.Return(false);
				}
				RotationDriver.m_iStopSignalCount = 0;
			}
			osMutexRelease(xGlobalMutexId);
			break;
		case STATE_RETURNING:
			if (!RotationDriver.IsReturning() && RotationDriver.m_iGlobalState != STATE_ERROR)
			{
				RotationDriver.m_iGlobalState = STATE_STANDBY;
			}
			break;
		case STATE_CARET_RETURNING:
			if (!RotationDriver.IsReturning() && RotationDriver.m_iGlobalState != STATE_ERROR)
			{
				RotationDriver.m_iGlobalState = STATE_DRUM_RETURNING;
				DrumDriverCtrl.Return(false);
			}
			break;
		case STATE_DRUM_RETURNING:
			if (!RotationDriver.IsReturning() && RotationDriver.m_iGlobalState != STATE_ERROR)
			{
				RotationDriver.m_iGlobalState = STATE_STANDBY;
				RotationDriver.m_iParameter[LENCOUNT] += RotationDriver.m_iThreadLength;
				RotationDriver.SaveParameters();
				RotationDriver.m_iCycleCount = 0;
				RotationDriver.m_iThreadLength = 0;
				RotationDriver.m_iImpCount = 0;
				RotationDriver.m_iStopSignalCount = 0;
			}
			break;
		case STATE_DRUM_WRAP:
			if (!RotationDriver.IsRunning() && RotationDriver.m_iGlobalState != STATE_ERROR)
			{
				RotationDriver.m_iGlobalState = STATE_STANDBY;
			}
			break;
		case STATE_SKIP_DIST:
			if (!RotationDriver.IsRunning() && RotationDriver.m_iGlobalState != STATE_ERROR)
			{
				RotationDriver.m_iGlobalState = STATE_RUNNING;
				RotationDriver.Run();
			}
			break;
		case STATE_ERROR:
			//port_OutRollerDown.Set(false);
			//port_OutDriveEnable1.Set(false);
			break;
		}

		//update LCD
		if (RotationDriver.m_bNeedRedraw) {
			RotationDriver.DisplayUpdate(abInputState);
		}
	}
}

void SystemTickHandler() {
	if (RotationDriver.m_iSeconds < 59)
		RotationDriver.m_iSeconds++;
	else
		RotationDriver.m_iSeconds = 0;
	RotationDriver.m_bNeedRedraw = true;

	if (RotationDriver.m_bCountSecondsToStart)
	{
		RotationDriver.m_iStartSeconds++;
		if (RotationDriver.m_iStartSeconds == RotationDriver.m_iRestrictionSeconds)
		{
			RotationDriver.m_bCountSecondsToStart = false;
		}
	}
}



void SwitchStateWithButton(int& state, int targetState, CPort* port) {
	if (!port->Get()) {
		DelayMs(DELAY_KEYRELEASE);
		while (!port->Get());
		state = targetState;
	}
}

void SwitchStateWithButton(int& state, int targetState, int& menuState, int targetMenuState, CPort* port) {
	if (!port->Get()) {
		DelayMs(DELAY_KEYRELEASE);
		while (!port->Get());
		state = targetState;
		menuState = targetMenuState;
		switch (targetMenuState) {
		case STATE_MENU_VELOCITY_SETUP:  RotationDriver.m_iTemp = RotationDriver.m_iParameter[VELOCITY];  break;
		case STATE_MENU_THICKNESS_SETUP: RotationDriver.m_iTemp = RotationDriver.m_iParameter[THICKNESS]; break;
		case STATE_MENU_DRUMDIRECTION_SETUP: RotationDriver.m_iTemp = RotationDriver.m_iParameter[DRUM_DIRECTION]; break;
		case STATE_MENU_MODE_SETUP: RotationDriver.m_iTemp = RotationDriver.m_iParameter[MIDSTOP]; break;
		}
	}
}

bool CheckButton(CPort* port) {
	if (!port->Get()) {
		DelayMs(DELAY_KEYRELEASE);
		while (!port->Get());
		return true;
	}
	else
		return false;
}