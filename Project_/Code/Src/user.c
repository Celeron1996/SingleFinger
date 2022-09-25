/**
  ******************************************************************************
  * @file     user.c
  * @author   ZhuSL
  * @brief    main code
  * @version  V1.0.0  2021年3月24日
  ******************************************************************************
  */

#include "user.h"


/*系统状态定义*/
User_HandlerTypeDef User_Handler;



/**
  * @brief  判断系统是否处于初始化状态
  * @param  None
  * @retval 返回结果
  *		#TRUE
  *		#FALSE
  */
bool User_isInit(void)
{
	if(User_Handler.InitState == UserInit_True)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/**
  * @brief  系统退出初始化状态
  * @param  None
  * @retval None
  */
void User_InitExit(void)
{
	User_Handler.InitState = UserInit_False;
	User_WriteFlash((uint8_t *)&User_Handler);
}


/**
  * @brief  配置ID号为UserID的用户为Type类型
  * @param  UserID:要配置的用户ID号
  *	@param	Type:要配置的类型
  * @retval None
  */
void User_Config(uint16_t UserID, UserType Type)
{
	uint8_t index = 0;			/*数组下标*/
	uint8_t bitoffset = 0;	/*byte中的位偏移量*/
	uint8_t temp = 0;

	index = UserID/4;
	bitoffset = (UserID%4)*2;

	temp = User_Handler.UserActiBitMap[index];
	temp &= (~(0x03 << bitoffset));

	switch(Type)
	{
		case User_Null:
			//temp |= (0x00 << bitoffset);
			break;
		case User_Normal:
			temp |= (0x01 << bitoffset);
			break;
		case User_Admini:
			temp |= (0x02 << bitoffset);
			break;
		default:
			//temp |= (0x00 << bitoffset);
			break;
	}

	User_Handler.UserActiBitMap[index] = temp;

	User_WriteFlash((uint8_t *)&User_Handler);
}


/**
  * @brief  获取一个未注册的空ID号
  * @param  UserID:如果返回TRUE，则存储空的ID号
  * @retval 返回结果
  *		#ERROR
  *		#SUCCESS
  */
ErrorStatus User_GetNoUsedID(uint16_t *UserID)
{
	uint8_t x, y;
	uint8_t temp;

	for(x = 0; x < ((USER_ALL_MAX_NUMBER/4) + (USER_ALL_MAX_NUMBER%4)); x++)
	{
		temp = User_Handler.UserActiBitMap[x];
		for(y = 0; y < 4; y++)
		{
			if((temp&0x03) == 0x00)
			{
				*UserID = (x*4) + y;
				return SUCCESS;
			}
			else
			{
				temp >>= 2;
			}
		}
	}

	return ERROR;
}


/**
  * @brief  根据ID号查询一个用户的类型
  * @param  UserID:查询的用户ID号
  * @retval 返回查询类型
  *		#User_Null:			该ID号没有被注册
  *		#User_Admini:		该ID号被注册为管理用户
  *		#User_Normal:		该ID号被注册为普通用户
  */
UserType User_CheckUserType(uint16_t UserID)
{
	uint8_t index = 0;			/*数组下标*/
	uint8_t bitoffset = 0;	/*byte中的位偏移量*/
	uint8_t temp;

	index = UserID/4;
	bitoffset = (UserID%4)*2;

	temp = User_Handler.UserActiBitMap[index];

	temp >>= bitoffset;

	switch((temp&0x03))
	{
		case 0x01:	return User_Normal;
		case 0x02:	return User_Admini;
		default:		return User_Null;
	}
}


/**
  * @brief  User_Init
  * @param  None
  * @retval None
  */
void User_Init(void)
{
	uint8_t i;

	User_ReadFlash((uint8_t *)&User_Handler);

	if((User_Handler.InitState != UserInit_True)&&(User_Handler.InitState != UserInit_False))
	{
		/*出厂状态，eeprom未初始化*/
		User_Handler.InitState = UserInit_True;
		for(i = 0; i < ((USER_ALL_MAX_NUMBER/4) + (USER_ALL_MAX_NUMBER%4)); i++)
		{
			User_Handler.UserActiBitMap[i] = 0x00;
		}

		User_WriteFlash((uint8_t *)&User_Handler);
	}
}

/**
  * @brief  将系统参数恢复至出厂状态
  * @param  None
  * @retval None
  */
void User_Clear(void)
{
	uint8_t i;

	User_Handler.InitState = UserInit_True;
	for(i = 0; i < ((USER_ALL_MAX_NUMBER/4) + (USER_ALL_MAX_NUMBER%4)); i++)
	{
		User_Handler.UserActiBitMap[i] = 0x00;
	}

	User_WriteFlash((uint8_t *)&User_Handler);
}


/**
  * @brief  将系统参数数据从eeprom中读出来
  * @param  pData:存储的数据，类型应为User_HandlerTypeDef
  * @retval None
  */
void User_ReadFlash(uint8_t *pData)
{
	uint8_t i;

	for(i = 0; i < sizeof(User_HandlerTypeDef); i++)
	{
		*pData++ = FLASH_ReadByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS + i);
	}
}


/**
  * @brief  将系统参数写进内部eeprom
  * @param  pData:写入的数据，User_HandlerTypeDef类型的数据指针
  * @retval None
  */
void User_WriteFlash(uint8_t *pData)
{
	uint8_t i;

	FLASH_Unlock(FLASH_MemType_Data);
	while(FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET);

	for(i = 0; i < sizeof(User_HandlerTypeDef); i++)
	{
		FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS + i, *pData++);
		while(FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET);
	}

	FLASH_Lock(FLASH_MemType_Data);
}


