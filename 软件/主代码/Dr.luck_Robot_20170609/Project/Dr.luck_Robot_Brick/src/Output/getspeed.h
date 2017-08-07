/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/getspeed.h 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   Main program body according to ST source
  ******************************************************************************
**/

#ifndef _GETSPEED_H_
#define _GETSPEED_H_

#include "sys.h"

/*
    �����������١�����
		���ܣ����١���ת��
		����źŽӿڣ�PA0��PA1
*/

//��ʼ������
void ENC_Init(void);		//�������ʼ��

//���ܺ���
void ENC_Reset(uint8_t Ochannel);
extern int ENC_GetCurrentPluse(uint8_t Ochannel);

//�Ƕȼ�����
//1msһ��
void ENC_SumAngleProcess(int SumPluse[4], int SumPluseForDisplay[4]);
//���ٽ���
//ÿ100msִ��һ��
void ENC_CurrentPowerProcess(int CurrentPower[4]);

//ȫ�ֱ���
extern uint8_t PowerProcessKey[4];
extern int CurrentPower[4];
extern int SumPluse[4];
extern int SumPluseForDisplay[4];

extern unsigned int Runtime_PID[4];

#endif
