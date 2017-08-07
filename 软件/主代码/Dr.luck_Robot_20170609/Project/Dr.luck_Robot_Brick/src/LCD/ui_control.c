/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/ui_control.c 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief  
  ******************************************************************************
**/

#include "main.h"
#include "myfont.h"
#include "music.h"
#include "text.h"
//#include "stdio.h"
//#include "ui_control.h"
//#include "usart_sensor.h"
//#include "iic_sensor.h"
//#include "C_Protocol.h"
//#include "ads7957.h" 
//#include "input.h" 
//#include "ui_pic.h"
//#include "adc.h"
//#include "myfatfs.h"
//#include "vs10xx.h"
//#include "w25qxx.h"

#define  PID_DEBUG 0
u8 PID_state=0;
float PID_P=0;
float PID_I=0;
float PID_D=0;
float PID_DM=0;

u8 motorcontrol_flag=0;
u8 UIRUN_flag=0;
u8 LCD_flag = 0;
u8 ButtonAction = 0;		//����������־λ�� 0���գ�1: �� 2���£�3����4���ң�5��ȷ�ϣ�6������
u8 JXL_Display_Choice = 0;

//u8 DeviceID[8] = {0,0,0,0,0,0,0,0} ;			//�豸id�����ݡ�Ӳ���˿����ݽ�������ȷ����0x00����˿�û���豸��
//�豸id��Ӧ���豸���ƣ�DeviceName[0]������豸
//char* DeviceName[11] = {"None","Color","Gyro","Infrare","MediumM","Temper",
//														"Touch","Ultra","Energy","NXTS","LargeM"};
//char* ChannelName[8] = {"1","2","3","4","5","6","7","8"};
char* SettingName_cn[7] = {"����","˯��","����","ϵͳ��Ϣ","����","����","��ʽ��"};
char* SettingName_en[7] = {"Volume","Sleep","Bluetooth","Brick_Info","Language","Light","File_Format"};

tabflag tab_flag;

Tab_Function_TypeDef  PLAYLAST_Tab = 
{
  0,
	5,
	{0,0,0,0,0,0,0,0},
};

Tab_Function_TypeDef  FILE_Tab = 
{
  0,
	5,
	{0,0,0,0,0,0,0,0},
};

Tab_Function_TypeDef  APPS_Tab = 
{
  0,
	3,
	{0,0,0,0,0,0,0,0},
};

Tab_Function_TypeDef  SETTINGS_Tab = 
{
  0,
	7,			
	{0,0,0,0,0,0,0,0},
};

Tab_Function_TypeDef  SubPortView_Tab = 
{
  0,
	8,					
	{0,0,0,0,0,0,0,0},
};	

 Tab_Function_TypeDef Tab[5];

//====================================================

void UI_Update(u8 Tab_Level, u8 MainTab_choice, Tab_Function_TypeDef Tab[]);
float Byte4ToFloat(uint8_t byte[], uint8_t len);
//void ChangeMode(UARTPORT UartPort[], UBYTE Port, UBYTE mode);

void UI_Control_Init(void)
{
	u8 i;
	
	LCD_UC1638C_Init();
	LCD_clear();										//����
	
	Tab[0] = PLAYLAST_Tab;
	Tab[1] = FILE_Tab;
	Tab[2] = APPS_Tab;
	Tab[3] = SETTINGS_Tab;
	Tab[4] = SubPortView_Tab;
	
	for(i=0;i<5;i++)								//�����๦�ܵ���ѡ��λ����0
		Tab[i].Location = 0;
	
	tab_flag.Tab_Level = 0;					//��ʾ������
	tab_flag.MainTab_choice =  0;		//���๦��0
	tab_flag.update_flag = 1;
	tab_flag.sound_flag = 1;
	
	strcpy(fpath,"1:");
	window_top = 0;
	for(i=0; i<file_name_num; i++)
	{
		strcpy(file_load[i].name,"");
		file_load[i].dir=0;
		file_load[i].type=0;
	}
	file_name_num = 0;

	scan_files(fpath,0);
	inversion_file_load();
	Tab[0].SubfunctionNum = file_name_num;

//	printf("file_name_num:%x\n\r",file_name_num);

}


//ui�����水����Ӧ����
void UI_Main_ButtonrRespond(u8 *ButtonAction)
{
	u8 i;
	if(*ButtonAction)
	{
		tab_flag.update_flag = 1;			//��Ĭ����Ҫ����
		tab_flag.sound_flag = 1;
	}
	switch(*ButtonAction)
	{
		case 0:
			tab_flag.update_flag = 0;
			tab_flag.sound_flag = 0;
			break;
		
		case 1:			//��
#if PID_DEBUG
			if(PID_state==0)
			{
				PID_P += 0.1f;
			}else if(PID_state==1)
			{
				PID_I += 0.1f;
			}else if(PID_state==2)
			{
				PID_D += 0.1f;
			}else if(PID_state==3)
			{
				PID_DM += 0.1f;
			}
#else 
			if(!Tab[tab_flag.MainTab_choice].Location)
			{
				tab_flag.update_flag = 0;			//����Ҫ����
				tab_flag.sound_flag = 0;
			}
			else 
			{
				Tab[tab_flag.MainTab_choice].Location--;
				if (Tab[tab_flag.MainTab_choice].Location < window_top)
				{
					window_top--;
				}
			}
#endif
			break;
			
		case 2:			//��

#if PID_DEBUG
			if(PID_state==0)
			{
				PID_P -= 0.1f;
			}else if(PID_state==1)
			{
				PID_I -= 0.1f;
			}else if(PID_state==2)
			{
				PID_D -= 0.1f;
			}else if(PID_state==3)
			{
				PID_DM -= 0.1f;
			}
#else 
			if(Tab[tab_flag.MainTab_choice].Location >= Tab[tab_flag.MainTab_choice].SubfunctionNum-1)
			{
				tab_flag.update_flag = 0;			//����Ҫ����
				tab_flag.sound_flag = 0;
			}
			else 
			{
				Tab[tab_flag.MainTab_choice].Location++;
				if (Tab[tab_flag.MainTab_choice].Location > window_top+4)
				{
					window_top++;
				}
			}
#endif
			
			break;
			
		case 3:			//��
			

#if PID_DEBUG
			if(0<= PID_state && PID_state <= 3)
			{
				PID_state--;
			}
#else 
			if(tab_flag.MainTab_choice<=0)
			{
				tab_flag.update_flag = 0;
				tab_flag.sound_flag = 0;
			}else
			{
				Tab[tab_flag.MainTab_choice].Location = 0;
				tab_flag.MainTab_choice--;
				
				strcpy(fpath,"1:");
				window_top = 0;
				for(i=0; i<file_name_num; i++)
				{
					strcpy(file_load[i].name,"");
					file_load[i].dir=0;
					file_load[i].type=0;
				}
				file_name_num = 0;
				if(tab_flag.MainTab_choice == 0)
				{
					scan_files(fpath,0);
					inversion_file_load();
					Tab[0].SubfunctionNum = file_name_num;
				}else if(tab_flag.MainTab_choice == 1)
				{
					scan_files(fpath,1);
					inversion_file_load();
					Tab[1].SubfunctionNum = file_name_num;
				}
			}
#endif
			break;
			
		case 4:			//��
#if PID_DEBUG
			if(0<= PID_state && PID_state <= 3)
			{
				PID_state++;
			}
#else 
			if(tab_flag.MainTab_choice>=3)
			{
				tab_flag.update_flag = 0;
				tab_flag.sound_flag = 0;
			}else
			{
				Tab[tab_flag.MainTab_choice].Location = 0;
				tab_flag.MainTab_choice++;
				
				strcpy(fpath,"1:");
				window_top = 0;
				for(i=0; i<file_name_num; i++)
				{
					strcpy(file_load[i].name,"");
					file_load[i].dir=0;
					file_load[i].type=0;
				}
				file_name_num = 0;
				if(tab_flag.MainTab_choice == 0)
				{
					scan_files(fpath,0);
					inversion_file_load();
					Tab[0].SubfunctionNum = file_name_num;
				}else if(tab_flag.MainTab_choice == 1)
				{
					scan_files(fpath,1);
					inversion_file_load();
					Tab[1].SubfunctionNum = file_name_num;
				}
			}
#endif
			break;
			
		case 5:			//ȷ��		
				if(tab_flag.MainTab_choice == 0)
				{
					if(file_name_num != 0)
					{	
						for(i=0; i<strlen(file_load[Tab[tab_flag.MainTab_choice].Location].path)+strlen(file_load[Tab[tab_flag.MainTab_choice].Location].name)+9; i++)
						{
							myfilename[i] = 0;
						}
						sprintf(&myfilename[strlen(myfilename)], "%s", file_load[Tab[tab_flag.MainTab_choice].Location].path); 
						sprintf(&myfilename[strlen(myfilename)], "/%s", file_load[Tab[tab_flag.MainTab_choice].Location].name); 
						sprintf(&myfilename[strlen(myfilename)], "%s", ".dbf"); 
						
//						for(i=0; i< strlen(file_load[Tab[tab_flag.MainTab_choice].Location].path)+strlen(file_load[Tab[tab_flag.MainTab_choice].Location].name)+8; i++)
//						{
//							printf("myfilename[%x]��%x\n",i,myfilename[i]);
//						}
						
						EnterProgram(myfilename);
//						tab_flag.Tab_Level = 1;			//ѡ���Ӻ�������
					}
				}else if(tab_flag.MainTab_choice == 1)
				{
					if(file_name_num != 0)
					{
						strcpy(oldfpath,fpath);
						sprintf(&fpath[strlen(fpath)], "/%s", file_load[Tab[tab_flag.MainTab_choice].Location].name); 
						for(i=0; i<file_name_num; i++)
						{
							strcpy(file_load[i].name,"");
							file_load[i].dir=0;
							file_load[i].type=0;
						}
						file_name_num = 0;
						window_top = 0;
						scan_files(fpath,1);
						inversion_file_load();
//						printf("fpath:%s \r\n", fpath);
//						printf("oldfpath:%s \r\n", oldfpath);
//						printf("file_name_num:%x \r\n", file_name_num);
						tab_flag.Tab_Level = 1;			//ѡ���Ӻ�������
					}
				}else
				{
					tab_flag.Tab_Level = 1;			//ѡ���Ӻ�������
				}
			break;
		
		case 6:			//����
			if(tab_flag.MainTab_choice)
			{
				//�����������
				Tab[tab_flag.MainTab_choice].Location = 0;
				tab_flag.MainTab_choice = 0;
				tab_flag.Tab_Level = 0;
				
				strcpy(fpath,"1:");
				window_top = 0;
				for(i=0; i<file_name_num; i++)
				{
					strcpy(file_load[i].name,"");
					file_load[i].dir=0;
					file_load[i].type=0;
				}
				file_name_num = 0;
				if(tab_flag.MainTab_choice == 0)
				{
					scan_files(fpath,0);
					inversion_file_load();
					Tab[0].SubfunctionNum = file_name_num;
				}else if(tab_flag.MainTab_choice == 1)
				{
					scan_files(fpath,1);
					inversion_file_load();
					Tab[1].SubfunctionNum = file_name_num;
				}
			}
			else	//�����������
			{
				tab_flag.Tab_Level = 2;			//ѡ��ػ���������
				tab_flag.ShutDown = 0;
			}
			break;
			
		case 17:
			if(tab_flag.MainTab_choice == 1)
				{
					if(file_name_num != 0)
					{
						sprintf(&fpath[strlen(fpath)], "/%s", file_load[Tab[tab_flag.MainTab_choice].Location].name); 
						tab_flag.Confirm = 0;
						tab_flag.Tab_Level = 6;			//ѡ���Ӻ�������
					}
				}else
				{
					tab_flag.update_flag = 0;
					tab_flag.sound_flag = 0;	
				}
		
				break;
				
//		case 18:
//			if(factorytest == 0)
//			{
//				factorytest = 1;
//			}else
//			{
//				factorytest = 0;
//			}
//				break;
		default:
				tab_flag.update_flag = 0;
				tab_flag.sound_flag = 0;	
				break;
	}

	*ButtonAction = 0;			//���������ÿ�
}

//ui�Ӻ������水����Ӧ����
void UI_Sub_ButtonrRespond(u8 *ButtonAction,u8 MainTab_choice,Tab_Function_TypeDef Tab[])
{
	int i;
	if(*ButtonAction)
	{
		tab_flag.update_flag = 1;			//��Ĭ����Ҫ����
		tab_flag.sound_flag = 1;
	}
	//�Բ�ͬ���Ӻ���������з���
	switch(MainTab_choice)					//�����ĸ����๦��
	{
		case 0:			//PLAYLAST_TAB
			switch(*ButtonAction)
					{
						case 0:
							tab_flag.update_flag = 0;
							tab_flag.sound_flag = 0;
							break;

						case 6://����
							EndProgram();
							if(LCD_flag == 1)
							{
								LCD_flag = 0;
							}
							tab_flag.Tab_Level = 0;					//ѡ��������
							Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							//���������겻������
							break;	
						default:
							tab_flag.update_flag = 0;
							tab_flag.sound_flag = 0;	
							break;
					}
					*ButtonAction = 0;			//���������ÿ�
			break;
			
		case 1:			//FILE_TAB
			switch(*ButtonAction)
					{
						case 0:
							tab_flag.update_flag = 0;
							tab_flag.sound_flag = 0;
							break;
						case 1:			//��
							if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]<=0)
							{
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;
								tab_flag.update_flag = 0;			//��Ĭ����Ҫ����
								tab_flag.sound_flag = 0;
							}else
							{
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]--;
								if (Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] < window_top)
								{
									window_top--;
								}
							}
//								tab_flag.update_flag = 0;				//����Ҫ����
							break;
							
						case 2:			//��	
							if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]>= file_name_num-1)
							{
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = file_name_num-1;
								tab_flag.update_flag = 0;			//��Ĭ����Ҫ����
								tab_flag.sound_flag = 0;
							}
							else
							{
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]++;
								if (Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] > (window_top + 4))
								{
									window_top++;
								}
							}
//								tab_flag.update_flag = 0;				//����Ҫ����
							break;
							
						case 3:			//��
//							tab_flag.update_flag = 0;				//����Ҫ����
//								tab_flag.sound_flag = 0;
						
								Tab[tab_flag.MainTab_choice].Location = 0;
								tab_flag.MainTab_choice--;
								
								strcpy(fpath,"1:");
								window_top = 0;
								for(i=0; i<file_name_num; i++)
								{
									strcpy(file_load[i].name,"");
									file_load[i].dir=0;
									file_load[i].type=0;
								}
								file_name_num = 0;
								if(tab_flag.MainTab_choice == 0)
								{
									scan_files(fpath,0);
									inversion_file_load();
									Tab[0].SubfunctionNum = file_name_num;
								}else if(tab_flag.MainTab_choice == 1)
								{
									scan_files(fpath,1);
									inversion_file_load();
									Tab[1].SubfunctionNum = file_name_num;
								}
								
							tab_flag.Tab_Level = 0;					//ѡ��������
							Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							break;
							
						case 4:			//��
//							tab_flag.update_flag = 0;				//����Ҫ����
//								tab_flag.sound_flag = 0;
						
								Tab[tab_flag.MainTab_choice].Location = 0;
								tab_flag.MainTab_choice++;
								
								strcpy(fpath,"1:");
								window_top = 0;
								for(i=0; i<file_name_num; i++)
								{
									strcpy(file_load[i].name,"");
									file_load[i].dir=0;
									file_load[i].type=0;
								}
								file_name_num = 0;
								if(tab_flag.MainTab_choice == 0)
								{
									scan_files(fpath,0);
									inversion_file_load();
									Tab[0].SubfunctionNum = file_name_num;
								}else if(tab_flag.MainTab_choice == 1)
								{
									scan_files(fpath,1);
									inversion_file_load();
									Tab[1].SubfunctionNum = file_name_num;
								}
								
							tab_flag.Tab_Level = 0;					//ѡ��������
							Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							break;
						
						case 5:			//ȷ��

							if(file_load[Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]].type == 0x01)
							{
								for(i=0; i<strlen(file_load[Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]].path)
												+ strlen(file_load[Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]].name)+9; i++)
								{
									myfilename[i] = 0;
								}
								sprintf(&myfilename[strlen(myfilename)], "%s", file_load[Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]].path); 
								sprintf(&myfilename[strlen(myfilename)], "/%s", file_load[Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]].name); 
								sprintf(&myfilename[strlen(myfilename)], "%s", ".dbf"); 
								
//								for(i=0; i< strlen(file_load[Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]].path)
//												+strlen(file_load[Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]].name)+8; i++)
//								{
//									printf("myfilename[%x]��%x\n",i,myfilename[i]);
//								}
								
								tab_flag.Tab_Level = 1;					//ѡ��������
