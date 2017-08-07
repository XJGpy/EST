/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/uart5.h 
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
#define  SYSTEM_DEBUG			1
#define  INPUT_DEBUG			1
#define  INSTRUCT_DEBUG		1
//#define  RAM_DEBUG				1
//#define  CASCADE_DEBUG				1
/* ---------------------------------------------------------------------------*/

//#define USART6_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART6_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
//	  	
//extern u8  USART6_RX_BUF[USART6_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
//extern u16 USART6_RX_STA;         		//����״̬���	

//����봮���жϽ��գ��벻Ҫע�����º궨��
extern void usart6_init(u32 bound);
void USART6_Send_Byte(u8 Data);


extern int usart6_flag;
	
#endif

