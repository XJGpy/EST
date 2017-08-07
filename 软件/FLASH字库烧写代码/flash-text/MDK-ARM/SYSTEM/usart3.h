/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/usart3.h 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   
  ******************************************************************************
**/
#ifndef __USART3_H
#define __USART3_H

#include "sys.h"
#include "lego_include.h"

#define EN_USART3_RX 					1						//ʹ�ܣ�1��/��ֹ��0������1����

extern UBYTE Uart3RecBuf[];     //���ջ���,���USART_REC_LEN���ֽ�.
extern u16 Uart3RecBufIn;
extern UBYTE Usart3BreakTimer;

//extern UBYTE USART3_TX_BUF[];     //���ͻ���,���USART3_SEN_LEN���ֽ�
//extern u16 USART3_TX_POINTER;
//extern u16 USART3_TX_LEN;

extern u16 USART3_RX_STA;         				//����״̬���	

//����봮���жϽ��գ��벻Ҫע�����º궨��
void usart3_init(u32 bound);

//UBYTE Uart3Write(u16 Data);
UBYTE USART3_Send_Byte(UBYTE Data);
void usart3_change(u32 bound);
	
#endif