//								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
								EnterProgram(myfilename);
							}else if(file_load[Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]].type == 0x02)
							{
								for(i=0; i<strlen(file_load[Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]].path)
												+ strlen(file_load[Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]].name)+9; i++)
								{
									myfilename[i] = 0;
								}
								sprintf(&myfilename[strlen(myfilename)], "%s", file_load[Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]].path); 
								sprintf(&myfilename[strlen(myfilename)], "/%s", file_load[Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]].name); 
								sprintf(&myfilename[strlen(myfilename)], "%s", ".dpf"); 
								
//								for(i=0; i< strlen(file_load[Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]].path)
//												+strlen(file_load[Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]].name)+8; i++)
//								{
//									printf("myfilename[%x]��%x\n",i,myfilename[i]);
//								}
								
								for(i=0; i < 1024*3; i++)
								{
									PICTURE_Buff[i]=0;
								}
								res_flash = f_open(&file2,(const TCHAR*)myfilename, FA_OPEN_EXISTING | FA_READ);	
								if(res_flash == FR_OK)
								{
//									printf("�����ļ��ɹ�-%x��\r\n",file2.fsize);
									res_flash = f_read(&file2, PICTURE_Buff,  1024*3, &fnum); 	
									if(res_flash == FR_OK)
									{
										tab_flag.update_flag = 1;
										tab_flag.Tab_Level = 5;					//ѡ��ͼƬ���Ž���
									}else
									{
									
									}
									f_close(&file2);
//									printf("�����ļ��ɹ�res_flash-%x��\r\n",res_flash);
								}
							}else if(file_load[Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]].type == 0x04)
							{
								for(i=0; i<strlen(file_load[Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]].path)
												+ strlen(file_load[Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]].name)+9; i++)
								{
									myfilename[i] = 0;
								}
								sprintf(&myfilename[strlen(myfilename)], "%s", file_load[Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]].path); 
								sprintf(&myfilename[strlen(myfilename)], "/%s", file_load[Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]].name); 
								sprintf(&myfilename[strlen(myfilename)], "%s", ".mp3"); 
								
								res_flash = f_open(&file3,(const TCHAR*)myfilename, FA_OPEN_EXISTING | FA_READ);
								if(res_flash == FR_OK)
								{
									SOUND_Buff_index = 0;
									SOUND_Buff_flag = 1;
									SOUND_UI = 1;
//									printf("res_flash:%x--%d\r\n",res_flash,file3.fsize);
								}
							}else{}
							break;
						
						case 6://����
//							EndProgram();
							if(LCD_flag == 1)
							{
								LCD_flag = 0;
							}
							
							strcpy(fpath,oldfpath);
							for(i=0; i<file_name_num; i++)
							{
								strcpy(file_load[i].name,"");
								file_load[i].dir=0;
								file_load[i].type=0;
							}
							file_name_num = 0;
							window_top = 0;
							scan_files(fpath,1);
							inversion_file_load();
//							printf("fpath:%s\n\r",fpath);
//							printf("oldfpath:%s\n\r",oldfpath);
//							printf("file_name_num:%x\n\r",file_name_num);
						
							tab_flag.Tab_Level = 0;					//ѡ��������
							Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							//���������겻������
							break;	
						default:
							tab_flag.update_flag = 0;
							tab_flag.sound_flag = 0;	
							break;
					}
					*ButtonAction = 0;			//���������ÿ�
			break;
			
		case 2:			//APPS_TAB
			switch(Tab[MainTab_choice].Location)					//�����ĸ����๦��
			{
				case 0:			//Port View
					switch(*ButtonAction)
					{	
						case 0:
							tab_flag.update_flag = 1;
							tab_flag.sound_flag = 0;
							break;
						
						case 1:			//��
							if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]>3)		
							{
								tab_flag.update_flag = 0;			//����Ҫ����
								tab_flag.sound_flag = 0;
							}
							else 
							{
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] + 4;
							}
							break;
							
						case 2:			//��
							if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]<=3)		
							{
								tab_flag.update_flag = 0;			//����Ҫ����
								tab_flag.sound_flag = 0;
							}
							else 
							{
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] - 4;
							}
							break;
							
						case 3:			//��
							if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]<=0)
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 7;
							else
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]--;
							break;
							
						case 4:			//��
							if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]>=7)
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;
							else
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]++;
							break;
						
						case 5:			//ȷ��
								tab_flag.MainTab_choice = 4; //ѡ���Ӻ���portview��������
							break;
						
						case 6:			//����
							tab_flag.Tab_Level = 0;					//ѡ��������
							Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							//���������겻������
							break;
						default:
							tab_flag.update_flag = 0;
							tab_flag.sound_flag = 0;	
							break;
					}
					*ButtonAction = 0;			//���������ÿ�
					break;
					
				case 1:			//Motor Control  //�������
					switch(*ButtonAction)
					{
						case 0:			//����
							if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]%0x10)	//�������������	
							{
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] & 0xF0;
								//���ֹͣ
								Motor_PowerSet(5,0);
								Motor_PowerSet(6,0);
								Motor_PowerSet(7,0);
								Motor_PowerSet(8,0);
								motorcontrol_flag = 0;
								STM_EVAL_LEDOn(LED4);															
								STM_EVAL_LEDOff(LED3);
							}
							tab_flag.sound_flag = 0;
							break;
						case 13:			//�ϣ���ס A��B�˿�ǰ��
							motorcontrol_flag = 1;
							STM_EVAL_LEDToggle(LED4);															
							STM_EVAL_LEDOff(LED3);
							if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]/0x10)	//��������ʹ��BC�˿�		
							{
								if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]==0x10)	//����BC��	
								{
									Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0x11;	//B��C��
									//B�˿ڵ��ǰ��
									Motor_PowerSet(6,80);						
								}
								else if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]==0x11)	//����B��C��	
								{
									tab_flag.update_flag = 0;			//����Ҫ����
									tab_flag.sound_flag = 0;
								}
							}
							else //��������ʹ��AD�˿�	
							{
								if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]==0x00)	//����AD��	
								{
									Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0x01;	//A��D��
									//A�˿ڵ��ǰ��
									Motor_PowerSet(5,80);		
								}
								else if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]==0x01)	
								{
									tab_flag.update_flag = 0;			//����Ҫ����
									tab_flag.sound_flag = 0;
								}
							}
								
							break;
							
						case 14:			//�£���ס A��B�˿ں���
							motorcontrol_flag = 1;
							STM_EVAL_LEDToggle(LED4);															
							STM_EVAL_LEDOff(LED3);
							if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]/0x10)	//��������ʹ��BC�˿�		
							{
								if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]==0x10)	//����BC��	
								{
									Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0x11;	//����B��C��
									//B�˿ڵ������
									Motor_PowerSet(6,-80);		
								}
								else if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]==0x11)	//����B��C��	
								{
									tab_flag.update_flag = 0;			//����Ҫ����
									tab_flag.sound_flag = 0;
								}
							}
							else //��������ʹ��AD�˿�	
							{
								if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]==0x00)	//����AD��	
								{
									Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0x01;	//A��D��
									//A�˿ڵ������
									Motor_PowerSet(5,-80);
								}
								else if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]==0x01)	
								{
									tab_flag.update_flag = 0;			//����Ҫ����
									tab_flag.sound_flag = 0;
								}
							}
							break;
							
						case 15:			//��, ��ס C��D�˿�ǰ��
							motorcontrol_flag = 1;
							STM_EVAL_LEDToggle(LED4);															
							STM_EVAL_LEDOff(LED3);
							if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]/0x10)	//��������ʹ��BC�˿�		
							{
								if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]==0x10)	//����BC��	
								{
									Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0x12;	//C��B��
									//C�˿ڵ��ǰ��
									Motor_PowerSet(7,80);
									
								}
								else if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]==0x12)	//C��B��	
								{
									tab_flag.update_flag = 0;			//����Ҫ����
									tab_flag.sound_flag = 0;
								}
							}
							else //��������ʹ��AD�˿�	
							{
								if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]==0x00)	//����AD��	
								{
									Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0x02;	//D��A��
									//D�˿ڵ��ǰ��
									Motor_PowerSet(8,80);
								}
								else if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]==0x02)	
								{
									tab_flag.update_flag = 0;			//����Ҫ����
									tab_flag.sound_flag = 0;
								}
							}
								
							break;
							
						case 16:			//��, ��ס C��D�˿ں���
							motorcontrol_flag = 1;
							STM_EVAL_LEDToggle(LED4);															
							STM_EVAL_LEDOff(LED3);
							if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]/0x10)	//��������ʹ��BC�˿�		
							{
								if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]==0x10)	//����BC��	
								{
									Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0x12;	//C��B��
									//C�˿ڵ������
									Motor_PowerSet(7,-80);
								}
								else if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]==0x12)	//C��B��	
								{
									tab_flag.update_flag = 0;			//����Ҫ����
									tab_flag.sound_flag = 0;
								}
							}
							else //��������ʹ��AD�˿�	
							{
								if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]==0x00)	//����AD��	
								{
									Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0x02;	//D��A��
									//D�˿ڵ������
									Motor_PowerSet(8,-80);
								}
								else if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location]==0x02)	
								{
									tab_flag.update_flag = 0;			//����Ҫ����
									tab_flag.sound_flag = 0;
								}
							}
							break;
							
						case 5:			//ȷ�ϣ��л��˿�
							if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location])
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0x00;		//����ʹ��AD�˿���Ϊһ��,�ް��·����
							else
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0x10;		//����ʹ��BC�˿���Ϊһ��,�ް��·����
							*ButtonAction = 0;			//���������ÿ�
							break;
						
						case 6:			//����
							motorcontrol_flag = 0;
							STM_EVAL_LEDOn(LED4);															
							STM_EVAL_LEDOff(LED3);
							tab_flag.Tab_Level = 0;					//ѡ��������
							Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0x00;			//�Ӻ�����������
							//���������겻������
							//���ֹͣ
							Large_Motor_Off(5,0);
							Large_Motor_Off(6,0);
							Large_Motor_Off(7,0);
							Large_Motor_Off(8,0);
							*ButtonAction = 0;			//���������ÿ�

							break;
					case 7:			//����
							tab_flag.sound_flag = 0;
							*ButtonAction = 0;
							break;
					case 8:			//����
							tab_flag.sound_flag = 0;
							*ButtonAction = 0;
							break;
					case 9:			//����
							tab_flag.sound_flag = 0;
							*ButtonAction = 0;
							break;
					case 10:		//����
							tab_flag.sound_flag = 0;
							*ButtonAction = 0;
							break;
					default:
							tab_flag.update_flag = 0;
							tab_flag.sound_flag = 0;	
							break;
					}
//					printf("*ButtonAction=%x\n\r",*ButtonAction);
//					*ButtonAction = 0;			//���������ÿ�
					break;
		  case 2:			//Ir_Control  //�������
					switch(*ButtonAction)
					{
						case 0:
							ChangeMode(4,2);
							if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] == 0x00)
							{
								motorcontrol_flag = 1;
								STM_EVAL_LEDToggle(LED4);															
								STM_EVAL_LEDOff(LED3);
								
								if(Uart4RecMes[3] == 0x01)
								{
									Motor_PowerSet(6,80);
									Motor_PowerSet(7,0);
								}
								if(Uart4RecMes[3] == 0x02)
								{
									Motor_PowerSet(6,-80);
									Motor_PowerSet(7,0);
								}
								if(Uart4RecMes[3] == 0x03)
								{
									Motor_PowerSet(6,0);
									Motor_PowerSet(7,80);
								}
								if(Uart4RecMes[3] == 0x04)
								{
									Motor_PowerSet(6,0);
									Motor_PowerSet(7,-80);
								}
								if(Uart4RecMes[3] == 0x05)
								{
									Motor_PowerSet(6,80);
									Motor_PowerSet(7,80);
								}
								if(Uart4RecMes[3] == 0x06)
								{
									Motor_PowerSet(6,80);
									Motor_PowerSet(7,-80);
								}
								if(Uart4RecMes[3] == 0x07)
								{
									Motor_PowerSet(6,-80);
									Motor_PowerSet(7,80);
								}
								if(Uart4RecMes[3] == 0x08)
								{
									Motor_PowerSet(6,-80);
									Motor_PowerSet(7,-80);
								}
								
								if(Uart4RecMes[4] == 0x01)
								{
									Motor_PowerSet(5,80);
									Motor_PowerSet(8,0);
								}
								if(Uart4RecMes[4] == 0x02)
								{
									Motor_PowerSet(5,-80);
									Motor_PowerSet(8,0);
								}
								if(Uart4RecMes[4] == 0x03)
								{ 
									Motor_PowerSet(5,0);
									Motor_PowerSet(8,80);
								}
								if(Uart4RecMes[4] == 0x04)
								{
									Motor_PowerSet(5,0);
									Motor_PowerSet(8,-80);
								}
								if(Uart4RecMes[4] == 0x05)
								{
									Motor_PowerSet(5,80);
									Motor_PowerSet(8,80);
								}
								if(Uart4RecMes[4] == 0x06)
								{
									Motor_PowerSet(5,80);
									Motor_PowerSet(8,-80);
								}
								if(Uart4RecMes[4] == 0x07)
								{ 
									Motor_PowerSet(5,-80);
									Motor_PowerSet(8,80);
								}
								if(Uart4RecMes[4] == 0x08)
								{
									Motor_PowerSet(5,-80);
									Motor_PowerSet(8,-80);
								}
								
								if(Uart4RecMes[3] == 0x00 && Uart4RecMes[4] == 0x00)
								{
									motorcontrol_flag = 0;
									STM_EVAL_LEDOn(LED4);															
									STM_EVAL_LEDOff(LED3);
									Motor_PowerSet(5,0);
									Motor_PowerSet(6,0);
									Motor_PowerSet(7,0);
									Motor_PowerSet(8,0);
								}
							}else if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] == 0x10)
							{
								motorcontrol_flag = 1;
								STM_EVAL_LEDToggle(LED4);															
								STM_EVAL_LEDOff(LED3);
								if(Uart4RecMes[1] == 1)
								{
									Motor_PowerSet(6,80);
									Motor_PowerSet(7,0);
								}
								if(Uart4RecMes[1] == 2)
								{
									Motor_PowerSet(6,-80);
									Motor_PowerSet(7,0);
								}
								if(Uart4RecMes[1] == 3)
								{
									Motor_PowerSet(6,0);
									Motor_PowerSet(7,80);
								}
								if(Uart4RecMes[1] == 4)
								{
									Motor_PowerSet(6,0);
									Motor_PowerSet(7,-80);
								}
								if(Uart4RecMes[1] == 5)
								{
									Motor_PowerSet(6,80);
									Motor_PowerSet(7,80);
								}
								if(Uart4RecMes[1] == 6)
								{
									Motor_PowerSet(6,80);
									Motor_PowerSet(7,-80);
								}
								if(Uart4RecMes[1] == 7)
								{
									Motor_PowerSet(6,-80);
									Motor_PowerSet(7,80);
								}
								if(Uart4RecMes[1] == 8)
								{
									Motor_PowerSet(6,-80);
									Motor_PowerSet(7,-80);
								}
								
								if(Uart4RecMes[2] == 1)
								{
									Motor_PowerSet(5,80);
									Motor_PowerSet(8,0);
								}
								if(Uart4RecMes[2] == 2)
								{
									Motor_PowerSet(5,-80);
									Motor_PowerSet(8,0);
								}
								if(Uart4RecMes[2] == 3)
								{
									Motor_PowerSet(5,0);
									Motor_PowerSet(8,80);
								}
								if(Uart4RecMes[2] == 4)
								{
									Motor_PowerSet(5,0);
									Motor_PowerSet(8,-80);
								}
								if(Uart4RecMes[2] == 5)
								{
									Motor_PowerSet(5,80);
									Motor_PowerSet(8,80);
								}
								if(Uart4RecMes[2] == 6)
								{
									Motor_PowerSet(5,80);
									Motor_PowerSet(8,-80);
								}
								if(Uart4RecMes[2] == 7)
								{
									Motor_PowerSet(5,-80);
									Motor_PowerSet(8,80);
								}
								if(Uart4RecMes[2] == 8)
								{
									Motor_PowerSet(5,-80);
									Motor_PowerSet(8,-80);
								}
								
								if(Uart4RecMes[1] == 0x00 && Uart4RecMes[2] == 0x00)
								{
									motorcontrol_flag = 0;
									STM_EVAL_LEDOn(LED4);															
									STM_EVAL_LEDOff(LED3);
									Motor_PowerSet(5,0);
									Motor_PowerSet(6,0);
									Motor_PowerSet(7,0);
									Motor_PowerSet(8,0);
								}
							}
