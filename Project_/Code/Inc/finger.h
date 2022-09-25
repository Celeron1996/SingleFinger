/**
  ******************************************************************************
  * @file     finger.h
  * @author   ZhuSL
  * @brief    通元微指纹模组驱动
  * @version  V1.0.0  2021年3月6日
  ******************************************************************************
  */

#ifndef __FINGER_H
#define __FINGER_H

#include "stm8l15x.h"


/*MCU串口发送引脚定义*/
#define FINGER_UART_TX_GPIO_PORT								GPIOA
#define FINGER_UART_TX_GPIO_PIN									GPIO_Pin_2

/*MCU串口接收引脚定义*/
#define FINGER_UART_RX_GPIO_PORT								GPIOA
#define FINGER_UART_RX_GPIO_PIN									GPIO_Pin_3

/*DSP电源控制引脚定义*/
#define FINGER_DSP_POWER_GPIO_PORT							GPIOA
#define FINGER_DSP_POWER_GPIO_PIN								GPIO_Pin_5

/*传感器电源引脚定义*/
#define FINGER_SENSOR_POWER_GPIO_PORT						GPIOA
#define FINGER_SENSOR_POWER_GPIO_PIN						GPIO_Pin_4

/*触摸信号引脚定义*/
#define FINGER_SIGNAL_GPIO_PORT									GPIOC
#define FINGER_SIGNAL_GPIO_PIN									GPIO_Pin_5
#define FINGER_SIGNAL_EXTI_PIN									EXTI_Pin_5
#define FINGER_SIGNAL_EXTI_SENSITIVITY					EXTI_Trigger_Rising

/*串口波特率定义*/
#define FINGER_UART_BAUDRATE										(57600u)

/*接收缓存长度定义*/
#define FINGER_UART_RXBUFFER_SIZE								(32u)

/*串口恢复缺省状态*/
#define FINGER_UART_DEINIT()										USART_DeInit(USART1)

/*串口初始化，初始化串口的一系列参数*/
#define FINGER_UART_INIT(BaudRate)							USART_Init(	USART1,BaudRate,\
																		USART_WordLength_8b,\
																		USART_StopBits_1,\
																		USART_Parity_No,\
																		(USART_Mode_TypeDef)(USART_Mode_Rx|USART_Mode_Tx))

/*串口中断配置*/
#define FINGER_UART_ITCONFIG(NewState)					do{	USART_ITConfig(USART1,USART_IT_RXNE,NewState);\
															USART_ITConfig(USART1,USART_IT_IDLE,NewState);\
															USART_ClearITPendingBit(USART1,USART_IT_RXNE);\
															USART_ClearITPendingBit(USART1,USART_IT_IDLE);}while(0)

/*串口时钟配置*/
#define FINGER_UART_CLKCONFIG(NewState)					CLK_PeripheralClockConfig(CLK_Peripheral_USART1, NewState)

/*串口启用或关闭*/
#define FINGER_UART_CMD(NewState)								USART_Cmd(USART1,NewState)

/*串口发送一个字节*/
#define FINGER_UART_SEND_BYTE(Byte)							do{USART_SendData8(USART1,Byte);while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);}while(0)

/*引脚映射*/
#define FINGER_UART_REMAPPINCONFIG()					do{SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA, ENABLE);}while(0)

/*延时*/
#define FINGER_DELAY_MS(Ms)											Delay_Ms(Ms)

/*控制DSP电源引脚、传感器电源引脚的高低*/
#define FINGER_DSP_POWER_GPIO_WRITE(State)			do{if(State){GPIO_SetBits(FINGER_DSP_POWER_GPIO_PORT, FINGER_DSP_POWER_GPIO_PIN);}else{GPIO_ResetBits(FINGER_DSP_POWER_GPIO_PORT, FINGER_DSP_POWER_GPIO_PIN);};}while(0)
#define FINGER_SENSOR_POWER_GPIO_WRITE(State)		do{if(State){GPIO_SetBits(FINGER_SENSOR_POWER_GPIO_PORT, FINGER_SENSOR_POWER_GPIO_PIN);}else{GPIO_ResetBits(FINGER_SENSOR_POWER_GPIO_PORT, FINGER_SENSOR_POWER_GPIO_PIN);};}while(0)

