/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/cascade.c 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief  
  ******************************************************************************
**/
#include "cascade.h"

/*
�����ܵ�ָ����˵��

1.��Ÿ��¹��ܣ�
host���ͣ�0x01+0x10(������)+TargetLevel
slave���ͣ�0x01+0x11(�ӵ���)+CurrentLevel

2.�ӻ��豸���ƣ�
host���ͣ�0x02+0x10(������)+TargetLevel+Port+DeviceID+mode+BuffSize+Buff
slave���ͣ�0x02+0x11(�ӵ���)+CurrentLevel+Port+DeviceID+mode

3.�ӻ��������豸ģʽ���ã�
host���ͣ�0x03+0x10(������)+TargetLevel+Port+DeviceID+mode
slave���ͣ�0x03+0x11(�ӵ���)+CurrentLevel+Port+DeviceID+mode+BuffSize+DataBuff

4.�ӻ������豸�ϱ�master
slave���ͣ�0x04+0x11(�ӵ���)+CurrentLevel+DeviceMessage[8](����8���˿ڵ��豸id)

5.��master���������Ÿ��¹��ܣ�
slave���ͣ�0x05+0x11(�ӵ���)+CurrentLevel

6.���¼�slave��������usb���
slave���ͣ�0x06+0x10(������)+CurrentLevel
*/

#define USB_ADCCH	6			//usbʹ��ADCͨ��6��������ѹ

//��ʾ�ӻ��˿�����״̬
//0,û������;
//1,�Ѿ�����;
u8 SlaveComConnected = 0;
u8 SlaveComConnected_old = 0;

u8 CascadeLevel = 0; 	//�����ļ���λ��
u8 CascadeNum = 0; 		//��������
u8 CascadeCheck = 0; 	//����ͨ�����
u8 CascadeMaster = 0;	//����������־��0����û�м�����1������������2������������
u8 usbrestart = 0;		//usb������־λ

Cascade_Function_TypeDef	Cascade_Function;

//Cascade_flag_TypeDef  Cascade_flag[4][8];


//----------------------�����µ��������ӻ����ݰ����ͺ���----------------------------

//�����Ÿ��°�
u8 HostSend_SortUpdate_Packet(u8 Current_Level)
{
	int _cnt=0;
	u8 data_to_send[3]={0};
	
	data_to_send[_cnt++]=0x01;					//������
	data_to_send[_cnt++]=0x10;					//host��slave
	data_to_send[_cnt++]=Current_Level;	//��ǰ�������� 0-3
	
	return USBH_SendData(data_to_send, _cnt);
}
//�����ŷ�����
void SlaveReply_SortUpdate_Packet(u8 Current_Level)
{
	int _cnt=0;
	static u8 data_to_send[3]={0};
	
	data_to_send[_cnt++]=0x01;					//������
	data_to_send[_cnt++]=0x11;					//host��slave
	data_to_send[_cnt++]=Current_Level;	//��ǰ�������� 0-3
	
	HID_SendBuffToHost(data_to_send, _cnt);
}


//�ӻ��豸�������ݰ�
//u8 Buff[]Ϊ������������ݰ�
u8 HostSend_DControl_Packet(u8 Target_Level, u8 Port, u8 DeviceId, u8 mode , u8 Buff[], u8 BuffSize)
{
	int _cnt=0,i;
	u8 data_to_send[21]={0};
	
	data_to_send[_cnt++]=0x02;					//������
	data_to_send[_cnt++]=0x10;					//host��slave
	data_to_send[_cnt++]=Target_Level;	//Ŀ�꼶������ 0-3
	data_to_send[_cnt++]=Port;					//���ƶ˿�
	data_to_send[_cnt++]=DeviceId;			//���ƶ˿ڵ��豸
	data_to_send[_cnt++]=mode;					//����ģʽ
	data_to_send[_cnt++]=BuffSize;
	for(i=0;i<BuffSize;i++)
		data_to_send[_cnt++]=Buff[i];
	
	return USBH_SendData(data_to_send, _cnt);
//	printf("ENTER\n");
}
//�ӻ��豸����������ɷ�����
void SlaveReply_DControl_Packet(u8 Current_Level, u8 Port, u8 DeviceId, u8 mode)
{
	int _cnt=0;
	static u8 data_to_send[5]={0};
	
	data_to_send[_cnt++]=0x02;					//������
	data_to_send[_cnt++]=0x11;					//host��slave
	data_to_send[_cnt++]=Current_Level;	//��ǰ�������� 0-3
	data_to_send[_cnt++]=Port;					//���ƶ˿�
	data_to_send[_cnt++]=DeviceId;			//���ƶ˿ڵ��豸
	data_to_send[_cnt++]=mode;					//����ģʽ
	
	HID_SendBuffToHost(data_to_send, _cnt);
}


