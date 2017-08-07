/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/C_Protocol.h
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   
  ******************************************************************************
**/
/*
	ʹ�÷�����
	1.��usbd_hid_core.c�е�USBD_HID_DataOut�����
		//-----------------------HID����֡������ÿ�����޶�����64��byte--------------------------------------
	if(!Receive_start&&!USB_Rx_Length)
	{
		for(i=0; i<USB_Rx_Cnt; i++)							//Ѱ�ҿ�ʼ��,USB_Rx_CntΪ64
		{
			if(*(USB_HID_Rx_Buffer + i) == 0x68) 	//���յ���ʼ��
			{
				Receive_start = 1;
				USB_TimePic = 3000; 								//����ʱ���ʱ
				break;															//��ת��i�����Լ�1
			}
		}
	}
	if(Receive_start)
	{
		if(!USB_Rx_Length)			//��һ�����ݰ�
			for(; i<USB_Rx_Cnt; i++) 
				USB_Rx_Buff[USB_Rx_Length++] = *(USB_HID_Rx_Buffer + i); 
		else 										//�������������ݰ�
			for(i=0; i<USB_Rx_Cnt; i++) 
				USB_Rx_Buff[USB_Rx_Length++] = *(USB_HID_Rx_Buffer + i); 
		
		if(USB_Rx_Length-1>4)		//����������
		{
			if((USB_Rx_Length>5+USB_Rx_Buff[3]+USB_Rx_Buff[4]*256+1))  				 //�ܿ����������ݵ��ڽ���������
			{
				for(i=6+USB_Rx_Buff[3]+USB_Rx_Buff[4]*256; i<USB_Rx_Length; i++) //Ѱ�ҽ�����
				{
					if(*(USB_Rx_Buff + i) == 0x16) 		//���յ�������
					{
						Receive_start = 0; 					 		//��֡���ݽ��ս���������Ȩ������һ֡����
						USB_Rx_Length = i+1; 				 		//��ȥ���������޹ص�����
						break;
					}
				}		
			}		
		}
	}
		
	2.��SysTick_Handler(void)�����
		//1msִ��һ��
		FrameOrFlashBuffLoseCheck();
		
	3.���������е���CheckOrder();

*/

#ifndef __C_PROTOCOL_H
#define __C_PROTOCOL_H

#include "sys.h"
#include "usart6.h"
#include "usbd_cdc_vcp.h"
#include "disassembly.h"
#include "usbd_hid_core.h"
#include "outputworkmode.h"
#include "flash.h"
#include "iap.h"

extern uint8_t USB_Rx_Buff[1024];
extern uint8_t Receive_start;
extern uint8_t Receive_end;

extern unsigned int USB_Rx_Length;
extern unsigned int USB_TimePic;
extern uint8_t errorflag;
extern uint8_t Channel_State[32][13];
extern uint8_t UserWareData[];	
extern u8 usbmasterflag;
extern int usbmastertimems;

extern uint8_t updateflash_start;				//�����ձ�־λ 

extern void EnterProgram(u8 myfilename[]);
extern void EndProgram(void);

extern void CheckOrder(void);
extern void FrameOrFlashBuffLoseCheck(void);
extern void UpdateState(u8 Channel_State[32][13], u8 port, u8 id, u8 mode, float SensorData);

#endif
