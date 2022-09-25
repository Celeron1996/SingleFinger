/**
  ******************************************************************************
  * @file     systick.h
  * @author   ZhuSL
  * @brief    系统时基
  * @version  V1.0.0  2021年3月11日
  ******************************************************************************
  */

#include "systick.h"


static void TIM4_Config(void);

__IO uint32_t Systick_Counter = 0;


/**
  * @brief  系统时基初始化
  * @param  None
  * @retval None
  */
void Systick_Init(void)
{
	TIM4_Config();
}


/**
  * @brief  获取时基
  * @param  None
  * @retval None
  */
uint32_t Systick_GetCounter(void)
{
	return (uint32_t)Systick_Counter;
}


/**
  * @brief  时基中断回调函数
  * @param  None
  * @retval None
  */
void Systick_Int_Callback(void)
{
	Systick_Counter++;
}


/**
  * @brief  Configure TIM4 to generate an update interrupt each 1ms 
  * @param  None
  * @retval None
  */
static void TIM4_Config(void)
{
  /* TIM4 configuration:
   - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
   clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
      min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example we need to generate a time base equal to 1 ms
   so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */

  /* Enable TIM4 CLK */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
  
  /* Time base configuration */
  TIM4_TimeBaseInit(TIM4_Prescaler_128, 124);
  /* Clear TIM4 update flag */
  TIM4_ClearFlag(TIM4_FLAG_Update);
  /* Enable update interrupt */
  TIM4_ITConfig(TIM4_IT_Update, ENABLE);
  
  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);
}