//�ӻ�����������ģʽ���ð�
//Port��Χ��1-8
u8 HostSend_DDataGet_Packet(u8 Target_Level, u8 Port, u8 DeviceId, u8 mode)
{
	int _cnt=0;
	u8 data_to_send[6]={0};
	
	data_to_send[_cnt++]=0x03;					//������
	data_to_send[_cnt++]=0x10;					//host��slave
	data_to_send[_cnt++]=Target_Level;	//Ŀ�꼶������ 0-3
	data_to_send[_cnt++]=Port;					//���ƶ˿�
	data_to_send[_cnt++]=DeviceId;			//���ƶ˿ڵ��豸
	data_to_send[_cnt++]=mode;					//����ģʽ
	
//	Channel_State[Target_Level*8+Port-1][0] = Port;
//	Channel_State[Target_Level*8+Port-1][1] = DeviceId;	//id���������ı�
//	Channel_State[Target_Level*8+Port-1][2] = mode;
	
	return USBH_SendData(data_to_send, _cnt);
}
//�ӻ��豸����������ɷ�����
//u8 DataBuff[] �ӻ�������ĳ��ģʽ�µ��������
void SlaveReply_DDataGet_Packet(u8 Current_Level,u8 Channel_State[32][13])
{
	int _cnt=0,i,j;
	static u8 data_to_send[63]={0};
	
	data_to_send[_cnt++]=0x03;					//������
	data_to_send[_cnt++]=0x11;					//host��slave
	data_to_send[_cnt++]=Current_Level;	//��ǰ�������� 0-3
	
	for(i=0;i<4;i++)
	{
		for(j=0;j<10;j++)
			data_to_send[_cnt++]=Channel_State[i][3+j];
	}
	for(i=4;i<8;i++)
	{
		for(j=0;j<4;j++)
			data_to_send[_cnt++]=Channel_State[i][3+j];
	}
	
	HID_SendBuffToHost(data_to_send, _cnt);
}


//�ӻ������豸�ϱ���
//u8 DeviceMessage[16]:�ӻ���ǰ8���˿���Ϣ���ֱ�ΪPort1DeviceID + .... + Port8DeviceID
void SlaveReply_ConDevice_Packet(u8 Current_Level, u8 Channel_State[32][13])
{
	int _cnt=0,i;
	static u8 data_to_send[27]={0};
	
	data_to_send[_cnt++]=0x04;					//������
	data_to_send[_cnt++]=0x11;					//host��slave
	data_to_send[_cnt++]=Current_Level;	//��ǰ�������� 0-3
	for(i=0;i<8;i++)
	{
		data_to_send[_cnt++]=Channel_State[i][0];	//�˿���Ϣ
		data_to_send[_cnt++]=Channel_State[i][1];	//�˿���Ϣ
		data_to_send[_cnt++]=Channel_State[i][2];	//�˿���Ϣ
	}
	
	HID_SendBuffToHost(data_to_send, _cnt);
}

//��master�������������Ÿ��°�
//u8 DeviceMessage[16]:�ӻ���ǰ8���˿���Ϣ���ֱ�ΪPort1DeviceID + .... + Port8DeviceID
void SlaveReply_RequestUpdate_Packet()
{
	int _cnt=0;
	static u8 data_to_send[2]={0};
	
	data_to_send[_cnt++]=0x05;					//������
	data_to_send[_cnt++]=0x11;					//host��slave

	HID_SendBuffToHost(data_to_send, _cnt);
}


