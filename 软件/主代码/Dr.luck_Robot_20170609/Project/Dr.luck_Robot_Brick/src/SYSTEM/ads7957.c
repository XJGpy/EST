#include "ads7957.h" 

float ADC_Channel_Vol[16];

void SPI_Init1(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;			//CS  CLK  MOSI  ACK
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_Init(GPIOC, &GPIO_InitStructure);

   GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 ;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;   //MISO
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
   GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	 CS_H;
	 SCLK_L; 
}

unsigned int Simulate_SPI_SendByte(unsigned int dt)  
{  
    u8 i;  
    unsigned int temp=0;  

		CS_L;
    for(i=16;i>0;i--)  
    {  
        //MOSI׼������λ,�Ӹ�λ����λ���
        if(dt&0x8000)	MOSI_H;  
        else MOSI_L;  
        dt<<=1;  
          
        //SCLK�β���������  
        SCLK_H;  
        __nop();  
        __nop();  
        __nop();  			
          
        //MISO��ȡ�յ�������λ�������1����ôtemp���λ��1������λ����
				temp<<=1;     
        if(MISO)temp++; 
        //SCLK�ָ�Ϊ�͵�ƽ��������һ��ʱ��
        SCLK_L;  
        __nop();  
        __nop();  
        __nop();  
			  
    }  
		CS_H;
    return temp;  
};


//------------------------���ܺ���----------------------------------

//��spi��ȡ��ADSоƬ���ݷ���Ž���ѹ������
void ADCChannel_To_Vol(float ADC_Channel_Vol[],unsigned int SPIReturnData)
{
	u8 channel;
	float voltage;
	
	channel = SPIReturnData/4096;
	voltage = (SPIReturnData%4096 * 5.0)/ 0x0fff;
	ADC_Channel_Vol[channel] = voltage;
}


