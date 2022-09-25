/**
  ******************************************************************************
  * @file     relay_beep.c
  * @author   ZhuSL
  * @brief    继电器和蜂鸣器驱动代码，因为继电器和蜂鸣器共用了电源
  * @version  V1.0.0  2021年3月24日
  ******************************************************************************
  */

#include "beep.h"
#include "delay.h"




/**
  * @brief  初始化蜂鸣器
  * @param  None
  * @retval None
  */
void Beep_Init(void)
{
  GPIO_Init(BEEP_GPIO_PORT, BEEP_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
  
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);

  TIM3_TimeBaseInit(TIM3_Prescaler_1, TIM3_CounterMode_Up, 40000);

  TIM3_OC2Init(TIM3_OCMode_PWM2, TIM3_OutputState_Enable, 20000, TIM3_OCPolarity_Low, TIM3_OCIdleState_Set);

  TIM3_OC2PreloadConfig(ENABLE);

  TIM3_ARRPreloadConfig(ENABLE);

  TIM3_CtrlPWMOutputs(ENABLE);
}


/**
  * @brief  deinit
  * @param  None
  * @retval None
  */
void Beep_DeInit(void)
{
  TIM3_DeInit();

  CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, DISABLE);

  GPIO_Init(BEEP_GPIO_PORT, BEEP_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
}


/**
  * @brief  蜂鸣器嘀嘀嘀...
  * @param  Count:滴的次数
  *	@param	每次的持续时间
  * @retval None
  */
void Beep_dididi(uint8_t Count, uint16_t Time)
{
	while(Count--)
	{
		Beep_Config(ENABLE);
		Delay_Ms(Time);
		Beep_Config(DISABLE);
		if(Count)
		{
			Delay_Ms(Time);
		}
	}
}





/**
  * @brief  蜂鸣器使能失能
  * @param  State
  * @retval None
  */
void Beep_Config(FunctionalState State)
{
  if(State == ENABLE)
  {
    Beep_Init();
    TIM3_Cmd(ENABLE);
  }
  else
  {
    TIM3_Cmd(DISABLE);
    Beep_DeInit();
  }
}


/**
  * @brief  蜂鸣器睡眠
  * @param  None
  * @retval None
  */
void Beep_Sleep(void)
{
  // TIM3_OC2PreloadConfig(DISABLE);

  // TIM3_ARRPreloadConfig(DISABLE);

  // TIM3_CtrlPWMOutputs(DISABLE);

  // CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, DISABLE);

  // TIM3_DeInit();

  // GPIO_Init(BEEP_GPIO_PORT, BEEP_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);

  // Beep_DeInit();
}


/**
  * @brief  beep wakeup
  * @param  None
  * @retval None
  */
void Beep_Wakeup(void)
{
  // Beep_Init();
}