#include "Bluetooth.h"
#include "ui_pic.h"
#include "uc1638c.h"
#include "main.h"
#include "usart6.h"
u8 bluetooth_locate=3;
u8 bluetooth_j=3;  //��ʶ���¸�������
u8 control = 0;  //����ת��
u8 interface_state = 0;    //״̬�л�   1�����������������棬��ӱ�ʶ��  2���������������棬ִ�з�������������̴�����������
u8 bluetooth_start = 1;    //��������
u8 bluetooth_visual = 0;  //�������ӻ�
u8 device[9][15];           //�����豸1
u8 dis_sum = 0;               //�����豸����
u8 device_sum = 0;          //���豸����
u8 device_bond[9][15];      //���豸
u8 bluetooth_state = 0;     //������ǰ״̬   0���Ͽ�  1���Ͽ��ȴ��ر�    2���豸1����    3���豸2����    4���豸3����
u8 time_open = 0;               //����תΪ�����������������ʱ��
u8 time_break = 0;              //�Ͽ�������תΪ�ӻ�����ʱ��
u8 time_dis = 0;                //���������ȴ�ʱ��
u8 time_link = 0;                //���������豸�ȴ��Ĺ��ɽ��� 
u8 time_close = 0;               //����תΪ�ӻ���ر���������ʱ��
u8 time_bondlink = 0;           //�������豸���ӵȴ�ʱ��
u8 time_clear = 0;             //����������Ӧ�������������Ϣ����ʱ��
u8 bluetooth_judge = 0;     //�жϵ�ǰ�����ĸ��豸
u16 len = 0;              //usart6�õ������ݳ���
u8 t = 0;
u8 judge_dis = 0;   //�ж������豸�������������滹��������������������   0���豸����������   1���������������������   2��������������Ƴ�����
u8 bluetooth_wait = 0;             //������渨������
u8 memory_Tab_Level = 0;      //����������Ӧ��ص���Ӧǰ�Ľ���ļ������
u8 bluetooth_window = 0;        //��������
u8 position_judge = 0;       //�����ʶ����������
u8 at_id=0;
u8 bluetooth_role = 0;
u8 device_name[15];
u8 blue_1_state=0;
u8 blue_2_state=0;
u8 blue_3_state=0;
void bluetooth_judge_key();   //ȷ�����ж�
int diffrence_judge(int num);
void bluetooth_msg()       //�����ж�
{
	u8 i;
  u8 temp = 0;
//	if(USART6_RX_STA&0x8000)
//	{	
//		len = USART6_RX_STA;  //�õ��˴ν��յ������ݳ���						
		USART_ClearFlag(USART6,USART_IT_TC);

		if(USART6_Receive("et:0"))      
		{
			if(at_id==1)      //��������
			{
				bluetooth_start = 1; 
//			W25QXX_Write((u8*)&bluetooth_start,16*1024*1024+300,sizeof(bluetooth_start));	
				interface_state = 1;
				status=0;
				printf("AT+RESET");
			}
			else if(at_id==2)    //�����л��ɴӻ�
			{
				if(bluetooth_state == 1)
				{
					bluetooth_state = 0;
					time_close = 1;
				}		
			}
		}
		if(USART6_Receive("et:1"))  
		{
			if(at_id==2)       //�����л�������
			{
				if(time_bondlink == 0)
					time_open = 1;
			}
			else if(at_id==3)
			{
				status=0;
				printf("AT+SHOW1");
				at_id=4;
			}
			else if(at_id==4)
			{
				status=0;
				printf("AT+NOTI1");
				at_id=5;
			}
			else if(at_id==5)
			{
				status=0;
				strcpy(device_name,"EST(");
				printf("AT+NAMEEST");
				at_id=6;
				
			}
		}
		if(USART6_Receive("et:E"))  
		{
			if(at_id==6)
			{
				status=0;
				USART6_RX_STA = 0;
				printf("AT+ADDR?");
				at_id=0;
			}
		}
		if(USART6_Receive("et:3"))  
		{
			if(at_id==1)    //�ر�����
			{
				bluetooth_start = 0;
	//		W25QXX_Write((u8*)&bluetooth_start,16*1024*1024+300,sizeof(bluetooth_start));	
				interface_state = 1;
				Tab[tab_flag.MainTab_choice].Location = 2;
				UI_Update(tab_flag.Tab_Level,tab_flag.MainTab_choice,Tab);
				NotificationBar();                    //֪ͨ��
				status=0;
				printf("AT+RESET");
			}
		}
		if(USART6_Receive("DDR:"))  
		{
				len=USART6_RX_STA;//�õ��˴ν��յ������ݳ���	
				for(t = 12;t<len;t++)
				{
					device_name[t-8] = USART6_RX_BUF[t];							
				}
				device_name[t-8]=')';
				t++;
				device_name[t-8]='\0';
				W25QXX_Write( (u8*)&device_name,16*1024*1024+600,sizeof(device_name) );
		}
		if(USART6_Receive("ONN"))     //����
		{
			time_link = 0;
			if(bluetooth_role==1)
			{
				printf("OK+NAM:%s\r\n",device_name);
			LCD_Display_pic(22, 57, 130, 38,bluetooth_connect);
			if(judge_dis == 1)  //��������������������������°��豸
			{
				if(diffrence_judge(device_sum))    ////�ж��Ƿ��Ѿ��󶨲���ȡ��ǰ�����豸��
				{
					strcpy(device_bond[0]," ");
					W25QXX_Write((u8*)&device_bond[0],16*1024*1024+500,sizeof(device_bond[0]));		
					strcpy((u8*)device_bond[0],(u8*)device[bluetooth_judge]);
					W25QXX_Write( (u8*)&device_bond[0],16*1024*1024+500,sizeof(device_bond[0]) );
					device_sum=1;
					W25QXX_Write( (u8*)&device_sum,16*1024*1024+400,sizeof(device_sum) );
				}
			}
			
				bluetooth_state = 2;
			NotificationBar();                    //֪ͨ��
		}
		else
		{
				memory_Tab_Level = tab_flag.Tab_Level;
				tab_flag.Tab_Level = 7;
				bluetooth_locate=3;
				bluetooth_j=3;
		}
		}
		if(USART6_Receive("OST"))    //�Ͽ�
		{
			at_id=2;
			bluetooth_role = 0;		
			if(bluetooth_state != 1)
			bluetooth_state = 0; 
		}
		if(USART6_Receive("AM:"))      //����������Ϣ
		{
				len=USART6_RX_STA-1;//�õ��˴ν��յ������ݳ���	
				for(t = 3;t<len;t++)
				{
						device[0][t-3] = USART6_RX_BUF[t];							
				}
				device[0][t-3]='\0';
				printf("%s",device[0]);
				if(diffrence_judge(device_sum))    ////�ж��Ƿ��Ѿ��󶨲���ȡ��ǰ�����豸��
				{
					strcpy(device_bond[0]," ");
					W25QXX_Write((u8*)&device_bond[0],16*1024*1024+500,sizeof(device_bond[0]));		
					strcpy((u8*)device_bond[0],(u8*)device[0]);
					W25QXX_Write((u8*)&device_bond[0],16*1024*1024+500,sizeof(device_bond[0]) );
					device_sum=1;
					W25QXX_Write((u8*)&device_sum,16*1024*1024+400,sizeof(device_sum) );
				}
		}
}
void bluetooth_respond(u8 *ButtonAction)       //����������Ӧ
{
//	tab_flag.update_flag = 1;				//����Ҫ����
//	tab_flag.sound_flag = 1;
	switch(control)//0:���������� 1���豸���������� 2������������� 3���豸�������� 4����ʾ����
	{
		case 0:               //0:���������� 
				switch(*ButtonAction)        //������Ӧ       bluetooth_locate��0����  1��ʾ  2����  3�˳�
				{
					case 0:
							tab_flag.update_flag = 1;
							tab_flag.sound_flag = 0;
							break;
					
					case 1:			//��
							if(bluetooth_locate > 0)
								bluetooth_locate--;	
							else
								tab_flag.sound_flag = 0;
							interface_state=3;
						break;
						
					case 2:			//��
							if(bluetooth_locate < 3)
								bluetooth_locate++;
							else
								tab_flag.sound_flag = 0;
							interface_state=3;
						break;
						
					case 3:			//��
//							tab_flag.update_flag = 0;				//����Ҫ����
							tab_flag.sound_flag = 0;
					interface_state=4;
						break;
						
					case 4:			//��
//							tab_flag.update_flag = 0;				//����Ҫ����
							tab_flag.sound_flag = 0;
							interface_state=4;
						break;
					
					case 5:			//ȷ��
//							tab_flag.update_flag = 1;
//							tab_flag.sound_flag = 1;
							bluetooth_judge_key();
						break;
					
					case 6:			//����
							bluetooth_locate = 3;
							bluetooth_j = 3;                //λ������
							interface_state = 0;
							tab_flag.Tab_Level = 0;					//ѡ��������
							Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							//���������겻������
						break;	
					default:
							tab_flag.update_flag = 0;
							tab_flag.sound_flag = 0;	
							break;					
				}
			break;
		case 1:                //1���豸����������
				switch(*ButtonAction)         //������Ӧ    bluetooth_locate��  2���� 3�˳�
				{
					case 0:
							tab_flag.update_flag = 1;
							tab_flag.sound_flag = 0;
							break;
					
					case 1:			//��
							if(bluetooth_locate > (9-device_sum))
								bluetooth_locate--;
							else
								tab_flag.sound_flag = 0;
							if(bluetooth_locate < 6-bluetooth_window)
							{
									bluetooth_window++;
									tab_flag.update_flag = 1;
									tab_flag.sound_flag = 1;
							}
							blue_1_state=1;
						break;	
							
					case 2:			//��
							if(bluetooth_locate < 10)
								bluetooth_locate++;
							else
								tab_flag.sound_flag = 0;
							if((bluetooth_locate > (8-bluetooth_window)) && (bluetooth_locate < 9))
							{
								bluetooth_window--;
								tab_flag.update_flag = 1;				
								tab_flag.sound_flag = 1;
							}
							blue_1_state=1;

						break;

					case 3:			//��
//							tab_flag.update_flag = 0;				//����Ҫ����
							tab_flag.sound_flag = 0;
						break;
						
					case 4:			//��
//							tab_flag.update_flag = 0;				//����Ҫ����
							tab_flag.sound_flag = 0;
						break;
					
					case 5:			//ȷ��
						blue_1_state=0;
							tab_flag.update_flag = 1;				
							tab_flag.sound_flag = 1;
							bluetooth_judge_key();
						break;
					
					case 6:			//����
								blue_1_state=0;
								tab_flag.update_flag = 1;				
								tab_flag.sound_flag = 1;
								bluetooth_locate = 3;				
								bluetooth_j = 3;
								control = 0;
								bluetooth_window = 0;
								interface_state = 2;
								tab_flag.Tab_Level = 0;					//ѡ��������
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������						
						break;
					default:
							tab_flag.update_flag = 0;
							tab_flag.sound_flag = 0;	
							break;
						
				}

			break;
		case 2:              //2�������������
				if(bluetooth_start == 1)       //�����������
				{
					switch(*ButtonAction)        //������Ӧ    bluetooth_locate��  0��һ���豸  1�ڶ����豸 ������
					{
					case 0:
							tab_flag.update_flag = 1;
							tab_flag.sound_flag = 0;
							break;
						
						case 1:			//��
							if(bluetooth_locate > 0)
								bluetooth_locate--;
							else
								tab_flag.sound_flag = 0;
							if(bluetooth_locate < bluetooth_window)
							{
								position_judge = 1;
								bluetooth_window--;
								tab_flag.update_flag = 1;				
								tab_flag.sound_flag = 1;
							}
							blue_2_state=1;
							break;
							
						case 2:			//��
							if(bluetooth_locate < dis_sum-1)
								bluetooth_locate++;
							else
								tab_flag.sound_flag = 0;
							if(bluetooth_locate > (4+bluetooth_window))
							{
								position_judge = 1;
								bluetooth_window++;
								tab_flag.update_flag = 1;				
								tab_flag.sound_flag = 1;
							}
							blue_2_state=1;
							break;
							
						case 3:			//��
//								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
							
						case 4:			//��
//								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
						
						case 5:			//ȷ��
							blue_2_state=0;
								if(dis_sum > 0)
									bluetooth_judge_key();
								tab_flag.update_flag = 1;				
								tab_flag.sound_flag = 1;
							break;
						
						case 6:			//����
							blue_2_state=0;
								tab_flag.update_flag = 1;				
								tab_flag.sound_flag = 1;
								at_id=2;
								bluetooth_role = 0;						
								printf("AT+ROLE0");
								dis_sum = 0;
								bluetooth_locate = 9;
								bluetooth_j = 9;
								control = 1;
								bluetooth_window = 0;
								strcpy(device[0]," ");
							break;
						default:
							tab_flag.update_flag = 0;
							tab_flag.sound_flag = 0;	
							break;
							
					}
					if(((*ButtonAction == 1) || (*ButtonAction == 2)) && (dis_sum > 0) )//��ʶ
					{
						if(bluetooth_j != bluetooth_locate)            //�жϱ�ʶ���޸���
						{
							LCD_Invert(29, 34+(bluetooth_j-bluetooth_window)*18, 113, 14);  //�ɱ�ʶ��ʧ
							bluetooth_j = bluetooth_locate;
							LCD_Invert(29, 34+(bluetooth_locate-bluetooth_window)*18, 113, 14); //�±�ʶ��ʾ
						}
					}
				}
				else           //�����ر����
					switch(*ButtonAction)        //������Ӧ    bluetooth_locate��  0��һ���豸  1�ڶ����豸 ������
					{
						case 0:
//								tab_flag.update_flag = 0;
//								tab_flag.sound_flag = 0;
							break;
						
						case 1:			//��
//								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
							
						case 2:			//��
//								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
							
						case 3:			//��
//								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
							
						case 4:			//��
//								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
						
						case 5:			//ȷ��
								tab_flag.update_flag = 1;				
								tab_flag.sound_flag = 1;
								bluetooth_locate = 3;
								bluetooth_j = 3;                //λ������
								interface_state = 0;
								control = 0;
								tab_flag.Tab_Level = 0;					//ѡ��������
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
								//���������겻������
							break;
						
						case 6:			//����
								tab_flag.update_flag = 1;				
								tab_flag.sound_flag = 1;
								bluetooth_locate = 3;
								bluetooth_j = 3;                //λ������
								interface_state = 0;
								control = 0;
								tab_flag.Tab_Level = 0;					//ѡ��������
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
								//���������겻������
							break;	
						default:
							tab_flag.update_flag = 0;
							tab_flag.sound_flag = 0;	
							break;						
					}
			break;
		case 3:               //3���豸��������
				switch(*ButtonAction)          //������Ӧ         bluetooth_locate�� 2�Ƴ�    3����/�Ͽ�
				{
					case 0:
							tab_flag.update_flag = 1;
							tab_flag.sound_flag = 0;
							break;
					
					case 1:			//��
							if(bluetooth_locate > 2)
								bluetooth_locate--;
							blue_3_state=1;
//							tab_flag.update_flag = 0;				//����Ҫ����
//							tab_flag.sound_flag = 0;
						break;
						
					case 2:			//��
							if(bluetooth_locate < 3)
								bluetooth_locate++;
							blue_3_state=1;
//							tab_flag.update_flag = 0;				//����Ҫ����
//							tab_flag.sound_flag = 0;
						break;
						
					case 3:			//��
//							tab_flag.update_flag = 0;				//����Ҫ����
							tab_flag.sound_flag = 0;
						break;
						
					case 4:			//��
//							tab_flag.update_flag = 0;				//����Ҫ����
							tab_flag.sound_flag = 0;
						break;
					
					case 5:			//ȷ��
							blue_3_state=0;
							tab_flag.update_flag = 1;				
							tab_flag.sound_flag = 1;
							bluetooth_judge_key();
						break;
					
					case 6:			//����
							blue_3_state=0;
							tab_flag.update_flag = 1;				
							tab_flag.sound_flag = 1;
							if(judge_dis == 1)        //���ص������������
							{
								position_judge = 1;
								bluetooth_locate = bluetooth_judge;
								bluetooth_j = bluetooth_judge;
								control = 2;
							}
							else                    //���ص��豸����������
							{
								blue_1_state==0;
								bluetooth_window = 0;
								bluetooth_locate = 9;
								bluetooth_j = 9;
								control = 1;
							}
						break;	
					default:
							tab_flag.update_flag = 0;
							tab_flag.sound_flag = 0;	
							break;							
				}
			break;
		case 4:                //4����ʾ����
				switch(*ButtonAction)     //������Ӧ
				{
					case 0:
							tab_flag.update_flag = 1;
							tab_flag.sound_flag = 0;
							break;
					
					case 1:			//��
//							tab_flag.update_flag = 0;				//����Ҫ����
							tab_flag.sound_flag = 0;
						break;
						
					case 2:			//��
//							tab_flag.update_flag = 0;				//����Ҫ����
							tab_flag.sound_flag = 0;
						break;
						
					case 3:			//��
//							tab_flag.update_flag = 0;				//����Ҫ����
							tab_flag.sound_flag = 0;
						break;
						
					case 4:			//��
//							tab_flag.update_flag = 0;				//����Ҫ����
							tab_flag.sound_flag = 0;
						break;
					
					case 5:			//ȷ��
							tab_flag.update_flag = 1;				
							tab_flag.sound_flag = 1;
							if((judge_dis == 1) && (time_link > 31) || (judge_dis == 2))  //���������������������ʧ�ܻ��Ƴ������豸�����������������
							{
								position_judge = 1;
								time_link = 0;
								bluetooth_locate = bluetooth_j;
								control = 2;
								if(judge_dis == 2)                //�Ƴ��豸������һ���豸�����ж�
								{
									if(bluetooth_locate == dis_sum)
									{
										bluetooth_locate--;
										bluetooth_j--;
									}
								}								
							}
							else              //����������������ӳɹ��������豸���������棬���豸�������������ӣ����۳ɹ���ʧ�ܶ������豸����������
							{
								dis_sum = 0;
								bluetooth_window = 0;
								bluetooth_locate = 9;
								bluetooth_j = 9;
								control = 1;
							}
						break;
					
					case 6:			//����
							tab_flag.update_flag = 1;				
							tab_flag.sound_flag = 1;
							if(((judge_dis == 1) && (time_link > 31)) || (judge_dis == 2))  //���������������������ʧ�ܻ��Ƴ������豸�����������������
							{
								position_judge = 1;
								time_link = 0;
								bluetooth_locate = bluetooth_j;
								control = 2;                  //2������������� 
								if(judge_dis == 2)                         //�Ƴ��豸������һ���豸�����ж�
								{
									if(bluetooth_locate == dis_sum)
									{
										bluetooth_locate--;
										bluetooth_j--;
									}
								}								
							}
							else              //����������������ӳɹ��������豸���������棬���豸�������������ӣ����۳ɹ���ʧ�ܶ������豸����������
							{
								dis_sum = 0;
								bluetooth_window = 0;
								bluetooth_locate = 9;
								bluetooth_j = 9;
								control = 1;                  //1���豸����������
							}
						break;
					default:
							tab_flag.update_flag = 0;
							tab_flag.sound_flag = 0;	
							break;							
				}
			break;
	}
}
void bluetooth_judge_key() //ȷ�����ж�
{
	u8 i = 0;
	switch(control)        //0:���������� 1���豸���������� 2������������� 3���豸�������� 4����ʾ����
	{
		case 0:          //0:����������   
				switch(bluetooth_locate)
				{
					case 0:         //�л�����
							bluetooth_locate = 9;         
							bluetooth_j = 9;
							control = 1;     //1���豸����������     bluetooth_locate�� ����
						break;
					case 1:          //���ӻ�
							if(bluetooth_state == 0)
							{
									bluetooth_visual = 1-bluetooth_visual;
									W25QXX_Write((u8*)&bluetooth_visual,16*1024*1024+200,sizeof(bluetooth_visual));
									interface_state = 1;
									Tab[tab_flag.MainTab_choice].Location = 2;
									UI_Update(tab_flag.Tab_Level,tab_flag.MainTab_choice,Tab);
//									tab_flag.update_flag = 0;				//����Ҫ����
//									tab_flag.sound_flag = 0;
//									NotificationBar();
							}
//								printf("AT+VIEW\r\n");
							else
							{
								bluetooth_locate = 2;
								control = 4;
							}
						break;
					case 2:          //��������
							if(bluetooth_state != 0)      //����������״̬ʱ�ر�����
							{   
								time_break = 1;								
								bluetooth_state = 1;
//								blue_break = 0;
								printf("AT");			
//								bluetooth_locate = 2;
//								control = 4;								
							}
							else                        //������������״̬ʱ�����͹ر�����
							{
								if(bluetooth_start==0)
									printf("AT+ADTY0");
								else
									printf("AT+ADTY3");
								at_id=1;
//							tab_flag.update_flag = 0;				//����Ҫ����
//							tab_flag.sound_flag = 0;
							}
						break;
					case 3:	
							bluetooth_locate = 3;				
							bluetooth_j = 3;
							interface_state = 0;
							tab_flag.Tab_Level = 0;					//ѡ��������
							Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							//���������겻������						
						break;
				}
			break;
		case 1:          //1���豸���������� 
				switch(bluetooth_locate)
				{
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
						case 6:
						case 7:
						case 8:
								bluetooth_judge = 8-bluetooth_locate;             // 0:���豸device_bond[0]   1:���豸device_bond[1]  .....8:���豸device_bond[8]
								if(bluetooth_start == 0) 
								{
									bluetooth_locate = 0;
									bluetooth_j = 0;
									control = 2;
								}
								else
								{
									judge_dis = 0;
									bluetooth_locate = 3;   
									bluetooth_j = 3;
									control = 3;
								}
							break;
					case 9:
							if(bluetooth_start == 1)   //��������ʱ
							{
								at_id=2;
								bluetooth_role=1;
								printf("AT+ROLE1");
								time_dis = 1;
							}
							bluetooth_locate = 0;
							bluetooth_j = 0;
							control = 2;          //2�������������    bluetooth_locate�� ��һ���豸
						break;
					case 10:	   //������һ����
							blue_1_state=0;
							bluetooth_locate = 3;				
							bluetooth_j = 3;
							control = 0;
							interface_state = 2;
							tab_flag.Tab_Level = 0;					//ѡ��������
							Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������						
						break;
				}			
			break;
		case 2:          //2�������������
				bluetooth_judge = bluetooth_locate;    // 0:���������豸1    1:���������豸2    .....     8�����������豸9
				judge_dis = 1;
				bluetooth_locate = 3;   
				control = 3;          //3���豸��������    bluetooth_locate�� ����/�Ͽ�
			break;
		case 3:        //3���豸��������
				switch(bluetooth_locate)
				{
					case 2:                    //�Ƴ��豸����
							if((judge_dis == 0) && (bluetooth_state == 0))         //�Ƴ����豸
							{
								strcpy(device_bond[0]," ");
								W25QXX_Write((u8*)&device_bond[0],16*1024*1024+500,sizeof(device_bond[0]));					
//								printf("AT+CLEAR%d\r\n",bluetooth_judge);
//								if((device_sum>bluetooth_judge+1))
//								{
//									for(i=bluetooth_judge;i<device_sum-1;i++)
//										strcpy(device_bond[i],device_bond[i+1]);
//									strcpy(device_bond[device_sum-1]," ");
//								}
								device_sum--;
								W25QXX_Write((u8*)&device_sum,16*1024*1024+400,sizeof(device_sum));														
							}
							if(judge_dis == 1)          //�Ƴ������豸
							{
								strcpy(device[bluetooth_judge]," ");
								if((dis_sum>bluetooth_judge+1))
								{
									for(i = bluetooth_judge;i < dis_sum-1;i++)
										strcpy(device[i],device[i+1]);
									strcpy(device[dis_sum-1]," ");
								}
								dis_sum--;
								judge_dis = 2;
							}
							control = 4;          //4����ʾ����
						break;
					case 3:                 //���ӡ��Ͽ��豸����
							if(bluetooth_state == 0)                //�����Ͽ�״̬
							{
								if(judge_dis == 0)         //���Ӱ��豸
								{
									at_id=2;
									bluetooth_role=1;
									time_link = 1;									
									printf("AT+ROLE1");
									time_bondlink = 1;
								}
								else                     //���������豸
								{
									time_link = 1;
									printf("AT+CONN%d",bluetooth_judge);
								}
								control=4;       //4����ʾ����
							}
							else                                 //��������״̬
							{
								if(bluetooth_state == bluetooth_judge+2)      //�������ӵ������豸�����������������豸ʱ�������жϿ�����
								{
									time_break = 1;
									bluetooth_state = 0;    //תΪ�Ͽ�״̬
//									blue_break = 0;
									printf("AT");
									bluetooth_wait = 1;
								}
								control = 4;       //4����ʾ����
							}
						break;
				}			
			break;
	}
}
void blue_break_Init()       //�Ͽ�����IO�����ú���
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	 //ʹ��PB�˿�ʱ��
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //GPIOD11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOD, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOD.14
	GPIO_SetBits(GPIOD,GPIO_Pin_11);						 //PD.11 �����

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; //GPIOD14
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOD, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOD.14
	GPIO_ResetBits(GPIOD,GPIO_Pin_14);						 //PD.14 �����
}
int diffrence_judge(int num)          //�жϰ��豸�����ظ�����
{
  int i;
  for(i = 0;i < num;i++)
  {
    if(strcmp(device_bond[i],device[bluetooth_judge]) == 0)
		{
			bluetooth_state = i+2;
      break;
		}
  }
  if(i == num)
	{
		bluetooth_state = num+2;
    return 1;               //���ظ�
	}
  else
		return 0;            //���豸�Ѵ���
}
void bluetooch_waiting()
{
		if((time_close != 0) && (time_close != 10))     //����תΪ�ӻ���ر���������ʱ��
	{
		time_close++;
	}
	else if(time_close == 10)
	{
		time_close = 0;
		printf("AT+ADTY3");
	}
	if((time_open != 0) && (time_open != 10))     //����תΪ�����������������ʱ��
		time_open++;
	else if(time_open == 10)
	{
		time_open = 0;
		printf("AT+DISC?");
	}
	if((time_bondlink != 0) && (time_bondlink != 10))     //����תΪ������������Ӱ��豸����ʱ��
		time_bondlink++;
	else if(time_bondlink == 10)
	{
		time_bondlink = 0;
		printf("AT+CONNL",bluetooth_judge);
	}
	if((time_break > 0) && (time_break < 20))   //�Ͽ�������תΪ�ӻ�����ʱ��
	{
		time_break++;
		if((bluetooth_wait == 1) && ((time_break == 1) || (time_break == 11)))
		{
			LCD_Display_pic(22, 50, 130, 52, bluetooth_wait2);
			LCD_Display_pic(22, 50, 130, 52, bluetooth_wait1);
		}
		else if((bluetooth_wait == 1)&&((time_break == 6)||(time_break == 16)))
		{
			LCD_Display_pic(22, 50, 130, 52, bluetooth_wait2);
			LCD_Display_pic(22, 50, 130, 52, bluetooth_wait3);
		}
		if(time_break == 10)
			blue_break = 1;
	}
	else if(time_break == 20)
	{
		time_break = 0;
		at_id=2;
		bluetooth_role=0;
		printf("AT+ROLE0");
		if(bluetooth_wait == 1)
		{
			bluetooth_wait = 0;
			LCD_Display_pic(22, 57, 130, 38,bluetooth_break);
		}
	}
	if(time_dis != 0)                         //��������ʱ����ʱ�估���ɽ���
	{
		time_dis++;
		if((time_dis == 1) || (time_dis == 11) || (time_dis == 21) || (time_dis == 31)|| (time_dis == 41)|| (time_dis == 51))     //���ɽ���
		{
			LCD_Display_pic(22, 50, 130, 52, bluetooth_wait2);
			LCD_Display_pic(22, 50, 130, 52, bluetooth_wait1);
		}
		else if((time_dis == 6) || (time_dis == 16) || (time_dis == 26) || (time_dis == 36)|| (time_dis == 46)|| (time_dis == 56))
		{
			LCD_Display_pic(22, 50, 130, 52, bluetooth_wait2);
			LCD_Display_pic(22, 50, 130, 52, bluetooth_wait3);
		}
		else if(time_dis == 61)          //����ʱ�䵽����ʾ�������
		{
			time_dis = 0;
			LCD_Display_pic(22, 12, 130, 111, bluetooth_ins);
			switch(dis_sum)                                     //�����豸��ʾ  
			{
				case 9:
				case 8:
				case 7:
				case 6:
				case 5:			
						LCD_Display_pic(35, 107, 13, 12,bluetooth_icon2);
						LCD_ShowString(58,107,device[4],12,0);	
				case 4:			
						LCD_Display_pic(35, 89, 13, 12,bluetooth_icon2);
						LCD_ShowString(58,89,device[3],12,0);				
				case 3:			
						LCD_Display_pic(35, 71, 13, 12,bluetooth_icon2);
						LCD_ShowString(58,71,device[2],12,0);
				case 2:			
						LCD_Display_pic(35, 53, 13, 12,bluetooth_icon2);
						LCD_ShowString(58,53,device[1],12,0);
				case 1:			
						LCD_Display_pic(35, 35, 13, 12,bluetooth_icon2);
						LCD_ShowString(58,35,device[0],12,0);
						LCD_Invert(29, 34, 113, 14);
					break;
				default:
					break;
			}
		}
	}
	if(time_link != 0)            //�����豸�ȴ��Ĺ��ɽ���             
	{
		time_link++;
		if((time_link == 1) || (time_link == 11) || (time_link == 21))
		{
			LCD_Display_pic(22, 50, 130, 52, bluetooth_wait2);
			LCD_Display_pic(22, 50, 130, 52, bluetooth_wait1);
		}
		else if((time_link == 6) || (time_link == 16) || (time_link == 26))
		{
			LCD_Display_pic(22, 50, 130, 52, bluetooth_wait2);
			LCD_Display_pic(22, 50, 130, 52, bluetooth_wait3);
		}
		else if((time_link == 31))          //��ʱ������ʧ��
		{
			LCD_Display_pic(22, 50, 130, 52, bluetooth_linklose);
			if(judge_dis == 0)
			{
				at_id=2;
				bluetooth_role=0;
				printf("AT+ROLE0");
			}
		}
	}
	if((time_clear > 0) && (time_clear < 60))          //����������Ӧ�������������Ϣ����ʱ��
		time_clear++;
	else if(time_clear == 60)
	{
		time_clear = 0;
//		printf("AT+CLEAR%d\r\n",device_sum);
	}
}

void bluetooth_init()
{
	status=0;
	printf("AT+IMME1");
	at_id=3;
}

