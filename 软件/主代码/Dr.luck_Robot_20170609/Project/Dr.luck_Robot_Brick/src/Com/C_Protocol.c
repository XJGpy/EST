/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/C_Protocol.c
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   
  ******************************************************************************
**/
#include <string.h>
#include "stdio.h"
#include "main.h"
//#include "stm32fxxx_it.h"
//#include "ui_control.h"
//#include "w25qxx.h"
//#include "brickperipheral.h"
//#include "myfatfs.h"

#define  Flash_Download_Size 1024-10

//u8 myfilename[] = {0x31,0x3A,0x2F,0x50,0x72,0x6F,0x6A,0x65,0x63,0x74,
//									0x2F,0xC0,0xD6,0xB2,0xA9,0xCA,0xBF,0xB2,0xE2,0xCA,
//										0xD4,0xCE,0xC4,0xBC,0xFE,0x2E,0x64,0x62,0x66};

uint8_t USB_Rx_Buff[1024];						//USB��������֡����
unsigned int USB_Rx_Length = 0;
unsigned int USB_TimePic = 1000;			//����ʱ���ʱ
uint8_t Receive_start = 0;						//֡�����ݽ��ձ�־λ
uint8_t errorflag = 0;								//֡β��ʧ
uint8_t Receive_end = 0;

uint32_t Flash_TimePic = 400000;				//������ʱ���ʱ
uint8_t flashbuff_start = 0;					//�����ձ�־λ
uint8_t flashtimeout = 0;							//�����ճ�ʱ

uint32_t UpdateFlash_TimePic = 400000;	//������ʱ���ʱ
uint8_t updateflash_start = 0;				//�����ձ�־λ
uint8_t updateflashtimeout = 0;				//�����ճ�ʱ
uint32_t Updateflash_length = 0;

uint8_t Channel_State[32][13] = {0};	//����2״̬��ѯ����Ķ˿����飬��¼���˿ڴ�����������״̬
																			//8�鴫�������ݣ�ÿ�����ݰ���PORT��ID��Mode��XX��XX��XX��XX��XX��XX��XX��XX��XX��XX
uint8_t UserWareData[10];							//���ڲ����ϴ����û���������
																			//��0λΪ������־λ��1-8Ϊ�û�����λ
uint8_t UserMotorControlData[13];			//�����ڵ����λ�����Ƶ��û���������
																			//��0λΪ������־λ��1-12Ϊ�û�����λ

u8 usbmasterflag = 0;
int usbmastertimems = 0;

uint8_t i,programlen[2];

//uint32_t GetRamBase(uint8_t par1, uint8_t par2, uint8_t par3, uint8_t par4);
//uint8_t GetByte(uint16_t rambase);

void Send_Heartbeat_Packet(void);
void Send_Sensor_Data(u8 Channel_State[32][13], u8 Connection_N);
void Send_System_Source(void);   
void Send_Program_Download(u8 mode, u8 flag);
//void Send_Firmware_Download(u8 flag);
void Send_Firmware_Download(u16 total, u16 now, u8 flag);
void Send_Wave(u8 Channel_State[32][13],u8 lay,u8 port,u8 id, u8 mode);
void GetRamData(u8 Address0,u8 Address1,u8 Address2,u8 Address3,u8 Length);
void Send_Error_Order(char n);
uint8_t ProgramToFlash(uint8_t Frame_Buff[], unsigned int Frame_Buff_Length, uint8_t flash_buff[], unsigned int *flash_length);
uint8_t ProgramToUpdateFlash(uint8_t Frame_Buff[], unsigned int Frame_Buff_Length, u32 *Updateflash_length);
void Send_MotorControl(void);  
void Send_FATFS_State(uint8_t mode, uint8_t state);
void Send_FATFS_Path(uint8_t totalframe, u8 nowframe,  uint8_t  Flash_Buff[],float Flash_BuffSize);
void Send_FATFS_File(uint8_t totalframe, u8 nowframe,  uint8_t  Flash_Buff[],unsigned int Flash_BuffSize);
void Send_Project_State(uint8_t state);
void Send_Project_Adr(uint8_t flag, uint32_t pc, uint8_t instruct);

