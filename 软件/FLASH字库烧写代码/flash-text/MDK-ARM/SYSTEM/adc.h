/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/adc.h 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   
  ******************************************************************************
**/
#ifndef __ADC_H
#define __ADC_H	

#include "sys.h" 
#include "lego_include.h"

extern __IO uint16_t ADC_BUF_16[];
extern __IO uint32_t ADC_BUF_32[];

void Adc_Init(void); 				//ADCͨ����ʼ��
u16  Get_Adc(u8 ch); 				//���ĳ��ͨ��ֵ 
u16 Get_Adc_Average(u8 ch,u8 times);//�õ�ĳ��ͨ����������������ƽ��ֵ  

#endif 
