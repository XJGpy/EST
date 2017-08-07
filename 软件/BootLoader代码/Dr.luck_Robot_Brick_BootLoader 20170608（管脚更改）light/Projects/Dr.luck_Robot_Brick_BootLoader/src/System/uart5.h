/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/uart5.h 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   
  ******************************************************************************
**/
#ifndef __UART5_H
#define __UART5_H
 
#include "sys.h" 

/* debug define --------------------------------------------------------------*/
#define  SYSTEM_DEBUG			1
//#define  INPUT_DEBUG			1
//#define  INSTRUCT_DEBUG		1
#define  RAM_DEBUG				1
/* ---------------------------------------------------------------------------*/

//#define UART5_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_UART5_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
//	  	
//extern u8  UART5_RX_BUF[UART5_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
//extern u16 UART5_RX_STA;         		//����״̬���	

//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart5_init(u32 bound);
void UART5_Send_Byte(u16 Data);

extern u8 UART5_flag;
	
#endif

