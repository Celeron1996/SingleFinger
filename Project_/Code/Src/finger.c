/**
  ******************************************************************************
  * @file     finger.c
  * @author   ZhuSL
  * @brief    通元微指纹模组驱动
  * @version  V1.0.0  2021年3月6日
  ******************************************************************************
  */

#include "finger.h"
#include "delay.h"
#include "systick.h"


static Finger_ErrorStatus Finger_SendPack(const uint8_t *pSendPack, uint8_t SendLength, uint8_t **pReceivePack, uint8_t TryCount, uint16_t Timeout);
static uint8_t *Finger_AnalysisPack(uint8_t *pData, uint8_t Length);
static Finger_ErrorStatus Finger_CheckSum(const uint8_t *pData);
static uint16_t Finger_GenSum(const uint8_t *pData);



__IO uint8_t Finger_RxBuffer[FINGER_UART_RXBUFFER_SIZE];
Finger_HandleTypeDef Finger_Handler = {.RxCounter = 0, .pRxBuffer = Finger_RxBuffer, .RxComplete = Finger_False};


/**
  * @brief  指纹模组初始化函数
  * @param  None
  * @retval None
  */
void Finger_Init(void)
{
  GPIO_Init(FINGER_DSP_POWER_GPIO_PORT, FINGER_DSP_POWER_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
  GPIO_Init(FINGER_SENSOR_POWER_GPIO_PORT, FINGER_SENSOR_POWER_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
  GPIO_Init(FINGER_SIGNAL_GPIO_PORT, FINGER_SIGNAL_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
  GPIO_Init(FINGER_UART_TX_GPIO_PORT, FINGER_UART_TX_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
  GPIO_Init(FINGER_UART_RX_GPIO_PORT, FINGER_UART_RX_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);    

	FINGER_DSP_POWER_DISABLE();
	FINGER_SENSOR_POWER_DISABLE();
	FINGER_DELAY_MS(10);

  FINGER_SENSOR_POWER_ENABLE();
  FINGER_DELAY_MS(10);
  FINGER_DSP_POWER_ENABLE();

  GPIO_Init(FINGER_SIGNAL_GPIO_PORT, FINGER_SIGNAL_GPIO_PIN, GPIO_Mode_In_FL_IT);
  GPIO_Init(FINGER_UART_TX_GPIO_PORT, FINGER_UART_TX_GPIO_PIN, GPIO_Mode_Out_PP_High_Fast);
  GPIO_Init(FINGER_UART_RX_GPIO_PORT, FINGER_UART_RX_GPIO_PIN, GPIO_Mode_In_PU_No_IT);
  GPIO_ExternalPullUpConfig(FINGER_UART_TX_GPIO_PORT, FINGER_UART_TX_GPIO_PIN, ENABLE);
  GPIO_ExternalPullUpConfig(FINGER_UART_RX_GPIO_PORT, FINGER_UART_RX_GPIO_PIN, ENABLE); 

  FINGER_UART_DEINIT();
  FINGER_UART_REMAPPINCONFIG();
  FINGER_UART_CLKCONFIG(ENABLE);
  FINGER_UART_INIT(FINGER_UART_BAUDRATE);
  FINGER_UART_ITCONFIG(ENABLE);
  FINGER_UART_CMD(ENABLE);

  EXTI_SetPinSensitivity(FINGER_SIGNAL_EXTI_PIN, FINGER_SIGNAL_EXTI_SENSITIVITY);

  Finger_Wait55();
}


/**
  * @brief  指纹模组休眠
  * @param  None
  * @retval None
  */
void Finger_Sleep(void)
{
	FINGER_SENSOR_POWER_ENABLE();

	FINGER_UART_CMD(DISABLE);
	FINGER_UART_ITCONFIG(DISABLE);
	FINGER_UART_CLKCONFIG(DISABLE);
	FINGER_UART_DEINIT();

	GPIO_Init(FINGER_UART_TX_GPIO_PORT, FINGER_UART_TX_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
	GPIO_Init(FINGER_UART_RX_GPIO_PORT, FINGER_UART_RX_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);

	FINGER_DSP_POWER_DISABLE();
}


/**
  * @brief  指纹模组唤醒
  * @param  None
  * @retval None
  */
void Finger_Wakeup(void)
{

	FINGER_SENSOR_POWER_DISABLE();
	FINGER_DELAY_MS(20);
	FINGER_SENSOR_POWER_ENABLE();
	FINGER_DELAY_MS(20);
	
	FINGER_DSP_POWER_ENABLE();

	GPIO_Init(FINGER_UART_TX_GPIO_PORT, FINGER_UART_TX_GPIO_PIN, GPIO_Mode_Out_PP_Low_Fast);
	GPIO_Init(FINGER_UART_RX_GPIO_PORT, FINGER_UART_RX_GPIO_PIN, GPIO_Mode_In_PU_No_IT);

  FINGER_UART_CLKCONFIG(ENABLE);
  FINGER_UART_INIT(FINGER_UART_BAUDRATE);
  FINGER_UART_ITCONFIG(ENABLE);
  FINGER_UART_CMD(ENABLE);

  Finger_Wait55();
}


/**
  * @brief  串口接收中断回调函数
  * @param  Data:接收到的数据
  * @param  Complete:标志本次中断是空闲中断还是接收中断
  *           #Finger_False:本次中断是接收中断
  *           #Finger_True:本次中断是空闲中断
  * @retval None
  */
void Finger_UartRx_IntCallback(uint8_t Data, Finger_Bool Complete)
{
	if(Complete == Finger_False)
	{
		if(Finger_Handler.RxCounter < FINGER_UART_RXBUFFER_SIZE)
		{
			Finger_Handler.pRxBuffer[Finger_Handler.RxCounter++] = Data;
		}
	}
	else
	{
		Finger_Handler.RxComplete = Finger_True;
	}
}


/**
  * @brief  触摸信号中断回调函数
  * @param  None
  * @retval None
  */
void Finger_Signal_IntCallback(void)
{

}


/**
  * @brief  串口发送数据
  * @param  pData:要发送的数据
  *	@param	Length:发送的数据长度
  * @retval None
  */
void Finger_SendData(const uint8_t *pData, uint8_t Length)
{
	while(Length--)
	{
		FINGER_UART_SEND_BYTE(*pData++);
	}
}


/**
  * @brief  清空接收缓存
  * @param  None
  * @retval None
  */
void Finger_RxBufferClear(void)
{
	Finger_Handler.RxComplete = Finger_False;
	Finger_Handler.RxCounter = 0;
}


/**
  * @brief  获取接收缓存
  * @param  None
  * @retval 返回接收缓存的指针
  */
uint8_t *Finger_GetRxBuffer(void)
{
	return (uint8_t *)Finger_Handler.pRxBuffer;
}


/**
  * @brief  获取接收缓存计数
  * @param  None
  * @retval 返回接收缓存计数
  */
uint8_t Finger_GetRxCounter(void)
{
	return (uint8_t)Finger_Handler.RxCounter;
}


/**
  * @brief  获取接收完成标志位
  * @param  None
  * @retval 返回接收是否完成
  *		#Finger_False
  *		#Finger_True
  */
Finger_Bool Finger_GetRxComplete(void)
{
	return (Finger_Bool)Finger_Handler.RxComplete;
}


/**
  * @brief  获取图像
  * @param  None
  * @retval 传感器是否有图像
  *		#Finger_Success
  *		#Finger_Error
  */
Finger_ErrorStatus Finger_CommandGetImage(void)
{
	Finger_Pack_Command_GetImage_TypeDef command_pack = {
		.Head 			= FINGER_PACK_HEAD_VALUE,
		.Address 		= FINGER_PACK_ADDRESS_VALUE,
		.Type 			= Finger_Pack_Cmd,
		.Length 		= sizeof(Finger_Pack_Command_GetImage_TypeDef) - FINGER_HEAD_ADDRESS_TYPE_LENGTH_SIZE_VALUE,
		.Command 		= Finger_Command_GetImage,
		.Sum 				= 0x0005
		};
	Finger_Pack_Reply_GetImage_TypeDef *reply_pack = (void *)0;
	Finger_ErrorStatus result = Finger_Error;

	result = Finger_SendPack((const uint8_t *)&command_pack, sizeof(Finger_Pack_Command_GetImage_TypeDef), (uint8_t **)&reply_pack, FINGER_PACK_SEND_TRYCOUNT, FINGER_PACK_SEND_TIMEOUT);

	if(result == Finger_Success)
	{
		switch(reply_pack->Result)
		{
			case Finger_Reply_SUCCESS:			return Finger_Success;
			case Finger_Reply_DataError:		return Finger_Error;
			case Finger_Reply_NoFinger:			return Finger_Error;
			case Finger_Reply_ImageError:		return Finger_Error;
			default:												return Finger_Error;
		}
	}
	else
	{
		return Finger_Error;
	}
}


/**
  * @brief  获取图像
  * @param  BufferID:特征缓冲区charbuffer的序号，1~5
  * @retval 传感器是否有图像
  *		#Finger_Success
  *		#Finger_Error
  */
Finger_ErrorStatus Finger_CommandGenChar(uint8_t BufferID)
{
	Finger_Pack_Command_GenChar_TypeDef command_pack = {
		.Head 			= FINGER_PACK_HEAD_VALUE,
		.Address 		= FINGER_PACK_ADDRESS_VALUE,
		.Type 			= Finger_Pack_Cmd,
		.Length 		= sizeof(Finger_Pack_Command_GenChar_TypeDef) - FINGER_HEAD_ADDRESS_TYPE_LENGTH_SIZE_VALUE,
		.Command 		= Finger_Command_GenChar,
		.BufferID 	= BufferID,
		.Sum 				= 0
		};
	Finger_Pack_Reply_GenChar_TypeDef *reply_pack = (void *)0;
	Finger_ErrorStatus result = Finger_Error;

	command_pack.Sum = Finger_GenSum((const uint8_t *)&command_pack);

	result = Finger_SendPack((const uint8_t *)&command_pack, sizeof(Finger_Pack_Command_GenChar_TypeDef), (uint8_t **)&reply_pack, FINGER_PACK_SEND_TRYCOUNT, FINGER_PACK_SEND_TIMEOUT);

	if(result == Finger_Success)
	{
		switch(reply_pack->Result)
		{
			case Finger_Reply_SUCCESS:			return Finger_Success;
			case Finger_Reply_DataError:		return Finger_Error;
			case Finger_Reply_CharFew:			return Finger_Error;
			case Finger_Reply_BuffNoImage:	return Finger_Error;
			default:												return Finger_Error;
		}
	}
	else
	{
		return Finger_Error;
	}	
}


/**
  * @brief  以特征文件缓冲区中的特征文件搜索整个或部分指纹库
  * @param  BufferID:特征缓冲区charbuffer的序号，1~5
  *	@param	StartID:搜索模板范围的起始编号
  *	@param	TempCount:搜索的模板数量
  *	@param	UserID:如果成功搜索到用户，则存储用户ID
  * @retval 返回搜索结果
  *		#Finger_Success
  *		#Finger_Error
  */
Finger_ErrorStatus Finger_CommandSearch(uint8_t BufferID, uint16_t StartID, uint16_t TempCount, uint16_t *UserID)
{
	Finger_Pack_Command_Search_TypeDef command_pack = {
		.Head 				= FINGER_PACK_HEAD_VALUE,
		.Address 			= FINGER_PACK_ADDRESS_VALUE,
		.Type 				= Finger_Pack_Cmd,
		.Length 			= sizeof(Finger_Pack_Command_Search_TypeDef) - FINGER_HEAD_ADDRESS_TYPE_LENGTH_SIZE_VALUE,
		.Command 			= Finger_Command_Search,
		.BufferID 		= BufferID,
		.StartID 			= StartID,
		.TempCount 		= TempCount,
		.Sum 					= 0
		};
	Finger_Pack_Reply_Search_TypeDef *reply_pack = (void *)0;
	Finger_ErrorStatus result = Finger_Error;

	command_pack.Sum = Finger_GenSum((const uint8_t *)&command_pack);

	result = Finger_SendPack((const uint8_t *)&command_pack, sizeof(Finger_Pack_Command_Search_TypeDef), (uint8_t **)&reply_pack, FINGER_PACK_SEND_TRYCOUNT, FINGER_PACK_SEND_TIMEOUT);

	if(result == Finger_Success)
	{
		switch(reply_pack->Result)
		{
			case Finger_Reply_SUCCESS:
				*UserID = reply_pack->TempID;
				return Finger_Success;
			case Finger_Reply_DataError:		return Finger_Error;
			case Finger_Reply_FNoSearch:		return Finger_Error;
			default:												return Finger_Error;
		}
	}
	else
	{
		return Finger_Error;
	}	
}


/**
  * @brief  将CharBuffer1~2合并生成一个模板，并存储到CharBuffer1中
  * @param  None
  * @retval 返回执行结果
  *		#Finger_Success
  *		#Finger_Error
  */
Finger_ErrorStatus Finger_CommandRegModel(void)
{
	Finger_Pack_Command_RegModel_TypeDef command_pack = {
		.Head 			= FINGER_PACK_HEAD_VALUE,
		.Address 		= FINGER_PACK_ADDRESS_VALUE,
		.Type 			= Finger_Pack_Cmd,
		.Length 		= sizeof(Finger_Pack_Command_RegModel_TypeDef) - FINGER_HEAD_ADDRESS_TYPE_LENGTH_SIZE_VALUE,
		.Command 		= Finger_Command_RegModel,
		.Sum 				= 0x0009
		};
	Finger_Pack_Reply_RegModel_TypeDef *reply_pack = (void *)0;
	Finger_ErrorStatus result = Finger_Error;

	result = Finger_SendPack((const uint8_t *)&command_pack, sizeof(Finger_Pack_Command_RegModel_TypeDef), (uint8_t **)&reply_pack, FINGER_PACK_SEND_TRYCOUNT, FINGER_PACK_SEND_TIMEOUT);

	if(result == Finger_Success)
	{
		switch(reply_pack->Result)
		{
			case Finger_Reply_SUCCESS:				return Finger_Success;
			case Finger_Reply_DataError:			return Finger_Error;
			case Finger_Reply_RegModelError:	return Finger_Error;
			default:													return Finger_Error;
		}
	}
	else
	{
		return Finger_Error;
	}
}


/**
  * @brief  将特征缓冲区中的文件储存到flash指纹库中
  * @param  BufferID:需要存储的特征区缓冲号，固定为1！
  *	@param	TempID:存储到flash中的指纹序列号，不能超过指纹容量大小
  * @retval 返回执行结果
  *		#Finger_Success
  *		#Finger_Error
  */
Finger_ErrorStatus Finger_CommandStoreChar(uint8_t BufferID, uint16_t TempID)
{
	Finger_Pack_Command_StoreChar_TypeDef command_pack = {
		.Head 			= FINGER_PACK_HEAD_VALUE,
		.Address 		= FINGER_PACK_ADDRESS_VALUE,
		.Type 			= Finger_Pack_Cmd,
		.Length 		= sizeof(Finger_Pack_Command_StoreChar_TypeDef) - FINGER_HEAD_ADDRESS_TYPE_LENGTH_SIZE_VALUE,
		.Command 		= Finger_Command_StoreChar,
		.BufferID 	= BufferID,
		.TempID			= TempID,
		.Sum 				= 0
		};
	Finger_Pack_Reply_StoreChar_TypeDef *reply_pack = (void *)0;
	Finger_ErrorStatus result = Finger_Error;

	command_pack.Sum = Finger_GenSum((const uint8_t *)&command_pack);

	result = Finger_SendPack((const uint8_t *)&command_pack, sizeof(Finger_Pack_Command_StoreChar_TypeDef), (uint8_t **)&reply_pack, FINGER_PACK_SEND_TRYCOUNT, FINGER_PACK_SEND_TIMEOUT);

	if(result == Finger_Success)
	{
		switch(reply_pack->Result)
		{
			case Finger_Reply_SUCCESS:			return Finger_Success;
			case Finger_Reply_DataError:		return Finger_Error;
			default:												return Finger_Error;
		}
	}
	else
	{
		return Finger_Error;
	}	
}


/**
  * @brief  删除flash指纹库中的一个或多个特征文件
  *	@param	TempID:需要删除的模板的起始序列号
  *	@param	TempCount:要删除的模板数量
  * @retval 返回执行结果
  *		#Finger_Success
  *		#Finger_Error
  */
Finger_ErrorStatus Finger_CommandDeletChar(uint16_t TempID, uint16_t TempCount)
{
	Finger_Pack_Command_DeletChar_TypeDef command_pack = {
		.Head 			= FINGER_PACK_HEAD_VALUE,
		.Address 		= FINGER_PACK_ADDRESS_VALUE,
		.Type 			= Finger_Pack_Cmd,
		.Length 		= sizeof(Finger_Pack_Command_DeletChar_TypeDef) - FINGER_HEAD_ADDRESS_TYPE_LENGTH_SIZE_VALUE,
		.Command 		= Finger_Command_DeletChar,
		.TempID			= TempID,
		.TempCount	= TempCount,
		.Sum 				= 0
		};
	Finger_Pack_Reply_DeletChar_TypeDef *reply_pack = (void *)0;
	Finger_ErrorStatus result = Finger_Error;

	command_pack.Sum = Finger_GenSum((const uint8_t *)&command_pack);

	result = Finger_SendPack((const uint8_t *)&command_pack, sizeof(Finger_Pack_Command_DeletChar_TypeDef), (uint8_t **)&reply_pack, FINGER_PACK_SEND_TRYCOUNT, FINGER_PACK_SEND_TIMEOUT);

	if(result == Finger_Success)
	{
		switch(reply_pack->Result)
		{
			case Finger_Reply_SUCCESS:				return Finger_Success;
			case Finger_Reply_DataError:			return Finger_Error;
			case Finger_Reply_DeletCharError:	return Finger_Error;
			default:													return Finger_Error;
		}
	}
	else
	{
		return Finger_Error;
	}	
}


///**
//  * @brief  清空flash指纹库
//  * @param  None
//  * @retval 返回执行结果
//  *		#Finger_Success
//  *		#Finger_Error
//  */
//Finger_ErrorStatus Finger_CommandEmpty(void)
//{
//	Finger_Pack_Command_Empty_TypeDef command_pack = {
//		.Head 			= FINGER_PACK_HEAD_VALUE,
//		.Address 		= FINGER_PACK_ADDRESS_VALUE,
//		.Type 			= Finger_Pack_Cmd,
//		.Length 		= sizeof(Finger_Pack_Command_Empty_TypeDef) - FINGER_HEAD_ADDRESS_TYPE_LENGTH_SIZE_VALUE,
//		.Command 		= Finger_Command_Empty,
//		.Sum 				= 0x0010
//		};
//	Finger_Pack_Reply_Empty_TypeDef *reply_pack = (void *)0;
//	Finger_ErrorStatus result = Finger_Error;
//
//	result = Finger_SendPack((const uint8_t *)&command_pack, sizeof(Finger_Pack_Command_Empty_TypeDef), (uint8_t **)&reply_pack, FINGER_PACK_SEND_TRYCOUNT, FINGER_PACK_SEND_TIMEOUT);
//
//	if(result == Finger_Success)
//	{
//		switch(reply_pack->Result)
//		{
//			case Finger_Reply_SUCCESS:				return Finger_Success;
//			case Finger_Reply_DataError:			return Finger_Error;
//			case Finger_Reply_DeletCharError:	return Finger_Error;
//			case Finger_Reply_EmptyError:			return Finger_Error;
//			default:													return Finger_Error;
//		}
//	}
//	else
//	{
//		return Finger_Error;
//	}
//}


///**
//  * @brief  获取图像
//  * @param  None
//  * @retval 传感器是否有图像
//  *		#Finger_Success
//  *		#Finger_Error
//  */
//Finger_ErrorStatus Finger_CommandGetEnrollImage(void)
//{
//	Finger_Pack_Command_GetEnrollImage_TypeDef command_pack = {
//		.Head 			= FINGER_PACK_HEAD_VALUE,
//		.Address 		= FINGER_PACK_ADDRESS_VALUE,
//		.Type 			= Finger_Pack_Cmd,
//		.Length 		= sizeof(Finger_Pack_Command_GetEnrollImage_TypeDef) - FINGER_HEAD_ADDRESS_TYPE_LENGTH_SIZE_VALUE,
//		.Command 		= Finger_Command_GetEnrollImage,
//		.Sum 				= 0x0005
//		};
//	Finger_Pack_Reply_GetEnrollImage_TypeDef *reply_pack = (void *)0;
//	Finger_ErrorStatus result = Finger_Error;
//
//	result = Finger_SendPack((const uint8_t *)&command_pack, sizeof(Finger_Pack_Command_GetEnrollImage_TypeDef), (uint8_t **)&reply_pack, FINGER_PACK_SEND_TRYCOUNT, FINGER_PACK_SEND_TIMEOUT);
//
//	if(result == Finger_Success)
//	{
//		switch(reply_pack->Result)
//		{
//			case Finger_Reply_SUCCESS:			return Finger_Success;
//			case Finger_Reply_DataError:		return Finger_Error;
//			case Finger_Reply_NoFinger:			return Finger_Error;
//			case Finger_Reply_ImageError:		return Finger_Error;
//			default:												return Finger_Error;
//		}
//	}
//	else
//	{
//		return Finger_Error;
//	}
//}


/**
  * @brief  清空flash指纹库
  * @param  IsEnroll:是注册还是验证？
  *		#Finger_True:注册
  *		#Finger_False:验证
  * @retval 返回执行结果
  *		#Finger_Success
  *		#Finger_Error
  */
Finger_ErrorStatus Finger_Detect(Finger_Bool IsEnroll)
{
	Finger_ErrorStatus result = Finger_Error;

	if((Systick_GetCounter()%FINGER_DETECT_PERIOD) == 0)
	{
		if(IsEnroll == Finger_True)
		{
			result = Finger_CommandGetImage();
		}
		else
		{
			result = Finger_CommandGetImage();
		}
	}

	return result;
}


/**
  * @brief  等待手指拿开或者超时30s
  * @param  None
  * @retval None
  */
void Finger_WaitTakeoff(void)
{
	uint16_t counter = 0;
	Finger_ErrorStatus result = Finger_Success;
	
	do
	{
		if((Systick_GetCounter()%FINGER_DETECT_PERIOD) == 0)
		{
			result = Finger_CommandGetImage();
			counter += FINGER_DETECT_PERIOD;
		}
		IWDG_ReloadCounter();
	}while((result == Finger_Success)&&(counter < 30000));
}


/**
  * @brief  发送一个数据包，并等待回应
  * @param  pSendPack:待发送的数据
  *	@param	SendLength:发送长度
  *	@param	pReceivePack:存储接收回应包的数据指针
  *	@param	TryCount:重发次数
  *	@param	Timeout:每次等待超时时间
  * @retval 如果收到模组返回的回应包，返回成功
  *		#Finger_Success
  *		#Finger_Error
  */
static Finger_ErrorStatus Finger_SendPack(const uint8_t *pSendPack, uint8_t SendLength, uint8_t **pReceivePack, uint8_t TryCount, uint16_t Timeout)
{
	uint16_t counter = 0;
	uint8_t *p_temp = (void *)0;
	
	while(TryCount--)
	{
		counter = Timeout;
		
		Finger_RxBufferClear();
		Finger_SendData(pSendPack, SendLength);
		
		while(counter--)
		{
			if(Finger_GetRxComplete() == Finger_True)
			{
				p_temp = Finger_AnalysisPack(Finger_GetRxBuffer(), Finger_GetRxCounter());
				if(p_temp != 0)
				{
					*pReceivePack = p_temp;
					return Finger_Success;
				}
				else
				{
					break;
				}
			}
			FINGER_DELAY_MS(1);
		}
	}

	return Finger_Error;
}


/**
  * @brief  解析一串数据是否符合包结构
  * @param  pData:待解析的数据
  *	@param	Length:数据长度
  * @retval 如果解析成功，返回包头数据的指针，否则，返回空指针
  */
static uint8_t *Finger_AnalysisPack(uint8_t *pData, uint8_t Length)
{
	while(Length >= FINGER_PACK_SIZE_MIN)
	{
		if(*((uint16_t *)pData) == FINGER_PACK_HEAD_VALUE)
		{
			if(*((uint32_t *)(pData + 2)) == FINGER_PACK_ADDRESS_VALUE)
			{
				if(Finger_CheckSum(pData) == Finger_Success)
				{
					return pData;
				}
				else
				{
					return (void *)0;
				}
			}
			else
			{
				pData++;
				Length--;
			}
		}
		else
		{
			pData++;
			Length--;
		}
	}

	return (void *)0;
}


/**
  * @brief  检查一个数据包的校验和是否正确
  * @param  pData:待检验的数据
  * @retval 如果收到模组返回的回应包，返回成功
  *		#Finger_Success
  *		#Finger_Error
  */
static Finger_ErrorStatus Finger_CheckSum(const uint8_t *pData)
{
	uint16_t i;
	uint16_t sum = 0;
	uint16_t length = 0;

	sum = pData[FINGER_PACK_TYPE_ARRAY_INDEX_START] + pData[FINGER_PACK_LENGTH_ARRAY_INDEX_START] + pData[FINGER_PACK_LENGTH_ARRAY_INDEX_START + 1];

	length = pData[FINGER_PACK_LENGTH_ARRAY_INDEX_START]*256 + pData[FINGER_PACK_LENGTH_ARRAY_INDEX_START + 1] - 2;
	
	for(i = 0; i < length; i++)
	{
		sum += pData[FINGER_PACK_DATA_ARRAY_INDEX_START + i];
	}

	if(sum == (pData[FINGER_PACK_DATA_ARRAY_INDEX_START + length]*256 + pData[FINGER_PACK_DATA_ARRAY_INDEX_START + length + 1]))
	{
		return Finger_Success;
	}
	else
	{
		return Finger_Error;
	}
}


/**
  * @brief  根据形参计算一个校验和并返回
  * @param  pData:待检验的数据
  * @retval 返回一个校验和
  */
static uint16_t Finger_GenSum(const uint8_t *pData)
{
	uint16_t i;
	uint16_t sum = 0;
	uint16_t length = 0;

	sum = pData[FINGER_PACK_TYPE_ARRAY_INDEX_START] + pData[FINGER_PACK_LENGTH_ARRAY_INDEX_START] + pData[FINGER_PACK_LENGTH_ARRAY_INDEX_START + 1];

	length = pData[FINGER_PACK_LENGTH_ARRAY_INDEX_START]*256 + pData[FINGER_PACK_LENGTH_ARRAY_INDEX_START + 1] - 2;
	
	for(i = 0; i < length; i++)
	{
		sum += pData[FINGER_PACK_DATA_ARRAY_INDEX_START + i];
	}

	return sum;
}


/**
  * @brief  等待超时或者在此之前接收到一个0x55字节提前退出
  * @param  None
  * @retval None
  */
void Finger_Wait55(void)
{
	uint16_t count = 0;
	
	Finger_RxBufferClear();

	while(count++ < FINGER_POWERUP_WAIT_TIMEOUT)
	{
		FINGER_DELAY_MS(1);

		if((Finger_GetRxComplete() == Finger_True)&&(Finger_Handler.RxCounter == 1)&&(Finger_Handler.pRxBuffer[0] == 0x55))
		{
			break;
		}
	}
}


/**
  * @brief  在唤醒后使用，用于检测指纹头是误触发被唤醒还是真实唤醒，就是消抖读触摸引脚
  * @param  None
  * @retval None
  */
Finger_Bool Finger_isWakeup(void)
{
	Delay_Ms(20);
	if(GPIO_ReadInputDataBit(FINGER_SIGNAL_GPIO_PORT, FINGER_SIGNAL_GPIO_PIN) == RESET)
	{
		/*误触发*/
		return Finger_False;
	}
	else
	{
		/*真实唤醒*/
		return Finger_True;
	}
}
