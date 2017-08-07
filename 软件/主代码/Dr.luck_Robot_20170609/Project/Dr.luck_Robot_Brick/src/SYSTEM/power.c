/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/power.c 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    06-Nov-2016
  * @brief   
  ******************************************************************************
**/
#include "power.h"

void POWER_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOC,GPIOEʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //K��Ӧ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��
	
//	GPIO_SetBits(GPIOE, GPIO_Pin_2);
} 