/*控制DSP电源的通断*/
#define FINGER_DSP_POWER_ENABLE()								FINGER_DSP_POWER_GPIO_WRITE(0)
#define FINGER_DSP_POWER_DISABLE()							FINGER_DSP_POWER_GPIO_WRITE(1)

/*控制传感器电源的通断*/
#define FINGER_SENSOR_POWER_ENABLE()						FINGER_SENSOR_POWER_GPIO_WRITE(0)
#define FINGER_SENSOR_POWER_DISABLE()						FINGER_SENSOR_POWER_GPIO_WRITE(1)

/*重发次数*/
#define FINGER_PACK_SEND_TRYCOUNT					((uint8_t)3)

/*等待超时*/
#define FINGER_PACK_SEND_TIMEOUT					((uint16_t)1000)

/*定义包头*/
#define FINGER_PACK_HEAD_VALUE						((uint16_t)0xEF01)

/*定义地址*/
#define FINGER_PACK_ADDRESS_VALUE					((uint32_t)0xFFFFFFFF)

/*收发包最小数据长度*/
#define FINGER_PACK_SIZE_MIN							((uint8_t)12)

/*指纹探测函数周期*/
#define FINGER_DETECT_PERIOD							(100u)

/*指纹模组上电等待时间*/
#define FINGER_POWERUP_WAIT_TIMEOUT				(500u)

/*包头+地址+类型+长度，四个成员固定长度的值*/
#define FINGER_HEAD_ADDRESS_TYPE_LENGTH_SIZE_VALUE	((uint8_t)9)

/*包头在数组中的索引*/
#define FINGER_PACK_HEAD_ARRAY_INDEX_START		((uint8_t)0)
/*地址在数组中的索引*/
#define FINGER_PACK_ADDRESS_ARRAY_INDEX_START	((uint8_t)2)
/*包类型在数组中的索引*/
#define FINGER_PACK_TYPE_ARRAY_INDEX_START		((uint8_t)6)
/*包长度在数组中的索引*/
#define FINGER_PACK_LENGTH_ARRAY_INDEX_START	((uint8_t)7)
/*包数据在数组中的索引*/
#define FINGER_PACK_DATA_ARRAY_INDEX_START		((uint8_t)9)

/*布尔值定义*/
typedef enum
{
	Finger_False	=	0,
	Finger_True		=	!Finger_False
} Finger_Bool;

/*错误状态定义*/
typedef enum
{
	Finger_Error		=	0,
	Finger_Success	=	!Finger_Error
} Finger_ErrorStatus;

/*定义指纹驱动的句柄*/
typedef struct
{
	__IO uint8_t 			RxCounter;			/*接收数据计数器*/
	__IO uint8_t 			*pRxBuffer;			/*接收数据缓存指针*/
	__IO Finger_Bool 	RxComplete;			/*接收完成中断标志位*/
} Finger_HandleTypeDef;


/*通讯包类型定义*/
typedef enum
{
	Finger_Pack_Cmd					= (uint8_t)0x01,			/*指令包，由上位机发送给指纹模组*/
	Finger_Pack_Data				= (uint8_t)0x02,			/*数据包，且非结束包*/
	Finger_Pack_DataEnd			= (uint8_t)0x08,			/*数据包，并且是最后一个数据包*/
	Finger_Pack_Reply				= (uint8_t)0x07				/*应答包，由指纹模组发送给上位机*/
} Finger_PackTypeDef;