//							printf("Uart4RecMes[1]=%x\n\r",Uart4RecMes[1]);
//							printf("Uart4RecMes[2]=%x\n\r",Uart4RecMes[2]);
//							printf("Uart4RecMes[3]=%x\n\r",Uart4RecMes[3]);
//							printf("Uart4RecMes[4]=%x\n\r",Uart4RecMes[4]);
							break;
						case 5:			//ȷ�ϣ��л��˿�
							if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location])
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0x00;		
							else
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0x10;		
//							*ButtonAction = 0;			//���������ÿ�
							break;	
						case 6://����
							motorcontrol_flag = 0;
							tab_flag.Tab_Level = 0;					//ѡ��������
							Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							//���������겻������
							break;	
						default:
							tab_flag.update_flag = 0;
							tab_flag.sound_flag = 0;	
							break;
					}
					*ButtonAction = 0;			//���������ÿ�
					break;	
			}
			
			break;
			
		case 3:			//SETTINGS_TAB
			switch(Tab[MainTab_choice].Location)			//�����ĸ����๦��
			{
				case 0:			//volume
					switch(*ButtonAction)
					{
						case 0:
								tab_flag.update_flag = 1;
								tab_flag.sound_flag = 0;
							break;
						
						case 1:			//��
								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
							
						case 2:			//��	
								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
							
						case 3:			//��
							if(sound_volume <= 0)
							{
								sound_volume = 0;
								tab_flag.update_flag = 0;
								tab_flag.sound_flag = 0;	
							}else
							{
								sound_volume -= 10;
								MYDMA_Disable(DMA1_Stream5,SPI_MP3_BUFNUM);
								SPI_I2S_DMACmd(SPI3,SPI_I2S_DMAReq_Tx,DISABLE);
								SPI_I2S_DeInit(SPI3);
								Mp3_GPIO_Init();//IO SPI
								VS_Set_Vol(sound_volume);
								SPI_I2S_DMACmd(SPI3,SPI_I2S_DMAReq_Tx,ENABLE);
								XDCS_0();	
							}

//								tab_flag.update_flag = 0;				//����Ҫ����
							break;
							
						case 4:			//��
							if(sound_volume >= 100)
							{
								sound_volume = 100;
								tab_flag.update_flag = 0;
								tab_flag.sound_flag = 0;	
							}else
							{
								sound_volume += 10;								
								MYDMA_Disable(DMA1_Stream5,SPI_MP3_BUFNUM);
								SPI_I2S_DMACmd(SPI3,SPI_I2S_DMAReq_Tx,DISABLE);
								SPI_I2S_DeInit(SPI3);
								Mp3_GPIO_Init();//IO SPI
								VS_Set_Vol(sound_volume);
								SPI_I2S_DMACmd(SPI3,SPI_I2S_DMAReq_Tx,ENABLE);
								XDCS_0();	
							}
							
//								tab_flag.update_flag = 0;				//����Ҫ����
							break;
						
						case 5:			//ȷ��
							
								W25QXX_Write(&sound_volume, volume_ADDR,1);
	
								tab_flag.Tab_Level = 0;					//ѡ��������
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							break;
						
						case 6:			//����
							
								W25QXX_Read(&sound_volume, volume_ADDR,1);
						
								MYDMA_Disable(DMA1_Stream5,SPI_MP3_BUFNUM);
								SPI_I2S_DMACmd(SPI3,SPI_I2S_DMAReq_Tx,DISABLE);
								SPI_I2S_DeInit(SPI3);
								Mp3_GPIO_Init();//IO SPI
								VS_Set_Vol(sound_volume);
								SPI_I2S_DMACmd(SPI3,SPI_I2S_DMAReq_Tx,ENABLE);
								XDCS_0();
						
							  tab_flag.Tab_Level = 0;					//ѡ��������
							  Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							//���������겻������
							break;
						default:
							tab_flag.update_flag = 0;
							tab_flag.sound_flag = 0;	
							break;
					}
					*ButtonAction = 0;			//���������ÿ�
					break;
				case 1:			//Sleep
					switch(*ButtonAction)
					{
						case 0:
								tab_flag.update_flag = 0;
								tab_flag.sound_flag = 0;
							break;
						
						case 1:			//��
								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
							
						case 2:			//��
								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
							
					case 3:			//��
							if(sleep_time<=0)
							{
								sleep_time = 0;
								tab_flag.update_flag = 0;
								tab_flag.sound_flag = 0;	
							}else
								sleep_time -= 1;
//								tab_flag.update_flag = 0;				//����Ҫ����
							break;
							
						case 4:			//��
							if(sleep_time>=5)
							{
								sleep_time = 5;
								tab_flag.update_flag = 0;
								tab_flag.sound_flag = 0;	
							}else
								sleep_time += 1;
//								tab_flag.update_flag = 0;				//����Ҫ����
							break;
						
						case 5:			//ȷ��
//								tab_flag.MainTab_choice = 3;
								W25QXX_Write(&sleep_time, sleep_ADDR,1);
								tab_flag.Tab_Level = 0;					//ѡ��������
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							break;
						
						case 6:			//����
							W25QXX_Read(&sleep_time, sleep_ADDR,1);
							tab_flag.Tab_Level = 0;					//ѡ��������
							Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							//���������겻������
							break;
						default:
							tab_flag.update_flag = 0;
							tab_flag.sound_flag = 0;	
							break;
					}
					*ButtonAction = 0;			//���������ÿ�
					break;
					
				case 2:			//Bluetooth
					bluetooth_respond( ButtonAction );
					*ButtonAction = 0;			//���������ÿ�
					break;
				
				case 3:			//brick info
					switch(*ButtonAction)
					{
						case 0:
								tab_flag.update_flag = 0;
								tab_flag.sound_flag = 0;
							break;
						
						case 1:			//��
								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
							
						case 2:			//��
								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
							
						case 3:			//��
								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
							
						case 4:			//��
								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
						
						case 5:			//ȷ��
//								tab_flag.MainTab_choice = 3;
								tab_flag.Tab_Level = 0;					//ѡ��������
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							break;
						
						case 6:			//����
							tab_flag.Tab_Level = 0;					//ѡ��������
							Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							//���������겻������
							break;
						default:
							tab_flag.update_flag = 0;
							tab_flag.sound_flag = 0;	
							break;
					}
					*ButtonAction = 0;			//���������ÿ�
					break;
				case 4:			//language
					switch(*ButtonAction)
					{
						case 0:
								tab_flag.update_flag = 0;
								tab_flag.sound_flag = 0;
							break;
						
						case 1:			//��
								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
							
						case 2:			//��
								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
							
						case 3:			//��
								if(language <= 0)
								{
									tab_flag.update_flag = 0;				//����Ҫ����
									tab_flag.sound_flag = 0;
								}else
								{
									language=0;
								}
							break;
							
						case 4:			//��
								if(language >= 1)
								{
									tab_flag.update_flag = 0;				//����Ҫ����
									tab_flag.sound_flag = 0;
								}else
								{
									language=1;
								}
							break;
						
						case 5:			//ȷ��
//								tab_flag.MainTab_choice = 3;
								W25QXX_Write(&language, language_ADDR,1);
								tab_flag.Tab_Level = 0;					//ѡ��������
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							break;
						
						case 6:			//����
								W25QXX_Read(&language, language_ADDR,1);
							tab_flag.Tab_Level = 0;					//ѡ��������
							Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							//���������겻������
							break;
						default:
							tab_flag.update_flag = 0;
							tab_flag.sound_flag = 0;	
							break;
					}
					*ButtonAction = 0;			//���������ÿ�
					break;
					
				case 5:			//light
					switch(*ButtonAction)
					{
						case 0:
								tab_flag.update_flag = 1;
								tab_flag.sound_flag = 0;
							break;
						
						case 1:			//��
								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
							
						case 2:			//��	
								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
							
						case 3:			//��
							if(light_volume <= 0)
							{
								light_volume = 0;
								tab_flag.update_flag = 0;
								tab_flag.sound_flag = 0;	
							}else
							{
								light_volume -= 10;						
							}
//							tab_flag.update_flag = 0;				//����Ҫ����
							break;
							
						case 4:			//��
							if(light_volume >= 100)
							{
								light_volume = 100;
								tab_flag.update_flag = 0;
								tab_flag.sound_flag = 0;	
							}else
							{
								light_volume += 10;								
							}
//							tab_flag.update_flag = 0;				//����Ҫ����
							break;
						
						case 5:			//ȷ��
							
								W25QXX_Write(&light_volume, light_ADDR,1);
	
								tab_flag.Tab_Level = 0;					//ѡ��������
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							break;
						
						case 6:			//����
							
								W25QXX_Read(&light_volume, light_ADDR,1);
							  tab_flag.Tab_Level = 0;					//ѡ��������
							  Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							//���������겻������
							break;
						default:
							tab_flag.update_flag = 0;
							tab_flag.sound_flag = 0;	
							break;
					}
					*ButtonAction = 0;			//���������ÿ�
					break;
					
				case 6:			//file_format
					switch(*ButtonAction)
					{
						case 0:
								tab_flag.update_flag = 1;
								tab_flag.sound_flag = 0;
							break;
						
						case 1:			//��
								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
							
						case 2:			//��	
								tab_flag.update_flag = 0;				//����Ҫ����
								tab_flag.sound_flag = 0;
							break;
							
						case 3:			//��
								if(format_flag <= 0)
								{
									tab_flag.update_flag = 0;				//����Ҫ����
									tab_flag.sound_flag = 0;
								}else
								{
									format_flag=0;
								}
							break;
							
						case 4:			//��
								if(format_flag >= 1)
								{
									tab_flag.update_flag = 0;				//����Ҫ����
									tab_flag.sound_flag = 0;
								}else
								{
									format_flag=1;
								}
							break;
						
						case 5:			//ȷ��
							if(format_flag == 1)
							{
								printf("�����ڸ�ʽ���ļ�ϵͳ��\r\n");
								res_flash=f_mkfs("1:",0,0);							
								if(res_flash == FR_OK)
								{
									printf("��FLASH�ѳɹ���ʽ���ļ�ϵͳ��\r\n");
									/* ��ʽ������ȡ������ */
									res_flash = f_mount(NULL,"1:",1);			
									/* ���¹���	*/			
									res_flash = f_mount(&fs,"1:",1);
									printf("�����¹����ļ�ϵͳ��\r\n");
								}else
								{
									printf("��FLASH��ʽ���ļ�ϵͳʧ�ܡ�\r\n");
								}
							}
						
								tab_flag.Tab_Level = 0;					//ѡ��������
								Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							break;
						
						case 6:			//����
							format_flag = 0;
							  tab_flag.Tab_Level = 0;					//ѡ��������
							  Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location] = 0;			//�Ӻ�����������
							//���������겻������
							break;
						default:
							tab_flag.update_flag = 0;
							tab_flag.sound_flag = 0;	
							break;
					}
					*ButtonAction = 0;			//���������ÿ�
					break;
			}
//			tab_flag.Tab_Level = 0;			//ѡ��������
			*ButtonAction = 0;			//���������ÿ�
			break;
			
		case 4:			//
			switch(*ButtonAction)
			{
				case 0:
					tab_flag.update_flag = 1;
					tab_flag.sound_flag = 0;
					break;
				
				case 1:			//��
					if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[2].SubLocation[Tab[2].Location]] <= 0)
					{
						Tab[tab_flag.MainTab_choice].SubLocation[Tab[2].SubLocation[Tab[2].Location]] =0;
						tab_flag.update_flag = 0;
						tab_flag.sound_flag = 0;
					}
					else
					{
						Tab[tab_flag.MainTab_choice].SubLocation[Tab[2].SubLocation[Tab[2].Location]]--;
					}
					break;
					
				case 2:			//��
					if(Tab[tab_flag.MainTab_choice].SubLocation[Tab[2].SubLocation[Tab[2].Location]] >= 
								GetShowModeNum(Channel_State[Tab[2].SubLocation[Tab[2].Location]][1])-1)
					{
						tab_flag.update_flag = 0;
						tab_flag.sound_flag = 0;
					}
					else
					{
						Tab[tab_flag.MainTab_choice].SubLocation[Tab[2].SubLocation[Tab[2].Location]]++;
					}
					break;
				case 5:			//ȷ�� 
					ChangeSensorMode( Tab[2].SubLocation[Tab[2].Location],
														Channel_State[Tab[2].SubLocation[Tab[2].Location]][1],  
														Tab[tab_flag.MainTab_choice].SubLocation[Tab[2].SubLocation[Tab[2].Location]]);
					tab_flag.MainTab_choice = 2;
					break;
				case 6:			//����
					Tab[tab_flag.MainTab_choice].SubLocation[Tab[2].SubLocation[Tab[2].Location]]=0;
					tab_flag.MainTab_choice = 2;
					break;
				default:
					tab_flag.update_flag = 0;
					tab_flag.sound_flag = 0;	
				break;
			}
			*ButtonAction = 0;			//���������ÿ�
			break;
			
		default:
				tab_flag.update_flag = 0;
				tab_flag.sound_flag = 0;	
				break;
	}
}

//ui�ػ��������水����Ӧ����
void UI_Shutdown_ButtonRespond(u8 *ButtonAction)
{
	if(*ButtonAction)
	{
		tab_flag.update_flag = 1;			//��Ĭ����Ҫ����
		tab_flag.sound_flag = 1;
	}
	switch(*ButtonAction)
	{
		case 0:			
			tab_flag.update_flag = 0;			//����Ҫ����
			tab_flag.sound_flag = 0;
			break;
		
		case 1:			//��
			tab_flag.update_flag = 0;			//����Ҫ����
			tab_flag.sound_flag = 0;
			break;
			
		case 2:			//��
			tab_flag.update_flag = 0;			//����Ҫ����
			tab_flag.sound_flag = 0;
			break;
			
		case 3:			//��
			if(tab_flag.ShutDown)
				tab_flag.ShutDown = 0;
			else
			{
				tab_flag.update_flag = 0;			//����Ҫ����
				tab_flag.sound_flag = 0;
			}
				break;
			
		case 4:			//��
			if(!tab_flag.ShutDown)
				tab_flag.ShutDown = 1;
			else
			{
				tab_flag.update_flag = 0;			//����Ҫ����
				tab_flag.sound_flag = 0;
			}
			break;
			
		case 5:			//ȷ��
			if(tab_flag.ShutDown)
			{
				//����ػ�����
				NVIC_SystemReset();
				//�ػ�
			}
			else
			{
				tab_flag.Tab_Level = 0;			//ѡ��������
				tab_flag.MainTab_choice = 0;
				Tab[tab_flag.MainTab_choice].Location = 0;
			}
			break;
		
		case 6:			//����
			tab_flag.Tab_Level = 0;			//ѡ��������
			tab_flag.MainTab_choice = 0;
			Tab[tab_flag.MainTab_choice].Location = 0;
			break;
		default:
				tab_flag.update_flag = 0;
				tab_flag.sound_flag = 0;	
				break;
	}

	*ButtonAction = 0;			//���������ÿ�
}

//�������н��水����Ӧ����
void UI_Running_ButtonRespond(u8 *ButtonAction)
{
	if(*ButtonAction)
	{
		tab_flag.update_flag = 1;			//��Ĭ����Ҫ����
		tab_flag.sound_flag = 1;
	}
	
	switch(*ButtonAction)
	{
		case 0:			//
			tab_flag.update_flag = 0;			//����Ҫ����
			tab_flag.sound_flag = 0;
			break;
		
		case 1:			//��
			tab_flag.update_flag = 0;			//����Ҫ����
			tab_flag.sound_flag = 0;
			break;
			
		case 2:			//��
			tab_flag.update_flag = 0;			//����Ҫ����
			tab_flag.sound_flag = 0;
			break;
			
		case 3:			//��
			tab_flag.update_flag = 0;			//����Ҫ����
			tab_flag.sound_flag = 0;
		break;
			
		case 4:			//��
			tab_flag.update_flag = 0;			//����Ҫ����
			tab_flag.sound_flag = 0;
		break;
			
		case 5:			//ȷ��
			tab_flag.update_flag = 0;			//����Ҫ����
			tab_flag.sound_flag = 0;
		break;
		 
		case 6:			//����
			if(LCD_flag == 1)
			{
				LCD_flag = 0;
			}
			EndProgram();

			//��ͣ���
			Motor_DirectionSet(5,0);
			Motor_DirectionSet(6,0);
			Motor_DirectionSet(7,0);
			Motor_DirectionSet(8,0);
			
//			*ButtonAction = 0;			//���������ÿ�
			break;
		default:
				tab_flag.update_flag = 0;
				tab_flag.sound_flag = 0;	
				break;
	}
	*ButtonAction = 0;			//���������ÿ�
}
//ͼƬ���水����Ӧ����
void PICTURE_ButtonRespond(u8 *ButtonAction)
{
	if(*ButtonAction)
	{
		tab_flag.update_flag = 1;			//��Ĭ����Ҫ����
		tab_flag.sound_flag = 1;
	}
	switch(*ButtonAction)
	{
		case 0:			
			tab_flag.update_flag = 0;			//����Ҫ����
			tab_flag.sound_flag = 0;
			break;
		
		case 1:			//��
			tab_flag.update_flag = 0;			//����Ҫ����
			tab_flag.sound_flag = 0;
			break;
			
		case 2:			//��
			tab_flag.update_flag = 0;			//����Ҫ����
			tab_flag.sound_flag = 0;
			break;
			
		case 3:			//��
			tab_flag.update_flag = 0;			//����Ҫ����
			tab_flag.sound_flag = 0;
				break;
			
		case 4:			//��
			tab_flag.update_flag = 0;			//����Ҫ����
			tab_flag.sound_flag = 0;
			break;
		case 5:			//ȷ��
			tab_flag.update_flag = 0;			//����Ҫ����
			tab_flag.sound_flag = 0;
			break;
		case 6:			//����
			if(LCD_flag == 1)
			{
				LCD_flag = 0;
			}
//			LCD_flag = 0;
			tab_flag.Tab_Level = 1;			//ѡ���ӽ���
			tab_flag.update_flag = 1;
			break;
		default:
			tab_flag.update_flag = 0;			//��Ĭ����Ҫ����
			tab_flag.sound_flag = 0;
			break;
	}
	*ButtonAction = 0;			//���������ÿ�
}