u8 project_file_num=0,project_file_num_now=0;
void CheckOrder(void){
		
		int i,res,updateflag;
		static char sum_checkcode = 0x00; 										//У�����ۼ�
//    u32 free,total;
		DIR dir;
			
		if(usbmastertimems>0)
			usbmastertimems--;
		else
			usbmasterflag = 0;

		if(!Receive_start&&USB_Rx_Length&&Receive_end)											//���ݽ�����ȫ��ʹ������
		{
				//����У����У������������
				sum_checkcode = 0;
				for(i=0;i<USB_Rx_Length-2;i++)
						sum_checkcode += USB_Rx_Buff[i];
				if(sum_checkcode != USB_Rx_Buff[USB_Rx_Length-2])	//У���벻ƥ�䣬���ݽ��ղ�����
						Send_Error_Order(0x02);												//У�������
				else
				{
						//����������
						switch(USB_Rx_Buff[2])
						{
								case 0x01:
													if(USB_Rx_Buff[3]==0x00&&USB_Rx_Buff[4]==0x00&&USB_Rx_Length==7)	//�ж��������ʽ�Ƿ��Ӧ
													{
														usbmasterflag = 1;
														usbmastertimems = 150;
														Send_Heartbeat_Packet();
													}
													else
														  Send_Error_Order(0x05); 																			//�������ʽ����Ӧ
													break;
								case 0x02:
													if(USB_Rx_Buff[3]==0x01&&USB_Rx_Buff[4]==0x00&&USB_Rx_Length==8)	//�ж��������ʽ�Ƿ��Ӧ
													{
														Send_Sensor_Data(Channel_State,USB_Rx_Buff[5]) ; 
//														for(i=0; i< 64; i++)
//														{
//															printf("USB_Rx:%x\n\r",USB_Rx_Buff[i]);
//														}
													}
													else
														  Send_Error_Order(0x05); 																			//�������ʽ����Ӧ
													break;
								case 0x03:
													if(USB_Rx_Buff[3]==0x00&&USB_Rx_Buff[4]==0x00&&USB_Rx_Length==7)	//�ж��������ʽ�Ƿ��Ӧ
															Send_System_Source();
													else
														  Send_Error_Order(0x05); 																			//�������ʽ����Ӧ
													break;
								case 0x04:
													if(USB_Rx_Buff[5] == 1)//���س���ģʽ1
													{
//														free = mf_showfree("1:",&total);
															if(USB_Rx_Buff[7] == 0)
															{
																BriLight_flag = 4;
															}
															project_file_num = USB_Rx_Buff[6];
															project_file_num_now = USB_Rx_Buff[7];
//														if((USB_Rx_Buff[6]+0xFF*USB_Rx_Buff[7]) <= free)//��ѯ�Ƿ����ڴ�ռ������ļ�
//														{
															for(i=0; i<USB_Rx_Buff[8]+5; i++)
															{
																myfilename[i] = 0;
															}
															myfilename[0] = 0x31;//����(1)
															myfilename[1] = 0x3A;//����(:)
															for(i=0; i< USB_Rx_Buff[8]-1; i++)
															{
																myfilename[2+i] = USB_Rx_Buff[9+i];
//																printf("myfilename[%x]��%x\n",2+i,myfilename[2+i]);
															}

															/* ���Դ�Ŀ¼ */
															res=f_opendir(&dir,(const TCHAR*)myfilename);
															if(res!=FR_OK)
															{
																/* ��Ŀ¼ʧ�ܣ�����Ŀ¼ */
																res=f_mkdir((const TCHAR*)myfilename);
//																f_opendir(&dir,(const TCHAR*)myfilename);
//																printf("����Ŀ¼ʧ�ܣ��ʹ���Ŀ¼��\n");
															}
															else
															{
																/* ���Ŀ¼�Ѿ����ڣ��ر��� */
																res=f_closedir(&dir);	
//																printf("��Ŀ¼�Ѵ��ڣ��ر�Ŀ¼��\n");
															}
																													
															if(res == FR_OK)
															{
																for(i=0; i<(USB_Rx_Buff[3]+0xFF*USB_Rx_Buff[4]-4)+5; i++)
																{
																	myfilename[i] = 0;
																}
																myfilename[0] = 0x31;//����(1)
																myfilename[1] = 0x3A;//����(:)
																for(i=0; i< (USB_Rx_Buff[3]+0xFF*USB_Rx_Buff[4]-4); i++)
																{
																	myfilename[2+i] = USB_Rx_Buff[9+i];
//																	printf("myfilename[%x]��%x\n",2+i,myfilename[2+i]);
																}
																res_flash = f_open(&file1, (const TCHAR*)myfilename, FA_CREATE_ALWAYS | FA_WRITE );
																if(res_flash == FR_OK)
																{
																	f_close(&file1);
																	Send_Program_Download(0x01,0x01);
																}
																else
																{
																	BriLight_flag = 0;
																	//���ش�ʧ��
																	Send_Program_Download(0x01,0x02);
																}
															}else
															{
																	BriLight_flag = 0;
//																STM_EVAL_LEDOn(LED4);																//������ʾ						
//																STM_EVAL_LEDOff(LED3);
																//���ش�ʧ��
																Send_Program_Download(0x01,0x02);
															}
//														}else
//														{
//															//����û�ռ�
//															Send_Program_Download(0x01,0x03);
//														}
														
//														STM_EVAL_LEDOff(LED3);																//������ʾ//������ʾ			
														
													
													}else if(USB_Rx_Buff[5] == 2)//���س���ģʽ2
													{
//															for(i=0;i<1024;i++)
//																{printf("%2x ",USB_Rx_Buff[i]);}
//															STM_EVAL_LEDOn(LED2);
															flashbuff_flag = ProgramToFlash(USB_Rx_Buff,USB_Rx_Length,Flash_Buff,&Flash_Buff_Length);
//															STM_EVAL_LEDOff(LED2);
															if(flashbuff_flag == 1 )
															{
//#ifdef	SYSTEM_DEBUG
//																printf("%s","flash download completed\n\r");
//																for(i=0;i<Flash_Buff_Length;i++)
//																	{printf("%2x ",Flash_Buff[i]);}
//																printf("\n\r");
//#end
																//�������سɹ��ź�                              //�ֲ�ʿ����
																Send_Program_Download(0x02,0x01);
																
																if(project_file_num_now>=project_file_num-1 && USB_Rx_Buff[7]>=USB_Rx_Buff[6]-1)
																{
																	
	//																STM_EVAL_LEDOn(LED4);																//������ʾ						
	//																STM_EVAL_LEDOff(LED3);		
																	BriLight_flag = 0;
																	
																	for(i=0;i<5;i++)								//�����๦�ܵ���ѡ��λ����0
																		Tab[i].Location = 0;
																	
																	tab_flag.Tab_Level = 0;					//��ʾ������
																	tab_flag.MainTab_choice =  0;		//���๦��0
																	
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
																	tab_flag.update_flag = 1;
																	tab_flag.sound_flag = 1;
																	
//																	downloadsound_index = 0;
																	downloadsound_flag = 1;
																}
															
															}else if(flashbuff_flag == 2)
															{
//																STM_EVAL_LEDOff(LED4);																//������ʾ						
//																STM_EVAL_LEDOn(LED3);		
																Send_Program_Download(0x02,0x01);
//																STM_EVAL_LEDOff(LED2);
															}else
															{
																BriLight_flag = 0;
																Send_Program_Download(0x02,0x02);
															}
													}
													break;
													
								case 0x05:
//													STM_EVAL_LEDOn(LED2);
													updateflag = ProgramToUpdateFlash(USB_Rx_Buff,USB_Rx_Length, &Updateflash_length);
//													STM_EVAL_LEDOff(LED2);
													if(updateflag == 1)
													{
														Send_Firmware_Download(USB_Rx_Buff[5]+USB_Rx_Buff[6]*0xff,USB_Rx_Buff[7]+USB_Rx_Buff[8]*0xff,0x01); 
														//�޸Ĺ�̬��־���������ݳ��ȣ�����ת��IAP
														iap_Func(Updateflash_length);
													}else if(updateflag == 2)
													{
														Send_Firmware_Download(USB_Rx_Buff[5]+USB_Rx_Buff[6]*0xff,USB_Rx_Buff[7]+USB_Rx_Buff[8]*0xff,0x01); 
//														STM_EVAL_LEDOff(LED2);
													}
													break; 
								case 0x06:
													if(USB_Rx_Buff[3]==0x09&&USB_Rx_Buff[4]==0x00&&USB_Rx_Length==16)	//�ж��������ʽ�Ƿ��Ӧ
													{		
														UserWareData[0] = 1;			//������־λ
														
														for(i=0;i<9;i++)					//�û�����λ
															UserWareData[1+i] = USB_Rx_Buff[5+i];
														
														Send_Wave(Channel_State,UserWareData[6],UserWareData[7],UserWareData[8],UserWareData[9]) ;
													}
													else
														  Send_Error_Order(0x05); 
							
													break; 
								case 0x07:
													if(USB_Rx_Buff[3]==0x05&&USB_Rx_Buff[4]==0x00&&USB_Rx_Length==12)	//�ж��������ʽ�Ƿ��Ӧ
															GetRamData(USB_Rx_Buff[5],USB_Rx_Buff[6],USB_Rx_Buff[7],USB_Rx_Buff[8],USB_Rx_Buff[9]);
													else
														  Send_Error_Order(0x05); 	
													break; 
								case 0x08:
													if(USB_Rx_Buff[3]==0x05&&USB_Rx_Buff[4]==0x00&&USB_Rx_Length==12)	//�ж��������ʽ�Ƿ��Ӧ
													{
														UserMotorControlData[0] = 1;		//������־λ
														
														for(i=0;i<6;i++)								//�û�����λ
															UserMotorControlData[1+i] = USB_Rx_Buff[5+i];
															if(UserMotorControlData[5] == 2)
															{
																Motor_PowerSet(UserMotorControlData[2],0);
															}else
															{
																Large_Motor_On(UserMotorControlData[2], 100.0,UserMotorControlData[5]);
															}
														
														Send_MotorControl(); 
													}
													else
														  Send_Error_Order(0x05); 	
													break; 
													
								case 0x09:
													if(USB_Rx_Buff[5] == 0x01)
													{
															for(i=0; i<USB_Rx_Buff[6]+5; i++)  //���ļ���
															{
																myfilename[i] = 0;
															}
															myfilename[0] = 0x31;//����(1)
															myfilename[1] = 0x3A;//����(:)
															for(i=0; i<USB_Rx_Buff[6]; i++)
															{
																myfilename[2+i] = USB_Rx_Buff[7+i];
//																printf("myfilename[%x]��%x\n",2+i,myfilename[2+i]);
															}
															
															res_flash = f_open(&file1,(const TCHAR*)myfilename, FA_OPEN_EXISTING | FA_READ);
															if(res_flash == FR_OK)
															{
																for(i=0; i<USB_Rx_Buff[USB_Rx_Buff[6]+7]+5; i++)  //���ļ���
																{
																	myfilename[i] = 0;
																}
//																printf("USB_Rx_Buff[%x]��%x\n",USB_Rx_Buff[6]+7,USB_Rx_Buff[USB_Rx_Buff[6]+7]);
																myfilename[0] = 0x31;//����(1)
																myfilename[1] = 0x3A;//����(:)
																for(i=0; i<USB_Rx_Buff[USB_Rx_Buff[6]+7]; i++)
																{
																	myfilename[2+i] = USB_Rx_Buff[USB_Rx_Buff[6]+8+i];
//																	printf("myfilename[%x]��%x\n",2+i,myfilename[2+i]);
																}
																
																res_flash = f_open(&file2, (const TCHAR*)myfilename, FA_CREATE_ALWAYS | FA_WRITE );
																if ( res_flash == FR_OK )
																{
																	Flash_Buff_frame = file1.fsize/Flash_Buffsize;
																	for(i=0; i< Flash_Buff_frame; i++)
																	{
																		res_flash = f_read(&file1+Flash_Buff_frame*Flash_Buffsize, Flash_Buff,  Flash_Buffsize, &fnum); 
																		res_flash=f_write(&file2+Flash_Buff_frame*Flash_Buffsize,Flash_Buff, Flash_Buffsize,&fnum);
																	}
																	res_flash = f_read(&file1+Flash_Buff_frame*Flash_Buffsize, Flash_Buff,  file1.fsize-Flash_Buff_frame*Flash_Buffsize , &fnum); 
																	res_flash=f_write(&file2+Flash_Buff_frame*Flash_Buffsize,Flash_Buff, file1.fsize-Flash_Buff_frame*Flash_Buffsize ,&fnum);
																	
																	/* ���ٶ�д���ر��ļ� */
																	f_close(&file2);
																	/* ���ٶ�д���ر��ļ� */
																	f_close(&file1);
																		//����ճ��
																	
																	Send_FATFS_State(0x01,1);
																}
																else
																	Send_FATFS_State(0x01,0);
															}
															else
															Send_FATFS_State(0x01,0);
													}else if(USB_Rx_Buff[5] == 0x02)
													{
															for(i=0; i< (USB_Rx_Buff[3]+0xFF*USB_Rx_Buff[4]-1)+5; i++)
															{
																myfilename[i] = 0;
															}
															myfilename[0] = 0x31;//����(1)
															myfilename[1] = 0x3A;//����(:)
															for(i=0; i< (USB_Rx_Buff[3]+0xFF*USB_Rx_Buff[4]-1); i++)
															{
																myfilename[2+i] = USB_Rx_Buff[6+i];
//																printf("myfilename[%x]��%x\n",2+i,myfilename[2+i]);
															}
															res_flash = f_unlink((const TCHAR*)myfilename);
															if(res_flash==FR_OK)
															{
//																printf("��ɾ���ɹ���\n");
																Send_FATFS_State(0x02,1);
															}else
															{
//																printf("��ɾ��ʧ�ܡ�\r\n");
																Send_FATFS_State(0x02,0);
															}
															
													}else if(USB_Rx_Buff[5] == 0x03)
													{
															for(i=0; i<allfile_name_num; i++)
															{
																strcpy(allfile[i],"");
															}
															allfile_name_num = 0;
															strcpy(fpath,"1:");
															scan_all_files(fpath);
															for(i=0; i<allfile_name_num; i++)
															{
																if(allfilesize[i]<1024)
																	Send_FATFS_Path(allfile_name_num,i,allfile[i],1);
																else
																	Send_FATFS_Path(allfile_name_num,i,allfile[i],(u8)(allfilesize[i]/1024));
//																printf("%x--%x\n",i,allfilesize[i]);
															}
													}else if(USB_Rx_Buff[5] == 0x04)
													{
															for(i=0; i< (USB_Rx_Buff[3]+0xFF*USB_Rx_Buff[4]-1)+5; i++)
															{
																myfilename[i] = 0;
															}
															myfilename[0] = 0x31;//����(1)
															myfilename[1] = 0x3A;//����(:)
															for(i=0; i< (USB_Rx_Buff[3]+0xFF*USB_Rx_Buff[4]-1); i++)
															{
																myfilename[2+i] = USB_Rx_Buff[6+i];
//																printf("myfilename[%x]��%x\n",2+i,myfilename[2+i]);
															}
															
															res_flash = f_open(&file1, (const TCHAR*)myfilename, FA_OPEN_EXISTING | FA_READ); 	 
															if(res_flash == FR_OK)
															{
//																printf("�����ļ��ɹ�:%lu\r\n",file1.fsize);
																res_flash = f_read(&file1, Flash_Buff, file1.fsize, &fnum); 
																if(res_flash==FR_OK)
																{
																	Flash_Buff_frame = (file1.fsize-1)/(1024-10);
																	for(i=0; i< Flash_Buff_frame; i++)
																	{
																		res_flash = f_read(&file1+Flash_Buff_frame*1014, Flash_Buff,  1014, &fnum);
																		Send_FATFS_File(Flash_Buff_frame,i,Flash_Buff,1014);
																	}
																	res_flash = f_read(&file1+Flash_Buff_frame*1014, Flash_Buff,  file1.fsize-Flash_Buff_frame*1014 , &fnum); 
																	Send_FATFS_File(Flash_Buff_frame+1,Flash_Buff_frame,Flash_Buff,file1.fsize-Flash_Buff_frame*1014);
																}
																else
																{
//																printf("�����ļ���ȡʧ�ܣ�(%d)\n",res_flash);
																}		
																/* ���ٶ�д���ر��ļ� */
																f_close(&file1);	
															}
															else
															{
//																printf("�������ļ�ʧ�ܡ�\r\n");
															}
															
													}
													break; 
													
								case 0x0A://��λ���������ر�
//													if(USB_Rx_Buff[3]==0x01&&USB_Rx_Buff[4]==0x00&&USB_Rx_Length==8)	//�ж��������ʽ�Ƿ��Ӧ
//													{
														if(USB_Rx_Buff[5] == 0x01)//����
														{
															for(i=0; i<(USB_Rx_Buff[3]+0xFF*USB_Rx_Buff[4]-1)+5; i++)
															{
																myfilename[i] = 0;
															}
															myfilename[0] = 0x31;//����(1)
															myfilename[1] = 0x3A;//����(:)
//															printf("myfilename[0]��%x\n",myfilename[0]);
//															printf("myfilename[1]��%x\n",myfilename[1]);
															for(i=0; i< (USB_Rx_Buff[3]+0xFF*USB_Rx_Buff[4]-1); i++)
															{
																myfilename[2+i] = USB_Rx_Buff[6+i];
//																printf("myfilename[%x]��%x\n",2+i,myfilename[2+i]);
															}
															
															tab_flag.Tab_Level = 0;			//ѡ��������
															tab_flag.update_flag = 1;
															EnterProgram(myfilename);
															Send_Project_State(0x01);
															
														}else if(USB_Rx_Buff[5] == 0x02)//�ر�
														{
															EndProgram();
															
															//��ͣ���
															Motor_DirectionSet(5,0);
															Motor_DirectionSet(6,0);
															Motor_DirectionSet(7,0);
															Motor_DirectionSet(8,0);
			
															Send_Project_State(0x01);
														}
														else
														{
															Send_Project_State(0x00);
														}
//													}
//													else
//														  Send_Error_Order(0x05); 																			//�������ʽ����Ӧ
													break; 
								case 0x0B://�������е�ַ
													if(USB_Rx_Buff[3]==0x00&&USB_Rx_Buff[4]==0x00&&USB_Rx_Length==7)	//�ж��������ʽ�Ƿ��Ӧ
													{
														if(programflag == 1 )
														{
															Send_Project_Adr(1, pc, Flash_Buff[pc]);
														}else
														{
															Send_Project_Adr(0, 0x00000000, 0x00);
														}
													}
													else
														  Send_Error_Order(0x05); 																			//�������ʽ����Ӧ
													break; 
								
								default: Send_Error_Order(0x04);	//��Ч������
						}
				}
				USB_Rx_Length = 0;												//���������һ������
				Receive_end = 0;
		}
		
		if(errorflag)
		{
				Send_Error_Order(0x03);										//֡��ʽ��ȫ
				errorflag = 0;
		}
		if(flashtimeout)
		{
				Send_Program_Download(0x02,0x02);							//�����ճ�ʱ
				flashtimeout = 0;
		}
		if(updateflashtimeout)
		{
				Send_Firmware_Download(USB_Rx_Buff[5],USB_Rx_Buff[6],0x02);							//�����ճ�ʱ
				updateflashtimeout = 0;
		}
}