/*指令*/
typedef enum
{
	Finger_Command_GetImage 							= (uint8_t)0x01,	/*验证用获取图像*/
	Finger_Command_GenChar 								= (uint8_t)0x02,	/*根据原始图像生成指纹特征存于特征文件缓冲区*/
	Finger_Command_Match 									= (uint8_t)0x03,	/*精确比对特征文件缓冲区中的特征文件*/
	Finger_Command_Search 								= (uint8_t)0x04,	/*以特征文件缓冲区中的特征文件搜索整个或部分指纹库*/
	Finger_Command_RegModel 							= (uint8_t)0x05,	/*将特征文件合并生成模板存于特征文件缓冲区*/
	Finger_Command_StoreChar 							= (uint8_t)0x06,	/*将特征缓冲区中的文件储存到flash指纹库中*/
	Finger_Command_LoadChar							  = (uint8_t)0x07,	/*从flash指纹库中读取一个模板到特征缓冲区*/
	Finger_Command_UpChar 								= (uint8_t)0x08,	/*将特征缓冲区中的文件上传给上位机*/
	Finger_Command_DownChar							  = (uint8_t)0x09,	/*从上位机下载一个特征文件到特征缓冲区*/
	Finger_Command_UpImage 								= (uint8_t)0x0A,	/*上传原始图像*/
	Finger_Command_DownImage 							= (uint8_t)0x0B,	/*下载原始图像*/
	Finger_Command_DeletChar 							= (uint8_t)0x0C,	/*删除flash指纹库中的一个或多个特征文件*/
	Finger_Command_Empty 									= (uint8_t)0x0D,	/*清空flash指纹库*/
	Finger_Command_WriteReg 							= (uint8_t)0x0E,	/*写SOC系统寄存器*/
	Finger_Command_ReadSysPara					  = (uint8_t)0x0F,	/*读系统基本参数*/
	Finger_Command_SetPwd								  = (uint8_t)0x12,	/*设置设备握手口令*/
	Finger_Command_VfyPwd								  = (uint8_t)0x13,	/*验证设备握手口令*/
	Finger_Command_GetRandomCode				  = (uint8_t)0x14,	/*采样随机数*/
	Finger_Command_SetChipAddr					  = (uint8_t)0x15,	/*设置芯片地址*/
	Finger_Command_ReadINFpage 						= (uint8_t)0x16,	/*读取FLASH Information Page内容*/
	Finger_Command_Port_Control					  = (uint8_t)0x17,	/*通讯端口(UART/USB)开关控制*/
	Finger_Command_WriteNotepad 					= (uint8_t)0x18,	/*写记事本*/
	Finger_Command_ReadNotepad 						= (uint8_t)0x19,	/*读记事本*/
	Finger_Command_BurnCode 							= (uint8_t)0x1A,	/*烧写片内FLASH*/
	Finger_Command_HighSpeedSearch 				= (uint8_t)0x1B,	/*高速搜索FLASH*/
	Finger_Command_GenBinImage 						= (uint8_t)0x1C,	/*生成二值化指纹图像*/
	Finger_Command_ValidTempleteNum 			= (uint8_t)0x1D,	/*读有效模板个数*/
	Finger_Command_UserGPIOCommand 				= (uint8_t)0x1E,	/*用户GPIO控制命令*/
	Finger_Command_ReadIndexTable 				= (uint8_t)0x1F,	/*读索引表*/
	Finger_Command_GetEnrollImage 				= (uint8_t)0x29,	/*注册用获取图像*/
	Finger_Command_Cancle 								= (uint8_t)0x30,	/*取消指令*/
	Finger_Command_AutoEnroll 						= (uint8_t)0x31,	/*自动注册模板指令*/
	Finger_Command_AutoIdentify 					= (uint8_t)0x32,	/*自动验证指纹指令*/
	Finger_Command_Sleep 									= (uint8_t)0x33,	/*休眠指令*/
	Finger_Command_GetChiCMN 							= (uint8_t)0x34,	/*读取芯片唯一序列号*/
	Finger_Command_HandShake 							= (uint8_t)0x35,	/*握手指令*/
	Finger_Command_CheckSensor 						= (uint8_t)0x36	/*校验传感器*/
} Finger_CommandTypeDef;