//uiȷ�ϲ������水����Ӧ����
void UI_Confirm_ButtonRespond(u8 *ButtonAction)
{
	u8 i;
	if(*ButtonAction)
	{
		tab_flag.update_flag = 1;			//��Ĭ����Ҫ����
		tab_flag.sound_flag = 1;
	}
	switch(*ButtonAction)
	{
		case 0:			
			tab_flag.update_flag = 0;			//����Ҫ����
			tab_flag.sound_flag = 0;
			break;
		
		case 1:			//��
			tab_flag.update_flag = 0;			//����Ҫ����
			tab_flag.sound_flag = 0;
			break;
			
		case 2:			//��
			tab_flag.update_flag = 0;			//����Ҫ����
			tab_flag.sound_flag = 0;
			break;
			
		case 3:			//��
			if(tab_flag.Confirm)
				tab_flag.Confirm = 0;
			else
			{
				tab_flag.update_flag = 0;			//����Ҫ����
				tab_flag.sound_flag = 0;
			}
				break;
			
		case 4:			//��
			if(!tab_flag.Confirm)
				tab_flag.Confirm = 1;
			else
			{
				tab_flag.update_flag = 0;			//����Ҫ����
				tab_flag.sound_flag = 0;
			}
			break;
			
		case 5:			//ȷ��
			if(tab_flag.Confirm)
			{
				//ȷ��
				res_flash = f_deldir(fpath);
				
				strcpy(fpath,"1:");
				window_top = 0;
				for(i=0; i<file_name_num; i++)
				{
					strcpy(file_load[i].name,"");
					file_load[i].dir=0;
					file_load[i].type=0;
				}
				file_name_num = 0;
				scan_files(fpath,1);
				inversion_file_load();
				Tab[1].SubfunctionNum = file_name_num;
				
				tab_flag.Tab_Level = 0;			//ѡ��������
			}
			else
			{
				tab_flag.Tab_Level = 0;			//ѡ��������
			}
			break;
		
		case 6:			//����
			tab_flag.Tab_Level = 0;			//ѡ��������
			break;
		default:
			tab_flag.update_flag = 0;			//��Ĭ����Ҫ����
			tab_flag.sound_flag = 0;
			break;
	}

	*ButtonAction = 0;			//���������ÿ�
}
void UI_connecting_BluetoothRespond(u8 *ButtonAction)	   //�����ӻ�������Ӧ����
{
	static u8 i=1;
	LCD_Display_pic(22, 50, 130, 52,bluetooth_linkjudge);
	if(*ButtonAction)
	{
		tab_flag.update_flag = 1;			//��Ĭ����Ҫ����
		tab_flag.sound_flag = 1;
	}
	
	switch(*ButtonAction)
	{
		case 0:			//
				tab_flag.update_flag = 0;			//����Ҫ����
				tab_flag.sound_flag = 0;
			break;
		
		case 1:			//��
				if(i==1)
					i=0;			
//			tab_flag.update_flag = 0;			//����Ҫ����
//			tab_flag.sound_flag = 0;
			break;
			
		case 2:			//��
				if(i==0)
					i=1;			
//			tab_flag.update_flag = 0;			//����Ҫ����
//			tab_flag.sound_flag = 0;
			break;
			
		case 3:			//��
				tab_flag.update_flag = 0;			//����Ҫ����
				tab_flag.sound_flag = 0;
			break;
			
		case 4:			//��
				tab_flag.update_flag = 0;			//����Ҫ����
				tab_flag.sound_flag = 0;
			break;
			
		case 5:			//ȷ��
				if(i==0)                  //�Ͽ������������
				{
					time_break=1;
					bluetooth_state=0;
					printf("AT");
					if(diffrence_judge(device_sum))    ////�ж��Ƿ��Ѿ��󶨲���ȡ��ǰ�����豸��
						time_clear=1;
				}
				else                           //������������¼���豸
				{
					if(diffrence_judge(device_sum))    ////�ж��Ƿ��Ѿ��󶨲���ȡ��ǰ�����豸��
					{
						W25QXX_Write((u8*)&device_bond[0],16*1024*1024+500,sizeof(device_bond[0]));		
						strcpy((u8*)device_bond[0],(u8*)device[bluetooth_judge]);
						W25QXX_Write((u8*)&device_bond[0],16*1024*1024+500,sizeof(device_bond[0]));
						device_sum=1;
						W25QXX_Write((u8*)&device_sum,16*1024*1024+400,sizeof(device_sum));
					}		
					bluetooth_state	=2;	
					NotificationBar();                    //֪ͨ��					
				}
				tab_flag.Tab_Level = memory_Tab_Level;			//ѡ��������
				*ButtonAction = 0;			//���������ÿ�
			break;
		 
		case 6:			//����
				time_break=1;
				bluetooth_state=0;
				printf("AT");
				if(diffrence_judge(device_sum))    ////�ж��Ƿ��Ѿ��󶨲���ȡ��ǰ�����豸��
					time_clear=1;
				tab_flag.Tab_Level = memory_Tab_Level;			//ѡ��������
				*ButtonAction = 0;			//���������ÿ�
			break;
		default:break;
	}
	*ButtonAction = 0;			//���������ÿ�
	if(i==0)
	{
		LCD_Invert(27, 68, 120, 14);
		LCD_Invert(27, 84, 120, 14);
	}

}

//��Բ�ͬ����İ�����Ӧ
void UI_ButtonrRespond(u8 *ButtonAction)
{
	if(SlaveComConnected_old != SlaveComConnected)
	{
		tab_flag.update_flag = 1;
	}
	SlaveComConnected_old = SlaveComConnected;
	//���½���
	if(tab_flag.update_flag == 1 &&  LCD_flag == 0)			//�и��±�־����ʼ��ʱ���±�־��1��
	{
		SystemCountTime = 0;
		UI_Update(tab_flag.Tab_Level,tab_flag.MainTab_choice,Tab);
		
		if(LCD_flag == 0)
			NotificationBar();                    //֪ͨ��
		tab_flag.update_flag = 0;
	}

#if PID_DEBUG
			if(PID_state==0)
			{
				LCD_ShowString(50,20, "PID_P" ,12,0);
			}else if(PID_state==1)
			{
				LCD_ShowString(50,20, "PID_I" ,12,0);
			}else if(PID_state==2)
			{
				LCD_ShowString(50,20, "PID_D" ,12,0);
			}else if(PID_state==3)
			{
				LCD_ShowString(50,20, "PID_DM" ,12,0);
			}
			
			LCD_ShowString(30,40, "PID_P" ,12,0);
			LCD_ShowString(30,60, "PID_I" ,12,0);
			LCD_ShowString(30,80, "PID_D" ,12,0);
			LCD_ShowString(30,100, "PID_DM" ,12,0);
			
			LCD_ShowFloat(80, 40, PID_P, 16);
			LCD_ShowFloat(80, 60, PID_I, 16);
			LCD_ShowFloat(80, 80, PID_D, 16);
			LCD_ShowFloat(80, 100, PID_DM, 16);
#endif
		if(LCD_flag == 0 && tab_flag.Tab_Level != 3)
		{
			LCD_refresh();
		}
	
	//���ݵ�ǰ����İ�����Ӧ
	switch(tab_flag.Tab_Level)			//��ʼ��ʱTab_Level��0
	{
		case 0:			//����������
			UI_Main_ButtonrRespond(ButtonAction);
			break;
		case 1:			//�����Ӻ�������
			UI_Sub_ButtonrRespond(ButtonAction,tab_flag.MainTab_choice,Tab);
			break;
		case 2:			//���ڹػ���������
			UI_Shutdown_ButtonRespond(ButtonAction);
			break;
		case 3:			//�������н���
			UI_Running_ButtonRespond(ButtonAction);
			break;
		case 4:			//���ڹ̼����½���
			break;
		case 5:			//����ͼƬ���Ž���
			PICTURE_ButtonRespond(ButtonAction);
			break;
		case 6:			//����ɾ��ȷ�Ͻ���
			UI_Confirm_ButtonRespond(ButtonAction);
			break;
		case 7:     //�����ӻ�������Ӧ����
			UI_connecting_BluetoothRespond(ButtonAction);
		default:break;
	}
	bluetooch_waiting();
}

u8 GetChoice(Tab_Function_TypeDef Tab[],u8 MainTab_choice, u8 num)
{
	if(Tab[MainTab_choice].Location == num){return 0;}
	else{	return 1;}
}

//����������ѡ����ʾ
void UI_Main_Display(u8 MainTab_choice, Tab_Function_TypeDef Tab[])
{
	int i,j;
	switch(MainTab_choice)
	{
		case 0:
			LCD_Display_pic(0, 12, 180,116, PLAYLASTTAB );
			if(file_name_num != 0)
			{
				for(i=0,j=0; i<5; i++)
				{
					j = i + window_top;
					if(j < file_name_num)
					{
						if(file_load[j].dir  == 1)
						{
							LCD_Display_pic(12,42+16*i, 14, 12, folder_icon );
						}else
						{
							if(file_load[j].type  == 0x01)
							{
								LCD_Display_pic(12,42+16*i, 12, 12, program_icon );
							}else if(file_load[j].type  == 0x02)
							{
								LCD_Display_pic(12,42+16*i, 13, 10, photo_icon );
							}else if(file_load[j].type  == 0x04)
							{
								LCD_Display_pic(12,42+16*i, 12, 10, sound_icon);
							}else if(file_load[j].type  == 0x08)
							{
		//						LCD_Display_pic(12,42+16*i, 12, 12, folder_icon );
							}else if(file_load[j].type  == 0x10)
							{
		//						LCD_Display_pic(12,42+16*i, 12, 12, folder_icon );
							}
						}
						Show_Str(30,42+16*i, 152, 14, file_load[j].name,12, 0);
						
						if(j == Tab[MainTab_choice].Location)
						{
							LCD_Invert(10,40+(8*2*i)+1,152,14);
						}		
					}
				}
			}
			
//			LCD_ShowString(10,40, "NONE" ,12,GetChoice(Tab,MainTab_choice,0));
			break;
		case 1:
			LCD_Display_pic(0, 12, 180,116, FILETAB );
//			LCD_Display_pic(0, 16, 178,24, FILE_TAB );
//			LCD_Display_pic(0, 40, 178,89, MainMenuBottom );
//			LCD_ShowString(12,40, "Project" ,12,1);
		
//			LCD_Display_pic(12,42, 12, 12, program_icon );
//			LCD_Display_pic(40,42, 12, 12, hanzi_5 );
//			LCD_Display_pic(52,42, 12, 12, hanzi_91 );//����
			if(file_name_num != 0)
			{
				for(i=0,j=0; i<5; i++)
				{
					j = i + window_top;
					if(j < file_name_num)
					{
						if(file_load[j].dir  == 1)
						{
							LCD_Display_pic(12,42+16*i, 14, 12, folder_icon );
						}else
						{
							if(file_load[j].type  == 0x01)
							{
								LCD_Display_pic(12,42+16*i, 12, 12, program_icon );
							}else if(file_load[j].type  == 0x02)
							{
								LCD_Display_pic(12,42+16*i, 13, 10, photo_icon  );
							}else if(file_load[j].type  == 0x04)
							{
								LCD_Display_pic(12,42+16*i, 12, 10, sound_icon );
							}else if(file_load[j].type  == 0x08)
							{
		//						LCD_Display_pic(12,42+16*i, 12, 12, folder_icon );
							}else if(file_load[j].type  == 0x10)
							{
		//						LCD_Display_pic(12,42+16*i, 12, 12, folder_icon );
							}
						}
						Show_Str(30,42+16*i, 152, 14, file_load[j].name,12, 0);
						
						if(j == Tab[MainTab_choice].Location)
						{
							LCD_Invert(10,40+(8*2*i)+1,152,14);
						}
					}
				}
			}
		
			break;
		case 2:
			LCD_Display_pic(0, 12, 180,116, APPSTAB );
			if(language == 1)
			{
				LCD_Display_pic(12,42, 16, 12, port_icon );
				LCD_Display_pic(14,58, 12, 12, motor_icon );
				LCD_Display_pic(12,74, 18, 12, ir_icon );
				LCD_ShowString(40,42, "Port_View" ,12,0);
				LCD_ShowString(40,58, "Motor_Control" ,12,0);	
				LCD_ShowString(40,74, "Ir_Control" ,12,0);		
			}else
			{
				LCD_Display_pic(12,42, 16, 12, port_icon );
				LCD_Display_pic(40,42, 12, 12, hanzi_19 );
				LCD_Display_pic(52,42, 12, 12, hanzi_39 );
				LCD_Display_pic(64,42, 12, 12, hanzi_32 );
				LCD_Display_pic(76,42, 12, 12, hanzi_55 );//�˿ڽ���
				LCD_Display_pic(14,58, 12, 12, motor_icon );
				LCD_Display_pic(40,58, 12, 12, hanzi_49 );
				LCD_Display_pic(52,58, 12, 12, hanzi_13 );
				LCD_Display_pic(64,58, 12, 12, hanzi_38 );
				LCD_Display_pic(76,58, 12, 12, hanzi_103 );//�������
				LCD_Display_pic(12,74, 18, 12, ir_icon );
				LCD_Display_pic(40,74, 12, 12, hanzi_24 );
				LCD_Display_pic(52,74, 12, 12, hanzi_82 );
				LCD_Display_pic(64,74, 12, 12, hanzi_38 );
				LCD_Display_pic(76,74, 12, 12, hanzi_103 );//�������
			}
			
			LCD_Invert(10,40+(8*2*Tab[MainTab_choice].Location)+1,152,14);	
			break;
		case 3:
				LCD_Display_pic(0, 12, 180,116, SETTINGSTAB );
				LCD_Display_pic(12,42, 12, 12, setting_icon );
				LCD_Display_pic(12,58, 12, 12, setting_icon );
				LCD_Display_pic(12,74, 12, 12, setting_icon );
				LCD_Display_pic(12,90, 12, 12, setting_icon );
				LCD_Display_pic(12,106, 12, 12, setting_icon );
		
			if(language == 1)
			{
				
				for(i=0,j=0; i<5; i++)
				{
					j = i + window_top;
					if(j < 7)
					{
						LCD_ShowString(40,(42+16*i), SettingName_en[j] ,12,0);
						
						if(j == Tab[MainTab_choice].Location)
						{
							LCD_Invert(10,40+(8*2*i)+1,152,14);
						}		
					}
				}
//				LCD_ShowString(40,42, "Volume" ,12,0);
//				LCD_ShowString(40,58, "Sleep" ,12,0);		
//				LCD_ShowString(40,74, "Bluetooth" ,12,0);		
//				LCD_ShowString(40,90, "Brick_Info" ,12,0);		
//				LCD_ShowString(40,106, "language" ,12,0);	
			}else
			{
				for(i=0,j=0; i<5; i++)
				{
					j = i + window_top;
					if(j < 7)
					{
						switch(j)
						{
							case 0:
								LCD_Display_pic(40,(42+16*i), 12, 12, hanzi_98 );
								LCD_Display_pic(52,(42+16*i), 12, 12, hanzi_45 );//����
								break;
							case 1:
								LCD_Display_pic(40,(42+16*i), 12, 12, hanzi_76 );
								LCD_Display_pic(52,(42+16*i), 12, 12, hanzi_54 );//˯��
								break;
							case 2:
								LCD_Display_pic(40,(42+16*i), 12, 12, hanzi_41 );
								LCD_Display_pic(52,(42+16*i), 12, 12, hanzi_93 );//����
								break;
							case 3:
								LCD_Display_pic(40,(42+16*i), 12, 12, hanzi_108 );
								LCD_Display_pic(52,(42+16*i), 12, 12, hanzi_109 );
								LCD_Display_pic(64,(42+16*i), 12, 12, hanzi_89 );
								LCD_Display_pic(76,(42+16*i), 12, 12, hanzi_85 );//ģ����Ϣ
								break;
							case 4:
								LCD_Display_pic(40,(42+16*i), 12, 12, hanzi_120 );
								LCD_Display_pic(52,(42+16*i), 12, 12, hanzi_121 );//����
								break;
							case 5:
								LCD_Display_pic(40,(42+16*i), 12, 12, hanzi_123 );
								LCD_Display_pic(52,(42+16*i), 12, 12, hanzi_18 );//����
								break;
							case 6:
								LCD_Display_pic(40,(42+16*i), 12, 12, hanzi_124 );
								LCD_Display_pic(52,(42+16*i), 12, 12, hanzi_125 );
								LCD_Display_pic(64,(42+16*i), 12, 12, hanzi_126 );//��ʽ��
								break;
							default:
								break;
						}
//						Show_Str(40,(42+16*i),180,16, SettingName_cn[j] ,12,0);
						
						if(j == Tab[MainTab_choice].Location)
						{
							LCD_Invert(10,40+(8*2*i)+1,152,14);
						}
					}
				}
//				LCD_Display_pic(40,42, 12, 12, hanzi_98 );
//				LCD_Display_pic(52,42, 12, 12, hanzi_45 );//����
//				LCD_Display_pic(40,58, 12, 12, hanzi_76 );
//				LCD_Display_pic(52,58, 12, 12, hanzi_54 );//˯��
//				LCD_Display_pic(40,74, 12, 12, hanzi_41 );
//				LCD_Display_pic(52,74, 12, 12, hanzi_93 );//����
//				LCD_Display_pic(40,90, 12, 12, hanzi_105 );
//				LCD_Display_pic(52,90, 12, 12, hanzi_27 );
//				LCD_Display_pic(64,90, 12, 12, hanzi_89 );
//				LCD_Display_pic(76,90, 12, 12, hanzi_85 );//ģ����Ϣ
//				LCD_Display_pic(40,106, 12, 12, hanzi_120 );
//				LCD_Display_pic(52,106, 12, 12, hanzi_121 );//����
//				
//				LCD_Display_pic(40,106, 12, 12, hanzi_123 );
//				LCD_Display_pic(52,106, 12, 12, hanzi_18 );//����
//				LCD_Display_pic(40,106, 12, 12, hanzi_124 );
//				LCD_Display_pic(40,106, 12, 12, hanzi_125 );
//				LCD_Display_pic(52,106, 12, 12, hanzi_126 );//��ʽ��
			}
			
//			LCD_Invert(10,40+(8*2*Tab[MainTab_choice].Location)+1,152,14);
			if(interface_state==2)
			{
				interface_state=0;
				ButtonAction= 5;
				Tab[tab_flag.MainTab_choice].Location=3;
				UI_Main_ButtonrRespond((u8 *)ButtonAction);				
			}					
			break;
	}
//	LCD_ShowString(155, 40+(8*2*Tab[MainTab_choice].Location), "<-" ,12,1);

//		//��֤�������˿������Ƿ���ȷ
//	for(i=0; i<4; i++)
//	{
//		Display_Hex2(7+2*i,  2, Channel_State[7][0] );
//		Display_Hex2(7+2*i, 22, Channel_State[7][1] );
//		Display_Hex2(7+2*i, 42, Channel_State[7][2] );
//		Display_Hex2(7+2*i, 62, Channel_State[7][3] );
//		Display_Hex2(7+2*i, 82, Channel_State[7][4] );
//		Display_Hex2(7+2*i, 102, Channel_State[7][5] );
//		Display_Hex2(7+2*i, 122, Channel_State[7][6] );
//	}

	  //��֤�ֿ��Ƿ���ȷ
//	Display_String_Eng(5,  2, " !#$%&'()*+,-./0123456" );
//	Display_String_Eng(7,  2, "789:;<=>?@ABCDEFGHIJKL" );
//	Display_String_Eng(9,  2, "MNOPQRSTUVWXYZ[\]^_`ab" );
//	Display_String_Eng(11, 2, "cdefghijklmnopqrstuvwx" );
//	Display_String_Eng(13, 2, "yz{|}~             z" );

		//���Ը�����ʾ����
//	Display_Char(7,  2, '5');   //ҳ  ��  �ַ�
//	Display_Hex2(9,  2, 0x9f);
//	Display_Dec2(7,  2, 40);
//	Display_Dec4(9,  2, 4000);
//	Display_Float(7,  2, 4.000) ;
//	Display_Chinese(9,  2, "��");
//	Display_String_Chs(11, 2, "�˿ڲ��Գ���");
}

