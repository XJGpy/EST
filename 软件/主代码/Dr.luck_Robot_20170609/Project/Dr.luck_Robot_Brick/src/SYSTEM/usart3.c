/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/usart3.c 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   
  ******************************************************************************
**/
#include "usart3.h"
#include "usart_sensor.h"

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

UBYTE Uart4RecBuf[UART_RECBUF_SIZE];     //���ջ���
u16 Uart4RecBufIn = 0 ;

//UBYTE USART3_TX_BUF[UART_TXBUF_SIZE];     //���ͻ���
//u16 USART3_TX_POINTER = 0 ;
//u16 USART3_TX_LEN = 0 ;

#if EN_USART3_RX   
//���ʹ���˽���
//����3�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���  

//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART3_RX_STA=0;       //����״̬���	

//��ʼ��IO ����3 
//bound:������
void usart3_init(u32 bound){
   //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOE,ENABLE); //ʹ��GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��
 
	//����3��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10����ΪUSART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11����ΪUSART3
	
	//USART3�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOB10��GPIOB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOB ,&GPIO_InitStructure); //��ʼ��PB10��PB11
	
	//RXIND_EN  E9
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;							//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;						//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;							//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);										//��ʼ��

	GPIO_ResetBits(GPIOE,GPIO_Pin_9);  //����ʹ��

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7  | GPIO_Pin_8| GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;							//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;						//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;							//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);										//��ʼ��

   //USART3 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART3, &USART_InitStructure); //��ʼ������3
						
  USART_Cmd(USART3, ENABLE);  //ʹ�ܴ���3 
	
	//USART_ClearFlag(USART3, USART_FLAG_TC);
	
#if EN_USART3_RX	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//����3�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif
	
  GPIO_SetBits(GPIOB,GPIO_Pin_10);  //����GPIOA6���ͣ� 
}

UBYTE USART3_Send_Byte(UBYTE Data)
{
  UBYTE   Result = 1;
   	while(!USART_GetFlagStatus(USART3,USART_FLAG_TXE));	  //USART_GetFlagStatus���õ�����״̬λ
														  //USART_FLAG_TXE:���ͼĴ���Ϊ�� 1��Ϊ�գ�0��æ״̬
	USART_SendData(USART3,Data);
	return(Result);
}

void usart3_change(u32 bound)
{
	//USART1 ��ʼ������
	USART_InitTypeDef USART_InitStructure;
	
	USART_Cmd(USART3, DISABLE);
	
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART3, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART3, ENABLE);  //ʹ�ܴ���1 
}

void USART3_IRQHandler(void)                	//����1�жϷ������
{
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);	//����жϱ�־λ
		Uart4RecBuf[Uart4RecBufIn] = USART_ReceiveData(USART3);			//�����ַ���������
//		printf("%c", Uart1RecBuf[Uart1RecBufIn]);
		
		if (++Uart4RecBufIn >= UART_RECBUF_SIZE)
      {
        Uart4RecBufIn  =  0;
      }
		UsartBreakTimer[3]  =  0;
  }
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
#endif	

 