/*指令应答*/
typedef enum
{
	Finger_Reply_SUCCESS 								= (uint8_t)0x00,	/*表示指令执行完毕或OK*/
	Finger_Reply_DataError 							= (uint8_t)0x01,	/*表示数据包接收错误*/
	Finger_Reply_NoFinger 							= (uint8_t)0x02,	/*表示传感器上没有手指*/
	Finger_Reply_ImageError 						= (uint8_t)0x03,	/*表示录入指纹图像失败*/
	Finger_Reply_ImageDefective0 				= (uint8_t)0x04,	/*表示指纹图像太干、太淡而生不成特征*/
	Finger_Reply_ImageDefective1 				= (uint8_t)0x05,	/*表示指纹图像太湿、太糊而生不成特征*/
	Finger_Reply_ImageDefective2 				= (uint8_t)0x06,	/*表示图像太乱而生不成特征*/
	Finger_Reply_CharFew 								= (uint8_t)0x07,	/*表示图像正常，但特征点太少（或面积太少）而生不成特征*/
	Finger_Reply_FMismatch 							= (uint8_t)0x08,	/*表示指纹不匹配*/
	Finger_Reply_FNoSearch 							= (uint8_t)0x09,	/*表示没搜索到指纹*/
	Finger_Reply_RegModelError 					= (uint8_t)0x0A,	/*表示特征合并失败*/
	Finger_Reply_FlashCross 						= (uint8_t)0x0B,	/*表示访问指纹库时地址序号超出指纹库范围*/
	Finger_Reply_FlashError 						= (uint8_t)0x0C,	/*表示从指纹库读取模板出错或无效*/
	Finger_Reply_UpCharError 						= (uint8_t)0x0D,	/*表示上传特征失败*/
	Finger_Reply_ReceiveError 					= (uint8_t)0x0E,	/*表示模块不能接收后续数据包*/
	Finger_Reply_UpImageError 					= (uint8_t)0x0F,	/*表示上传图像失败*/
	Finger_Reply_DeletCharError 				= (uint8_t)0x10,	/*表示删除模板失败*/
	Finger_Reply_EmptyError 						= (uint8_t)0x11,	/*表示清空指纹库失败*/
	Finger_Reply_SleepError 						= (uint8_t)0x12,	/*表示不能进入低功耗状态*/
	Finger_Reply_PwdError 							= (uint8_t)0x13,	/*表示口令不正确*/
	Finger_Reply_ResetError 						= (uint8_t)0x14,	/*表示系统复位失败*/
	Finger_Reply_BuffNoImage 						= (uint8_t)0x15,	/*表示缓冲区内没有有效原始图而生不成图像*/
	Finger_Reply_UpDateError 						= (uint8_t)0x16,	/*表示在线升级失败*/
	Finger_Reply_FingerNoMove 					= (uint8_t)0x17,	/*表示残留的指纹或两次采集之间手指没有移动过*/
	Finger_Reply_RWFlashError 					= (uint8_t)0x18,	/*表示读写FLASH出错*/
	Finger_Reply_RandomCode_Error 			= (uint8_t)0x19,	/*随机数生成失败*/
	Finger_Reply_RegNumber_Error 				= (uint8_t)0x1A,	/*无效寄存器号*/
	Finger_Reply_RegValue_Error 				= (uint8_t)0x1B,	/*寄存器设定内容错误号*/
	Finger_Reply_NotePage_Error 				= (uint8_t)0x1C,	/*笔记本页码指定错误*/
	Finger_Reply_Port_Error 						= (uint8_t)0x1D,	/*端口操作失败*/
	Finger_Reply_AutoEnroll_Error 			= (uint8_t)0x1E,	/*自动注册失败*/
	Finger_Reply_Flash_Full 						= (uint8_t)0x1F,	/*指纹库满*/
	Finger_Reply_DeviAddr_Error 				= (uint8_t)0x20,	/*设备地址错误*/
	Finger_Reply_PassError 							= (uint8_t)0x21,	/*密码有误*/
	Finger_Reply_FModelNoEmpty				  = (uint8_t)0x22,	/*指纹模板非空*/
	Finger_Reply_FModelEmpty 						= (uint8_t)0x23,	/*指纹模板为空*/
	Finger_Reply_Flash_Empty 						= (uint8_t)0x24,	/*指纹库为空*/
	Finger_Reply_EnrCount_Error 				= (uint8_t)0x25,	/*录入次数设置错误*/
	Finger_Reply_TimeOut 								= (uint8_t)0x26,	/*超时*/
	Finger_Reply_FingerIsExist 					= (uint8_t)0x27,	/*指纹已存在*/
	Finger_Reply_FmodelParallel 				= (uint8_t)0x28,	/*指纹模板有并联*/
	Finger_Reply_SensorInit_Error 			= (uint8_t)0x29,	/*传感器初始化失败*/
	Finger_Reply_SUCCESS_DATA 					= (uint8_t)0xF0,	/*有后续数据包的指令，正确接收后用(uint8_t)0xF0应答*/
	Finger_Reply_SUCCESS_COMD 					= (uint8_t)0xF1,	/*有后续数据包的指令，命令包用(uint8_t)0xF1应答*/
	Finger_Reply_WFlash_SumError 				= (uint8_t)0xF2,	/*表示烧写内部FLASH时，校验和错误*/
	Finger_Reply_WFlash_IdeError 				= (uint8_t)0xF3,	/*表示烧写内部FLASH时，包标识错误*/
	Finger_Reply_WFlash_LenError 				= (uint8_t)0xF4,	/*表示烧写内部FLASH时，包长度错误*/
	Finger_Reply_WFlash_CodError 				= (uint8_t)0xF5,	/*表示烧写内部FLASH时，代码长度太长*/
	Finger_Reply_WFlash_Error 					= (uint8_t)0xF6,	/*表示烧写内部FLASH时，烧写FLASH失败*/
}	Finger_Reply_TypeDef;