//u8 Buff[]Ϊ������������ݰ�
u8 HostSend_USBReset_Packet(void)
{
	int _cnt=0;
	u8 data_to_send[2]={0};
	
	data_to_send[_cnt++]=0x06;					//������
	data_to_send[_cnt++]=0x10;					//host��slave

	return USBH_SendData(data_to_send, _cnt);
}


//----------------------usb�˿����ݽ��մ���----------------------------

//usb device�˿ڽ������ݵĽ���
//����usb device������������λ��ִ��
void SlaveCom_DataDecode(u8 RecBuff[], int RecBuffSize)
{
	int i,count;
	
	switch(RecBuff[0])	//������
	{
		case 0x01:				//�������
			CascadeLevel = RecBuff[2]+1;	//RecBuff[2]�������ϼ����
			if(HostComConnected==0 && SlaveComConnected==1)		//���Ϊ ���һ��������
			{
				SlaveReply_SortUpdate_Packet(CascadeLevel);
			}
			else		//���� �������¼����ͱ�Ÿ��°�
			{
				HostSend_SortUpdate_Packet(CascadeLevel);
			}
			break;
		case 0x02:				//�豸����
			if(CascadeLevel == RecBuff[2])	//��� ������Ŀ���ռ���
			{
				//�����ݴ�����Ӧλ�ò��ñ�־λ
				count = 4;
	
				Cascade_Function.Cascade_Order[RecBuff[3]-1].id 				= RecBuff[count++];
				Cascade_Function.Cascade_Order[RecBuff[3]-1].mode 			= RecBuff[count++];
				Cascade_Function.Cascade_Order[RecBuff[3]-1].bufflength = RecBuff[count++];
				for(i=0;i<Cascade_Function.Cascade_Order[RecBuff[3]-1].bufflength;i++)
				{
					Cascade_Function.Cascade_Order[RecBuff[3]-1].buff[i] 	= RecBuff[count++];
				}
				Cascade_Function.Cascade_Order[RecBuff[3]-1].flag = 1;

				Cascade_Function.allflag 												= 1;
				
#ifdef	CASCADE_DEBUG
					printf("		�������ָ��\n");
				
					printf("i %x  \r\n",i);
					printf("ID %x  \r\n",Cascade_Function.Cascade_Order[RecBuff[3]-1].id);
					printf("MODE %x  \r\n",Cascade_Function.Cascade_Order[RecBuff[3]-1].mode);
					printf("FLAG %x  \r\n",Cascade_Function.Cascade_Order[RecBuff[3]-1].flag);
					printf("bufflength %x  \r\n",Cascade_Function.Cascade_Order[RecBuff[3]-1].bufflength);
#endif
				
			}
			else		//���� ԭ�ⲻ���������¼����Ϳ��ư�
			{
				USBH_SendData(RecBuff, RecBuffSize);
			}
			break;
		case 0x03:				//������ģʽ����
			if(CascadeLevel == RecBuff[2])	//��� ������Ŀ���ռ���
			{
				//�����ݴ�����Ӧλ�ò��ñ�־λ
				count = 4;
				Cascade_Function.Cascade_Order[RecBuff[3]-1].id 	= RecBuff[count++];
				Cascade_Function.Cascade_Order[RecBuff[3]-1].mode = RecBuff[count++];
				Cascade_Function.Cascade_Order[RecBuff[3]-1].flag = 1;
				Cascade_Function.allflag 													= 1;
				
				
#ifdef	CASCADE_DEBUG
				printf("		���봫����ģʽ����ָ��\n");
#endif
				
			}
			else		//���� ԭ�ⲻ���������¼����Ϳ��ư�
			{
				USBH_SendData(RecBuff, RecBuffSize);
			}
			break;
		case 0x06:				//usb restart����
			
			usbrestart = 1;
			USBH_SendData(RecBuff, RecBuffSize);		//�������¼����Ϳ��ư�

			break;
	}
}

