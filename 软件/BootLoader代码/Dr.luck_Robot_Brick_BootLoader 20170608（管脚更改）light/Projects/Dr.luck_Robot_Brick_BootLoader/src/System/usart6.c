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
_sys_exit(int x)
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
//u8 USART6_RX_BUF[USART6_REC_LEN];     //���ջ���,���UART_REC_LEN���ֽ�.
////����״̬
////bit15��	������ɱ�־
////bit14��	���յ�0x0d
////bit13~0��	���յ�����Ч�ֽ���Ŀ
//u16 USART6_RX_STA=0;       //����״̬���	

//��ʼ��IO ����1 
//bound:������
void usart6_init(u32 bound){
	
//	USART_InitTypeDef USART_InitStructure;	
//	GPIO_InitTypeDef GPIO_InitStructure;    
//	
//    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOG  , ENABLE);    
//    //PG14->TX  PG9->Rx
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOG, &GPIO_InitStructure);
//    
//    GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6);
//    GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6);
//	//---------------------------------------------------------------
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);    
//    USART_InitStructure.USART_BaudRate = bound;
//    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//    USART_InitStructure.USART_StopBits = USART_StopBits_1;
//    USART_InitStructure.USART_Parity = USART_Parity_No;
//    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//    
//    USART_Init(USART6,&USART_InitStructure);   
//    USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);
//    
//    USART_Cmd(USART6,ENABLE);
	
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOG,ENABLE); //ʹ��GPIOGʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//ʹ��USART6ʱ��

	//��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;							//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;						//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;							//����
  GPIO_Init(GPIOD, &GPIO_InitStructure);										//��ʼ��

	GPIO_ResetBits(GPIOD,GPIO_Pin_8);  //�������ͣ� 
	
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6); //GPIOG9����ΪUSART6
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6); //GPIOG14����ΪUSART6
	
	//USART6�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //GPIOC12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOG,&GPIO_InitStructure); //��ʼ��GPIOC12
	//USART6�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; //GPIOD2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOG,&GPIO_InitStructure); //��ʼ��GPIOD2

   //USART6 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART6, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART6, ENABLE);  //ʹ�ܴ���1
	
	//USART_ClearFlag(USART6, USART_FLAG_TC);
	
#if EN_USART6_RX
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart6 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif

}

void USART6_Send_Byte(u8 Data)
{
	while(!USART_GetFlagStatus(USART6,USART_FLAG_TXE));
	//USART_GetFlagStatus���õ�����״̬λ
	//USART_FLAG_TXE:���ͼĴ���Ϊ�� 1��Ϊ�գ�0��æ״̬
	USART_SendData(USART6,Data);
}

int usart6_flag = -1;

void USART6_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res2;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		USART_ClearITPendingBit(USART6,USART_IT_RXNE);	//����жϱ�־λ
		Res2=USART_ReceiveData(USART6);			//�����ַ���������
		USART6_Send_Byte(Res2);
		
		usart6_flag = Res2-'0';
  }
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
}
#endif	
 