/*验证用获取图像的指令包结构*/
typedef struct
{
	uint16_t 								Head;			/*包头*/
	uint32_t 								Address;	/*地址*/
	Finger_PackTypeDef 			Type;			/*包类型*/
	uint16_t 								Length;		/*长度*/
	Finger_CommandTypeDef 	Command;	/*指令*/
	uint16_t 								Sum;			/*校验和*/
} Finger_Pack_Command_GetImage_TypeDef;

/*验证用获取图像的回应包结构*/
typedef struct
{
	uint16_t 								Head;			/*包头*/
	uint32_t 								Address;	/*地址*/
	Finger_PackTypeDef 			Type;			/*包类型*/
	uint16_t 								Length;		/*长度*/
	Finger_Reply_TypeDef		Result;		/*应答结果*/
	uint16_t 								Sum;			/*校验和*/
} Finger_Pack_Reply_GetImage_TypeDef;

/*生成特征值指令包结构*/
typedef struct
{
	uint16_t 								Head;			/*包头*/
	uint32_t 								Address;	/*地址*/
	Finger_PackTypeDef 			Type;			/*包类型*/
	uint16_t 								Length;		/*长度*/
	Finger_CommandTypeDef 	Command;	/*指令*/
	uint8_t									BufferID;	/*特征缓冲区号*/
	uint16_t 								Sum;			/*校验和*/
} Finger_Pack_Command_GenChar_TypeDef;

/*生成特征值指令的回应包结构*/
typedef struct
{
	uint16_t 								Head;			/*包头*/
	uint32_t 								Address;	/*地址*/
	Finger_PackTypeDef 			Type;			/*包类型*/
	uint16_t 								Length;		/*长度*/
	Finger_Reply_TypeDef		Result;		/*应答结果*/
	uint16_t 								Sum;			/*校验和*/
} Finger_Pack_Reply_GenChar_TypeDef;

/*搜索特征指令包结构*/
typedef struct
{
	uint16_t 								Head;				/*包头*/
	uint32_t 								Address;		/*地址*/
	Finger_PackTypeDef 			Type;				/*包类型*/
	uint16_t 								Length;			/*长度*/
	Finger_CommandTypeDef 	Command;		/*指令*/
	uint8_t									BufferID;		/*特征缓冲区号*/
	uint16_t								StartID;		/*搜索模板范围的起始编号*/
	uint16_t								TempCount;	/*搜索的模板数量*/
	uint16_t 								Sum;				/*校验和*/
} Finger_Pack_Command_Search_TypeDef;

/*搜索特征回应包结构*/
typedef struct
{
	uint16_t 								Head;				/*包头*/
	uint32_t 								Address;		/*地址*/
	Finger_PackTypeDef 			Type;				/*包类型*/
	uint16_t 								Length;			/*长度*/
	Finger_Reply_TypeDef		Result;			/*应答结果*/
	uint16_t								TempID;			/*匹配上的指纹特征编号*/
	uint16_t								Score;			/*匹配得分*/
	uint16_t 								Sum;				/*校验和*/
} Finger_Pack_Reply_Search_TypeDef;

/*生成模板指令包结构*/
typedef struct
{
	uint16_t 								Head;				/*包头*/
	uint32_t 								Address;		/*地址*/
	Finger_PackTypeDef 			Type;				/*包类型*/
	uint16_t 								Length;			/*长度*/
	Finger_CommandTypeDef 	Command;		/*指令*/
	uint16_t 								Sum;				/*校验和*/
} Finger_Pack_Command_RegModel_TypeDef;

/*生成模板回应包结构*/
typedef struct
{
	uint16_t 								Head;				/*包头*/
	uint32_t 								Address;		/*地址*/
	Finger_PackTypeDef 			Type;				/*包类型*/
	uint16_t 								Length;			/*长度*/
	Finger_Reply_TypeDef		Result;			/*应答结果*/
	uint16_t 								Sum;				/*校验和*/
} Finger_Pack_Reply_RegModel_TypeDef;

