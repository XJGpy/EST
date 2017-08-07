/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/usart6.h 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   
  ******************************************************************************
**/
#ifndef __USART6_H
#define __USART6_H
 
#include "sys.h" 

/* debug define --------------------------------------------------------------*/
#define  SYSTEM_DEBUG
//#define  INPUT_DEBUG
//#define  INSTRUCT_DEBUG
//#define  RAM_DEBUG
//#define  CASCADE_DEBUG
/* ---------------------------------------------------------------------------*/

#define USART6_REC_LEN  			1024  	//�����������ֽ��� 200
#define EN_USART6_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
//
extern u8  USART6_RX_BUF[USART6_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART6_RX_STA;         		//����״̬���	

//����봮���жϽ��գ��벻Ҫע�����º궨��
void usart6_init(u32 bound);
void USART6_Send_Byte(u16 Data);
int USART6_Receive(const char *str);
extern int usart6_flag;
extern u8 status;

#endif

