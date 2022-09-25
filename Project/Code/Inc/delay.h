/**
  ******************************************************************************
  * @file     delay.h
  * @author   ZhuSL
  * @brief    延时
  * @version  V1.0.0  2021年3月14日
  ******************************************************************************
  */

#ifndef __DELAY_H
#define __DELAY_H

#include "stm8s.h"


#define DELAY_TIMER_PERIOD_VALUE	((uint16_t)65535)


/**
  * @brief  延时初始化
  * @param  None
  * @retval None
  */
void Delay_Init(void);

/**
  * @brief  毫秒级延时
  * @param  Time:延时时间
  * @retval None
  */
void Delay_Ms(uint16_t Time);

/**
  * @brief  微妙级延时
  * @param  Time:延时时间
  * @retval None
  */
void Delay_Us(uint16_t Time);

#endif