//-----------�ӹ��ܺ���-----------------

//��������������
void Send_Heartbeat_Packet(void)   
{
		int _cnt,i;
		u8 data_to_send[64]={"0"},jiaoyanma;
		_cnt = 0;jiaoyanma = 0x00;

		data_to_send[_cnt++]=0x68;
		data_to_send[_cnt++]=0x21;
		data_to_send[_cnt++]=0x01;
		data_to_send[_cnt++]=0x06;	
		data_to_send[_cnt++]=0x00;
		
		for(i=0;i<6;i++)
		{
			data_to_send[_cnt++] = version[i];
		}

		for(i=0;i<_cnt;i++)	jiaoyanma += data_to_send[i];
		data_to_send[_cnt++]=jiaoyanma;
		
		data_to_send[_cnt++]=0x16;
		
//		UsbToPc_SendString(data_to_send,_cnt);			//���⴮�ڷ���
		HID_SendBuff(data_to_send,64);
		
}

//���ʹ��������ݺ���
//Connection_N Ϊ����������1-4��
void Send_Sensor_Data(u8 Channel_State[32][13], u8 Connection_N)   	//���޸ģ�������
{
		int _cnt = 0;
		u8 i,j;
		u8 data_to_send[192]={"0"},jiaoyanma = 0x00;
//		u8 DataLength,LengthL,LengthH;

		data_to_send[_cnt++]=0x68;
		data_to_send[_cnt++]=0x21;
		data_to_send[_cnt++]=0x02;
		data_to_send[_cnt++]=0x21;
		data_to_send[_cnt++]=0x00;
		
		data_to_send[_cnt++]=Connection_N;
		
		for(i=0;i<8*(Connection_N+1);i++)
		{
			data_to_send[_cnt++] = 0;
			for(j=0;j<3;j++)
			{
				data_to_send[_cnt++] = Channel_State[i][j];
			}
		}
				
		for(i=0;i<_cnt;i++)	jiaoyanma += data_to_send[i];
		data_to_send[_cnt++]=jiaoyanma;
		
		data_to_send[_cnt++]=0x16;
		
		HID_SendBuff(data_to_send,((_cnt-1)/64+1)*64);
}


