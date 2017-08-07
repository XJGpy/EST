#include "iap.h"		
#include "uc1638c.h"						
											
#define Update_Buff_Size 1024						//1K

//u16 iapbuf[1024] = {0}; //���ڻ������ݵ�����
//u16 receiveDataCur = 0;	//��ǰiapbuffer���Ѿ��������ݳ���,һ���������֮��д��flash������
//u32 addrCur = FLASH_APP1_ADDR;			//��ǰϵͳд���ַ,ÿ��д��֮���ַ����2048


void App_Update_Check(void)
{
	static uint32_t Update_Count_Adderss  = UPDATE_ADDRESS;
	static uint32_t App_Count_Adderss  = APPLICATION_ADDRESS;
	static u16 Update_Buff[Update_Buff_Size] = {0x0000};
	u16 Buff_Num = 0;
	u16 Buff_Remain = 0;	
	u16 Update_Size;
	u32 Update_data_length;
	u16 i;
	
	//����Ƿ��г�����¹̻���־
	if(STMFLASH_ReadHalfWord(Update_Data_Length_L16)!=0xffff || STMFLASH_ReadHalfWord(Update_Data_Length_H16)!=0xffff)
	{
		Update_data_length = STMFLASH_ReadHalfWord(Update_Data_Length_L16)+STMFLASH_ReadHalfWord(Update_Data_Length_H16)*0x10000;
		printf("���ڸ���...\n");	
		printf("���ȣ�%x\n\r",STMFLASH_ReadHalfWord(Update_Data_Length_L16));	
		printf("���ȣ�%x\n\r",STMFLASH_ReadHalfWord(Update_Data_Length_H16));	
		
		LCD_Display_pic(0, 0, 180, 128, UPDATE);
		LCD_refresh();
		
		FLASH_Unlock();			//����flash FLASH_If_GetSectorNumber
		FLASH_EraseSector(FLASH_If_GetSectorNumber(ADDR_FLASH_SECTOR_3),VoltageRange_3);
		FLASH_EraseSector(FLASH_If_GetSectorNumber(ADDR_FLASH_SECTOR_4),VoltageRange_3);
		FLASH_EraseSector(FLASH_If_GetSectorNumber(ADDR_FLASH_SECTOR_5),VoltageRange_3);
		FLASH_EraseSector(FLASH_If_GetSectorNumber(ADDR_FLASH_SECTOR_6),VoltageRange_3);
		FLASH_EraseSector(FLASH_If_GetSectorNumber(ADDR_FLASH_SECTOR_7),VoltageRange_3);
		
		Buff_Num = Update_data_length/Update_Buff_Size;
		Buff_Remain = Update_data_length%Update_Buff_Size;
		
		printf("Buff_Num:%x,Buff_Remain:%x\n",Buff_Num,Buff_Remain);
		
		for(i=0;i<=Buff_Num;i++)
		{
			if(Buff_Num != 0)
				Update_Size = Update_Buff_Size;
			else					//���һ��
				Update_Size = Buff_Remain;

			//��ȡUpdate������
			STMFLASH_Read(Update_Count_Adderss,Update_Buff,Update_Size);//������������������
			Update_Count_Adderss += Update_Size;
			
			printf("��ȡUpdate������\n");
		
			//������д��App�������д��ʱ�Ƿ���Ҫ������
			STMFLASH_Write_NoCheck(App_Count_Adderss,Update_Buff,Update_Size);
			App_Count_Adderss += Update_Size;
			
			if(i == Buff_Num/10 || i == 2*Buff_Num/10 || i == 3*Buff_Num/10 || i == 4*Buff_Num/10 || i == 5*Buff_Num/10 ||
				 i == 6*Buff_Num/10 || i == 7*Buff_Num/10 || i == 8*Buff_Num/10 || i == 9*Buff_Num/10 || i == 10*Buff_Num/10 )
			{
				fupd_prog(114,70,16,Buff_Num,i);	
			}
			printf("Buff_Num:%x,Buff_Remain:%x\n",Buff_Num,i);
			printf("������д��App��\n");
		}
		FLASH_Lock();//����
		printf("���д��\n");
	}
	else
	{
		printf("�������\n");
	}
	if(STMFLASH_ReadHalfWord(APP_CONFIG_ADDR)!=APP_CONFIG_SET_VALUE )
	{
		FLASH_Unlock();			//����flash
		//��Update_Data_Length����
		FLASH_EraseSector(FLASH_Sector_2, VoltageRange_3);
		FLASH_Lock();//����
		
		Test_Write(Update_Data_Length_L16,0xFFFF) ;
		Test_Write(Update_Data_Length_H16,0xFFFF) ;
		//��APP_CONFIG_ADDR��λ�������������������ת��App��
		Test_Write(APP_CONFIG_ADDR,APP_CONFIG_SET_VALUE) ;
	}
}

typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.
iapfun jump2app;

////����ջ����ַ
////addr:ջ����ַ
//__asm void MSR_MSP(u32 addr) 
//{
//    MSR MSP, r0 			//set Main Stack value
//    BX r14
//}


//��ת��Ӧ�ó����
//appxaddr:�û�������ʼ��ַ.
void iap_load_app(u32 appxaddr)
{
	printf("appxaddr:%x\n\r",appxaddr);
	printf("*appxaddr:%x\n\r",((*(vu32*)appxaddr)));
	printf("*appxaddr&0x2FFD0000:%x\n\r",((*(vu32*)appxaddr)&0x2FFD0000));
	if(((*(vu32*)appxaddr)&0x2FFD0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.0x20000000��sram����ʼ��ַ,Ҳ�ǳ����ջ����ַ //(0x2FFD0000)
	{
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(vu32*)appxaddr);					//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		jump2app();									//��ת��APP.
	}
	else
	{	
//		printf("program in flash is error\r\n");
	}
}

//��ת��app��������
void iap_jump_app_s(void)
{
	iap_load_app(APPLICATION_ADDRESS);//��ת��app�ĸ�λ������ַ
}



