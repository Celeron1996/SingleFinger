/**
  ******************************************************************************
  * @file     switch.c
  * @author   ZhuSL
  * @brief    拨动开关
  * @version  V1.0.0  2021年3月14日
  ******************************************************************************
  */

#include "switch.h"
#include "delay.h"
#include "finger.h"
#include "user.h"
#include "lowpower.h"
#include "beep.h"

/**
  * @brief  拨动开关gpio初始化
  * @param  None
  * @retval None
  */
void Switch_Init(void)
{
	GPIO_Init(SWITCH_GPIO_PORT, SWITCH_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
}


/**
  * @brief  获取拨动开关状态，即解锁模式
  * @param  None
  * @retval 返回当前解锁模式
  *		#Mode_AutoLock
  *		#Mode_NoAutoLock
  */
Mode_TypeDef Switch_GetMode(void)
{
	GPIO_Init(SWITCH_GPIO_PORT, SWITCH_GPIO_PIN, GPIO_Mode_In_PU_No_IT);

	Delay_Ms(20); /*等待状态切换稳定*/

	if(GPIO_ReadInputDataBit(SWITCH_GPIO_PORT, SWITCH_GPIO_PIN) == RESET)
	{
		GPIO_Init(SWITCH_GPIO_PORT, SWITCH_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
		return Mode_AutoLock;
	}
	else
	{
		GPIO_Init(SWITCH_GPIO_PORT, SWITCH_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
		return Mode_NoAutoLock;
	}
}


/**
  * @brief  强制清空检测
  * @param  None
  * @retval None
  */
void Switch_ClearDetect(void)
{
	uint16_t counter = 0;
	BitStatus status;
	uint8_t active = 0;
	BitStatus current;

	GPIO_Init(SWITCH_GPIO_PORT, SWITCH_GPIO_PIN, GPIO_Mode_In_PU_No_IT);
	Beep_dididi(1, 100);

	status = GPIO_ReadInputDataBit(SWITCH_GPIO_PORT, SWITCH_GPIO_PIN);
	while((counter++ < 3000) && (active < 10))
	{
		current = GPIO_ReadInputDataBit(SWITCH_GPIO_PORT, SWITCH_GPIO_PIN);
		if(status != current)
		{
			status = current;
			active++;
			counter = 0;
			Beep_dididi(1, 20);
		}
		Delay_Ms(1);
	}

	GPIO_Init(SWITCH_GPIO_PORT, SWITCH_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);

	if(active >= 10)
	{
		Finger_CommandDeletChar(0, 100);
		User_Clear();
		Beep_dididi(2, 500);
	}

	Lowpower_ReloadCounter();
}




