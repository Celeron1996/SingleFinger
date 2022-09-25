/**
  ******************************************************************************
  * @file     main.c
  * @author   ZhuSL
  * @brief    main code
  * @version  V1.0.0  2021年3月14日
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void CLK_Config(void);






/**
  * @brief  Configure the AWU time base to 12s
  * @param  None
  * @retval None
  */
void AWU_Config(void)
{
    /* Initialization of AWU */
     /* LSI calibration for accurate auto wake up time base*/
    AWU_LSICalibrationConfig(128000);

     /* The delay corresponds to the time we will stay in Halt mode */
    AWU_Init(AWU_TIMEBASE_512MS);
}




/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
	enum {
		step_init,
		step_detect,
		step_unlock,
		step_press,
		step_enroll,
		step_clear,
		step_error,
		step_success,
		step_sleep,
		step_wakeup
	} step = step_init;
	enum {
		cmd_verify,
		cmd_enroll_one,
		cmd_enroll_two
	} command = cmd_verify;
	uint32_t press_count = 0;	/*判断手指长按时间*/
	User_TypeDef user_verify;
	User_TypeDef user_enroll;


	/*时钟配置*/
	CLK_Config();

	/*初始化看门狗*/
	IWDG_Init();

	AWU_DeInit();
	AWU_Init(AWU_TIMEBASE_256MS);
	AWU_Cmd(ENABLE);

	CLK_SlowActiveHaltWakeUpCmd(ENABLE); //关闭活跃停机模式下的电压调节器(MVR)
	CLK_FastHaltWakeUpCmd(DISABLE); //关闭快速唤醒
	FLASH_SetLowPowerMode(FLASH_LPMODE_POWERDOWN); //设置为停机后flash掉电

	/*初始化系统心跳*/
	Systick_Init();

	/*初始化延时函数*/
	Delay_Init();

	/*系统参数初始化*/
	User_Init();

	/*初始化拨动开关*/
  Switch_Init();

  /*初始化继电器和蜂鸣器*/
  Relay_Beep_Init();

	/*初始化指纹模组*/
  Finger_Init();

	/*未使用的悬空GPIO输出低电平*/
	GPIO_Init(GPIOA, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(GPIOA, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);

	/*开启总中断*/
	enableInterrupts();


  Relay_Beep_PowerConfig(ENABLE);
  
  Switch_ClearDetect();

  while (1)
  {
  	IWDG_ReloadCounter();

  	switch(step)
  	{
  		case step_init:
  			command = cmd_verify;
  			step = step_detect;
  			break;
  		case step_detect:
  			if(Finger_Detect((command == cmd_verify)?(Finger_False):(Finger_True)) == Finger_Success)
  			{
  				Lowpower_ReloadCounter();

  				if((command == cmd_verify)&&(User_isInit() == TRUE))
  				{
  					/*说明处于初始化状态，没有注册管理员，默认开锁，且以管理员身份*/
  					user_verify.ID = 0;
  					user_verify.Type = User_Admini;
  					step = step_unlock;
  				}
  				else
  				{
  					if(command != cmd_verify)
  					{
  						/*注册*/
  						if(Finger_CommandGenChar((command == cmd_enroll_one)?(1):(2)) == Finger_Success)
  						{
  							if((command == cmd_enroll_two)&&(Finger_CommandRegModel() == Finger_Success))
  							{
  								if(Finger_CommandStoreChar(1, user_enroll.ID) == Finger_Success)
  								{
  									User_Config(user_enroll.ID, user_enroll.Type);
  									step = step_success;
  								}
  								else
  								{
  									step = step_error;
  								}

  							}
  							else if(command == cmd_enroll_one)
  							{
                  Beep_dididi(1, 200);
  								command = cmd_enroll_two;
  								step = step_detect;
  							}
  							else
  							{
  								command = cmd_verify;
  								step = step_error;
  							}
  						}
  					}
  					else
  					{
  						/*验证*/
		  				if(Finger_CommandGenChar(1) == Finger_Success)
		  				{
		  					if(Finger_CommandSearch(1, 0, USER_ALL_MAX_NUMBER, &user_verify.ID) == Finger_Success)
		  					{
									user_verify.Type = User_CheckUserType(user_verify.ID);
									if(user_verify.Type == User_Null)
									{
										step = step_error;	/*虽然在模组被注册了，但没有登记在mcu内部*/
									}
									else
									{
										step = step_unlock;
									}
		  					}
		  					else
		  					{
		  						step = step_error;
		  					}
		  				}
		  				else
		  				{
		  					step = step_error;
		  				}
  					}

  				}
  			}
  			else
  			{
  				step = step_detect;
  			}
  			break;
  		case step_unlock:
  			if(Switch_GetMode() == Mode_AutoLock)
  			{
  				if(Relay_GetState() == ENABLE)	/*自锁模式，每次验证成功后改变状态*/
  				{
  					Relay_Config(DISABLE);
  				}
  				else
  				{
  					Relay_Config(ENABLE);
  				}

  				Delay_Ms(1000);
  			}
  			else
  			{
  				Relay_Config(ENABLE);
  				Delay_Ms(3000);	/*点动模式，3s钟后自动反锁*/
  				Relay_Config(DISABLE);
  			}
  			Lowpower_ReloadCounter();
				step = step_press;
  			break;
  		case step_press:
				/*以下判断是否长按*/
				press_count = 0;
				while(user_verify.Type == User_Admini)
				{
					IWDG_ReloadCounter();
					Lowpower_ReloadCounter();

					if((Systick_GetCounter() % 200) == 0)
					{
						press_count += 200;
						if(Finger_CommandGetImage() != Finger_Success)
						{
							break;
						}
					}

					if(press_count >= 60000)
					{
						break;
					}

					/*=====================================================
						>管理用户按住3秒钟以上进入添加管理用户，蜂鸣器滴三声
						>管理用户按住6秒钟以上进入添加普通用户，蜂鸣器滴六声
						>管理用户按住10秒钟以上进入清空系统,蜂鸣器滴十声
						=====================================================*/
					if(press_count == 3000)
					{
						Beep_dididi(3, 80);
						press_count += 400;
					}
					else if(press_count == 6000)
					{
						Beep_dididi(6, 80);
						press_count += 800;
					}
					else if(press_count == 10000)
					{
						Beep_dididi(10, 50);
						press_count += 800;
					}
				}

				if(press_count >= 10000)
				{
					/*清空*/
					step = step_clear;
				}
				else if(press_count >= 6000)
				{
					/*注册普通用户*/
					user_enroll.Type = User_Normal;
					if(User_isInit() == TRUE)
					{
						/*注册普通用户之前必须至少注册一个管理员!*/
						step = step_error;
					}
					else
					{
						step = step_enroll;
					}
				}
				else if(press_count >= 3000)
				{
					/*注册管理用户*/
					step = step_enroll;
					user_enroll.Type = User_Admini;
				}
				else
				{
					/*继续验证*/
					step = step_detect;
					user_verify.ID = 0;
					user_verify.Type = User_Null;
					command = cmd_verify;
				}

				Lowpower_ReloadCounter();
  			break;
  		case step_enroll:
  			if(User_GetNoUsedID(&user_enroll.ID) == SUCCESS)
  			{
	  			command = cmd_enroll_one;
	  			step = step_detect;
  			}
  			else
  			{
  				command = cmd_verify;
  				step = step_error;
  			}
  			break;
  		case step_clear:
  			if(Finger_CommandDeletChar(0, 100) == Finger_Success)
  			{
  				User_Clear();
  				step = step_success;
  			}
  			else
  			{
  				step = step_error;
  			}
  			break;
  		case step_error:
  			Beep_dididi(1, 1000);
  			command = cmd_verify;
  			step = step_detect;
  			break;
  		case step_success:
  			Beep_dididi(2, 500);
  			if(command == cmd_enroll_two)
  			{
  				/*允许继续注册*/
  				command = cmd_enroll_one;
  				if((user_enroll.Type == User_Admini)&&(User_isInit() == TRUE))
  				{
  					User_InitExit();
  				}
  				step = step_enroll;
  			}
  			else
  			{
  				command = cmd_verify;
  				step = step_detect;
  			}
  			break;
  		case step_sleep:
				Finger_Sleep();
				Beep_Sleep();

        if(Relay_GetState() == DISABLE)
        {
        	Relay_Beep_PowerConfig(DISABLE);
        }

				TIM4_Cmd(DISABLE);
				
				Lowpower_Enter();

//        CLK_Config();

        TIM4_Cmd(ENABLE);

//				AWU_Cmd(DISABLE);
//				AWU_DeInit();

				Lowpower_ReloadCounter();
  			step = step_wakeup;
  			break;
  		case step_wakeup:
  			Finger_Wakeup();
  			Lowpower_ReloadCounter();
  			Relay_Beep_PowerConfig(ENABLE);
  			command = cmd_verify;
  			step = step_detect;
  			break;
  		default:
  			step = step_init;
  			break;
  	}

  	if(Lowpower_Detect() == TRUE)
  	{
  		step = step_sleep;
  	}
  }
}



















/**
  * @brief  Configure system clock to run at 16Mhz
  * @param  None
  * @retval None
  */
static void CLK_Config(void)
{
	/* Initialization of the clock */
	/* Clock divider to HSI/1 */
//	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

	volatile uint16_t counter = 20000;

	CLK_HSICmd(ENABLE);
  CLK_LSICmd(ENABLE);
	CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);

	while(counter--)
	{}
}


#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
