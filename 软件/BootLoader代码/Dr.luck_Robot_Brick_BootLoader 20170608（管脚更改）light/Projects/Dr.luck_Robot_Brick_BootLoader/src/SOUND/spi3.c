/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/spi3.c 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief  
  ******************************************************************************
**/
#define SPI_GLOBALS

#include "sys.h"
#include "spi3.h"

#define	MP3SPI			SPI3
#define DMA_Stream0_IT_MASK     (uint32_t)(DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 | \
                                           DMA_LISR_TEIF0 | DMA_LISR_HTIF0 | \
                                           DMA_LISR_TCIF0)
#define DMA_Stream1_IT_MASK     (uint32_t)(DMA_Stream0_IT_MASK << 6)
#define DMA_Stream5_IT_MASK     (uint32_t)(DMA_Stream1_IT_MASK | (uint32_t)0x20000000)

void SPI_Mp3_Init(void)
{	SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	/* PB15-MOSI2,PB13-SCK2*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 |GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* SPI2 configuration */
	SPI_Cmd(MP3SPI, DISABLE); 												//�����Ƚ���,���ܸı�MODE
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		//����ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;							//��
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;						//8λ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;								//CPOL=1 ʱ�����ո�
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;							//CPHA=1 ���ݲ����2��
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;								//���NSS
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;		//2��Ƶ
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;						//��λ��ǰ
    SPI_InitStructure.SPI_CRCPolynomial = 7;								//CRC7    
	SPI_Init(MP3SPI, &SPI_InitStructure);
	
    SPI_Cmd(MP3SPI, ENABLE); 
}
/***********************************************************************
* ���ƣ�main()
* ���ܣ�����������GUI���Լ���ʾ��
***********************************************************************/
//SPI_BaudRatePrescaler_2   2��Ƶ   
//SPI_BaudRatePrescaler_8   8��Ƶ   
//SPI_BaudRatePrescaler_16  16��Ƶ  
//SPI_BaudRatePrescaler_256 256��Ƶ 
void SPI_Mp3_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	SPI_Cmd(MP3SPI, DISABLE);
	MP3SPI->CR1&=0XFFC7;
	MP3SPI->CR1|=SPI_BaudRatePrescaler;	//����SPI2�ٶ� 
	SPI_Cmd(MP3SPI,ENABLE);
} 
void SPI_Mp3_SetDMA(u8 en)
{	SPI_Cmd(MP3SPI, DISABLE);
	if( en == ENABLE )
	{	MP3SPI->CR2 |= 0x0003;	}
	else
	{	MP3SPI->CR2 &= 0xfffc;	}
	SPI_Cmd(MP3SPI,ENABLE);
}
/***********************************************************************
* ���ƣ�main()
* ���ܣ�����������GUI���Լ���ʾ��
***********************************************************************/
u8 SPI_Mp3_Byte(u8 byte)
{
	/*�ȴ����ͼĴ�����*/
	while((MP3SPI->SR & SPI_I2S_FLAG_TXE)==RESET);
    /*����һ���ֽ�*/
	MP3SPI->DR = byte;
	/* �ȴ����ռĴ�����Ч*/
	while((MP3SPI->SR & SPI_I2S_FLAG_RXNE)==RESET);
	return(MP3SPI->DR);
}
void SPI_MP3_ReceiveSendByte( u16 num )
{
// 	DMA1_Stream7->CCR &= ~( 1 << 0 ) ;         //�ر�DMAͨ��3
// 	DMA1_Stream0->CCR &= ~( 1 << 0 ) ;         //�ر�DMAͨ��2

// 	DMA1_Stream0->CNDTR = num ;         //������������Ϊbuffersize��
// 	DMA1_Stream7->CNDTR = num ;         //������������Ϊbuffersize��

// 	DMA1->IFCR = 0xF000 ;                         //���ͨ��4�ı�־λ
// 	DMA1->IFCR = 0xF0000 ;                        //���ͨ��5�ı�־λ

// 	MP3SPI->DR ;								//����ǰ��һ��SPI1->DR����֤���ջ�����Ϊ��
// 	while( ( MP3SPI->SR & 0x02 ) == 0 );	
// 	DMA1_Stream7->CCR |= 1 << 0 ;              //����DMAͨ��3
// 	DMA1_Stream0->CCR |= 1 << 0 ;              //����DMAͨ��2	
}
/***********************************************************************
* ���ƣ�main()
* ���ܣ�����������GUI���Լ���ʾ��
***********************************************************************/
void SPI_MP3_DMA_Configuration( void )
{   
// 	RCC->AHBENR |= 1<<0 ;                     //DMA1ʱ��ʹ��
// 	/*------------------����SPI2_RX_DMAͨ��Channel4---------------------*/
//     DMA1_Stream0->CCR &= ~( 1<<14 ) ;        //�Ǵ洢�����洢��ģʽ
// 	DMA1_Stream0->CCR |=    2<<12   ;        //ͨ�����ȼ���
// 	DMA1_Stream0->CCR &= ~( 3<<10 ) ;        //�洢�����ݿ��8bit
// 	DMA1_Stream0->CCR &= ~( 3<<8  ) ;        //�������ݿ��8bit
// 	DMA1_Stream0->CCR |=    1<<7    ;        //�洢����ַ����ģʽ
// 	DMA1_Stream0->CCR &= ~( 1<<6  ) ;        //��ִ�������ַ����ģʽ
// 	DMA1_Stream0->CCR &= ~( 1<<5  ) ;        //ִ��ѭ������
// 	DMA1_Stream0->CCR &= ~( 1<<4  ) ;        //�������

// 	DMA1_Stream0->CNDTR &= 0x0000   ;        //���������Ĵ�������
// 	DMA1_Stream0->CNDTR = SPI_MP3_BUFNUM ;       //������������Ϊbuffersize��

// 	DMA1_Stream0->CPAR = (u32)(&(MP3SPI->DR));   //���������ַ��ע��PSIZE
// 	DMA1_Stream0->CMAR = (u32)SPI_Mp3_RxBuf ; //����DMA�洢����ַ��ע��MSIZE

// 	/*------------------����SPI2_TX_DMAͨ��Channel5---------------------*/
// 	DMA1_Stream7->CCR &= ~( 1<<14 ) ;        //�Ǵ洢�����洢��ģʽ
// 	DMA1_Stream7->CCR |=    0<<12   ;        //ͨ�����ȼ����
// 	DMA1_Stream7->CCR &= ~( 3<<10 ) ;        //�洢�����ݿ��8bit
// 	DMA1_Stream7->CCR &= ~( 3<<8 )  ;        //�������ݿ��8bit
// 	DMA1_Stream7->CCR |=    1<<7    ;        //�洢����ַ����ģʽ
// 	DMA1_Stream7->CCR &= ~( 1<<6 )  ;        //��ִ�������ַ����ģʽ
// 	DMA1_Stream7->CCR &= ~( 1<<5 ) ;         //��ִ��ѭ������
// 	DMA1_Stream7->CCR |=    1<<4    ;        //�Ӵ洢����

// 	DMA1_Stream7->CNDTR &= 0x0000   ;        //���������Ĵ�������
// 	DMA1_Stream7->CNDTR = SPI_MP3_BUFNUM ;       //������������Ϊbuffersize��
// 	
// 	DMA1_Stream7->CPAR = (u32)(&(MP3SPI->DR)) ;      //���������ַ��ע��PSIZE
// 	DMA1_Stream7->CMAR = (u32)SPI_Mp3_TxBuf ; //����DMA�洢����ַ��ע��MSIZE				 
}

