#include "sys.h"
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
#include "uc1638c.h"
#include "stdio.h"

//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif	  
 
u32 GBK_OVER_Flag=0;
u8 GBK_BUF_Flag =2;
u8 Usart6_Rece_Buf0[Usart6_DMA_Len+1];
u8 Usart6_Rece_Buf1[Usart6_DMA_Len+1];
u16 Usart6_Rec_Len=0;
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
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE); //ʹ��GPIOC��GPIODʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//ʹ��USART6ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6); //GPIOC12����ΪUSART6
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6); //GPIOD2����ΪUSART6
	
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

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART6, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART6, ENABLE);  //ʹ�ܴ���1 
	
	USART_ClearFlag(USART6, USART_FLAG_TC);
	
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//��������ж�
  USART_DMACmd(USART6,USART_DMAReq_Rx,ENABLE);

	//USART6 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

	//DMA����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2 ʱ��ʹ��
	DMA_DeInit(DMA2_Stream1);    //�ָ�Ĭ��ֵ ����1������DMA2������2ͨ��4
	while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//�ȴ� DMA ������
	/* ���� DMA Stream */
	DMA_InitStructure.DMA_Channel = DMA_Channel_5; //ͨ��ѡ��
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART6->DR;//DMA �����ַ
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)Usart6_Rece_Buf0;//DMA �洢�� 0 ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//���赽�洢��ģʽ
	DMA_InitStructure.DMA_BufferSize = Usart6_DMA_Len;//���ݴ�����
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݳ���:8 λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//�洢�����ݳ���:8 λ	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//ע�⣺��������Ϊѭ��ģʽ����Ȼ���������ڶ��δ���
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//�е����ȼ�
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;//FIFO ģʽ��ֹ
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;//FIFO ��ֵ
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
	DMA_DoubleBufferModeConfig(DMA2_Stream1, (uint32_t)Usart6_Rece_Buf1, DMA_Memory_0);  //Usart6_Rece_Buf0 �Ȼ���
  DMA_DoubleBufferModeCmd(DMA2_Stream1, ENABLE);	
		
	DMA_Init(DMA2_Stream1, &DMA_InitStructure);//��ʼ�� DMA Stream	
	DMA_Cmd(DMA2_Stream1, ENABLE); //���� DMA ����
	
	DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,ENABLE);	//ʹ��DMA��������ж�
		
	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;//DMA2_Stream1_IRQn�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����	
}

//����һ�� DMA ����
//DMA_Streamx:DMA ������,DMA2_Stream1~7/DMA2_Stream0~7
//ndtr:���ݴ�����
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
	DMA_Cmd(DMA_Streamx, DISABLE); //�ر� DMA ����
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){} //ȷ�� DMA ���Ա�����
	DMA_SetCurrDataCounter(DMA_Streamx,ndtr); //���ݴ�����
	DMA_Cmd(DMA_Streamx, ENABLE); //���� DMA ����
}


void USART6_IRQHandler(void)                	//����1�жϷ������
{
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
   USART_ReceiveData(USART6);//(USART1->DR);	//��ȡ���յ������� 		
  } 
  }
void DMA2_Stream1_IRQHandler(void)                	//����1�жϷ������
{

 if(DMA_GetFlagStatus(DMA2_Stream1,DMA_FLAG_TCIF1)==SET) 
	{
    DMA_ClearFlag(DMA2_Stream1,DMA_FLAG_TCIF1);	  
		
		//**********************����֡����******************//
		if(1==DMA_GetCurrentMemoryTarget(DMA2_Stream1))
			GBK_BUF_Flag=0;
		else
			GBK_BUF_Flag=1;
		//**************************************************//

  		 
  } 
} 
//void USART6_Send_Byte(u16 Data)
//{
//	while(!USART_GetFlagStatus(USART6,USART_FLAG_TXE));
//	//USART_GetFlagStatus���õ�����״̬λ
//	//USART_FLAG_TXE:���ͼĴ���Ϊ�� 1��Ϊ�գ�0��æ״̬
//	USART_SendData(USART6,Data);
//}
 