//usb host�˿ڽ������ݵĽ���
//����usb host���մӻ�����λ��ִ��
void HostCom_DataDecode(u8 RecBuff[], int RecBuffSize)
{
	int i,j;
	
	switch(RecBuff[0])	//������
	{
		case 0x01:				//������������
			if(CascadeMaster==1)		//���Ϊ Maaster������
			{
				CascadeNum = RecBuff[2];	//RecBuff[2]�������ϼ����
				CascadeCheck = CascadeNum;
			}
			else		//���� ԭ�ⲻ���������ϼ����ͼ�������������
			{
				HID_SendBuffToHost(RecBuff, RecBuffSize);
			}
			break;
		case 0x02:				//�豸����
			if(CascadeMaster==1)		//���Ϊ Maaster������
			{
				//�������
#ifdef	CASCADE_DEBUG
				printf("		Level:%d,Port:%d,ID:%d,Mode:%d  �������\n",RecBuff[2],RecBuff[3],RecBuff[4],RecBuff[5]);
#endif
				

				Cascade_Function.Cascade_flag[RecBuff[2]][RecBuff[3]].id		= RecBuff[4];
				Cascade_Function.Cascade_flag[RecBuff[2]][RecBuff[3]].mode	= RecBuff[5];
				Cascade_Function.Cascade_flag[RecBuff[2]][RecBuff[3]].flag = 1;
			}
			else		//���� ԭ�ⲻ���������ϼ�����������ɰ�
			{
				HID_SendBuffToHost(RecBuff, RecBuffSize);
			}
			break;
		case 0x03:				//������ģʽ����
			if(CascadeMaster==1)		//���Ϊ Master������
			{
				//��ȡ����������
				
//#ifdef	CASCADE_DEBUG
				printf("		�յ�Level:%d  �ϴ������ݣ�\n",RecBuff[2]);
//#endif
				
				
				for(i=0;i<4;i++)		//1-4�Ŷ˿�
				{
					for(j=0;j<10;j++)
					{
						Channel_State[RecBuff[2]*8+i][3+j] = RecBuff[3+i*10+j];
					}
				}
				for(i=0;i<4;i++)		//5-8�Ŷ˿�
				{
					for(j=0;j<4;j++)
					{
						Channel_State[RecBuff[2]*8+4+i][3+j] = RecBuff[43+i*4+j];
					}
				}
				
			}
			else		//���� ԭ�ⲻ���������ϼ�
			{
				HID_SendBuffToHost(RecBuff, RecBuffSize);
			}
			break;
		case 0x04:				//�ӻ������豸��Ϣ
			if(CascadeMaster==1)		//���Ϊ Maaster������
			{
				//��ȡ�ӻ������豸��Ϣ
				
#ifdef	CASCADE_DEBUG
				printf("		�յ��ӻ�Level:%d�ϴ����豸��Ϣ��\n",RecBuff[2]);
#endif

				for(i=0;i<8;i++)
				{
					for(j=0;j<3;j++)
					{
						Channel_State[RecBuff[2]*8+i][j] = RecBuff[3+i*3+j];
					}
				}
			}
			else		//���� ԭ�ⲻ���������ϼ�
			{
				HID_SendBuffToHost(RecBuff, RecBuffSize);
			}
			break;
		case 0x05:				//�ӻ�����������
			if(CascadeMaster==1)		//���Ϊ Maaster������
			{
				//���ͱ�Ű�
				HostSend_SortUpdate_Packet(0x00);
			}
			else		//���� ԭ�ⲻ���������ϼ�
			{
				HID_SendBuffToHost(RecBuff, RecBuffSize);
			}
			break;
	}
	
}


//----------------------������������----------------------------

