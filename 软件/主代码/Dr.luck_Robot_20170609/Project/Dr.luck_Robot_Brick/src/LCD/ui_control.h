/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/ui_control.h 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief  
  ******************************************************************************
**/
#ifndef __UI_CONTROL_H
#define __UI_CONTROL_H

#include "stm32fxxx_it.h" 
#include "ui_pic.h"
#include "uc1638c.h"
#include "outputworkmode.h"

typedef struct Tab_Function
{
	uint8_t Location;						//����������
	uint8_t SubfunctionNum;			//�ӹ���ѡ������
	uint8_t SubLocation[8];			//һ�����๦�����ɴ洢4���Ӻ������ڲ�����λ��

//  uint8_t  (*Init)         (void *pdev , uint8_t cfgidx);

} Tab_Function_TypeDef;

//��־λ�ṹ��
typedef struct TabFlag{
	
	uint8_t Tab_Level;				//0Ϊ�����棬1Ϊ�Ӻ������棬2Ϊ�ػ��������棬3Ϊ�������н���
	uint8_t MainTab_choice;	//0~3������4���๦��ѡ��,4�Ӻ���portview
  uint8_t update_flag; 		//ui����±�־λ
	uint8_t ShutDown;				//���ڹػ�����
	uint8_t sound_flag;				//������־λ
	uint8_t Confirm;				//���ڹػ�����
}tabflag;

extern u8 state;
extern float PID_P;
extern float PID_I;
extern float PID_D;
extern float PID_DM;

void UI_Control_Init(void);
//ÿ10msִ��һ��
void UI_ButtonrRespond(u8 *ButtonAction);
void Running_Interface(void);
void NotificationBar(void);
void PICTURE_ButtonRespond(u8 *ButtonAction);
void UI_Confirm_ButtonRespond(u8 *ButtonAction);
	
u8 GetChoice(Tab_Function_TypeDef Tab[], u8 MainTab_choice, u8 num);
	
//uint8_t LocationToMode(uint8_t Type, uint8_t location);
uint8_t GetShowModeNum(uint8_t Type);
void ShowSenserData(uint8_t type, uint8_t	id, uint8_t	mode);

void ChangeSensorMode(UBYTE Port, uint8_t Type, uint8_t mode);
void SelectSensorMode(uint8_t AllSelectModeLen, uint8_t Type);
void SelectSensorPicture(uint8_t Type, uint8_t Mode);
void Showusartname(uint8_t port, uint8_t id, uint8_t mode);
void Showusartname1(uint8_t id);	
extern u8 motorcontrol_flag;
extern u8 UIRUN_flag;
extern u8 LCD_flag;
extern u8 ButtonAction;
extern u8 DeviceID[8];
extern u8 JXL_Display_Choice;

extern tabflag tab_flag;

extern Tab_Function_TypeDef Tab[5];

extern void factory_test(void);

#endif

