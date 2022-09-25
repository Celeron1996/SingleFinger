/**
  ******************************************************************************
  * @file     lowpower.c
  * @author   ZhuSL
  * @brief    低功耗实现
  * @version  V1.0.0  2021年3月24日
  ******************************************************************************
  */

#include "lowpower.h"


__IO uint16_t Lowpower_Counter = 0;

__IO bool AWU_Int_Flag = FALSE;



/**
  * @brief  lowpower detect
  * @param  None
  * @retval 是否进入睡眠
  *		#TRUE:到时间进入睡眠
  *		#FALSE:没有到时间
  */
bool Lowpower_Detect(void)
{
	if(Lowpower_Counter >= LOWPOWER_COUNTER_VALUE)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/**
  * @brief  心跳计数回调函数
  * @param  None
  * @retval None
  */
void Lowpower_TickInt_Callback(void)
{
	Lowpower_Counter++;
}


/**
  * @brief  重置低功耗计数器
  * @param  None
  * @retval None
  */
void Lowpower_ReloadCounter(void)
{
	Lowpower_Counter = 0;
}


/**
  * @brief  进入低功耗模式
  * @param  None
  * @retval None
  */
void Lowpower_Enter(void)
{
	AWU_Int_Flag = FALSE;

	while(1)
	{
		halt();

		IWDG_ReloadCounter();

		if(AWU_Int_Flag == TRUE)
		{
			/*AWU唤醒了睡眠，喂狗后继续睡眠*/
			AWU_Int_Flag = FALSE;
		}
		else
		{
			break;
		}
	}
}




/**
  * @brief  自动唤醒单元中断回调函数
  * @param  None
  * @retval None
  */
void Lowpower_AWU_Int_Callback(void)
{
	AWU_Int_Flag = TRUE;
}

