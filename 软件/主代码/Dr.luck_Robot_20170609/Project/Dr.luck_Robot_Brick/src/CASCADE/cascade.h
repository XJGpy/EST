/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/cascade.c 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief  
  ******************************************************************************
**/
#ifndef __CASCADE_H
#define __CASCADE_H 	

//#include "sys.h"
//#include "usbh_hid_core.h"
#include "main.h"

#define laySize  	4
#define portSize  8

u8 HostSend_SortUpdate_Packet(u8 Current_Level);
void SlaveReply_SortUpdate_Packet(u8 Current_Level);

u8 HostSend_DControl_Packet(u8 Target_Level, u8 Port, u8 DeviceId, u8 mode , u8 Buff[], u8 BuffSize);
void SlaveReply_DControl_Packet(u8 Current_Level, u8 Port, u8 DeviceId, u8 mode);

u8 HostSend_DDataGet_Packet(u8 Target_Level, u8 Port, u8 DeviceId, u8 mode);
void SlaveReply_DDataGet_Packet(u8 Current_Level,u8 Channel_State[8][13]);
void SlaveReply_ConDevice_Packet(u8 Current_Level,u8 Channel_State[8][13]);
u8 HostSend_USBReset_Packet(void);
	

extern void SlaveCom_DataDecode(u8 RecBuff[], int RecBuffSize);
extern void HostCom_DataDecode(u8 RecBuff[], int RecBuffSize);

void UpdateCascadeNum_Process(void);
void SensorDataUpload_Process(void);

extern u8 SlaveComConnected;
extern u8 SlaveComConnected_old;
extern u8 CascadeLevel; 	//�����ļ���λ��
extern u8 CascadeNum; 		//��������
extern u8 usbrestart;

//�Լ����豸�Ĳ��������
typedef struct Cascade_Order
{
	u8 flag;
	u8 id;
	u8 mode;
	u8 bufflength;
	u8 buff[13];
	
}Cascade_Order_TypeDef;

////�������Ʊ�־λ
//typedef struct Cascade_flag
//{
//	u8 id;
//	u8 mode;
//	u8 flag;
//	
//}Cascade_flag_TypeDef; 

//extern Cascade_flag_TypeDef  Cascade_flag[4][8];

//�����豸��Ϣ�����ݻ���
typedef struct Cascade_Device
{
	u8 id;
	u8 mode;
	u8 bufflength;
	u8 buff[9];
	
} Cascade_Device_TypeDef;

//����������Ϣ
typedef struct Cascade_Function
{
	u8 allflag;
	Cascade_Order_TypeDef	Cascade_Order[portSize];  //�ӻ�����ָ���
	Cascade_Order_TypeDef Cascade_flag[laySize][portSize];  //��������ָ���
	
//	Cascade_Device_TypeDef Cascade_Device[4][8];	//4̨brick��8������
	
} Cascade_Function_TypeDef;

extern Cascade_Function_TypeDef	Cascade_Function;


#endif

