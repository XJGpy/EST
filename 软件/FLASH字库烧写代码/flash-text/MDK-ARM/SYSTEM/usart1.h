/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/usart1.h 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   
  ******************************************************************************
**/
#ifndef __USART1_H
#define __USART1_H

#include "sys.h"
#include "lego_include.h"

#define EN_USART1_RX 					1						//ʹ�ܣ�1��/��ֹ��0������1����

extern UBYTE Uart1RecBuf[];     //���ջ���,���USART_REC_LEN���ֽ�.
extern u16 Uart1RecBufIn;
extern UBYTE UsartBreakTimer;

//extern UBYTE USART1_TX_BUF[];     //���ͻ���,���USART1_SEN_LEN���ֽ�
//extern u16 USART1_TX_POINTER;
//extern u16 USART1_TX_LEN;

extern u16 USART1_RX_STA;         				//����״̬���	

//����봮���жϽ��գ��벻Ҫע�����º궨��
void usart1_init(u32 bound);

//UBYTE Uart1Write(u16 Data);
UBYTE USART1_Send_Byte(UBYTE Data);
void usart1_change(u32 bound);
	
#endif