//����brickϵͳ��Դ����
void Send_System_Source(void)   
{
		int _cnt,i;
		u32 total,free;
		u8 data_to_send[64]={"0"},jiaoyanma;
		_cnt = 0;jiaoyanma = 0x00;

		data_to_send[_cnt++]=0x68;
		data_to_send[_cnt++]=0x21;
		data_to_send[_cnt++]=0x03;
		data_to_send[_cnt++]=0x04;	
		data_to_send[_cnt++]=0x00;
		
		free = mf_showfree("1:",&total);
		
		data_to_send[_cnt++] = (u8)total;
		data_to_send[_cnt++] = (u8)(total>>8);
		
		data_to_send[_cnt++] = (u8)free;
		data_to_send[_cnt++] = (u8)(free>>8);
		
		for(i=0;i<_cnt;i++)	jiaoyanma += data_to_send[i];
		data_to_send[_cnt++]=jiaoyanma;
		
		data_to_send[_cnt++]=0x16;
		
		HID_SendBuff(data_to_send,64);
}


//�������ط��غ���
void Send_Program_Download(u8 mode, u8 flag)   
{
		int _cnt,i;
		u8 data_to_send[64]={"0"},jiaoyanma;
		_cnt = 0;jiaoyanma = 0x00;

		data_to_send[_cnt++]=0x68;
		data_to_send[_cnt++]=0x21;
		data_to_send[_cnt++]=0x04;
		data_to_send[_cnt++]=0x02;	
		data_to_send[_cnt++]=0x00;
		
		data_to_send[_cnt++]=mode;
		data_to_send[_cnt++]=flag;
		
		for(i=0;i<_cnt;i++)	jiaoyanma += data_to_send[i];
		data_to_send[_cnt++]=jiaoyanma;
		
		data_to_send[_cnt++]=0x16;
		
//		UsbToPc_SendString(data_to_send,_cnt);
		HID_SendBuff(data_to_send,64);
		
}

