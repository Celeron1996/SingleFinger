/**
  ******************************************************************************
  * @file     lowpower.h
  * @author   ZhuSL
  * @brief    低功耗实现
  * @version  V1.0.0  2021年3月24日
  ******************************************************************************
  */

#ifndef __LOWPOWER_H
#define __LOWPOWER_H


#include "stm8s.h"


#define LOWPOWER_COUNTER_VALUE	((uint16_t)5000)



/**
  * @brief  lowpower detect
  * @param  None
  * @retval 是否进入睡眠
  *		#TRUE:到时间进入睡眠
  *		#FALSE:没有到时间
  */
bool Lowpower_Detect(void);

/**
  * @brief  心跳计数回调函数
  * @param  None
  * @retval None
  */
void Lowpower_TickInt_Callback(void);

/**
  * @brief  重置低功耗计数器
  * @param  None
  * @retval None
  */
void Lowpower_ReloadCounter(void);

/**
  * @brief  进入低功耗模式
  * @param  None
  * @retval None
  */
void Lowpower_Enter(void);

/**
  * @brief  自动唤醒单元中断回调函数
  * @param  None
  * @retval None
  */
void Lowpower_AWU_Int_Callback(void);




#endif