//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ������SD Card/W25Q64/NRF24L01						  
//SPI�ڳ�ʼ��
//�������Ƕ�SPI2�ĳ�ʼ��

void SPI3_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

	RCC_AHB1PeriphClockCmd(	RCC_AHB1Periph_GPIOC, ENABLE );//PORTBʱ��ʹ�� 
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI3,  ENABLE );//SPI2ʱ��ʹ�� 	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  //PB13/14/15����������� 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;											//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;									//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC

 	GPIO_SetBits(GPIOC,GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12);  //PC10/11/12����
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SPI3); 							//PB3����Ϊ SPI1
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SPI3); 							//PB4����Ϊ SPI1
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SPI3);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI3, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPI3, ENABLE); //ʹ��SPI����
	
	SPI3_ReadWriteByte(0xff);//��������	
}   
//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ   
//SPI_BaudRatePrescaler_8   8��Ƶ   
//SPI_BaudRatePrescaler_16  16��Ƶ  
//SPI_BaudRatePrescaler_256 256��Ƶ 
  
void SPI3_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI3->CR1&=0XFFC7;
	SPI3->CR1|=SPI_BaudRatePrescaler;	//����SPI2�ٶ� 
	SPI_Cmd(SPI3,ENABLE);
} 

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI3_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI_I2S_SendData(SPI3, TxData); //ͨ������SPIx����һ������
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET)//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI_I2S_ReceiveData(SPI3); //����ͨ��SPIx������յ�����					    
}

