#ifndef __WKUP_H
#define __WKUP_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//�������� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
					    
//���Ŷ���
/*******************************************************/
#define WKUP_INT_GPIO_PORT                GPIOE
#define WKUP_INT_GPIO_CLK                 RCC_AHB1Periph_GPIOE
#define WKUP_INT_GPIO_PIN                 GPIO_Pin_4
#define WKUP_INT_EXTI_PORTSOURCE          EXTI_PortSourceGPIOE
#define WKUP_INT_EXTI_PINSOURCE           EXTI_PinSource4
#define WKUP_INT_EXTI_LINE                EXTI_Line4
#define WKUP_INT_EXTI_IRQ                 EXTI4_IRQn

#define WKUP_IRQHandler                   EXTI4_IRQHandler

void WKUP_Init(void); 			//PE4 WKUP���ѳ�ʼ��
void Sys_Enter_Stopby(void);	//ϵͳ�������ģʽ
#endif


