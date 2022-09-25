/**
  ******************************************************************************
  * @file     relay_beep.c
  * @author   ZhuSL
  * @brief    继电器和蜂鸣器驱动代码，因为继电器和蜂鸣器共用了电源
  * @version  V1.0.0  2021年3月24日
  ******************************************************************************
  */

#include "relay.h"
#include "delay.h"


static FunctionalState RELAY_STATE = DISABLE;

/**
  * @brief  初始化蜂鸣器和继电器
  * @param  None
  * @retval None
  */
void Relay_Init(void)
{
  GPIO_Init(RELAY_ENABLE_GPIO_PORT, RELAY_ENABLE_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
  Relay_Config(DISABLE);
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
		RELAY_STATE = ENABLE;
	}
	else
	{
		RELAY_DISABLE();
		RELAY_STATE = DISABLE;
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
	return RELAY_STATE;
}