void UI_Sub_Display(u8 MainTab_choice, Tab_Function_TypeDef Tab[])
{
	u8 i,j;
	u32 total,free;
	u8 storage[32];
	u8 sleep_value;
	
	switch(MainTab_choice)
	{
		case 0:		
			LCD_clear();																																	//����
			Running_Interface();
			break;
		
		case 1:
			LCD_clear();																																	//����
			LCD_Display_pic(0, 12, 180,116, FILETAB );
			for(i=0,j=0; i<5; i++)
			{
				j = i + window_top;
				if(j < file_name_num)
				{
					if(file_load[j].dir  == 1)
					{
						LCD_Display_pic(12,42+16*i, 14, 12, folder_icon );
					}else
					{
						if(file_load[j].type  == 0x01)
						{
							LCD_Display_pic(12,42+16*i, 12, 12, program_icon );
						}else if(file_load[j].type  == 0x02)
						{
							LCD_Display_pic(12,42+16*i, 13, 10,	photo_icon  );
						}else if(file_load[j].type  == 0x04)
						{
							LCD_Display_pic(12,42+16*i, 12, 10, sound_icon );
						}else if(file_load[j].type  == 0x08)
						{
	//						LCD_Display_pic(12,42+16*i, 12, 12, folder_icon );
						}else if(file_load[j].type  == 0x10)
						{
	//						LCD_Display_pic(12,42+16*i, 12, 12, folder_icon );
						}
					}
					Show_Str(30,42+16*i, 152, 14, file_load[j].name,12, 0);
					
					if(j == Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location])
					{
						LCD_Invert(10,40+(8*2*i)+1,152,14);
					}
				}
			}
//			LCD_Invert(10,40+(8*2*Tab[tab_flag.MainTab_choice].SubLocation[Tab[tab_flag.MainTab_choice].Location])+1,152,14);
			break;
		
		case 2:			
			LCD_clear();																																	//����
			switch(Tab[MainTab_choice].Location)
			{
				case 0:		//Port_View
					switch(Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location])			//��ѡ�˿�
					{
						case 0:			//1
							LCD_Display_pic(0, 12, 180, 116,P1);
							break;
						case 1:			//2
							LCD_Display_pic(0, 12, 180, 116,P2);
							break;
						case 2:			//3
							LCD_Display_pic(0, 12, 180, 116,P3);
							break;
						case 3:			//4
							LCD_Display_pic(0, 12, 180, 116,P4);
							break;
						case 4:			//A
							LCD_Display_pic(0, 12, 180, 116,PA);
							break;
						case 5:			//B
							LCD_Display_pic(0, 12, 180, 116,PB);
							break;
						case 6:			//C
							LCD_Display_pic(0, 12, 180, 116,PC);
							break;
						case 7:			//D
							LCD_Display_pic(0, 12, 180, 116,PD);
							break;
					}
					
					//��ʾ��ǰѡ��˿������豸
					if(Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]<=3)														//port
					{
//						LCD_Display_pic(2, 0, 24, 178, P0_top );
						LCD_ShowChar(60,44,Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]+49,12,0);  
						LCD_ShowChar(68,44,':',12,0);  
						
						if(	Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][1] == 0x01 || //29
								Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][1] == 0x02 || //30
								Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][1] == 0x03 || //32
								Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][1] == 0x08 )  //33				����
						{
							Showusartname(Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location],
														Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][1],
														Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][2]);
