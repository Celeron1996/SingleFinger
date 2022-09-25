/**
  ******************************************************************************
  * @file     systick.h
  * @author   ZhuSL
  * @brief    系统时基
  * @version  V1.0.0  2021年3月11日
  ******************************************************************************
  */

#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm8s.h"


/**
  * @brief  系统时基初始化
  * @param  None
  * @retval None
  */
void Systick_Init(void);

/**
  * @brief  获取时基
  * @param  None
  * @retval None
  */
uint32_t Systick_GetCounter(void);

/**
  * @brief  时基中断回调函数
  * @param  None
  * @retval None
  */
void Systick_Int_Callback(void);


#endif