//�̼����ط��غ���
void Send_Firmware_Download(u16 total, u16 now, u8 flag)
{
		int _cnt,i;
		u8 data_to_send[64]={"0"},jiaoyanma;
		_cnt = 0;jiaoyanma = 0x00;

		data_to_send[_cnt++]=0x68;
		data_to_send[_cnt++]=0x21;
		data_to_send[_cnt++]=0x05;
		data_to_send[_cnt++]=0x05;	
		data_to_send[_cnt++]=0x00;
		
		data_to_send[_cnt++]=total;
		data_to_send[_cnt++]=total>>8;
		data_to_send[_cnt++]=now;
		data_to_send[_cnt++]=now>>8;
		data_to_send[_cnt++]=flag;
		
		for(i=0;i<_cnt;i++)	jiaoyanma += data_to_send[i];
		data_to_send[_cnt++]=jiaoyanma;
		
		data_to_send[_cnt++]=0x16;
		
		HID_SendBuff(data_to_send,64);
		
}


//�����ϴ�����
void Send_Wave(u8 Channel_State[32][13],u8 lay,u8 port,u8 id, u8 mode)   //n������������		//���޸ģ�������
{
		int _cnt,i;
		u8 data_to_send[64]={"0"},jiaoyanma;
		_cnt = 0;jiaoyanma = 0x00;

		data_to_send[_cnt++]=0x68;
		data_to_send[_cnt++]=0x21;
		data_to_send[_cnt++]=0x06;
		data_to_send[_cnt++]=0x0E;	
		data_to_send[_cnt++]=0x00;

		//id��modeƥ��
		if(Channel_State[port-1][0]			== port && 
				Channel_State[port-1][1]		== id   && 
					Channel_State[port-1][2]	== mode  )
		{
			data_to_send[_cnt++] = lay;
			for(i=0;i<13;i++)
				data_to_send[_cnt++] = Channel_State[port-1][i];
		}
		else
		{
			for(i=0;i<14;i++)
				data_to_send[_cnt++] = 0xff;
		}
		for(i=0;i<_cnt;i++)	jiaoyanma += data_to_send[i];
		data_to_send[_cnt++]=jiaoyanma;
		
		data_to_send[_cnt++]=0x16;
		
		HID_SendBuff(data_to_send,64);
		
}
//�ļ�ϵͳ����
void Send_FATFS_State(uint8_t mode, uint8_t state)
{
	int _cnt,i;
	u8 data_to_send[64]={"0"},jiaoyanma;
	_cnt = 0;jiaoyanma = 0x00;

	data_to_send[_cnt++]=0x68;
	data_to_send[_cnt++]=0x21;
	data_to_send[_cnt++]=0x09;
	data_to_send[_cnt++]=0x02;
	data_to_send[_cnt++]=0x00;
	
	data_to_send[_cnt++]=mode;
	data_to_send[_cnt++]=state;
		
	for(i=0;i<_cnt;i++)	jiaoyanma += data_to_send[i];
	data_to_send[_cnt++]=jiaoyanma;
	
	data_to_send[_cnt++]=0x16;
	
	HID_SendBuff(data_to_send,64);
}

//�ļ�·������
void Send_FATFS_Path(uint8_t totalframe, u8 nowframe,  uint8_t  buff[],float len)
{
	uint8_t *p;
	int _cnt;
	unsigned int i;
	u8 data_to_send[128]={"0"},jiaoyanma,LengthL,LengthH,name_len;
	
	_cnt = 0;jiaoyanma = 0x00;
	
	p=(uint8_t *)(&len);
	
	name_len = (strlen(buff)-2);
	LengthL = (name_len+7)%256;
	LengthH = (name_len+7)/256;
	
	data_to_send[_cnt++]=0x68;
	data_to_send[_cnt++]=0x21;
	data_to_send[_cnt++]=0x09;
	data_to_send[_cnt++]=LengthL;
	data_to_send[_cnt++]=LengthH;
	
	data_to_send[_cnt++]=0x03;
	
	data_to_send[_cnt++]=totalframe;
	data_to_send[_cnt++]=nowframe;

//	printf("len:%f\r\n",len);
	
	data_to_send[_cnt++] = *(p);
	data_to_send[_cnt++] = *(p+1);
	data_to_send[_cnt++] = *(p+2);
	data_to_send[_cnt++] = *(p+3);
	
	for(i=0; i<name_len; i++)
	{
		data_to_send[_cnt++]=buff[2+i];//ȥ����1����
	}
	
	for(i=0;i<_cnt;i++)	jiaoyanma += data_to_send[i];
	data_to_send[_cnt++]=jiaoyanma;
	
	data_to_send[_cnt++]=0x16;
	
	HID_SendBuff(data_to_send,((_cnt-1)/64+1)*64);
}

