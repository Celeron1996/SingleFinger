/**
  ******************************************************************************
  * @file     switch.h
  * @author   ZhuSL
  * @brief    拨动开关
  * @version  V1.0.0  2021年3月14日
  ******************************************************************************
  */

#ifndef __SWITCH_H
#define __SWITCH_H

#include "stm8l15x.h"



#define SWITCH_GPIO_PORT					GPIOB
#define SWITCH_GPIO_PIN						GPIO_Pin_6



/*解锁模式定义*/
typedef enum
{
	Mode_AutoLock = 0,								/*自锁模式，验证正确开锁、验证正确关锁、验证正确开锁......*/
	Mode_NoAutoLock = !Mode_AutoLock	/*点动模式，解锁一段时间后自动上锁*/
} Mode_TypeDef;


/**
  * @brief  拨动开关gpio初始化
  * @param  None
  * @retval None
  */
void Switch_Init(void);

/**
  * @brief  获取拨动开关状态，即解锁模式
  * @param  None
  * @retval 返回当前解锁模式
  *		#Mode_AutoLock
  *		#Mode_NoAutoLock
  */
Mode_TypeDef Switch_GetMode(void);

/**
  * @brief  强制清空检测
  * @param  None
  * @retval None
  */
void Switch_ClearDetect(void);


#endif