/*存储模板指令包结构*/
typedef struct
{
	uint16_t 								Head;				/*包头*/
	uint32_t 								Address;		/*地址*/
	Finger_PackTypeDef 			Type;				/*包类型*/
	uint16_t 								Length;			/*长度*/
	Finger_CommandTypeDef 	Command;		/*指令*/
	uint8_t									BufferID;		/*存储的缓冲区号*/
	uint16_t								TempID;			/*存储的特征序列号*/	
	uint16_t 								Sum;				/*校验和*/
} Finger_Pack_Command_StoreChar_TypeDef;

/*存储模板回应包结构*/
typedef struct
{
	uint16_t 								Head;				/*包头*/
	uint32_t 								Address;		/*地址*/
	Finger_PackTypeDef 			Type;				/*包类型*/
	uint16_t 								Length;			/*长度*/
	Finger_Reply_TypeDef		Result;			/*应答结果*/
	uint16_t 								Sum;				/*校验和*/
} Finger_Pack_Reply_StoreChar_TypeDef;

/*删除模板指令包结构*/
typedef struct
{
	uint16_t 								Head;				/*包头*/
	uint32_t 								Address;		/*地址*/
	Finger_PackTypeDef 			Type;				/*包类型*/
	uint16_t 								Length;			/*长度*/
	Finger_CommandTypeDef 	Command;		/*指令*/
	uint16_t								TempID;			/*存储的缓冲区号*/
	uint16_t								TempCount;	/*存储的特征序列号*/	
	uint16_t 								Sum;				/*校验和*/
} Finger_Pack_Command_DeletChar_TypeDef;

/*删除模板回应包结构*/
typedef struct
{
	uint16_t 								Head;				/*包头*/
	uint32_t 								Address;		/*地址*/
	Finger_PackTypeDef 			Type;				/*包类型*/
	uint16_t 								Length;			/*长度*/
	Finger_Reply_TypeDef		Result;			/*应答结果*/
	uint16_t 								Sum;				/*校验和*/
} Finger_Pack_Reply_DeletChar_TypeDef;

/*清空全部模板指令包结构*/
typedef struct
{
	uint16_t 								Head;			/*包头*/
	uint32_t 								Address;	/*地址*/
	Finger_PackTypeDef 			Type;			/*包类型*/
	uint16_t 								Length;		/*长度*/
	Finger_CommandTypeDef 	Command;	/*指令*/
	uint16_t 								Sum;			/*校验和*/
} Finger_Pack_Command_Empty_TypeDef;

/*清空全部模板回应包结构*/
typedef struct
{
	uint16_t 								Head;			/*包头*/
	uint32_t 								Address;	/*地址*/
	Finger_PackTypeDef 			Type;			/*包类型*/
	uint16_t 								Length;		/*长度*/
	Finger_Reply_TypeDef		Result;		/*应答结果*/
	uint16_t 								Sum;			/*校验和*/
} Finger_Pack_Reply_Empty_TypeDef;

/*注册用用获取图像的指令包结构*/
typedef struct
{
	uint16_t 								Head;			/*包头*/
	uint32_t 								Address;	/*地址*/
	Finger_PackTypeDef 			Type;			/*包类型*/
	uint16_t 								Length;		/*长度*/
	Finger_CommandTypeDef 	Command;	/*指令*/
	uint16_t 								Sum;			/*校验和*/
} Finger_Pack_Command_GetEnrollImage_TypeDef;

/*注册用用获取图像的回应包结构*/
typedef struct
{
	uint16_t 								Head;			/*包头*/
	uint32_t 								Address;	/*地址*/
	Finger_PackTypeDef 			Type;			/*包类型*/
	uint16_t 								Length;		/*长度*/
	Finger_Reply_TypeDef		Result;		/*应答结果*/
	uint16_t 								Sum;			/*校验和*/
} Finger_Pack_Reply_GetEnrollImage_TypeDef;


/**
  * @brief  指纹模组初始化函数
  * @param  None
  * @retval None
  */
void Finger_Init(void);

/**
  * @brief  串口接收中断回调函数
  * @param  Data:接收到的数据
  * @retval None
  */
void Finger_UartRx_IntCallback(uint8_t Data, Finger_Bool Complete);

/**
  * @brief  触摸信号中断回调函数
  * @param  None
  * @retval None
  */
