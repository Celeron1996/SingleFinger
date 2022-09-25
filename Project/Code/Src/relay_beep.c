/**
  ******************************************************************************
  * @file     relay_beep.c
  * @author   ZhuSL
  * @brief    继电器和蜂鸣器驱动代码，因为继电器和蜂鸣器共用了电源
  * @version  V1.0.0  2021年3月24日
  ******************************************************************************
  */

#include "relay_beep.h"
#include "delay.h"

static FunctionalState Relay_State = DISABLE;
static FunctionalState Relay_Beep_Power_State = DISABLE;
volatile FunctionalState BeepFlag = DISABLE;


/**
  * @brief  初始化蜂鸣器和继电器
  * @param  None
  * @retval None
  */
void Relay_Beep_Init(void)
{

	/*初始化蜂鸣器,GPIO固定为PD4*/
//  BEEP_Cmd(DISABLE);
//  BEEP_Init(BEEP_FREQUENCY_2KHZ);
//    BEEP_DeInit();
//    BEEP_LSICalibrationConfig(128000);
//    BEEP_Init(BEEP_FREQUENCY_2KHZ);

  GPIO_Init(BEEP_GPIO_PORT, BEEP_GPIO_PIN, GPIO_MODE_OUT_PP_LOW_FAST);

  RELAY_INIT();

	RELAY_BEEP_POWER_INIT();
}


/**
  * @brief  继电器使能或失能
  * @param  State:状态
  *		#ENABLE
  *		#DISABLE
  * @retval None
  */
void Relay_Config(FunctionalState State)
{
	if(State == ENABLE)
	{
		RELAY_ENABLE();
		Relay_State = ENABLE;
	}
	else
	{
		RELAY_DISABLE();
		Relay_State = DISABLE;
	}
}


/**
  * @brief  获取继电器状态
  * @param  None
  * @retval 返回最新状态
  *		#ENABLE
  *		#DISABLE
  */
FunctionalState Relay_GetState(void)
{
	return Relay_State;
}


/**
  * @brief  继电器和蜂鸣器电源控制
  * @param  State:状态
  *		#ENABLE
  *		#DISABLE
  * @retval None
  */
void Relay_Beep_PowerConfig(FunctionalState State)
{
	if(State == ENABLE)
	{
		RELAY_BEEP_POWER_ENABLE();
		Relay_Beep_Power_State = ENABLE;
	}
	else
	{
		RELAY_BEEP_POWER_DISABLE();
		Relay_Beep_Power_State = DISABLE;
	}
}


/**
  * @brief  获取继电器状态
  * @param  None
  * @retval 返回最新状态
  *		#ENABLE
  *		#DISABLE
  */

FunctionalState Relay_Beep_GetPowerState(void)
{
	return Relay_Beep_Power_State;
}


/**
  * @brief  蜂鸣器嘀嘀嘀...
  * @param  Count:滴的次数
  *	@param	每次的持续时间
  * @retval None
  */
void Beep_dididi(uint8_t Count, uint16_t Time)
{
	while(Count--)
	{
		Beep_Config(ENABLE);
		Delay_Ms(Time);
		Beep_Config(DISABLE);
		if(Count)
		{
			Delay_Ms(Time);
		}
	}
}


/**
  * @brief  蜂鸣器
  * @param  None
  * @retval None
  */
void Beep_Tick_Int_Callback(void)
{
	if(BeepFlag == ENABLE)
	{
		GPIO_WriteReverse(BEEP_GPIO_PORT, BEEP_GPIO_PIN);
	}
}


/**
  * @brief  蜂鸣器使能失能
  * @param  State
  * @retval None
  */
void Beep_Config(FunctionalState State)
{
	BeepFlag = State;
}


/**
  * @brief  蜂鸣器睡眠
  * @param  None
  * @retval None
  */
void Beep_Sleep(void)
{
	Beep_Config(DISABLE);
	Delay_Ms(2);
	GPIO_Init(BEEP_GPIO_PORT, BEEP_GPIO_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
}


