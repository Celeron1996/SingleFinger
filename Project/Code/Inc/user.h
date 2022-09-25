/**
  ******************************************************************************
  * @file     user.h
  * @author   ZhuSL
  * @brief    main code
  * @version  V1.0.0  2021年3月24日
  ******************************************************************************
  */

#ifndef __USER_H
#define __USER_H


#include "stm8s.h"


/*用户类型定义*/
typedef enum
{
	User_Null		= (uint8_t)0x00,
	User_Admini = (uint8_t)0x01,
	User_Normal = (uint8_t)0x02
} UserType;


/*用户数量定义*/
#define USER_ADMINI_MAX_NUMBER	(2u)
#define USER_NORMAL_MAX_NUMBER	(98u)
#define USER_ALL_MAX_NUMBER			(USER_ADMINI_MAX_NUMBER + USER_NORMAL_MAX_NUMBER)


/*用户类型*/
typedef struct
{
	UserType Type;
	uint16_t ID;
} User_TypeDef;

/*系统初始化状态定义*/
typedef enum
{
	UserInit_True		= ((uint8_t)(0xAA)),
	UserInit_False	= ((uint8_t)(0x55))
} UserInitState_TypeDef;


/*系统状态定义*/
typedef struct
{
	UserInitState_TypeDef	InitState;																													/*系统是否被初始化*/
	uint8_t 							UserActiBitMap[(USER_ALL_MAX_NUMBER/4) + (USER_ALL_MAX_NUMBER%4)];	/*用户是否被注册*/
	/*
		00:没有使用
		01:普通用户
		10:管理用户
	*/
} User_HandlerTypeDef;



/**
  * @brief  判断系统是否处于初始化状态
  * @param  None
  * @retval 返回结果
  *		#TRUE
  *		#FALSE
  */
bool User_isInit(void);

/**
  * @brief  系统退出初始化状态
  * @param  None
  * @retval None
  */
void User_InitExit(void);

/**
  * @brief  配置ID号为UserID的用户为Type类型
  * @param  UserID:要配置的用户ID号
  *	@param	Type:要配置的类型
  * @retval None
  */
void User_Config(uint16_t UserID, UserType Type);

/**
  * @brief  获取一个未注册的空ID号
  * @param  UserID:如果返回TRUE，则存储空的ID号
  * @retval 返回结果
  *		#ERROR
  *		#SUCCESS
  */
ErrorStatus User_GetNoUsedID(uint16_t *UserID);

/**
  * @brief  根据ID号查询一个用户的类型
  * @param  UserID:查询的用户ID号
  * @retval 返回查询类型
  *		#User_Null:			该ID号没有被注册
  *		#User_Admini:		该ID号被注册为管理用户
  *		#User_Normal:		该ID号被注册为普通用户
  */
UserType User_CheckUserType(uint16_t UserID);

/**
  * @brief  User_Init
  * @param  None
  * @retval None
  */
void User_Init(void);

/**
  * @brief  将系统参数恢复至出厂状态
  * @param  None
  * @retval None
  */
void User_Clear(void);

/**
  * @brief  将系统参数数据从eeprom中读出来
  * @param  pData:存储的数据，类型应为User_HandlerTypeDef
  * @retval None
  */
void User_ReadFlash(uint8_t *pData);

/**
  * @brief  将系统参数写进内部eeprom
  * @param  pData:写入的数据，User_HandlerTypeDef类型的数据指针
  * @retval None
  */
void User_WriteFlash(uint8_t *pData);




#endif