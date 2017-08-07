/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/key,h
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   
  ******************************************************************************
**/
#ifndef __KEY_H
#define __KEY_H	 

#include "sys.h" 

/*����ķ�ʽ��ͨ��ֱ�Ӳ����⺯����ʽ��ȡIO*/
#define KEY0 		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14)	//PC14
#define KEY1 		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15)	//PC15
#define KEY2 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_0) 	//PF0
#define KEY3 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3) 	//PE3
#define KEY4 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_1)		//PF1
#define KEY5 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) 	//PE4

void KEY_Init(void);	//IO��ʼ��
u8 KEY_Scan(u8);  		//����ɨ�躯��	

#endif