//							LCD_ShowString(76, 44, (char* )TypeData[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]]		//��ѡport
//																					[UartPort[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]].Mode].Name,12,1);//mode
								
						}else if(Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][1] == 0x06) 				//�¶�
						{
							if(Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][2] == 1)   							//mode
							{
								if(language == 1)
								{
									LCD_ShowString(76, 44, "NXT-TEMP-C",12,0);
								}else
								{
									LCD_Display_pic(76,44, 12, 12, hanzi_68 );
									LCD_Display_pic(88,44, 12, 12, hanzi_70 );
									LCD_Display_pic(100,44, 12, 12, hanzi_83 );
									LCD_Display_pic(112,44, 12, 12, hanzi_18 );//�����¶�
								}
								
							}else if(Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][2] == 2)
							{
								if(language == 1)
								{
									LCD_ShowString(76, 44, "NXT-TEMP-F",12,0);
								}else
								{
									LCD_Display_pic(76,44, 12, 12, hanzi_25 );
									LCD_Display_pic(88,44, 12, 12, hanzi_70 );
									LCD_Display_pic(100,44, 12, 12, hanzi_83 );
									LCD_Display_pic(112,44, 12, 12, hanzi_18 );//�����¶�
								}
							}
						}else if(Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][1] == 0x09 )	//������ 99
						{
							switch(Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][2])
							{
								case 1:
											if(language == 1)
											{
												LCD_ShowString(76, 44, "ES-IN-VOLT",12,0);
											}else
											{
												LCD_Display_pic(76,44, 12, 12, hanzi_74 );
												LCD_Display_pic(88,44, 12, 12, hanzi_65 );
												LCD_Display_pic(100,44, 12, 12, hanzi_17 );
												LCD_Display_pic(112,44, 12, 12, hanzi_92 );//�����ѹ
											}
									break;
								case 2:
											if(language == 1)
											{
												LCD_ShowString(76, 44, "ES-IN-AMP",12,0);
											}else
											{
												LCD_Display_pic(76,44, 12, 12, hanzi_74 );
												LCD_Display_pic(88,44, 12, 12, hanzi_65 );
												LCD_Display_pic(100,44, 12, 12, hanzi_17 );
												LCD_Display_pic(112,44, 12, 12, hanzi_46 );//�������
											}
									break;
								case 4:
											if(language == 1)
											{
												LCD_ShowString(76, 44, "ES-OUT-VOLT",12,0);
											}else
											{
												LCD_Display_pic(76,44, 12, 12, hanzi_74 );
												LCD_Display_pic(88,44, 12, 12, hanzi_6 );
												LCD_Display_pic(100,44, 12, 12, hanzi_17 );
												LCD_Display_pic(112,44, 12, 12, hanzi_92 );//�����ѹ
											}
									break;
								case 5:
											if(language == 1)
											{
												LCD_ShowString(76, 44, "ES-OUT-AMP",12,0);
											}else
											{
												LCD_Display_pic(76,44, 12, 12, hanzi_74 );
												LCD_Display_pic(88,44, 12, 12, hanzi_6 );
												LCD_Display_pic(100,44, 12, 12, hanzi_17 );
												LCD_Display_pic(112,44, 12, 12, hanzi_46 );//�������
											}
									break;
								case 7:
											if(language == 1)
											{
												LCD_ShowString(76, 44, "ES-JOULE",12,0);
											}else
											{
												LCD_Display_pic(76,44, 12, 12, hanzi_59 );
												LCD_Display_pic(88,44, 12, 12, hanzi_45 );
												LCD_Display_pic(100,44, 12, 12, hanzi_28 );//������
											}
									break;
								case 3:
											if(language == 1)
											{
												LCD_ShowString(76, 44, "ES-IN-WATT",12,0);
											}else
											{
												LCD_Display_pic(76,44, 12, 12, hanzi_74 );
												LCD_Display_pic(88,44, 12, 12, hanzi_65 );
												LCD_Display_pic(100,44, 12, 12, hanzi_22 );
												LCD_Display_pic(112,44, 12, 12, hanzi_48 );//���빦��
											}
									break;
								case 6:
											if(language == 1)
											{
												LCD_ShowString(76, 44, "ES-OUT-WATT",12,0);
											}else
											{
												LCD_Display_pic(76,44, 12, 12, hanzi_74 );
												LCD_Display_pic(88,44, 12, 12, hanzi_6 );
												LCD_Display_pic(100,44, 12, 12, hanzi_22 );
												LCD_Display_pic(112,44, 12, 12, hanzi_48 );//�������
											}
									break;
							}
						}else if(Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][1] == 0x0A) //SOUND 3
						{
							if(Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][2] == 1)//mode
							{
								if(language == 1)
								{
									LCD_ShowString(76, 44, "NXT-SND-DB",12,0);
								}else
								{
									LCD_Display_pic(76,44, 12, 12, hanzi_69 );
									LCD_Display_pic(88,44, 12, 12, hanzi_98);
									LCD_ShowString(100, 44, "DB",12,0);//����DB
								}
							}else if(Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][2] == 2 )
							{
								if(language == 1)
								{
									LCD_ShowString(76, 44, "NXT-SND-DBA",12,0);
								}else
								{
									LCD_Display_pic(76,44, 12, 12, hanzi_69 );
									LCD_Display_pic(88,44, 12, 12, hanzi_98);
									LCD_ShowString(100, 44, "DBA",12,0);//����DBA
								}							
							}
						}else if(Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][1] == 0x07) //TOUCH  16
						{
							if(language == 1)
							{
								LCD_ShowString(76, 44, "TOUCH",12,0);
							}else
							{
								LCD_Display_pic(76,44, 12, 12, hanzi_9 );
								LCD_Display_pic(88,44, 12, 12, hanzi_60 );//����
							}
						}else if(Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][1] == 0)
						{
							if(language == 1)
							{
								LCD_ShowString(76, 44, "NONE",12,0);
							}else
							{
								LCD_Display_pic(76,44, 12, 12, hanzi_110 );//û��
							}
							Tab[4].SubLocation[Tab[4].Location] = 0;																												//����
						}
					}else //���
					{
						LCD_ShowChar(60,44,Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]-4+65,12,0); 
						LCD_ShowChar(68,44,':',12,0);  
						
						if(Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][1] == 0x05)				//���͵�� 8
						{
							if(Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][2] == 1)				//mode
							{
								if(language == 1)
								{
									LCD_ShowString(76, 44, "M-MOTOR-DEG",12,0);
								}else
								{
									LCD_Display_pic(76,44, 12, 12, hanzi_104 );
									LCD_Display_pic(88,44, 12, 12, hanzi_90 );
									LCD_Display_pic(100,44, 12, 12, hanzi_17 );
									LCD_Display_pic(112,44, 12, 12, hanzi_27 );
									LCD_ShowString(124, 44, "-",12,0);
									LCD_Display_pic(136,44, 12, 12, hanzi_18 );
									LCD_Display_pic(148,44, 12, 12, hanzi_75 );//���͵��-����
								}
							}else if(Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][2] == 2)
							{
								if(language == 1)
								{
									LCD_ShowString(76, 44, "M-MOTOR-ROT",12,0);
								}else
								{
									LCD_Display_pic(76,44, 12, 12, hanzi_104 );
									LCD_Display_pic(88,44, 12, 12, hanzi_90 );
									LCD_Display_pic(100,44, 12, 12, hanzi_17 );
									LCD_Display_pic(112,44, 12, 12, hanzi_27 );
									LCD_ShowString(124, 44, "-",12,0);
									LCD_Display_pic(136,44, 12, 12, hanzi_64 );
									LCD_Display_pic(148,44, 12, 12, hanzi_75 );//���͵��-Ȧ��
								}
							}
						}else if(Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][1] == 0x04)	//���͵�� 7
						{
							if(Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][2] == 1)				//mode
							{
								if(language == 1)
								{
									LCD_ShowString(76, 44, "L-MOTOR-DEG",12,0);									
								}else
								{
									LCD_Display_pic(76,44, 12, 12, hanzi_14 );
									LCD_Display_pic(88,44, 12, 12, hanzi_90 );
									LCD_Display_pic(100,44, 12, 12, hanzi_17 );
									LCD_Display_pic(112,44, 12, 12, hanzi_27 );
									LCD_ShowString(124, 44, "-",12,0);
									LCD_Display_pic(136,44, 12, 12, hanzi_18 );
									LCD_Display_pic(148,44, 12, 12, hanzi_75 );//���͵��-����
								}
							}else if(Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][2] == 2)
							{
								if(language == 1)
								{
									LCD_ShowString(76, 44, "L-MOTOR-ROT",12,0);
								}else
								{
									LCD_Display_pic(76,44, 12, 12, hanzi_14 );
									LCD_Display_pic(88,44, 12, 12, hanzi_90 );
									LCD_Display_pic(100,44, 12, 12, hanzi_17 );
									LCD_Display_pic(112,44, 12, 12, hanzi_27 );
									LCD_ShowString(124, 44, "-",12,0);
									LCD_Display_pic(136,44, 12, 12, hanzi_64 );
									LCD_Display_pic(148,44, 12, 12, hanzi_75 );//���͵��-Ȧ��
								}
							}
						}else if(Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][1] == 0x00)
						{
							if(language == 1)
							{
								LCD_ShowString(76, 44, "NONE",12,0);
							}else
							{
								LCD_Display_pic(76,44, 12, 12, hanzi_110 );//û��
							}
								Tab[4].SubLocation[Tab[4].Location] = 0;																												//����
							
								//����˿����帴λ
								for(j=0;j<4;j++)
									SumPluseForDisplay[j] = 0;
						}
					}
					//ѡ��ͼƬ
					SelectSensorPicture(Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][1],	//ID
															Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][2]);	//MODE
					//��ʾ����
					if(Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][1] != 0)
					{
						ShowSenserData(Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location],												//port
														Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][1],		//ID
															Channel_State[Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]][2]);	//mode			
					}
					break;
					
				case 1://Motor_Control	
					if(Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]/0x10)		//����ʹ��BC�˿���Ϊһ��
					{
						LCD_Display_pic(0, 12, 180, 116, MotorCtlBC );
					}
					else//����ʹ��AD�˿���Ϊһ��
					{
						LCD_Display_pic(0, 12, 180, 116, MotorCtlAD );
					}
					break;
					
				case 2:  //Ir_Control  
					if(Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]/0x10)
					{
					 LCD_Display_pic(0, 12, 180, 116, IrCtl12 );
					}
					else
					{
					 LCD_Display_pic(0, 12, 180, 116, IrCtl34 );
					}
					break;
			}
			break;
		
		case 3:
			switch(Tab[MainTab_choice].Location)
			{
				case 0://Volume
						LCD_Display_pic(22, 50, 130, 30, volume_chinese);
						if(language == 1)
						{
							LCD_ShowString(45, 58, "Volume",12,0);
						}else
						{
							LCD_Display_pic(45, 58, 12, 12, hanzi_98);
							LCD_Display_pic(63, 58, 12, 12, hanzi_45);
						}
				
			    	LCD_ShowNum(98,58,sound_volume,3,12);
					break;
				
				case 1://Sleep
						LCD_Display_pic(22, 50, 130, 30, sleep_chinese);
						if(language == 1)
						{
							LCD_ShowString(30, 58, "Sleep",12,0);
							LCD_ShowString(100, 58, "minute",12,0);
						}else
						{
							LCD_Display_pic(30, 58, 12, 12, hanzi_76);
							LCD_Display_pic(48, 58, 12, 12, hanzi_54);
							LCD_Display_pic(106, 58, 12, 12, hanzi_115);
							LCD_Display_pic(119, 58, 12, 12, hanzi_116);					
						}
						switch(sleep_time)
						{
							case 0:sleep_value = 0;
								break;
							case 1:sleep_value = 2;
								break;
							case 2:sleep_value = 5;
								break;
							case 3:sleep_value = 10;
								break;
							case 4:sleep_value = 30;
								break;
							case 5:sleep_value = 60;
								break;
							default:break;
						}

						LCD_ShowNum(78,58,sleep_value,3,12);						
					break;
				case 2://Bluetooth
					if(control==0)   //0:���������� 
					{									
						if(interface_state==0)
						{
							LCD_Display_pic(22, 30, 130, 83, bluetooth);
							if(bluetooth_start==1)   //��������
								LCD_Display_pic(129, 73, 10, 9, bluetooth_yes);
							else
								LCD_Display_pic(129, 73, 10, 9, bluetooth_no);
							if(bluetooth_visual==1)    //�������ӻ�
								LCD_Display_pic(129, 55, 10, 9, bluetooth_yes);
							else
								LCD_Display_pic(129, 55, 10, 9, bluetooth_no);								
						}
						else if(interface_state==1)
						{
							if(bluetooth_start==1)   //��������
							{
								if(bluetooth_locate==2)
									LCD_Invert(28, 71, 118, 14);	                   //��ʶ��
								LCD_Display_pic(129, 73, 10, 9, bluetooth_yes);
								if(bluetooth_locate==2)
									LCD_Invert(28, 71, 118, 14);                   //��ʶ��
							}
							else 
							{
								if(bluetooth_locate==2)
									LCD_Invert(28, 71, 118, 14);	
								LCD_Display_pic(129, 73, 10, 9, bluetooth_no);
								if(bluetooth_locate==2)
									LCD_Invert(28, 71, 118, 14);	
							}						
							if(bluetooth_visual==1)    //�������ӻ�
							{
								if(bluetooth_locate==1)
									LCD_Invert(28, 53, 118, 14);	
								LCD_Display_pic(129, 55, 10, 9, bluetooth_yes);
								if(bluetooth_locate==1)
									LCD_Invert(28, 53, 118, 14);	
							}
							else 
							{
								if(bluetooth_locate==1)
									LCD_Invert(28, 53, 118, 14);	
								LCD_Display_pic(129, 55, 10, 9, bluetooth_no);
								if(bluetooth_locate==1)
									LCD_Invert(28, 53, 118, 14);	
							}
						}
						else if(interface_state==3)
						{						
							if(bluetooth_j != bluetooth_locate)    //�жϱ�ʶ���޸���
							{
								if(bluetooth_j == 3)                     //�ɱ�ʶ��ʧ
									LCD_Invert(79, 95, 16, 12);
								else						
									LCD_Invert(28, 35+bluetooth_j*18, 118, 14);					
								bluetooth_j = bluetooth_locate;         //����
								if(bluetooth_locate == 3)      //�±�ʶ��ʾ
									LCD_Invert(79, 95, 16, 12);
								else						
									LCD_Invert(28, 35+bluetooth_locate*18, 118, 14);
							}
						}
					}
					else if(control==1)    //1���豸���������� 
					{
						if(blue_1_state==0)
						{
						LCD_Display_pic(22, 12, 130, 111, bluetooth_con);
						if(device_sum<4)
						{
							switch(device_sum)     //��ʾ���豸      
							{															
//								case 3:			
//									LCD_Display_pic(35, 35, 13, 12,bluetooth_icon2);
//									LCD_ShowString(58,35,device_bond[2],12,0);
//									
//									LCD_Display_pic(35, 51, 13, 12,bluetooth_icon2);
//									LCD_ShowString(58,51,device_bond[1],12,0);
//									
//									LCD_Display_pic(35, 67, 13, 12,bluetooth_icon2);
//									LCD_ShowString(58,67,device_bond[0],12,0);
//								break;
//								case 2:			
//									LCD_Display_pic(35, 35, 13, 12,bluetooth_icon2);
//									LCD_ShowString(58,35,device_bond[1],12,0);
//								
//									LCD_Display_pic(35, 51, 13, 12,bluetooth_icon2);
//									LCD_ShowString(58,51,device_bond[0],12,0);
//								break;
								case 1:			
									LCD_Display_pic(35, 35, 13, 12,bluetooth_icon2);
									LCD_ShowString(58,35,device_bond[0],12,0);
									break;
								default:
									break;
							}	
						}
						else
						{
//							LCD_Invert(28, 85, 116, 16);
								LCD_Display_pic(35, 35, 13, 12,bluetooth_icon2);
								LCD_ShowString(58,35,device_bond[2+bluetooth_window],12,0);
								
								LCD_Display_pic(35, 51, 13, 12,bluetooth_icon2);
								LCD_ShowString(58,51,device_bond[1+bluetooth_window],12,0);
								
								LCD_Display_pic(35, 67, 13, 12,bluetooth_icon2);
								LCD_ShowString(58,67,device_bond[0+bluetooth_window],12,0);
//							LCD_Invert(29,(j-bluetooth_window)*16-62, 113, 14);
							if(bluetooth_locate+bluetooth_window==6)
							{
								LCD_Invert(29, 34, 113, 14);
								LCD_Invert(28, 85, 116, 16);
							}
							else if(bluetooth_locate+bluetooth_window==8)
							{
								LCD_Invert(29, 66, 113, 14);
								LCD_Invert(28, 85, 116, 16);
							}
						}
					}
						
				if(blue_1_state!=0)
				{
					if(bluetooth_j != bluetooth_locate)            //�жϱ�ʶ���޸���
					{
						if(bluetooth_j == 10)
							LCD_Invert(79, 107, 16, 12);       //�ɱ�ʶ��ʧ
						else if(bluetooth_j == 9)						
							LCD_Invert(28, 85, 116, 16);
						else 
						{
							if(device_sum == 1)
								LCD_Invert(29, bluetooth_j*16-94, 113, 14);
							else if(device_sum == 2)
								LCD_Invert(29, bluetooth_j*16-78, 113, 14);
							else 
								LCD_Invert(29,(bluetooth_j+bluetooth_window)*16-62, 113, 14);
						}
						bluetooth_j= bluetooth_locate;           //����
						if(bluetooth_j == 10)                          //�±�ʶ��ʾ
							LCD_Invert(79, 107, 16, 12);       
						else if(bluetooth_j == 9)						
							LCD_Invert(28, 85, 116, 16);
						else 
						{
							if(device_sum == 1)
								LCD_Invert(29, bluetooth_j*16-94, 113, 14);
							else if(device_sum == 2)
								LCD_Invert(29, bluetooth_j*16-78, 113, 14);
							else 
								LCD_Invert(29,(bluetooth_j+bluetooth_window)*16-62, 113, 14);
						}
					}
				}
				
					}
					else if(control==2)    //2�������������
					{
						if(bluetooth_start==0)                     //����û����
								LCD_Display_pic(22, 50, 130, 52,bluetooth_close_error);
						else if((time_dis==0)&&(blue_2_state==0) )     //û����������ȴ�����ʾ�������
						{
							LCD_Display_pic(22, 12, 130, 111, bluetooth_ins);
							if(dis_sum<6)
							{
								switch(dis_sum)        //��ʾ�����豸��Ϣ
								{
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
										break;
									default:
										break;
								}	
							}
							else
							{
								if((strcmp(device[4+bluetooth_window]," ")==0)&&(judge_dis==2))
									bluetooth_window--;
								LCD_Display_pic(35, 107, 13, 12,bluetooth_icon2);
								LCD_ShowString(58,107,device[4+bluetooth_window],12,0);

								LCD_Display_pic(35, 89, 13, 12,bluetooth_icon2);
								LCD_ShowString(58,89,device[3+bluetooth_window],12,0);

								LCD_Display_pic(35, 71, 13, 12,bluetooth_icon2);
								LCD_ShowString(58,71,device[2+bluetooth_window],12,0);

								LCD_Display_pic(35, 53, 13, 12,bluetooth_icon2);
								LCD_ShowString(58,53,device[1+bluetooth_window],12,0);

								LCD_Display_pic(35, 35, 13, 12,bluetooth_icon2);
								LCD_ShowString(58,35,device[0+bluetooth_window],12,0);
//								if(bluetooth_locate==(4+bluetooth_window))
//									LCD_Invert(29, 106, 113, 14);
//								else if(bluetooth_locate==(bluetooth_window))
//									LCD_Invert(29, 34, 113, 14);

							}
							if(position_judge==1)
							{
								position_judge=0;
								if(dis_sum>0)
								LCD_Invert(29, 34+(bluetooth_locate-bluetooth_window)*18, 113, 14);  
							}
							LCD_Invert(29, 34+(bluetooth_j-bluetooth_window)*18, 113, 14); 
						}
						if(blue_2_state!=0)
						{
							if( dis_sum > 0 )//��ʶ
							{
								if(bluetooth_j != bluetooth_locate)            //�жϱ�ʶ���޸���
								{
									LCD_Invert(29, 34+(bluetooth_j-bluetooth_window)*18, 113, 14);  //�ɱ�ʶ��ʧ
									bluetooth_j = bluetooth_locate;
									LCD_Invert(29, 34+(bluetooth_locate-bluetooth_window)*18, 113, 14); //�±�ʶ��ʾ
								}
							}
						}						
					}
					else if(control==3)    //3���豸�������� 
					{
//						if(bluetooth_state==0)    //�����Ͽ�״̬ 
//							LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rac);
//						else              //����״̬
//							LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rab);
						if(judge_dis==0)    //���豸�������������
						{
							switch(bluetooth_judge)        //��ʾ�����豸��Ϣ
							{
								case 0:
										if(bluetooth_state==2)
										LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rab);
										else
										LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rac);
									break;
								case 1:
										if(bluetooth_state==3)
										LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rab);
										else
										LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rac);
									break;
								case 2:
										if(bluetooth_state==4)
										LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rab);
										else
										LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rac);
									break;
								case 3:
										if(bluetooth_state==5)
										LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rab);
										else
										LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rac);
									break;
								case 4:
										if(bluetooth_state==6)
										LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rab);
										else
										LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rac);
									break;
								case 5:
										if(bluetooth_state==7)
										LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rab);
										else
										LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rac);
									break;
								case 6:
										if(bluetooth_state==8)
										LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rab);
										else
										LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rac);
									break;
								case 7:
										if(bluetooth_state==9)
										LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rab);
										else
										LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rac);
									break;
								case 8:
										if(bluetooth_state==10)
										LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rab);
										else
										LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rac);
									break;										
									}
							LCD_ShowString(70,52,device_bond[bluetooth_judge],12,0);								
						}
						else if(judge_dis==1)
						{
							LCD_Display_pic(22, 50, 130, 52,bluetooth_choice_rac);
							LCD_ShowString(70,52,device[bluetooth_judge],12,0);						
						}
					if(blue_3_state!=0)
					{
					if(bluetooth_locate == 2)
					{
						LCD_Invert(27, 36+3*16, 120, 14);			  //�ɱ�ʶ��ʧ
						LCD_Invert(27, 36+2*16, 120, 14);    //�±�ʶ��ʾ
					}
					else
					{
//						LCD_Invert(27, 36+2*16, 120, 14);			  //�ɱ�ʶ��ʧ
//						LCD_Invert(27, 36+3*16, 120, 14);    //�±�ʶ��ʾ
					}
				}
				
						
					}
					else if(control==4)    //4����ʾ����
					{
						if(time_link==0)
						{
							if(bluetooth_locate==2)
							{
								if(bluetooth_state==0)
									LCD_Display_pic(22, 57, 130, 38,bluetooth_remove);
								else
									LCD_Display_pic(22, 50, 130, 52,bluetooth_tip);
							}
							else if(bluetooth_locate==3)
							{
								if(bluetooth_state==2)
									LCD_Display_pic(22, 57, 130, 38,bluetooth_connect);
								else if(bluetooth_wait==0)
									LCD_Display_pic(22, 57, 130, 38,bluetooth_break);
								else if((bluetooth_state!=bluetooth_judge+2)&&(bluetooth_state>=2))
									LCD_Display_pic(22, 50, 130, 52,bluetooth_tip);
							}
						}						
					}
					break;
				
				
				case 3://Brick_Info
					
							free = mf_showfree("1:",&total);
							sprintf(&storage[0], ":%.1f/%.1f MB",((float)free)/8/1024,((float)total)/8/1024);
				
						LCD_Display_pic(15, 12, 150, 111, brick_info);
						if(language == 1)
						{
							LCD_ShowString(60, 14, "Brick Info",12,0);
							LCD_ShowString(25, 32, "Brick-HW:V1.01",12,0);
							LCD_ShowString(25, 48, "Brick-ID:00165345027d",12,0);
							LCD_ShowString(25, 64, "Brick-OS:Dr.luck",12,0);
							LCD_ShowString(25, 80, "MENORY Free:12MB",12,0);
//							LCD_ShowString(73, 32, ":V",12,0);	
//							LCD_ShowString(85, 32, version,12,0);//�����汾	
//							LCD_ShowString(49, 48, "ID:",12,0);//����ID
//							LCD_ShowString(67, 48, System_ID,12,0);//�����汾	
//							LCD_ShowString(73, 64, ":",12,0);//����ϵͳ
//							LCD_ShowString(79, 64, System_Name,12,0);//�����汾	
							
							LCD_ShowString(73, 80, storage,12,0);//�����ڴ�
						}else
						{
							LCD_Display_pic(65,14, 12, 12, hanzi_105 );
							LCD_Display_pic(77,14, 12, 12, hanzi_27 );
							LCD_Display_pic(89,14, 12, 12, hanzi_89 );
							LCD_Display_pic(101,14, 12, 12, hanzi_85 );//������Ϣ
					
							LCD_Display_pic(25,32, 12, 12, hanzi_105 );
							LCD_Display_pic(37,32, 12, 12, hanzi_27 );
							LCD_Display_pic(49,32, 12, 12, hanzi_106 );
							LCD_Display_pic(61,32, 12, 12, hanzi_107 );
							LCD_ShowString(73, 32, ":V",12,0);	
							LCD_ShowString(85, 32, version,12,0);//�����汾	

							
							LCD_Display_pic(25,48, 12, 12, hanzi_105 );
							LCD_Display_pic(37,48, 12, 12, hanzi_27 );
							LCD_ShowString(49, 48, "ID:",12,0);//����ID
							LCD_ShowString(67, 48, System_ID,12,0);//�����汾
					
							LCD_Display_pic(25,64, 12, 12, hanzi_105 );
							LCD_Display_pic(37,64, 12, 12, hanzi_27 );
							LCD_Display_pic(49,64, 12, 12, hanzi_108 );
							LCD_Display_pic(61,64, 12, 12, hanzi_109 );
							LCD_ShowString(73, 64, ":",12,0);//����ϵͳ
							LCD_ShowString(79, 64, System_Name,12,0);//����ϵͳ
							
							LCD_Display_pic(25,80, 12, 12, hanzi_110 );
							LCD_Display_pic(37,80, 12, 12, hanzi_111 );
							LCD_Display_pic(49,80, 12, 12, hanzi_112 );
							LCD_Display_pic(61,80, 12, 12, hanzi_10 );//�����ڴ�
						
							LCD_ShowString(73, 80, storage,12,0);//�����ڴ�
						}
					break;
						
					case 4://language
						LCD_Display_pic(16, 50, 144, 32, Language_pic);
						if(language == 0)
						{
							LCD_Display_pic(76, 60, 12, 12, hanzi_104);
							LCD_Display_pic(92, 60, 12, 12, hanzi_122);
						}else
						{
							LCD_ShowString(68, 60, "English",12,0);
						}
					break;
						
					case 5://light
						LCD_Display_pic(22, 50, 130, 30, sleep_chinese);
						if(language == 1)
						{
							LCD_ShowString(30, 58, "Light",12,0);
						}else
						{
							LCD_Display_pic(30, 58, 12, 12, hanzi_123);
							LCD_Display_pic(48, 58, 12, 12, hanzi_45);
						}
				
			    	LCD_ShowNum(98,58,light_volume,3,12);
					break;
						
					case 6://file_format
						LCD_Display_pic(16, 50, 144, 32, Language_pic);
						if(language == 1)
						{
							if(format_flag == 0)
							{
								LCD_ShowString(80, 62, "No",12,0);
							}else
							{
								LCD_ShowString(80, 62, "Yes",12,0);
							}
						}
						else
						{
							if(format_flag == 0)
							{
//								Show_Str(80, 60, 100, 16, "ȡ��",12,0);
								LCD_Display_pic(75, 60, 12, 12, hanzi_129);
								LCD_Display_pic(93, 60, 12, 12, hanzi_130);
							}else
							{
//								Show_Str(80, 60, 100, 16, "ȷ��",12,0);
								LCD_Display_pic(75, 60, 12, 12, hanzi_127);
								LCD_Display_pic(93, 60, 12, 12, hanzi_128);
							}
						}
						
					break;
			}
			break;
		
		case 4:
				LCD_clear();										//����
				switch(Tab[2].SubLocation[Tab[2].Location])			//��ѡ�˿�
					{
						case 0:			//1
							LCD_Display_pic(0, 12, 180, 116,P1);
							break;
						case 1:			//2
							LCD_Display_pic(0, 12, 180, 116,P2);
							break;
						case 2:			//3
							LCD_Display_pic(0, 12, 180, 116,P3);
							break;
						case 3:			//4
							LCD_Display_pic(0, 12, 180, 116,P4);
							break;
						case 4:			//A
							LCD_Display_pic(0, 12, 180, 116,PA);
							break;
						case 5:			//B
							LCD_Display_pic(0, 12, 180, 116,PB);
							break;
						case 6:			//C
							LCD_Display_pic(0, 12, 180, 116,PC);
							break;
						case 7:			//D
							LCD_Display_pic(0, 12, 180, 116,PD);
							break;
					}