//������Ÿ��½���
//1sִ��һ��
void UpdateCascadeNum_Process(void)
{
	static u8 HostComConnectedOld = 0;
	static u8 ComIDOld[9] = {0};
	static u8 SendCount = 0;
//	static u8 time100ms = 0;
	int i;
	
	if(usbmasterflag)	//˵����master������һ����������ͨ���м���
	{
		if(HostComConnected==1)		//˵�����ڼ���״̬
		{
			CascadeLevel = 0;
			CascadeMaster = 1;
			if(!CascadeNum)
			{
				SendCount++;
				if(SendCount>=10)
				{
					HostSend_USBReset_Packet();
//					usbrestart = 1;
					SendCount = 0;

#ifdef	CASCADE_DEBUG
				printf("����usb\n");
#endif
				
				}
				else{
					//���ͱ�Ű�
					HostSend_SortUpdate_Packet(0x00);
				}
			}
			else{
				SendCount = 0;
			}
		}
		else{		// û�м���
			CascadeLevel = 0; 	//��ձ����ļ���λ��
			CascadeNum = 0; 		//��������
			CascadeMaster = 0;
		}
	}
	else	//Ҳ������master���ߴӻ�
	{
		if(HostComConnected==1&&SlaveComConnected==0)				//���Ϊ �����µ�Maaster������
		{
			CascadeLevel = 0;
			CascadeMaster = 1;
			if(!CascadeNum || !CascadeCheck)
			{
				//���ͱ�Ű�
				HostSend_SortUpdate_Packet(0x00);
				
				SendCount++;
				if(SendCount>=10)
				{
//					HostSend_USBReset_Packet();
//					usbrestart = 1;
					SendCount = 0;
					
#ifdef	CASCADE_DEBUG
				printf("����usb\n");
#endif
					
				}
			}
			else{
				SendCount = 0;
			}
		}
		else if(HostComConnected==0 && SlaveComConnected==0)		//��� û�м���
		{
			CascadeLevel = 0; 	//��ձ����ļ���λ��
			CascadeNum = 0; 		//��������
			CascadeMaster = 0;
		}
		else	//��� ����������master
		{
			CascadeNum = 0; 		//��������
			CascadeMaster = 2;
			if(!CascadeLevel)
			{
				//���ϼ����ͱ�������
				SlaveReply_RequestUpdate_Packet();
			}
		}
	}
	
	
	if(CascadeMaster==2)		//����������
	{
		//---slave�豸�Ĳ�����γ�---
		if(!HostComConnectedOld && HostComConnected)	//��� ������slave�豸
		{
			//���ϼ����ͱ�������
			SlaveReply_RequestUpdate_Packet();
		}
		else if(HostComConnectedOld && !HostComConnected)	//���	�γ���slave�豸
		{
			//���ϼ����ͱ�������
			SlaveReply_RequestUpdate_Packet();
		}
		HostComConnectedOld = HostComConnected;
		
		
		//---�ӻ����������γ�---
		for(i=0;i<8;i++)
		{
			if(!ComIDOld[i] && Channel_State[i][1])			//��� ʶ������
			{
				//�ϴ�������Ϣ
				SlaveReply_ConDevice_Packet(CascadeLevel,Channel_State);
			}
			else if(ComIDOld[i] && !Channel_State[i][1])		//��� ����γ�
			{
				//�ϴ�������Ϣ
				SlaveReply_ConDevice_Packet(CascadeLevel,Channel_State);
			}
			ComIDOld[i] = Channel_State[i][1];
		}
		
	}
	
//	//---3s����һ��usbͨ�������Ϣ---
//	if(CascadeMaster==1)		//��������
//	{
//		time100ms++;
//		if(time100ms>=30)
//		{
//			CascadeCheck = 0;
//			time100ms = 0;
//		}
//	}
	
	
}

//�ӻ������������ϴ�����
//100msִ��һ��
void SensorDataUpload_Process(void)
{
	if(CascadeMaster==2)		//����������
	{
		if(Channel_State[0][1]||Channel_State[1][1]||Channel_State[2][1]||Channel_State[3][1]||		//��� �ӻ��˿����豸����
		Channel_State[4][1]||Channel_State[5][1]||Channel_State[6][1]||Channel_State[7][1])
		{
			//�ӻ��ϴ�����
			SlaveReply_DDataGet_Packet(CascadeLevel,Channel_State);
		}
	}
}

