/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/usart6.c 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   
  ******************************************************************************
**/
#include "usart6.h"
#include "stdio.h"
#include "Bluetooth.h"
#include "C_Protocol.h"

//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif	  
 
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART6->SR&0X40)==0);//ѭ������,ֱ���������   
	USART6->DR = (u8) ch;      
	return ch;
}
#endif

#if EN_USART6_RX   //���ʹ���˽���
////����1�жϷ������
////ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART6_RX_BUF[USART6_REC_LEN];     //���ջ���,���UART_REC_LEN���ֽ�.
////����״̬
////bit15��	������ɱ�־
////bit14��	���յ�0x0d
////bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART6_RX_STA=0;       //����״̬���	

//��ʼ��IO ����1 
//bound:������
void usart6_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIOC��GPIODʱ��
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//ʹ��UART5ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6); //GPIOC12����ΪUART5
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6); //GPIOD2����ΪUART5
	
	//UART5�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //GPIOC12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOG,&GPIO_InitStructure); //��ʼ��GPIOC12
	//UART5�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; //GPIOD2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOG,&GPIO_InitStructure); //��ʼ��GPIOD2

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART6, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART6, ENABLE);  //ʹ�ܴ���1 
	
	//USART_ClearFlag(UART5, USART_FLAG_TC);
	
#if EN_USART6_RX
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//��������ж�

	//Uart5 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif
	
}

void USART6_Send_Byte(u16 Data)
{
	while(!USART_GetFlagStatus(USART6,USART_FLAG_TXE));
	//USART_GetFlagStatus���õ�����״̬λ
	//USART_FLAG_TXE:���ͼĴ���Ϊ�� 1��Ϊ�գ�0��æ״̬
	USART_SendData(USART6,Data);
}

int usart6_flag = -1;
u8 status=0;
u8 DATA_LEN;
u8 dis_sign=0;
u8 dis_data_len=0;
void USART6_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res2,i=0,t,j=0;
	
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		USART_ClearITPendingBit(USART6,USART_IT_RXNE);	//����жϱ�־λ
		
//		Res2=USART_ReceiveData(USART6);			//�����ַ���������
//		USART6_Send_Byte(Res2);
//		
//		usart6_flag = Res2-'0';
		
		Res2=USART_ReceiveData(USART6);			//�����ַ���������
		

			if(!Receive_start&&!USB_Rx_Length)
			{
				if(Res2 == 0x68) 	//���յ���ʼ��
				{
					Receive_start = 1;
					USB_TimePic = 3000; 								//����ʱ���ʱ
				}
				else
				{
					switch (status)
					{
						case 0:						
							if( Res2==0x4F)   //O
							{
								status++;
							}
							break;
						case 1:
							if( Res2==0x4B)  //K
								status++;
							else status = 0;
							break;
						case 2:
							if( Res2==0x2B)  //+
								status++;
							else status = 0;
							break;
						case 3:
							if(Res2==0x41)  //A
							{
								DATA_LEN = 16;
								status++;
							}
							else if(Res2==0x4C)  //L
							{
								DATA_LEN = 3;
								status++;
							}
							else if(Res2==0x53)  //S
							{
								DATA_LEN = 4;
								status++;
							}
							else if(Res2==0x4E)  //N
							{
								status++;
								status++;
							}
							else if(Res2==0x43)  //C
							{
								DATA_LEN = 3;
								status++;
							}
							else if(Res2==0x44)  //D
							{
								dis_sign=1;
								status++;
							}
							else status = 0;
							break;
						case 4:
							if(dis_sign!=0)
							{
								if(Res2==0x49)  //I
								dis_sign=2;
								else if((Res2==0x53)&&(dis_sign==2))  //S
								dis_sign=3;
								else if((Res2>=0x30)&&(Res2<=0x37)&&(dis_sign==3))  //0~7
								{
									USART6_RX_BUF[USART6_RX_STA]=Res2;
									USART6_RX_STA++;
									dis_sign=4;
								}
								else if(dis_sign==4)
								{
									USART6_RX_BUF[USART6_RX_STA]=Res2;
									USART6_RX_STA++;
									if(Res2==0X0D) 
									{ 
										status = 0; 
										dis_sign=0;
										if(memcmp(USART6_RX_BUF+17,"NAME:",strlen("NAME:"))==0)
										{
											dis_sum++;
											j=1;
											t=0;
											while(j)
											{
												device[dis_sum-1][t] = USART6_RX_BUF[t+22];
												t++;
												if(USART6_RX_BUF[t+22]==0x0d)
													j=0;
											}
											device[dis_sum-1][t] = '(';
											t++;
												for(j=0;j<4;j++)
												{
													device[dis_sum-1][t] = USART6_RX_BUF[j+10];
													t++;												
												}
											device[dis_sum-1][t] = ')';
											device[dis_sum-1][t+1]='\0';
										}		
										USART6_RX_STA = 0;   //����
									}
								}
								else
								{
										status = 0; 
										dis_sign=0;
								}
							}
							else
							{
								USART6_RX_BUF[USART6_RX_STA]=Res2 ;
								USART6_RX_STA++;
								if(USART6_RX_STA==DATA_LEN) 
								{ 
									status = 0; 
									bluetooth_msg();
									USART6_RX_STA = 0;   //����
								}
							}
							break;
						case 5:
							USART6_RX_BUF[USART6_RX_STA]=Res2 ;
							USART6_RX_STA++;
								if(Res2==0X0D) 
								{ 
									status = 0; 
									bluetooth_msg();
									USART6_RX_STA = 0;   //����
								}
							break;					
					}
				}
			}
			if(Receive_start)
			{
				USB_Rx_Buff[USB_Rx_Length++] = Res2; 
				if(USB_Rx_Length-1>4)		//����������
				{
					if((USB_Rx_Length>5+USB_Rx_Buff[3]+USB_Rx_Buff[4]*256))  				 //�ܿ����������ݵ��ڽ���������
					{
//						for(i=6+USB_Rx_Buff[3]+USB_Rx_Buff[4]*256; i<USB_Rx_Length; i++) //Ѱ�ҽ�����
//						{
							if(Res2 == 0x16) 		//���յ�������
							{
								Receive_start = 0; 					 		//��֡���ݽ��ս���������Ȩ������һ֡����
								Receive_end = 1;
//								USB_Rx_Length = i+1; 				 		//��ȥ���������޹ص�����
								
//								for(i=0;i<USB_Rx_Length;i++)
//									printf("%x ",USB_Rx_Buff[i]);
//								break;
							}
//						}		
					}		
				}
			}

  }
	
//	if(bluetooth_state==0||bluetooth_state==1)
//		bluetooth_msg();
//	else
//		Show_Str(10,10,150,100,USB_Rx_Buff,12,0);
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
}
#endif	
 
 int USART6_Receive(const char *str)//�жϽ��յ��ַ���
{
	int i;
		if(memcmp(USART6_RX_BUF,str,strlen(str))==0)
		{
//			Show_Str(10,10,150,100,USART6_RX_BUF,12,0);
//				printf("bluetooth:");
//			for(i=0;i<strlen(str);i++)
//			{
//				printf("%x",USART6_RX_BUF[i]);
//			}
//				printf("\n\r");
			return 1;
		}
		else
		{
//			Show_Str(10,10,150,100,USART6_RX_BUF,12,0);
				return 0;
		}
		
}