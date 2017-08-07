/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/flash.h 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief  
  ******************************************************************************
**/
#ifndef __FLASH_H__
#define __FLASH_H__
#include "stm32f4xx.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 2048 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1                      //ʹ��FLASHд��(0��������;1��ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////


//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
//FLASH������ֵ
//#define RDP_Key                  ((uint16_t)0x00A5)
//#define FLASH_KEY1               ((uint32_t)0x45670123)
//#define FLASH_KEY2               ((uint32_t)0xCDEF89AB)
 
 
#define APP_CONFIG_ADDR 		0X08008010	//���õ�ַ
#define APP_CONFIG_SET_VALUE	0X5555	//����ֵ
#define APP_CONFIG_CLEAR_VALUE	0x0000	//����ֵ

#define Update_Data_Length_L16 		0x08008000	//���õ�ַ
#define Update_Data_Length_H16 		0x08008008	//���õ�ַ

#define UPDATE_ADDRESS        (uint32_t)0x08080000 

u16 STMFLASH_ReadHalfWord(u32 faddr);		  //��������  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);				//ָ����ַ��ʼ��ȡָ����������
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);
//����д��
void Test_Write(u32 WriteAddr,u16 WriteData);								   
#endif

















