/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/usart2.h 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   
  ******************************************************************************
**/
#ifndef __USART2_H
#define __USART2_H

#include "sys.h"
#include "lego_include.h"

#define EN_USART2_RX 					1						//ʹ�ܣ�1��/��ֹ��0������1����

extern UBYTE Uart2RecBuf[];     //���ջ���,���USART_REC_LEN���ֽ�.
extern u16 Uart2RecBufIn;

//extern UBYTE USART2_TX_BUF[];     //���ͻ���,���USART2_SEN_LEN���ֽ�
//extern u16 USART2_TX_POINTER;
//extern u16 USART2_TX_LEN;

extern u16 USART2_RX_STA;         				//����״̬���	

//����봮���жϽ��գ��벻Ҫע�����º궨��
void usart2_init(u32 bound);
//UBYTE Uart2Write(u16 Data);

UBYTE USART2_Send_Byte(UBYTE Data);
void usart2_change(u32 bound);
	
#endif


