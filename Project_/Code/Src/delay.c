/**
  ******************************************************************************
  * @file     delay.c
  * @author   ZhuSL
  * @brief    延时
  * @version  V1.0.0  2021年3月14日
  ******************************************************************************
  */

#include "delay.h"
#include "systick.h"

/**
  * @brief  延时初始化
  * @param  None
  * @retval None
  */
void Delay_Init(void)
{
//  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);
//  
//  TIM2_TimeBaseInit(TIM2_PRESCALER_16, DELAY_TIMER_PERIOD_VALUE);
//
//  TIM2_ClearFlag(TIM2_FLAG_UPDATE);
//  
//  TIM2_Cmd(ENABLE);
}


/**
  * @brief  毫秒级延时
  * @param  Time:延时时间
  * @retval None
  */
void Delay_Ms(uint16_t Time)
{
//	while(Time--)
//	{
//		Delay_Us(1000);
//		IWDG_ReloadCounter();
//	}

//	uint32_t counter = Systick_GetCounter();;

//	while(Time)
//	{
//		if(counter != Systick_GetCounter())
//		{
//			counter = Systick_GetCounter();
//			Time--;
//		}
//    IWDG_ReloadCounter();
//	}
	volatile uint16_t counter;

	while(Time--)
	{
		for(counter = 0; counter < 1200; counter++)
		{}
		IWDG_ReloadCounter();
	}
}


/**
  * @brief  微妙级延时
  * @param  Time:延时时间
  * @retval None
  */
void Delay_Us(uint16_t Time)
{
//	uint16_t counter_old = TIM2_GetCounter();
//	uint16_t counter_curr = 0;
//	uint16_t counter_time = 0;
//
//	while(Time)
//	{
//		counter_curr = TIM2_GetCounter();
//		if(counter_curr != counter_old)
//		{
//			if(counter_curr > counter_old)
//			{
//				counter_time += (counter_curr - counter_old);
//			}
//			else	/*(counter_curr < counter_old)*/
//			{
//				counter_time += DELAY_TIMER_PERIOD_VALUE - counter_old + counter_curr;
//			}
//			counter_old = counter_curr;
//			if(counter_time >= Time)
//			{
//				break;
//			}
//		}
//	}

  volatile uint8_t counter;
  
  while(Time--)
  {
    for(counter = 0; counter < 20;counter++)
    {}
  }
}