//�ļ����ݷ���
void Send_FATFS_File(uint8_t totalframe, u8 nowframe,  uint8_t  buff[],unsigned int len)
{
	int _cnt;
	unsigned int i;
	u8 data_to_send[1024]={"0"},jiaoyanma,LengthL,LengthH;
	_cnt = 0;jiaoyanma = 0x00;

	LengthL = (len+3)%256;
	LengthH = (len+3)/256;
	
	data_to_send[_cnt++]=0x68;
	data_to_send[_cnt++]=0x21;
	data_to_send[_cnt++]=0x09;
	data_to_send[_cnt++]=LengthL;
	data_to_send[_cnt++]=LengthH;
	
	data_to_send[_cnt++]=0x04;
	data_to_send[_cnt++]=totalframe;
	data_to_send[_cnt++]=nowframe;
		
//	printf("Flash_BuffSize:%x\n\r",Flash_BuffSize);
	for(i=0; i<len; i++)
	{
		data_to_send[_cnt++]=buff[i];
//		printf("Flash_Buff[%x]:%x\n\r",i,Flash_Buff[i]);
	}
	
	for(i=0;i<_cnt;i++)	jiaoyanma += data_to_send[i];
	data_to_send[_cnt++]=jiaoyanma;
	data_to_send[_cnt++]=0x16;
	
	for(i=0; i<(_cnt-1)/64+1; i++)
	{
		HID_SendBuff(data_to_send+64*i,64);
	}
}

//��λ���������ر�
void Send_Project_State(uint8_t state)
{
	int _cnt,i;
	u8 data_to_send[64]={"0"},jiaoyanma;
	_cnt = 0;jiaoyanma = 0x00;

	data_to_send[_cnt++]=0x68;
	data_to_send[_cnt++]=0x21;
	data_to_send[_cnt++]=0x0A;
	data_to_send[_cnt++]=0x01;
	data_to_send[_cnt++]=0x00;
	
	data_to_send[_cnt++]=state;
	
	for(i=0;i<_cnt;i++)	jiaoyanma += data_to_send[i];
	data_to_send[_cnt++]=jiaoyanma;
	
	data_to_send[_cnt++]=0x16;
	
	HID_SendBuff(data_to_send,64);
}

//���س������е�ַ
void Send_Project_Adr(uint8_t flag, uint32_t pc, uint8_t instruct)
{
	int _cnt,i;
	u8 data_to_send[64]={"0"},jiaoyanma;
	_cnt = 0;jiaoyanma = 0x00;

	data_to_send[_cnt++]=0x68;
	data_to_send[_cnt++]=0x21;
	data_to_send[_cnt++]=0x0B;
	data_to_send[_cnt++]=0x06;
	data_to_send[_cnt++]=0x00;
	if(flag == 1)
	{
		data_to_send[_cnt++]=0x01;
		data_to_send[_cnt++]= (uint8_t) ( pc & 0xFF000000) >> 8*3;
		data_to_send[_cnt++]= (uint8_t) ( pc & 0x00FF0000) >> 8*2;
		data_to_send[_cnt++]= (uint8_t) ( pc & 0x0000FF00) >> 8;
		data_to_send[_cnt++]= (uint8_t) ( pc & 0x000000FF);
		data_to_send[_cnt++]=instruct;
	}else
	{
		data_to_send[_cnt++]=0x00;
		data_to_send[_cnt++]=0x00;
		data_to_send[_cnt++]=0x00;
		data_to_send[_cnt++]=0x00;
		data_to_send[_cnt++]=0x00;
		data_to_send[_cnt++]=0x00;
	}
	
	for(i=0;i<_cnt;i++)	jiaoyanma += data_to_send[i];
	data_to_send[_cnt++]=jiaoyanma;
	
	data_to_send[_cnt++]=0x16;
	
	HID_SendBuff(data_to_send,64);
}

//RAM��ַ���ݲ�ѯ
void GetRamData(u8 Address0,u8 Address1,u8 Address2,u8 Address3,u8 Length)
{
	int _cnt = 0;
	u16 i,DataLength;
	u8 data_to_send[320]={"0"},jiaoyanma = 0x00;
	
	DataLength = 5+Length;

	data_to_send[_cnt++]=0x68;
	data_to_send[_cnt++]=0x21;
	data_to_send[_cnt++]=0x07;
	data_to_send[_cnt++]=DataLength%256;
	data_to_send[_cnt++]=DataLength/256;
	
	data_to_send[_cnt++]=Address0;
	data_to_send[_cnt++]=Address1;
	data_to_send[_cnt++]=Address2;
	data_to_send[_cnt++]=Address3;
	data_to_send[_cnt++]=Length;
	for(i=0;i<Length;i++)			//0x00---0xff
	{
		data_to_send[_cnt++]=GetByte(GetRamBase(Address0,Address1,Address2,Address3)+i);
	}
	
	for(i=0;i<_cnt;i++)	jiaoyanma += data_to_send[i];
	data_to_send[_cnt++]=jiaoyanma;
	data_to_send[_cnt++]=0x16;
	
	HID_SendBuff(data_to_send,64+(_cnt-1)/64*64);

}
	

void Send_MotorControl(void)   
{
		int _cnt,i;
		u8 data_to_send[64]={"0"},jiaoyanma;
		_cnt = 0;jiaoyanma = 0x00;

		data_to_send[_cnt++]=0x68;
		data_to_send[_cnt++]=0x21;
		data_to_send[_cnt++]=0x08;
		data_to_send[_cnt++]=0x00;	
		data_to_send[_cnt++]=0x00;
		
		for(i=0;i<_cnt;i++)	jiaoyanma += data_to_send[i];
		data_to_send[_cnt++]=jiaoyanma;
		
		data_to_send[_cnt++]=0x16;
		
		HID_SendBuff(data_to_send,64);
}

//����ָ��غ���
void Send_Error_Order(char n)   //n����ָ���������
{
		int _cnt,i;
		u8 data_to_send[64]={"0"},jiaoyanma;
		_cnt = 0;jiaoyanma = 0x00;

		data_to_send[_cnt++]=0x68;
		data_to_send[_cnt++]=0x21;
		data_to_send[_cnt++]=0xee;
		data_to_send[_cnt++]=0x01;	
		data_to_send[_cnt++]=0x00;
		
		switch(n){
				case 0x01:data_to_send[_cnt++]=0x01;break;
				case 0x02:data_to_send[_cnt++]=0x02;break;
				case 0x03:data_to_send[_cnt++]=0x03;break;
				case 0x04:data_to_send[_cnt++]=0x04;break;
				case 0x05:data_to_send[_cnt++]=0x05;break;
				case 0x06:data_to_send[_cnt++]=0x06;break;
				case 0x07:data_to_send[_cnt++]=0x07;break;
		}
		
		for(i=0;i<_cnt;i++)	jiaoyanma += data_to_send[i];
		data_to_send[_cnt++]=jiaoyanma;
		
		data_to_send[_cnt++]=0x16;
		
//		UsbToPc_SendString(data_to_send,_cnt);
		HID_SendBuff(data_to_send,64);
		
}

