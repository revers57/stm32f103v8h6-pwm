
#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f10x.h"                  // Device header
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "RTE_Components.h"             // Component selection

#include "RotationDriver.h"
#include "DriverControl.h"
#include "Port.h"
#include "Timer.h"
#include "SysTick.h"
#include "CANProtocol.h"

extern osMutexId  xGlobalMutexId;

extern int InitRotationDriverThread();

//Pins
const uint32_t GPIO_PIN_0  = 0x0000;  /* Pin 0 selected    */
const uint32_t GPIO_PIN_1  = 0x0001;  /* Pin 1 selected    */
const uint32_t GPIO_PIN_2  = 0x0002;  /* Pin 2 selected    */
const uint32_t GPIO_PIN_3  = 0x0003;  /* Pin 3 selected    */
const uint32_t GPIO_PIN_4  = 0x0004;  /* Pin 4 selected    */
const uint32_t GPIO_PIN_5  = 0x0005;  /* Pin 5 selected    */
const uint32_t GPIO_PIN_6  = 0x0006;  /* Pin 6 selected    */
const uint32_t GPIO_PIN_7  = 0x0007;  /* Pin 7 selected    */
const uint32_t GPIO_PIN_8  = 0x0008;  /* Pin 8 selected    */
const uint32_t GPIO_PIN_9  = 0x0009;  /* Pin 9 selected    */
const uint32_t GPIO_PIN_10 = 0x000A;  /* Pin 10 selected   */
const uint32_t GPIO_PIN_11 = 0x000B;  /* Pin 11 selected   */
const uint32_t GPIO_PIN_12 = 0x000C;  /* Pin 12 selected   */
const uint32_t GPIO_PIN_13 = 0x000D;  /* Pin 13 selected   */
const uint32_t GPIO_PIN_14 = 0x000E;  /* Pin 14 selected   */
const uint32_t GPIO_PIN_15 = 0x000F;  /* Pin 15 selected   */

//Inputs
const auto Sensor0_Pin    = GPIO_PIN_0;
const auto Sensor0_Port   = GPIOC;
const auto Sensor1_Pin   = GPIO_PIN_1;
const auto Sensor1_Port  = GPIOC;
const auto Sensor2_Pin    = GPIO_PIN_2;
const auto Sensor2_Port   = GPIOC;
const auto Sensor3_Pin   = GPIO_PIN_3;
const auto Sensor3_Port  = GPIOC;
const auto DriverFault0_Pin   = GPIO_PIN_9;
const auto DriverFault0_Port  = GPIOE;
const auto DriverFault1_Pin   = GPIO_PIN_13;
const auto DriverFault1_Port  = GPIOE;

//Buttons
const auto ButtonMenu_Pin     = GPIO_PIN_14;
const auto ButtonMenu_Port    = GPIOC;
const auto ButtonOK_Pin       = GPIO_PIN_15;
const auto ButtonOK_Port      = GPIOC;
const auto ButtonUp_Pin       = GPIO_PIN_12;
const auto ButtonUp_Port      = GPIOC;
const auto ButtonDown_Pin     = GPIO_PIN_13;
const auto ButtonDown_Port    = GPIOC;

//Outputs
const auto Step0_Pin          = GPIO_PIN_0;
const auto Step0_Port         = GPIOA;
const auto Step1_Pin          = GPIO_PIN_8;
const auto Step1_Port         = GPIOA;
const auto Dir0_Pin           = GPIO_PIN_1;
const auto Dir0_Port          = GPIOA;
const auto Dir1_Pin           = GPIO_PIN_9;
const auto Dir1_Port          = GPIOA;
const auto DriveEnable0_Pin   = GPIO_PIN_8;
const auto DriveEnable0_Port  = GPIOD;
const auto DriveEnable1_Pin   = GPIO_PIN_12;
const auto DriveEnable1_Port  = GPIOD;

//LED
const auto LEDReady_Pin       = GPIO_PIN_3;
const auto LEDReady_Port      = GPIOA;

//Inputs
static CPort port_InDrumStartSensor  	(Sensor3_Port,   Sensor3_Pin,  IN);

static CPort port_InCaretStartSensor 	(Sensor1_Port, 	 Sensor1_Pin,  IN);
static CPort port_InCaretStopSensor 	(Sensor2_Port, 	 Sensor2_Pin,  IN);
//static CPort port_InDriverFault0 			(DriverFault0_Port, DriverFault0_Pin,  IN);
//static CPort port_InDriverFault1 			(DriverFault1_Port, DriverFault1_Pin,  IN);

//Buttons
static CPort port_InButtonMenu   (ButtonMenu_Port,   ButtonMenu_Pin,    IN);
static CPort port_InButtonOK     (ButtonOK_Port,     ButtonOK_Pin,      IN);
static CPort port_InButtonUp     (ButtonUp_Port,     ButtonUp_Pin,      IN);
static CPort port_InButtonDown   (ButtonDown_Port,   ButtonDown_Pin,    IN);
static CPort port_InButtonHome   (Sensor0_Port,      Sensor0_Pin,       IN);
static CPort port_InButtonStart  (DriverFault0_Port, DriverFault0_Pin,  IN);
static CPort port_InButtonWrap 	 (DriverFault1_Port, DriverFault1_Pin,  IN);

//Outputs
static CPort port_Step0        	 (Step0_Port,        Step0_Pin,         OUT, true);
static CPort port_Step1        	 (Step1_Port,        Step1_Pin,         OUT, true);
static CPort port_OutDir0        (Dir0_Port,         Dir0_Pin,          OUT);
static CPort port_OutDir1        (Dir1_Port,         Dir1_Pin,          OUT);
static CPort port_OutRollerDown	 (DriveEnable0_Port, DriveEnable0_Pin,  OUT);
//static CPort port_OutDriveEnable1(DriveEnable1_Port, DriveEnable1_Pin,  OUT);

//LED
static CPort port_OutLED         (LEDReady_Port,     LEDReady_Pin,      OUT);

void SysInit (void);

void TimerHandler0();
void TimerHandler1();
void RestrictionTimerHandler();
void StopHandler();
void Fault0Handler();
void Fault1Handler();
void GoHomeHandler();
void CycleCounterHandler();

static CPWMTimer PWMTimer1(1, 1, 5, 2000000, TimerHandler1);
static CPWMTimer PWMTimer0(2, 1, 5, 2000000, TimerHandler0);


#endif
