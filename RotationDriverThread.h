#ifndef __RotationDriver_THREAD_H
#define __RotationDriver_THREAD_H

#include "cmsis_os.h"
#include "Port.h"
#include "RotationDriver.h"
#include "SysTick.h"
#include "Timer.h"
#include "LCDIF.h"
#include "main.h"

#define INVERSION true

static CFlashMemory FlashMemory;

static LCDIF LCD(Screen128x64);

void SystemTickHandler();

static CTimer timerSystemTick(3, 1, SystemTickHandler, 1.0);

static bool abInputState[6];

int  InitRotationDriverThread(void);
void RotationDriverThread(void const *argument);

void SwitchStateWithButton (int &state, int targetState, CPort* port);
void SwitchStateWithInput  (int &state, int targetState, CPort* port, bool inversion = false);
void WaitForInput (CPort* port, bool inversion = false); //by PMA on 08/10/2020
void SwitchStateWithButton (int &state, int targetState, int &menuState, int targetMenuState, CPort* port);
bool CheckButton (CPort* port); //by PMA on 30/09/2020
void TestStageOutput (CPort* port, bool complete = false);

static osThreadId RotationDriverThreadId;
osThreadDef(RotationDriverThread, osPriorityNormal, 1, 0);

#endif