//					Tab[MainTab_choice].SubLocation[Tab[MainTab_choice].Location]
					if(Channel_State[Tab[2].SubLocation[Tab[2].Location]][1] != 0)
					{
						SelectSensorMode(GetShowModeNum(Channel_State[Tab[2].SubLocation[Tab[2].Location]][1]),													//����
														Channel_State[Tab[2].SubLocation[Tab[2].Location]][1]);																					//��ѡID
						
						if(Channel_State[Tab[2].SubLocation[Tab[2].Location]][1] == 4			||
								Channel_State[Tab[2].SubLocation[Tab[2].Location]][1] == 5		||
									Channel_State[Tab[2].SubLocation[Tab[2].Location]][1] == 6 	||
										Channel_State[Tab[2].SubLocation[Tab[2].Location]][1] == 9	) //�����iic
						{
							SelectSensorPicture(Channel_State[Tab[2].SubLocation[Tab[2].Location]][1],   																	//��ѡID
																Tab[tab_flag.MainTab_choice].SubLocation[Tab[2].SubLocation[Tab[2].Location]]+1);           //ģʽ											
						}else//����
						{
							SelectSensorPicture(Channel_State[Tab[2].SubLocation[Tab[2].Location]][1],   																	//��ѡID
																		LegoModetoEstMode(UartPort[Channel_State[Tab[2].SubLocation[Tab[2].Location]][0]-1].Type,
																												Tab[tab_flag.MainTab_choice].SubLocation[Tab[2].SubLocation[Tab[2].Location]]));//��ѡMODE
						}
						LCD_Invert(60,44+16*Tab[tab_flag.MainTab_choice].SubLocation[Tab[2].SubLocation[Tab[2].Location]],100,14);			//��ʶ
//						Tab[tab_flag.MainTab_choice].SubLocation[Tab[2].SubLocation[Tab[2].Location]]
//	LCD_Invert(10,40+(8*2*Tab[MainTab_choice].Location)+1,152,14);
					}else 
					{
						tab_flag.MainTab_choice = 2;//���� portview
					}
			break;
		
		default:break;
	}
}

void UI_Shutdown_Display(void)
{
	LCD_Display_pic(15, 35, 150, 61, PORT_3 );
	LCD_Display_pic(78, 40, 26, 26, shutdown);
	
	switch(tab_flag.ShutDown)
  {
		case 0:
			LCD_Display_pic(60, 74, 23,18, NOT_1);
			LCD_Display_pic(100, 74, 23, 18, YES_1);
	    break;
		
		case 1:
			LCD_Display_pic(60, 74, 23,18, NOT_2);
			LCD_Display_pic(100, 74, 23, 18, YES_2);	
		  break;
		
		default:
			break;
  }
}
void UI_Confirm_Display(void)
{
	LCD_Display_pic(15, 35, 150, 61, PORT_3 );
//	LCD_Display_pic(78, 40, 26, 26, shutdown);
	
	switch(tab_flag.Confirm)
  {
		case 0:
			LCD_Display_pic(75, 40, 24,25, CONFIRM);
			LCD_Display_pic(60, 74, 23,18, NOT_1);
			LCD_Display_pic(100, 74, 23, 18, YES_1);
	    break;
		
		case 1:
			LCD_Display_pic(75, 40, 24,25, CONFIRM);
			LCD_Display_pic(60, 74, 23,18, NOT_2);
			LCD_Display_pic(100, 74, 23, 18, YES_2);	
		  break;
		
		default:
			break;
  }
}
void UI_Update(u8 Tab_Level, u8 MainTab_choice, Tab_Function_TypeDef Tab[])
{
	switch(Tab_Level)
	{
		case 0:																//��ʾ������	
			LCD_clear();												//����
			UI_Main_Display(MainTab_choice,Tab);
//			Show_Str(10,100,150,12,"��ͷ��ѧ",12,1);
			break;
		
		case 1:																//ѡ���Ӻ�������
			UI_Sub_Display(MainTab_choice,Tab);
			break;
		
		case 2:																//ѡ��ػ���������
			UI_Shutdown_Display();
			break;
		
		case 3:																//�������н���
			Running_Interface();
			LCD_refresh();
			break;
		
		case 4:																//�̼�����
//			LCD_clear();
//			LCD_Display_pic(0, 12, 180, 128, DOWNLOAD_FIRMWARE);
			break;
		case 5:																//ͼƬ���Ž���
			LCD_clear();
			LCD_Display_pic(0, 0,PICTURE_Buff[0], PICTURE_Buff[1], PICTURE_Buff+2);		
			LCD_flag = 1;
			LCD_refresh();
			break;
		case 6:																//ȷ�Ͻ���
			UI_Confirm_Display();
			break;
		default:break;
	}
}

void NotificationBar(void)
{
	u16 Value;
	if(SlaveComConnected == 1)
	{
		LCD_Display_pic(143, 2, 16, 8, USB);
	}
  LCD_Display_pic(78, 1, 27, 7, EST);
	LCD_Display_pic(0, 10, 180, 1, LINE);
	
	Value = ReadADC3AverageValue(0)*2500/4096;
	if(BATTERY() == 0)//﮵��
	{
		if( Value > 1660)									//4��������
		{
			LCD_Display_pic(159, 1, 17, 8, battery_4);
		}else if(1590 < Value )						//3�����
		{
			LCD_Display_pic(159, 1, 17, 8, battery_3);
		}else if(1520 < Value )						//2�����
		{
			LCD_Display_pic(159, 1, 17, 8, battery_2);
		}else if(1450 < Value )						//1�����
		{
			LCD_Display_pic(159, 1, 17, 8, battery_1);
		}else if(Value < 1450)						//0��͵���
		{
			LCD_Display_pic(159, 1, 17, 8, battery_0);
		}
	}else//�ɵ��
	{
		if( Value > 1890)									//4��������
		{
			LCD_Display_pic(159, 1, 17, 8, battery_4);
		}else if(1780 < Value )						//3�����
		{
			LCD_Display_pic(159, 1, 17, 8, battery_3);
		}else if(1670 < Value )						//2�����
		{
			LCD_Display_pic(159, 1, 17, 8, battery_2);
		}else if(1560 < Value )						//1�����
		{
			LCD_Display_pic(159, 1, 17, 8, battery_1);
		}else if(Value < 1560)						//0��͵���
		{
			LCD_Display_pic(159, 1, 17, 8, battery_0);
		}
	}
	if(bluetooth_start==1)
	{
		if(bluetooth_state==0||bluetooth_state==1)
		{
			if(bluetooth_visual==0)
				LCD_Display_pic(5, 0, 16, 9, bluetooth_open_noview_icon);
			else
				LCD_Display_pic(5, 0, 19, 9, bluetooth_open_view_icon);
		}
		else
		{
			if(bluetooth_visual==0)
				LCD_Display_pic(5, 0, 19, 9, bluetooth_link_noview_icon);
			else
				LCD_Display_pic(5, 0, 19, 9, bluetooth_link_view_icon);
		}
		
	}
	else
		LCD_Display_pic(5, 0, 16, 9, bluetooth_white_icon);
}

//================���ܺ���=============================================
//�������н���
void Running_Interface(void)
{
	LCD_clear();																		//����
	LCD_Display_pic(0, 12, 180, 128, UI_RUN);
}

uint8_t GetShowModeNum(uint8_t Type)
{
	uint8_t AllSelectModeLen = 0;
	if(Type == 0x07)//TOUCH
	{
		AllSelectModeLen = 1;
	}else if(Type == 0x0A || Type == 0x06 || Type == 0x04 || Type == 0x05 || Type == 0x02)//SOUND���¶ȡ����е����������
	{
		AllSelectModeLen = 2;
	}else if(Type == 0x01 || Type == 0x03 || Type == 0x08)//��ɫ������������
	{
		AllSelectModeLen = 3;
	}else if(Type == 0x09)//������
	{
		AllSelectModeLen = 8;
	}
	return AllSelectModeLen;
}

void ShowSenserData(uint8_t port, uint8_t	id, uint8_t mode)
{
	float temp;
	if(id == 0x01 && mode == 0x01)//��ɫ colorģʽ
	{
		LCD_ShowNum(64, 64,(u32)Channel_State[port][3],1, 16);
	}else if(id == 0x03 && mode == 0x02)//���� seekģʽ
	{
		temp = Byte4ToFloat(Channel_State[port],1);
		if(temp < 180)
		{
			LCD_ShowFloat(64, 64, temp, 16);
		}else
		{
			LCD_ShowFloat(64, 64, -(255-temp), 16);
		}
	}else if(id == 0x03 && mode == 0x03)//���� remoteģʽ
	{
		LCD_ShowNum(64, 64,(u32)Channel_State[port][4],2, 16);
	}else if(id == 0x08 && mode == 0x05)//���� presentģʽ
	{
		LCD_ShowNum(64, 64,(u32)Channel_State[port][3],1, 16);
	}else if(id == 0x07 && mode == 0x01)//����
	{
		LCD_ShowNum(64, 64,(u32)Channel_State[port][3],1, 16);
	}else
	{
		LCD_ShowFloat(64, 64, Byte4ToFloat(Channel_State[port],0), 16);
	}
}
	
void SelectSensorMode(uint8_t AllSelectModeLen, uint8_t Type)
{
	uint8_t i;
//	LCD_ShowString(60, 44, "NONE",12,0);
	if(AllSelectModeLen <= 3)
	{
//		for(i=0; i<AllSelectModeLen; i++)
//		{
//			if(Type == 0x01 || Type == 0x02 || Type == 0x03 || Type == 0x08)//����
//			{
//				LCD_ShowString(60, 44+16*i, (char* )TypeData[Tab[2].SubLocation[Tab[2].Location]][i].Name,12,0);
//				
//			}
//		}
//		Showusartname1(Type,i);
		if(Type == 0x01 || Type == 0x02 || Type == 0x03 || Type == 0x08)//����
		{
			Showusartname1(Type);
		}
		if(Type == 0x07 )
		{
			if(language == 1 )
			{
				LCD_ShowString(60, 44, "TOUCH",12,0);
			}else
			{
				LCD_Display_pic(63,45, 12, 12, hanzi_9 );
				LCD_Display_pic(75,45, 12, 12, hanzi_60 );//����
			}
		}else if(Type == 0x0A )
		{
			if(language == 1 )
			{
				LCD_ShowString(60, 44, "NXT-SND-DB",12,0);
				LCD_ShowString(60, 60, "NXT-SND-DBA",12,0);
			}else
			{
				LCD_Display_pic(63,45, 12, 12, hanzi_69 );
				LCD_Display_pic(75,45, 12, 12, hanzi_98);
				LCD_ShowString(87, 45, "DB",12,0);//����DB
				LCD_Display_pic(63,61, 12, 12, hanzi_69 );
				LCD_Display_pic(75,61, 12, 12, hanzi_98);
				LCD_ShowString(87, 61, "DBA",12,0);//����DBA
			}
		}else if(Type == 0x06 )
		{
			if(language == 1 )
			{
				LCD_ShowString(60, 44, "NXT-TEMP-C",12,0);
				LCD_ShowString(60, 60, "NXT-TEMP-F",12,0);
			}else
			{
				LCD_Display_pic(63,45, 12, 12, hanzi_68 );
				LCD_Display_pic(75,45, 12, 12, hanzi_70 );
				LCD_Display_pic(87,45, 12, 12, hanzi_83 );
				LCD_Display_pic(99,45, 12, 12, hanzi_18 );//�����¶�
				LCD_Display_pic(63,61, 12, 12, hanzi_25 );
				LCD_Display_pic(75,61, 12, 12, hanzi_70 );
				LCD_Display_pic(87,61, 12, 12, hanzi_83 );
				LCD_Display_pic(99,61, 12, 12, hanzi_18 );//�����¶�
			}
		}else if(Type == 0x05 )//���͵��
		{
			if(language == 1 )
			{
				LCD_ShowString(60, 44, "M-MOTOR-DEG",12,0);
				LCD_ShowString(60, 60, "M-MOTOR-ROT",12,0);
			}else
			{
				LCD_Display_pic(63,45, 12, 12, hanzi_104 );
				LCD_Display_pic(75,45, 12, 12, hanzi_90 );
				LCD_Display_pic(87,45, 12, 12, hanzi_17 );
				LCD_Display_pic(99,45, 12, 12, hanzi_27 );
				LCD_ShowString(111, 45, "-",12,0);
				LCD_Display_pic(123,45, 12, 12, hanzi_18 );
				LCD_Display_pic(135,45, 12, 12, hanzi_75 );//���͵��-����
				
				LCD_Display_pic(63,61, 12, 12, hanzi_104 );
				LCD_Display_pic(75,61, 12, 12, hanzi_90 );
				LCD_Display_pic(87,61, 12, 12, hanzi_17 );
				LCD_Display_pic(99,61, 12, 12, hanzi_27 );
				LCD_ShowString(111, 61, "-",12,0);
				LCD_Display_pic(124,61, 12, 12, hanzi_64 );
				LCD_Display_pic(135,61, 12, 12, hanzi_75 );//���͵��-Ȧ��
			}
			
		}else if(Type == 0x04 )//���͵��
		{
			if(language == 1 )
			{
				LCD_ShowString(60, 44, "L-MOTOR-DEG",12,0);
				LCD_ShowString(60, 60, "L-MOTOR-ROT",12,0);
			}else
			{
				LCD_Display_pic(63,45, 12, 12, hanzi_14 );
				LCD_Display_pic(75,45, 12, 12, hanzi_90 );
				LCD_Display_pic(87,45, 12, 12, hanzi_17 );
				LCD_Display_pic(99,45, 12, 12, hanzi_27 );
				LCD_ShowString(111, 45, "-",12,0);
				LCD_Display_pic(124,45, 12, 12, hanzi_18 );
				LCD_Display_pic(135,45, 12, 12, hanzi_75 );//���͵��-����
				
				LCD_Display_pic(63,61, 12, 12, hanzi_14 );
				LCD_Display_pic(75,61, 12, 12, hanzi_90 );
				LCD_Display_pic(87,61, 12, 12, hanzi_17 );
				LCD_Display_pic(99,61, 12, 12, hanzi_27 );
				LCD_ShowString(111, 61, "-",12,0);
				LCD_Display_pic(124,61, 12, 12, hanzi_64 );
				LCD_Display_pic(135,61, 12, 12, hanzi_75 );//���͵��-Ȧ��
			}
		}
	}else if(AllSelectModeLen > 3)
	{
		if(Type == 0x09 )
		{
			if(language == 1 )
			{
				LCD_ShowString(60, 44, "ES-IN-VOLT",12,0);
				LCD_ShowString(60, 60, "ES-IN-AMP",12,0);	
				LCD_ShowString(60, 76, "ES-OUT-VOLT",12,0);
			}else
			{
				LCD_Display_pic(60,44, 12, 12, hanzi_74 );
				LCD_Display_pic(72,44, 12, 12, hanzi_65 );
				LCD_Display_pic(84,44, 12, 12, hanzi_17 );
				LCD_Display_pic(96,44, 12, 12, hanzi_92 );//�����ѹ
				LCD_Display_pic(60,60, 12, 12, hanzi_74 );
				LCD_Display_pic(72,60, 12, 12, hanzi_65 );
				LCD_Display_pic(84,60, 12, 12, hanzi_17 );
				LCD_Display_pic(96,60, 12, 12, hanzi_46 );//�������
				LCD_Display_pic(60,76, 12, 12, hanzi_74 );
				LCD_Display_pic(72,76, 12, 12, hanzi_6 );
				LCD_Display_pic(84,76, 12, 12, hanzi_17 );
				LCD_Display_pic(96,76, 12, 12, hanzi_92 );//�����ѹ	
			}	
		}
	}
}