//�ڳ�������ʱ������������֡�����������ݷ���flash�У����԰��ȷ��������У�
uint8_t ProgramToFlash(uint8_t Frame_Buff[], unsigned int Frame_Buff_Length, 
												uint8_t flash_buff[], unsigned int *flash_length)
{
	int i;
	
	u32 count=0;
//	static unsigned int i = 0;
	if(!Frame_Buff[7])																		//��֡
	{
		count=0;
		*flash_length = 0;
		Flash_TimePic = 400000;																//������ʱ��Ϊ400ms
		flashbuff_start = 1; 
		
//		f_close(&file1);
//		f_unlink((const TCHAR*)myfilename);
		f_open(&file1, (const TCHAR*)myfilename,FA_CREATE_ALWAYS | FA_WRITE);
//		printf("begain:%d-count:%x\n\r",*flash_length,count);
//		printf("begain\n\r");
		
	}
	
	if(flashbuff_start)
	{ 
//		for(i=0;i<(Frame_Buff[3]+256*Frame_Buff[4]-3);i++)	//�����ļ�����
//		{
//			flash_buff[(*flash_length)++] = Frame_Buff[8+i];
//		}
		count = Frame_Buff[3]+256*Frame_Buff[4]-3;
//		if(count >= Flash_Download_Size)
//		{
//			f_lseek (&file1, (*flash_length));
//			res_flash=f_write(&file1,&Frame_Buff[8],Flash_Download_Size,&fnum);
//			(*flash_length) += Flash_Download_Size;
////			f_close(&file1);
//			
////			for(i=0;i<count;i++)
////				{printf("%2x ",Frame_Buff[8+i]);}
//			
//		}
		if(Frame_Buff[7]>=Frame_Buff[6]-1)									//β֡
		{
//			printf("%s----","here111");
			
			for(i=0; i<count; i++)
			{
				Flash_Buff[(*flash_length)+i] = Frame_Buff[8+i];
			}
			(*flash_length) += count;
			res_flash=f_write(&file1,Flash_Buff,(*flash_length),&fnum);
//			f_lseek (&file1, (*flash_length));
//			res_flash=f_write(&file1,&Frame_Buff[8],count,&fnum);
			f_close(&file1);
			
			for(i=0; i<(*flash_length); i++)
			{
				Flash_Buff[(*flash_length)+i] = 0;							//����buffer����
			}
			
			flashbuff_start = 0;															//����program������ɣ���������µ�program
			
			
			
//			for(i=0;i<count;i++)
//				{printf("%2x ",Frame_Buff[8+i]);}
//			printf("\n\r");
//			printf("--------------------------\n\r");
//			printf("flash_length:%d",(*flash_length));
			return 1;																					//program�������
		}else
		{
//			f_lseek (&file1, (*flash_length));
//			res_flash=f_write(&file1,&Frame_Buff[8],count,&fnum);
			for(i=0; i<count; i++)
			{
				Flash_Buff[(*flash_length)+i] = Frame_Buff[8+i];
			}
			(*flash_length) += count;
//			for(i=0;i<count;i++)
//				{printf("%2x ",Frame_Buff[8+i]);}
//			printf("\n\r");
//			printf("--------------------------\n\r");
//			printf("flash_length:%d",(*flash_length));
			return 2;
		}
	}
	return 0;
}


//�̼����£���usb���յ���֡���ݴ浽����flash��Update��
uint8_t ProgramToUpdateFlash(uint8_t Frame_Buff[], unsigned int Frame_Buff_Length, u32 *Updateflash_length)
{
	static unsigned int i = 0;
	static uint32_t Update_Count_Adderss;
	u16 Update_Length = 0;
	u16 Update_Buff[1024] = {0}; 													//���ڻ������ݵ�����
	u16 temp = 0;
	
	if(Frame_Buff[7]+Frame_Buff[8]*0xff == 0)																		//��֡
	{
		*Updateflash_length = 0;
		UpdateFlash_TimePic = 400000;												//������ʱ��Ϊ400s
		updateflash_start = 1;
		
		LCD_clear();
		LCD_Display_pic(0, 12, 180, 128, DOWNLOAD_FIRMWARE);
		
		FLASH_Unlock();																//������ϵͳ����
		FLASH_EraseSector(FLASH_Sector_8, VoltageRange_3);
		FLASH_EraseSector(FLASH_Sector_9, VoltageRange_3);
		FLASH_EraseSector(FLASH_Sector_10, VoltageRange_3);
		FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3);
		
		factorytest = 0;
		tab_flag.update_flag = 1;
		tab_flag.Tab_Level = 4;
		
		Update_Count_Adderss = UPDATE_ADDRESS;
//		printf("�߳������� \n\r");
	}
	
	if(updateflash_start)
	{
		//���ݰ�λ�ں�Ϊ16λ
		for(i = 0; i < Frame_Buff[3]+256*Frame_Buff[4]-4; i+=2)
		{
			temp = (((u16)Frame_Buff[9+i+1])<<8) + ((u16)Frame_Buff[9+i]);
			Update_Buff[Update_Length] = temp;
			Update_Length++;
		}
		
		//���ںϺ�����ݷŵ�flash�ĸ�����
		STMFLASH_Write_NoCheck(Update_Count_Adderss,Update_Buff,Update_Length);

//		STMFLASH_Write(Update_Count_Adderss,Update_Buff,Update_Length);
//		for(i=0; i<Update_Length ; i++)
//		{
//			printf("%4x",Update_Buff[i]);
//		}
		Update_Count_Adderss += 2*Update_Length;
		
		//ͳ�Ƹ��µ��ļ�����
		*Updateflash_length += 2*Update_Length;
		
		fupd_prog(130,84,16,Frame_Buff[5]+Frame_Buff[6]*0xff,Frame_Buff[7]+Frame_Buff[8]*0xff);	
		
		if((Frame_Buff[5]+Frame_Buff[6]*0xff) <= (Frame_Buff[7]+Frame_Buff[8]*0xff)+1)									//β֡
		{
//			printf("��ȡ����\n\r");    
			FLASH_Lock();//����
			updateflash_start = 0;															//����program������ɣ���������µ�program
			return 1;																					//program�������
		}else
		{
			return 2;
		}
	}
	return 0;
}

