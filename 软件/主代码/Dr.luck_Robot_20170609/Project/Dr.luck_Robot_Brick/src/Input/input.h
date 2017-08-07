/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/input.h
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   
  ******************************************************************************
**/
#ifndef __INPUT_H
#define __INPUT_H 	

#include "sys.h" 

void Input_init(void);
	
extern void Color(uint8_t port,uint8_t mode, uint32_t bufferrambase);							//��ɫ
extern void Gyro(uint8_t port,uint8_t mode, uint32_t bufferrambase);							//������
extern void Infrared(uint8_t port,uint8_t mode, uint32_t bufferrambase);					//���⴫��
extern void Temperature(uint8_t port,uint8_t mode, uint32_t bufferrambase);				//�¶�
extern void Touch(uint8_t port,uint8_t mode, uint32_t bufferrambase);							//����
extern void Ultrasonic(uint8_t port,uint8_t mode, uint32_t bufferrambase);				//������
extern void Recharge(uint8_t port,uint8_t mode, uint32_t bufferrambase);					//����
extern void Sound(uint8_t port,uint8_t mode, uint32_t bufferrambase);							//NXT����
extern void Motor(uint8_t port,uint8_t mode, uint32_t bufferrambase);                    //���
extern uint8_t LegoIdtoEstId(uint8_t ID);
extern uint8_t LegoModetoEstMode(uint8_t ID, uint8_t Mode);

#endif