void ChangeSensorMode(UBYTE Port, uint8_t Type, uint8_t mode)
{
	if(Type == 0x01 || Type == 0x02 || Type == 0x03 || Type == 0x08)	//����
	{
		ChangeMode(Port+1, mode);
	}else if(Type == 0x06 || Type == 0x09)													//IIC
	{
		IicPort[Port].Mode = mode;
	}else if(Type == 0x0A || Type == 0x07)													//ADC
	{
		Channel_State[Port][2] = mode+1;
	}else if(Type == 0x04 || Type == 0x05)													//���
	{
		Channel_State[Port][2] = mode+1;
	}
}

void SelectSensorPicture(uint8_t Type, uint8_t Mode)
{
	switch(Type)
	{
		case 0x00:
			break;
		
		case 0x01://��ɫ
			if(Mode == 2)
			{
				LCD_Display_pic(14, 53, 29, 34, color_ref );
			}else if(Mode == 3)
			{
				LCD_Display_pic(14, 53, 33, 34, color_amb );
			}else if(Mode == 1)
			{	
				LCD_Display_pic(14, 53, 31, 34, color_col );
			}
			break;
		
		case 0x02://������
			if(Mode == 1)
			{
				LCD_Display_pic(12, 53, 30, 34, gyro_a );
			}else if(Mode == 2)
			{
				LCD_Display_pic(12, 45, 30, 42, gyro_rate );
			}
			break;
		
		case 0x03://����
			if(Mode == 1)
			{
				LCD_Display_pic(2, 53, 36, 34, ir_prox );
			}else if(Mode == 2)
			{	
				LCD_Display_pic(2, 53, 36, 45, ir_bpos );
			}else if(Mode == 3)
			{
				LCD_Display_pic(9, 46, 36, 34, ir_bbut );
			}
			break;
		
		case 0x04://���͵��
			
			if(Mode == 1)
			{	
				LCD_Display_pic(2, 48, 39, 43, lrg_motor_d );
			}else if(Mode == 2)
			{
				LCD_Display_pic(2, 48, 30, 42, lrg_motor );
			}
			break;
		
		case 0x05://���͵��:
			if(Mode == 1)
			{	
				LCD_Display_pic(11, 54, 32, 35, med_motor_d );
			}else if(Mode == 2)
			{
				LCD_Display_pic(11, 54, 26, 34, med_motor );
			}
			break;
		
		case 0x06://�¶�
			if(Mode == 1)
			{
				LCD_Display_pic(2, 51, 37, 27, temp_c );
			}else if(Mode == 2)
			{	
				LCD_Display_pic(2, 51, 37, 27, temp_f );
			}
			break;
		
		case 0x07://����
			LCD_Display_pic(14, 53, 26, 34, touch );
			break;
		
		case 0x08://����
			LCD_Display_pic(11, 50, 32, 40, us_prox );
			break;
		
		case 0x0A://����
			LCD_Display_pic(14, 53, 26, 34, NXT_device );
			break;
		
//		case 99:
//			Display_pic(6, 5, 46, 38, other );
//			break;
		
		default:
			LCD_Display_pic(14, 53, 26, 34, other );
			break;
	}
}

void Showusartname(uint8_t port, uint8_t id, uint8_t mode)
{
	if(language == 1 )
	{
	LCD_ShowString(76, 44, (char* )TypeData[port]		//��ѡport
									[UartPort[port].Mode].Name,12,0);//mode
	}else
	{
		switch(id)
		{
			case 0x01://��ɫ
				if(mode == 1)
				{

					LCD_Display_pic(76,44, 12, 12, hanzi_94 );
					LCD_Display_pic(88,44, 12, 12, hanzi_66 );
					LCD_Display_pic(100,44, 12, 12, hanzi_117 );
					LCD_Display_pic(112,44, 12, 12, hanzi_118 );//��ɫ����
				
				}
				else if(mode == 2)
				{
					LCD_Display_pic(76,44, 12, 12, hanzi_21 );
					LCD_Display_pic(88,44, 12, 12, hanzi_67 );
					LCD_Display_pic(100,44, 12, 12, hanzi_23 );
					LCD_Display_pic(112,44, 12, 12, hanzi_63 );
					LCD_Display_pic(124,44, 12, 12, hanzi_18 );//�����ǿ��
				}			
				else if(mode == 3)
				{
					LCD_Display_pic(76,44, 12, 12, hanzi_26 );
					LCD_Display_pic(88,44, 12, 12, hanzi_34 );
					LCD_Display_pic(100,44, 12, 12, hanzi_23 );
					LCD_Display_pic(112,44, 12, 12, hanzi_63 );
					LCD_Display_pic(124,44, 12, 12, hanzi_18 );//������ǿ��
				}
				break;
			case 0x02://������
					if(mode == 1)
				{

					LCD_Display_pic(76,44, 12, 12, hanzi_81 );
					LCD_Display_pic(88,44, 12, 12, hanzi_47 );
					LCD_Display_pic(100,44, 12, 12, hanzi_96 );
					LCD_Display_pic(112,44, 12, 12, hanzi_30 );
					LCD_Display_pic(124,44, 12, 12, hanzi_18);//�����ǽǶ�
				
				}
				else if(mode == 2)
				{
					LCD_Display_pic(76,44, 12, 12, hanzi_81 );
					LCD_Display_pic(88,44, 12, 12, hanzi_47 );
					LCD_Display_pic(100,44, 12, 12, hanzi_96 );
					LCD_Display_pic(112,44, 12, 12, hanzi_30 );
					LCD_Display_pic(124,44, 12, 12, hanzi_78 );
					LCD_Display_pic(136,44, 12, 12, hanzi_18 );//�����ǽ��ٶ�
				}		
				break;
			case 0x03://����
				if(mode == 1)
				{

					LCD_Display_pic(76,44, 12, 12, hanzi_24 );
					LCD_Display_pic(88,44, 12, 12, hanzi_82 );
					LCD_Display_pic(100,44, 12, 12, hanzi_33 );
					LCD_Display_pic(112,44, 12, 12, hanzi_5 );//�������
				
				}
				else if(mode == 2)
				{
					LCD_Display_pic(76,44, 12, 12, hanzi_24 );
					LCD_Display_pic(88,44, 12, 12, hanzi_82 );
					LCD_Display_pic(100,44, 12, 12, hanzi_89 );
					LCD_Display_pic(112,44, 12, 12, hanzi_1 );//�����ű�
				}			
				else if(mode == 3)
				{
					LCD_Display_pic(76,44, 12, 12, hanzi_24 );
					LCD_Display_pic(88,44, 12, 12, hanzi_82 );
					LCD_Display_pic(100,44, 12, 12, hanzi_101 );
					LCD_Display_pic(112,44, 12, 12, hanzi_5 );//����Զ��
				}
				break;
			case 0x08://����
					if(mode == 3)
				{

					LCD_Display_pic(76,44, 12, 12, hanzi_4 );
					LCD_Display_pic(88,44, 12, 12, hanzi_69 );
					LCD_Display_pic(100,44, 12, 12, hanzi_2);
					LCD_Display_pic(112,44, 12, 12, hanzi_36 );
					LCD_Display_pic(124,44, 12, 12, hanzi_43 );
					LCD_Display_pic(136,44, 12, 12, hanzi_42 );
					LCD_Display_pic(148,44, 12, 12, hanzi_52 );//��������������
				
				}
				else if(mode == 4)
				{
					LCD_Display_pic(76,44, 12, 12, hanzi_4 );
					LCD_Display_pic(88,44, 12, 12, hanzi_69 );
					LCD_Display_pic(100,44, 12, 12, hanzi_2 );
					LCD_Display_pic(112,44, 12, 12, hanzi_36 );
					LCD_Display_pic(124,44, 12, 12, hanzi_43 );
					LCD_Display_pic(136,44, 12, 12, hanzi_99 );
					LCD_Display_pic(148,44, 12, 12, hanzi_11 );//����������Ӣ��
				}			
				else if(mode == 5)
				{
					LCD_Display_pic(76,44, 12, 12, hanzi_4 );
					LCD_Display_pic(88,44, 12, 12, hanzi_69 );
					LCD_Display_pic(100,44, 12, 12, hanzi_2 );
					LCD_Display_pic(112,44, 12, 12, hanzi_102 );
					LCD_Display_pic(124,44, 12, 12, hanzi_80 );//����������
				}
				break;
			default:break;
		}
	}
}
void Showusartname1(uint8_t id)
{
	if(language == 1 )
	{
		switch(id)
		{
			case 0x01://��ɫ
					LCD_ShowString(63, 45, "COL-REFLECT",12,0);//�����ǿ��
					LCD_ShowString(63, 61, "COL-AMBIENT",12,0);//������ǿ��
					LCD_ShowString(63, 77, "COL-COLOR",12,0);//��ɫ����
				break;
			case 0x02://������
					LCD_ShowString(63, 45, "GYPO-ANG",12,0);//������ת�Ƕ���
					LCD_ShowString(63, 61, "GYPO-RATE",12,0);//�����ǽ��ٶ�
				break;
			case 0x03://����
					LCD_ShowString(63, 45, "IR-PROX",12,0);//�������
					LCD_ShowString(63, 61, "IR-SEEK",12,0);//�����ű�
					LCD_ShowString(63, 77, "IR-REMOTE",12,0);//����Զ��
				break;
			case 0x08://����
					LCD_ShowString(63, 45, "US-DIST-CM",12,0);//��������������
					LCD_ShowString(63, 61, "US-DIST-IN",12,0);//����������Ӣ��
					LCD_ShowString(63, 77, "US-LISTEN",12,0);//����������
				break;
			default:break;
		}
	}else
	{
		switch(id)
		{
			case 0x01://��ɫ
					LCD_Display_pic(63,45, 12, 12, hanzi_21 );
					LCD_Display_pic(75,45, 12, 12, hanzi_67 );
					LCD_Display_pic(87,45, 12, 12, hanzi_23 );
					LCD_Display_pic(99,45, 12, 12, hanzi_63 );
					LCD_Display_pic(111,45, 12, 12, hanzi_18 );//�����ǿ��
			
					LCD_Display_pic(63,61, 12, 12, hanzi_26 );
					LCD_Display_pic(75,61, 12, 12, hanzi_34 );
					LCD_Display_pic(87,61, 12, 12, hanzi_23 );
					LCD_Display_pic(99,61, 12, 12, hanzi_63 );
					LCD_Display_pic(111,61, 12, 12, hanzi_18 );//������ǿ��

					LCD_Display_pic(63,77, 12, 12, hanzi_94 );
					LCD_Display_pic(75,77, 12, 12, hanzi_66 );
					LCD_Display_pic(87,77, 12, 12, hanzi_117 );
					LCD_Display_pic(99,77, 12, 12, hanzi_118 );//��ɫ����
				
				break;
			case 0x02://������

					LCD_Display_pic(63,45, 12, 12, hanzi_81 );
					LCD_Display_pic(75,45, 12, 12, hanzi_47 );
					LCD_Display_pic(87,45, 12, 12, hanzi_96 );
					LCD_Display_pic(99,45, 12, 12, hanzi_119 );
					LCD_Display_pic(111,45, 12, 12, hanzi_30 );
					LCD_Display_pic(123,45, 12, 12, hanzi_18);
					LCD_Display_pic(135,45, 12, 12, hanzi_75);//������ת�Ƕ���

					LCD_Display_pic(63,61, 12, 12, hanzi_81 );
					LCD_Display_pic(75,61, 12, 12, hanzi_47 );
					LCD_Display_pic(87,61, 12, 12, hanzi_96 );
					LCD_Display_pic(99,61, 12, 12, hanzi_30 );
					LCD_Display_pic(111,61, 12, 12, hanzi_78 );
					LCD_Display_pic(123,61, 12, 12, hanzi_18 );//�����ǽ��ٶ�
		
				break;
			case 0x03://����

					LCD_Display_pic(63,45, 12, 12, hanzi_24 );
					LCD_Display_pic(75,45, 12, 12, hanzi_82 );
					LCD_Display_pic(87,45, 12, 12, hanzi_33 );
					LCD_Display_pic(99,45, 12, 12, hanzi_5 );//�������
		
					LCD_Display_pic(63,61, 12, 12, hanzi_24 );
					LCD_Display_pic(75,61, 12, 12, hanzi_82 );
					LCD_Display_pic(87,61, 12, 12, hanzi_89 );
					LCD_Display_pic(99,61, 12, 12, hanzi_1 );//�����ű�

					LCD_Display_pic(63,77, 12, 12, hanzi_24 );
					LCD_Display_pic(75,77, 12, 12, hanzi_82 );
					LCD_Display_pic(87,77, 12, 12, hanzi_101 );
					LCD_Display_pic(99,77, 12, 12, hanzi_5 );//����Զ��
				
				break;
			case 0x08://����
		
					LCD_Display_pic(63,45, 12, 12, hanzi_4 );
					LCD_Display_pic(75,45, 12, 12, hanzi_69 );
					LCD_Display_pic(87,45, 12, 12, hanzi_2);
					LCD_Display_pic(99,45, 12, 12, hanzi_36 );
					LCD_Display_pic(111,45, 12, 12, hanzi_43 );
					LCD_Display_pic(123,45, 12, 12, hanzi_42 );
					LCD_Display_pic(135,45, 12, 12, hanzi_52 );//��������������

					LCD_Display_pic(63,61, 12, 12, hanzi_4 );
					LCD_Display_pic(75,61, 12, 12, hanzi_69 );
					LCD_Display_pic(87,61, 12, 12, hanzi_2 );
					LCD_Display_pic(99,61, 12, 12, hanzi_36 );
					LCD_Display_pic(111,61, 12, 12, hanzi_43 );
					LCD_Display_pic(123,61, 12, 12, hanzi_99 );
					LCD_Display_pic(135,61, 12, 12, hanzi_11 );//����������Ӣ��

					LCD_Display_pic(63,77, 12, 12, hanzi_4 );
					LCD_Display_pic(75,77, 12, 12, hanzi_69 );
					LCD_Display_pic(87,77, 12, 12, hanzi_2 );
					LCD_Display_pic(99,77, 12, 12, hanzi_102 );
					LCD_Display_pic(111,77, 12, 12, hanzi_80 );//����������

				break;
			default:break;
		}
	}
}

