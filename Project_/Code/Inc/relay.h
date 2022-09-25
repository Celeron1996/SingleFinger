/**
  ******************************************************************************
  * @file     relay.h
  * @author   ZhuSL
  * @brief    继电器驱动代码，因为继电器和蜂鸣器共用了电源
  * @version  V1.0.0  2021年3月24日
  ******************************************************************************
  */

#ifndef __RELAY_H
#define __RELAY_H


#include "stm8l15x.h"


#define RELAY_ENABLE_GPIO_PORT      GPIOD
#define RELAY_ENABLE_GPIO_PIN       GPIO_Pin_1

#define RELAY_ENABLE()              GPIO_SetBits(RELAY_ENABLE_GPIO_PORT, RELAY_ENABLE_GPIO_PIN)
#define RELAY_DISABLE()             GPIO_ResetBits(RELAY_ENABLE_GPIO_PORT, RELAY_ENABLE_GPIO_PIN)


/**
  * @brief  初始化继电器
  * @param  None
  * @retval None
  */
void Relay_Init(void);


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

#endif