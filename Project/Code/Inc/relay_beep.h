/**
  ******************************************************************************
  * @file     relay_beep.h
  * @author   ZhuSL
  * @brief    继电器和蜂鸣器驱动代码，因为继电器和蜂鸣器共用了电源
  * @version  V1.0.0  2021年3月24日
  ******************************************************************************
  */

#ifndef __RELAY_BEEP_H
#define __RELAY_BEEP_H


#include "stm8s.h"


#define RELAY_BEEP_POWER_GPIO_PORT	GPIOC
#define RELAY_BEEP_POWER_GPIO_PIN		GPIO_PIN_3

#define RELAY_BEEP_POWER_INIT()			GPIO_Init(RELAY_BEEP_POWER_GPIO_PORT, RELAY_BEEP_POWER_GPIO_PIN, GPIO_MODE_OUT_PP_LOW_FAST)
#define RELAY_BEEP_POWER_ENABLE()		GPIO_WriteHigh(RELAY_BEEP_POWER_GPIO_PORT, RELAY_BEEP_POWER_GPIO_PIN)
#define RELAY_BEEP_POWER_DISABLE()	GPIO_WriteLow(RELAY_BEEP_POWER_GPIO_PORT, RELAY_BEEP_POWER_GPIO_PIN)

#define RELAY_GPIO_PORT							GPIOD
#define RELAY_GPIO_PIN							GPIO_PIN_3
#define RELAY_INIT()								GPIO_Init(RELAY_GPIO_PORT, RELAY_GPIO_PIN, GPIO_MODE_OUT_PP_LOW_FAST)
#define RELAY_ENABLE()							GPIO_WriteHigh(RELAY_GPIO_PORT, RELAY_GPIO_PIN)
#define RELAY_DISABLE()							GPIO_WriteLow(RELAY_GPIO_PORT, RELAY_GPIO_PIN)
#define RELAY_TOGGLE()							GPIO_WriteReverse(RELAY_GPIO_PORT, RELAY_GPIO_PIN)


#define BEEP_GPIO_PORT							GPIOD
#define BEEP_GPIO_PIN								GPIO_PIN_4


/**
  * @brief  初始化蜂鸣器和继电器
  * @param  None
  * @retval None
  */
void Relay_Beep_Init(void);


/**
  * @brief  继电器使能或失能
  * @param  State:状态
  *		#ENABLE
  *		#DISABLE
  * @retval None
  */
void Relay_Config(FunctionalState State);


/**
  * @brief  获取继电器状态
  * @param  None
  * @retval 返回最新状态
  *		#ENABLE
  *		#DISABLE  
  */
FunctionalState Relay_GetState(void);


/**
  * @brief  继电器和蜂鸣器电源控制
  * @param  State:状态
  *		#ENABLE
  *		#DISABLE
  * @retval None
  */
void Relay_Beep_PowerConfig(FunctionalState State);


/**
  * @brief  获取继电器状态
  * @param  None
  * @retval 返回最新状态
  *		#ENABLE
  *		#DISABLE  
  */
FunctionalState Relay_Beep_GetPowerState(void);


/**
  * @brief  蜂鸣器嘀嘀嘀...
  * @param  Count:滴的次数
  *	@param	每次的持续时间
  * @retval None
  */
void Beep_dididi(uint8_t Count, uint16_t Time);


/**
  * @brief  蜂鸣器
  * @param  None
  * @retval None
  */
void Beep_Tick_Int_Callback(void);



/**
  * @brief  蜂鸣器使能失能
  * @param  State
  * @retval None
  */
void Beep_Config(FunctionalState State);


/**
  * @brief  蜂鸣器睡眠
  * @param  None
  * @retval None
  */
void Beep_Sleep(void);


#endif