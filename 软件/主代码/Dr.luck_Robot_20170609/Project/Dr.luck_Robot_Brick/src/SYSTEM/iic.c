/**
  ******************************************************************************
  * @file    EST Brick of Dr.luck/iic.c 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   
  ******************************************************************************
**/
#include "main.h"
#include "iic.h"
#include "delay.h"

//��ʼ��IIC

//GPIO_TypeDef* GPIO_DIGI0_PORT[4] = {GPIOF,GPIOF,GPIOG,GPIOE};
//const uint16_t GPIO_DIGI0_PIN[4] = {GPIO_Pin_9,GPIO_Pin_13,GPIO_Pin_1,GPIO_Pin_11};
//GPIO_TypeDef* GPIO_DIGI1_PORT[4] = {GPIOF,GPIOF,GPIOG,GPIOE};
//const uint16_t GPIO_DIGI1_PIN[4] = {GPIO_Pin_7,GPIO_Pin_11,GPIO_Pin_0,GPIO_Pin_12};

//void IIC_init_port(u8 Port)
//{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);			//ʹ��GPIOEʱ��
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_DIGI0_PIN[Port];
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;							//��ͨ���ģʽ
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;						//�������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//100MHz
////	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;							//����
//	GPIO_Init(GPIO_DIGI0_PORT[Port], &GPIO_InitStructure);										//��ʼ��
//		
//	GPIO_InitStructure.GPIO_Pin = GPIO_DIGI1_PIN[Port];
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;							//��ͨ���ģʽ
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;						//�������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//100MHz
////	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;							//����
//	GPIO_Init(GPIO_DIGI1_PORT[Port], &GPIO_InitStructure);										//��ʼ��
//}
	

void IIC_Init(void)
{	//DIGIA0		DIGIA1		DIGIB0		DIGIB1		DIGIC0		DIGIC1		DIGID0		DIGID1
	//F9		 		F7				F13				F11				G1				G0				E11				E12
	
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);			//ʹ��GPIOEʱ��

	//5������
	
  //��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;							//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;						//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;							//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);										//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;							//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;						//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;							//����
  GPIO_Init(GPIOF, &GPIO_InitStructure);										//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;							//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;						//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;							//����
  GPIO_Init(GPIOG, &GPIO_InitStructure);										//��ʼ��

	//6������

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;							//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;						//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;							//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);										//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;							//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;						//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;							//����
  GPIO_Init(GPIOF, &GPIO_InitStructure);										//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;							//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;						//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;							//����
  GPIO_Init(GPIOG, &GPIO_InitStructure);										//��ʼ��

//	IIC_SCL_1	=	1;	IIC_SDA_1	=	1;
//	IIC_SCL_2	=	1;	IIC_SDA_2	=	1;
//	IIC_SCL_3	=	1;	IIC_SDA_3	=	1;
//	IIC_SCL_4	=	1;	IIC_SDA_4	=	1;

}