void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx,u32 par,u32 mar,u16 ndtr)
{ 
 
	DMA_InitTypeDef  DMA_InitStructure;
	
	if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2ʱ��ʹ�� 
		
	}else 
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1ʱ��ʹ�� 
	}
  DMA_DeInit(DMA_Streamx);
	
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}//�ȴ�DMA������ 
	
  /* ���� DMA Stream */
  DMA_InitStructure.DMA_Channel = chx;  //ͨ��ѡ��
  DMA_InitStructure.DMA_PeripheralBaseAddr = par;//DMA�����ַ
  DMA_InitStructure.DMA_Memory0BaseAddr = mar;//DMA �洢��0��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//�洢��������ģʽ
  DMA_InitStructure.DMA_BufferSize = ndtr;//���ݴ����� 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݳ���:8λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//�洢�����ݳ���:8λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// ʹ����ͨģʽ 
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//�е����ȼ�
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
  DMA_Init(DMA_Streamx, &DMA_InitStructure);//��ʼ��DMA Stream

} 

//����һ��DMA����
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:���ݴ�����  
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
	
	DMA_Cmd(DMA_Streamx, DISABLE);                      //�ر�DMA���� 
	
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}	//ȷ��DMA���Ա�����  
	
	DMA_SetCurrDataCounter(DMA_Streamx,ndtr);          //���ݴ�����  

	DMA1->HIFCR = DMA_Stream5_IT_MASK;
		
	DMA_Cmd(DMA_Streamx, ENABLE);                      //����DMA���� 
		
	while (DMA_GetCmdStatus(DMA_Streamx) != ENABLE){}
}	

void MYDMA_Disable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
	
	DMA_Cmd(DMA_Streamx, DISABLE);                      //�ر�DMA���� 
	
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}	//ȷ��DMA���Ա�����  
	
//	DMA_SetCurrDataCounter(DMA_Streamx,ndtr);          //���ݴ�����  

	DMA1->HIFCR = DMA_Stream5_IT_MASK;
		
}

////SPI1��ʼ��
//void SPI1_Init(void)
//{	    
// 	GPIO_InitTypeDef GPIO_InitStructure;
//  	SPI_InitTypeDef  SPI_InitStructure;
//	 
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);								//ʹ��GPIOBʱ��
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);								//ʹ��SPI1ʱ��

//	//GPIOF9,F10��ʼ������
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;			//PB3~5���ù������	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;							//���ù���
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							//�������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;						//100MHz
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;							//����
//	GPIO_Init(GPIOB, &GPIO_InitStructure);

// 	GPIO_SetBits(GPIOB,GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5); //PB3.4.5����
//		
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1); 							//PB3����Ϊ SPI1
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1); 							//PB4����Ϊ SPI1
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1);
//	
//	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
//	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
//	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
//	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
//	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//���ݲ����ڵڶ���ʱ����
//	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
//	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ16
//	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
//	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
//	SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
// 
//	SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����
//	
//	SPI1_ReadWriteByte(0xff);//��������	
//}



//void SPI1_SetSpeed(u8 SpeedSet)
//{
//	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
//	SPI1->CR1&=0XFFC7; 
//	SPI1->CR1|=SpeedSet;
//	SPI_Cmd(SPI1,ENABLE); 
//}


////SPIx ��дһ���ֽ�
////TxData:Ҫд����ֽ�
////����ֵ:��ȡ�����ֽ�
//u8 SPI1_ReadWriteByte(u8 TxData)
//{				   			 
//	u8 retry=0;				 	
//	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
//	{
//		retry++;
//		if(retry>200)return 0;
//	}			  
//	SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ������
//	retry=0;

//	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
//	{
//		retry++;
//		if(retry>200)return 0;
//	}	  						    
//	return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����					    
//}





