//����֡��������ʧ���	
void FrameBuffLoseCheck(void)
{	
	if(USB_TimePic) USB_TimePic--;		
	if(!USB_TimePic&&Receive_start)		//������ʱ������,usb��û������ȫ����֡,��ϵͳ����
	{
		Receive_start = 0;							//�������½�������
		USB_Rx_Length = 0;
		errorflag = 1;
	}
}

//�����ݰ���ʧ���
void FlashBuffLoseCheck(void)
{
	if(Flash_TimePic) Flash_TimePic--;		
	if(!Flash_TimePic&&flashbuff_start)	//������ʱ������,������δ���
	{
		flashbuff_start = 0;							//�������½�������
		flashtimeout = 1;									//��ʾ�����ճ�ʱ
	}
	
	if(UpdateFlash_TimePic) UpdateFlash_TimePic--;
	if(!UpdateFlash_TimePic&&updateflash_start)	//������ʱ������,������δ���
	{
		updateflash_start = 0;							//�������½�������
		updateflashtimeout = 1;									//��ʾ�����ճ�ʱ
	}
}

void FrameOrFlashBuffLoseCheck(void)
{
	FrameBuffLoseCheck();
	FlashBuffLoseCheck();
}

void UpdateState(u8 Channel_State[32][13], u8 port, u8 id, u8 mode, float SensorData)
{
	uint8_t *p,i;
	p=(uint8_t *)(&SensorData);
	
	Channel_State[port-1][0] = port;
	Channel_State[port-1][1] = id;
	Channel_State[port-1][2] = mode;
	for (i=0; i<10; i++) 
	{
		Channel_State[port-1][3+i] = *(p+i);
	}
}
void EnterProgram(u8 myfilename[])
{
	int i=0;
//	W25QXX_Read(programlen,8*1024*1024,2);							
//	W25QXX_Read(Flash_Buff,8*1024*1024+2,(programlen[0]+256*programlen[1]));

//	res_flash = f_open(&file1, "1:/Project/�ֲ�ʿ�����ļ�.dbf", FA_OPEN_EXISTING | FA_READ); 	
	res_flash = f_open(&file1,(const TCHAR*)myfilename, FA_OPEN_EXISTING | FA_READ);	
	if(res_flash == FR_OK)
	{
//		printf("�����ļ��ɹ���\r\n");
		res_flash = f_read(&file1, Flash_Buff,  file1.fsize, &fnum); 
		if(res_flash==FR_OK)
		{
//			for(i=0;i<file1.fsize;i++)
//				{printf("%2x ",Flash_Buff[i]);}
//				printf("file1.fsize%d \n\r",(int)file1.fsize);
//			printf("���ļ���ȡ�ɹ�,�����ֽ����ݣ�%d\r\n",fnum);
//			printf("����ȡ�õ��ļ�����Ϊ��\r\n%s \r\n", Flash_Buff);	
			
			//�������
			programflag = 1;
			UIRUN_flag  = 1;
			BriLight_flag = 1;

			//UI����
			tab_flag.Tab_Level = 3;
			tab_flag.update_flag = 1;

			//����˿����帴λ
			for(i=0;i<4;i++)
			SumPluseForDisplay[i] = 0;

			//RAM��ʼ��
			memset(RAM_Buff,0,sizeof(RAM_Buff));

			//�����̳߳�ʼ��
			for(i=0;i<ThreadSize;i++)
			{
			Thread[i].state			= 0;
			Thread[i].Threadpc	= 0x00000000;
			Thread[i].timetmp		= 0;
			}
			Thread[0].state = 1;

			//����ʱ���ʼ��																						
			ProgramTime = 0;
			for(i=0; i<8; i++)
			{
			Timer[i].time = 0;
			}
			
		}
		else
		{
//			printf("�����ļ���ȡʧ�ܣ�(%d)\n",res_flash);
		}		
		/* ���ٶ�д���ر��ļ� */
		f_close(&file1);	
		
	}
	else
	{
//		printf("�������ļ�ʧ��:%x��\r\n",res_flash);
	}
				
	
}

void EndProgram(void)
{
	int i;
	//�˳�����
	programflag = 0;
	BriLight_flag = 0;
	pc = 0x0000;

  //UI��λ
	LCD_flag = 0;
	if(tab_flag.MainTab_choice == 1)
	{
		tab_flag.Tab_Level = 1;			//ѡ���ӽ���
		tab_flag.update_flag = 1;
	}else
	{
		tab_flag.Tab_Level = 0;			//ѡ��������
		tab_flag.update_flag = 1;
	}
			
//	//��ͣ���
//	Motor_DirectionSet(5,0);
//	Motor_DirectionSet(6,0);
//	Motor_DirectionSet(7,0);
//	Motor_DirectionSet(8,0);

	//��ת��λ
	for(i=0;i<4;i++)
	{
		if(motor_on_state[i] == 1)
		{
			Motor_PowerSet(i+5,0);
		}
		motor_state[i]  = 0;
		Motor_Invert[i] = 0;
	}
	
//	//������λ
//	DMA_Cmd(DMA1_Stream5, DISABLE);                      //�ر�DMA���� 
//	while (DMA_GetCmdStatus(DMA1_Stream5) != DISABLE){}	//ȷ��DMA���Ա�����  		

	for(i=0; i < 32; i++)
	{
		SPI_Mp3_TxBuf[i]=0;
	}

	if(SOUND_Buff_flag == 1)
	{
		f_close(&file3);
	}
	
	BriLight_flag = 0;
	
	SOUND_UI=0;
	SOUND_Buff_index = 0;
	SOUND_Buff_flag = 0;
	piano_Buff_flag = 0;
	piano_index = 0;
	
	//����˿����帴λ
	for(i=0;i<4;i++)
	SumPluseForDisplay[i] = 0;

	//RAM��ʼ��
	memset(RAM_Buff,0,sizeof(RAM_Buff));
	memset(Flash_Buff,0,sizeof(Flash_Buff));
//	for(i=0;i<Flash_Buffsize;i++)
//	{
//		Flash_Buff[i]=0;
//	}
	
	//�����̳߳�ʼ��
	for(i=0;i<ThreadSize;i++)
	{
		Thread[i].state			= 0;
		Thread[i].Threadpc	= 0x00000000;
		Thread[i].timetmp		= 0;
	}
	Thread[0].state = 1;

	//����ʱ���ʼ��																						
	ProgramTime = 0;
	for(i=0; i<8; i++)
	{
		Timer[i].time = 0;
	}
}

