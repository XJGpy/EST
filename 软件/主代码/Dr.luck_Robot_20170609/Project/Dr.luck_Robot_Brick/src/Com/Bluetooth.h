 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK��ӢSTM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define blue_break PDout(11)// PD11
#define blue_break2 PDout(14)// PD14
//void bluetooth_Init(u8 at);
void blue_break_Init();
void bluetooth_respond(u8 *ButtonAction);
void bluetooch_waiting();
void bluetooth_msg();
void bluetooth_init();
extern u8 control;
extern u8 bluetooth_j;
extern u8 bluetooth_start;		 				    
extern u8 bluetooth_visual;	
extern u8 interface_state;
extern u8 bluetooth_locate;
extern u8 time_close;         
extern u8 time_open;         
extern u8 time_break;         
extern u8 time_dis;             
extern u8 time_bondlink;        
extern u8 time_link;
extern u8 time_clear;
extern u8 device_sum;        //���豸�豸����
extern u8 dis_sum;               //�����豸����
extern u8 device_bond[9][15];  //���豸
extern u8 device[9][15];       //�����豸
extern u8 bluetooth_state;      //������ǰ״̬   0���Ͽ�  1������     2
extern u8 judge_dis;
extern u8 bluetooth_judge;
extern u8 memory_Tab_Level;
extern u8 bluetooth_wait;
extern u8 bluetooth_window;
extern u8 position_judge;
extern u8 device_name[15];
extern u8 blue_1_state;
extern u8 blue_2_state;
extern u8 blue_3_state;