void Finger_Signal_IntCallback(void);

/**
  * @brief  串口发送数据
  * @param  pData:要发送的数据
  *	@param	Length:发送的数据长度
  * @retval None
  */
void Finger_SendData(const uint8_t *pData, uint8_t Length);

/**
  * @brief  清空接收缓存
  * @param  None
  * @retval None
  */
void Finger_RxBufferClear(void);

/**
  * @brief  获取接收缓存
  * @param  None
  * @retval 返回接收缓存的指针
  */
uint8_t *Finger_GetRxBuffer(void);

/**
  * @brief  获取接收缓存计数
  * @param  None
  * @retval 返回接收缓存计数
  */
uint8_t Finger_GetRxCounter(void);

/**
  * @brief  获取接收完成标志位
  * @param  None
  * @retval 返回接收是否完成
  *		#Finger_False
  *		#Finger_True
  */
Finger_Bool Finger_GetRxComplete(void);

/**
  * @brief  获取图像
  * @param  None
  * @retval 传感器是否有图像
  *		#Finger_Success
  *		#Finger_Error
  */
Finger_ErrorStatus Finger_CommandGetImage(void);

/**
  * @brief  获取图像
  * @param  BufferID:特征缓冲区charbuffer的序号，1~5
  * @retval 传感器是否有图像
  *		#Finger_Success
  *		#Finger_Error
  */
Finger_ErrorStatus Finger_CommandGenChar(uint8_t BufferID);

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
Finger_ErrorStatus Finger_CommandSearch(uint8_t BufferID, uint16_t StartID, uint16_t TempCount, uint16_t *UserID);

/**
  * @brief  将CharBuffer1~2合并生成一个模板，并存储到CharBuffer1中
  * @param  None
  * @retval 返回执行结果
  *		#Finger_Success
  *		#Finger_Error
  */
Finger_ErrorStatus Finger_CommandRegModel(void);

/**
  * @brief  将特征缓冲区中的文件储存到flash指纹库中
  * @param  BufferID:需要存储的特征区缓冲号，固定为1！
  *	@param	TempID:存储到flash中的指纹序列号，不能超过指纹容量大小
  * @retval 返回执行结果
  *		#Finger_Success
  *		#Finger_Error
  */
Finger_ErrorStatus Finger_CommandStoreChar(uint8_t BufferID, uint16_t TempID);

/**
  * @brief  删除flash指纹库中的一个或多个特征文件
  *	@param	TempID:需要删除的模板的起始序列号
  *	@param	TempCount:要删除的模板数量
  * @retval 返回执行结果
  *		#Finger_Success
  *		#Finger_Error
  */
Finger_ErrorStatus Finger_CommandDeletChar(uint16_t TempID, uint16_t TempCount);

/**
  * @brief  清空flash指纹库
  * @param  None
  * @retval 返回执行结果
  *		#Finger_Success
  *		#Finger_Error
  */
Finger_ErrorStatus Finger_CommandEmpty(void);

/**
  * @brief  获取图像
  * @param  None
  * @retval 传感器是否有图像
  *		#Finger_Success
  *		#Finger_Error
  */
Finger_ErrorStatus Finger_CommandGetEnrollImage(void);

/**
  * @brief  清空flash指纹库
  * @param  IsEnroll:是注册还是验证？
  *		#Finger_True:注册
  *		#Finger_False:验证
  * @retval 返回执行结果
  *		#Finger_Success
  *		#Finger_Error
  */
Finger_ErrorStatus Finger_Detect(Finger_Bool IsEnroll);

/**
  * @brief  等待超时或者在此之前接收到一个0x55字节提前退出
  * @param  None
  * @retval None
  */
void Finger_Wait55(void);

/**
  * @brief  指纹模组休眠
  * @param  None
  * @retval None
  */
void Finger_Sleep(void);

/**
  * @brief  指纹模组唤醒
  * @param  None
  * @retval None
  */
void Finger_Wakeup(void);

/**
  * @brief  等待手指拿开或者超时30s
  * @param  None
  * @retval None
  */
void Finger_WaitTakeoff(void);

/**
  * @brief  在唤醒后使用，用于检测指纹头是误触发被唤醒还是真实唤醒，就是消抖读触摸引脚
  * @param  None
  * @retval None
  */
Finger_Bool Finger_isWakeup(void);



extern Finger_HandleTypeDef Finger_Handler;


#endif
