/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/pwm.h
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   Main program body according to ST source
  ******************************************************************************
**/

#ifndef _PWM_H
#define _PWM_H

#include "sys.h"
#include "OptimizePid.h"

#define PORT_A0    PGout(10) 
#define PORT_A1    PGout(11) 

#define PORT_B0    PDout(4) 
#define PORT_B1    PDout(5)

#define PORT_C0    PGout(7) 
#define PORT_C1    PGout(8) 

#define PORT_D0    PGout(6) 
#define PORT_D1    PGout(5) 


/*
	ʹ�÷�����
	��ʼ����
			TIM3_PWM_Init(100-1,84-1);	//84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ500������PWMƵ��Ϊ 1M/500=2Khz.  
	�޸�ռ�ձȣ�
			TIM_SetCompare1(TIM3,100);	//�޸ıȽ�ֵ���޸�ռ�ձ�,0-100,
	�趨TIM3��ͨ��1����Ӧ�ܽ�ΪPA6
*/

void PWM_Init(void);
void TIM4_PWM_Init(u32 arr,u32 psc);
void PWM_IO_Init(void);

//���ܺ���
void Motor_DirectionSet(uint8_t OChannel, int MotorDirection);
void Motor_PowerSet(uint8_t OChannel, int MotorSpeed);

#endif
