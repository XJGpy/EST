/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/iap.c 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief  
  ******************************************************************************
**/
#include "iap.h"
#include "stdio.h"
#include "sys.h"

#define FLASH_IAP_ADDR		(uint32_t)0x0800C000  	//iap������ʼ��ַ(�����FLASH)
											//����0X08000000~0X0800FFFF�Ŀռ�ΪIAPʹ��

typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.
iapfun jump2iap;


////����ջ����ַ
////addr:ջ����ַ
//__asm void MSR_MSP(u32 addr) 
//{
//    MSR MSP, r0 			//set Main Stack value
//    BX r14
//}


void iap_jump(u32 iapxaddr)
{
	if(((*(vu32*)iapxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.0x20000000��sram����ʼ��ַ,Ҳ�ǳ����ջ����ַ
	{ 		
		jump2iap=(iapfun)*(vu32*)(iapxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(vu32*)iapxaddr);					//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		jump2iap();									//��ת��APP.
	}
}


void iap_Func(u32 Update_Data_Length)
{
	FLASH_Unlock();	
	//�޸�Update_Data_Length����
	FLASH_EraseSector(FLASH_Sector_2, VoltageRange_3);
	Test_Write(Update_Data_Length_L16,Update_Data_Length%0x10000);
	Test_Write(Update_Data_Length_H16,Update_Data_Length/0x10000);
	//��APP_CONFIG_ADDR�ÿ�
	Test_Write(APP_CONFIG_ADDR,APP_CONFIG_CLEAR_VALUE) ;
	FLASH_Lock();	
	
//	printf("��ȡ����:%x\n\r",Update_Data_Length%0x10000);
//	printf("��ȡ����:%x\n\r",Update_Data_Length/0x10000);
//	printf("��ȡ����\n\r");
	
	NVIC_SystemReset();
//	iap_jump(FLASH_IAP_ADDR);//��ת��iap�ĸ�λ������ַ
}


 











