/**
  ******************************************************************************
  * @file     relay_beep.h
  * @author   ZhuSL
  * @brief    继电器和蜂鸣器驱动代码，因为继电器和蜂鸣器共用了电源
  * @version  V1.0.0  2021年3月24日
  ******************************************************************************
  */

#ifndef __BEEP_H
#define __BEEP_H


#include "stm8l15x.h"


#define BEEP_GPIO_PORT    GPIOD
#define BEEP_GPIO_PIN     GPIO_Pin_0


/**
  * @brief  初始化蜂鸣器
  * @param  None
  * @retval None
  */
void Beep_Init(void);


/**
  * @brief  deinit
  * @param  None
  * @retval None
  */
void Beep_DeInit(void);


/**
  * @brief  蜂鸣器嘀嘀嘀...
  * @param  Count:滴的次数
  *	@param	每次的持续时间
  * @retval None
  */
void Beep_dididi(uint8_t Count, uint16_t Time);


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


/**
  * @brief  beep wakeup
  * @param  None
  * @retval None
  */
void Beep_Wakeup(void);


#endif