/**
  ******************************************************************************
  * @file    EST Brick of Dr.luck/iic.h 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   
  ******************************************************************************
**/
#ifndef __MYIIC_H
#define __MYIIC_H

#include "sys.h" 

//DIGIA0		DIGIA1		DIGIB0		DIGIB1		DIGIC0		DIGIC1		DIGID0		DIGID1
//F9		 		F7				F13				F11				G1				G0				E11				E12
	
//IO��������
#define SDA_IN_1()  {GPIOF->MODER&=~(3<<(7*2));GPIOF->MODER|=0<<7*2;}//GPIOF->PUPDR&=~(3<<(7*2));GPIOF->PUPDR|=2<<7*2;}	//PF7����ģʽ
#define SDA_OUT_1() {GPIOF->MODER&=~(3<<(7*2));GPIOF->MODER|=1<<7*2;} //PF7���ģʽ

#define SDA_IN_2()  {GPIOF->MODER&=~(3<<(11*2));GPIOF->MODER|=0<<11*2;}	//PF11����ģʽ
#define SDA_OUT_2() {GPIOF->MODER&=~(3<<(11*2));GPIOF->MODER|=1<<11*2;} //PF11���ģʽ

#define SDA_IN_3()  {GPIOG->MODER&=~(3<<(0*2));GPIOG->MODER|=0<<0*2;}	//PG0����ģʽ
#define SDA_OUT_3() {GPIOG->MODER&=~(3<<(0*2));GPIOG->MODER|=1<<0*2;} //PG0���ģʽ

#define SDA_IN_4()  {GPIOE->MODER&=~(3<<(12*2));GPIOE->MODER|=0<<12*2;}	//PE12����ģʽ
#define SDA_OUT_4() {GPIOE->MODER&=~(3<<(12*2));GPIOE->MODER|=1<<12*2;} //PE12���ģʽ

//IO��������	 
#define IIC_SCL_1    PFout(9) //SCL
#define IIC_SDA_1    PFout(7) //SDA
#define READ_SDA_1   PFin(7)  //����SDA

#define IIC_SCL_2    PFout(13) //SCL
#define IIC_SDA_2    PFout(11) //SDA
#define READ_SDA_2   PFin(11)  //����SDA

#define IIC_SCL_3    PGout(1) //SCL
#define IIC_SDA_3    PGout(0) //SDA
#define READ_SDA_3   PGin(0)  //����SDA

#define IIC_SCL_4    PEout(11) //SCL
#define IIC_SDA_4    PEout(12) //SDA
#define READ_SDA_4   PEin(12)  //����SDA

////IIC���в�������
void IIC_Init(void);          //��ʼ��IIC��IO��				 
void IIC_init_port(u8 Port);

#endif

