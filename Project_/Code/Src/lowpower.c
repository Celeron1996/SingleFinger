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

__IO bool RTC_Int_Flag = FALSE;



/**
  * @brief  lowpower init
  * @param  None
  * @retval None
  */
void Lowpower_Init(void)
{
  /* LSI to RTC clock DIV 1 */
  CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);

  /* Enable RTC clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);

  /* Configures the RTC DIV 16 */
  RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);

  /* Enable RTC interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);  

  /* 2375*(1/(38K/16)) = 1 second */
  RTC_SetWakeUpCounter(2375);

  /* Enable RTC wakeup */
  RTC_WakeUpCmd(ENABLE);  
}


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
	RTC_Int_Flag = FALSE;

  IWDG_ReloadCounter();

	while(1)
	{
		halt();

		if(RTC_Int_Flag == TRUE)
		{
			/*RTC唤醒了睡眠，喂狗后继续睡眠*/
			RTC_Int_Flag = FALSE;
      IWDG_ReloadCounter();
		}
		else
		{
      IWDG_ReloadCounter();
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
	RTC_Int_Flag = TRUE;
}

