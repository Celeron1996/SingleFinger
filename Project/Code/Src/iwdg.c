/**
  ******************************************************************************
  * @file     iwdg.c
  * @author   ZhuSL
  * @brief    看门狗驱动
  * @version  V1.0.0  2021年3月21日
  ******************************************************************************
  */

#include "iwdg.h"


/**
  * @brief  iwdg init
  * @param  None
  * @retval None
  */
void IWDG_Init(void)
{
	/* Check if the system has resumed from IWDG reset */
	if (RST_GetFlagStatus(RST_FLAG_IWDGF) != RESET)
	{
	  /* Clear IWDGF Flag */
	  RST_ClearFlag(RST_FLAG_IWDGF);
	}

  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
  
  /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
     dispersion) */
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  
  /* IWDG counter clock: LSI/128 */
  IWDG_SetPrescaler(IWDG_Prescaler_256);
  
  /* Set counter reload value to obtain 250ms IWDG Timeout.
    Counter Reload Value = 255
   */
  IWDG_SetReload((uint8_t)(0xFF));
  
  /* Reload IWDG counter */
  IWDG_ReloadCounter();
}


