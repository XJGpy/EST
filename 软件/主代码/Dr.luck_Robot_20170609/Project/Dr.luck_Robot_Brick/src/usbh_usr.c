/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file includes the user application layer
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "lcd_log.h"
#include "usbh_usr.h"
#include "usbh_hid_mouse.h"
#include "usbh_hid_keybd.h"
#include "delay.h" 
#include "usbd_cdc_vcp.h"



/** @addtogroup USBH_USER
* @{
*/

/** @addtogroup USBH_HID_DEMO_USER_CALLBACKS
* @{
*/

/** @defgroup USBH_USR
* @brief This file is the Header file for usbh_usr.c
* @{
*/ 


/** @defgroup USBH_CORE_Exported_TypesDefinitions
* @{
*/ 
#define KYBRD_FIRST_COLUMN               (uint16_t)319
#define KYBRD_LAST_COLUMN                (uint16_t)7
#define KYBRD_FIRST_LINE                 (uint8_t)120
#define KYBRD_LAST_LINE                  (uint8_t)200


/**
* @}
*/ 
uint8_t  KeybrdCharXpos           = 0;
uint16_t KeybrdCharYpos           = 0;
//extern  int16_t  x_loc, y_loc; 
//extern __IO int16_t  prev_x, prev_y;

//��ʾUSB����״̬
//0,û������;
//1,�Ѿ�����;
vu8 bDeviceState=0;		//Ĭ��û������  


//��ʾ�����˿�usb�߲���״̬
//0,û�в���;
//1,�Ѿ�����;
u8 HostComInsert = 0;



/** @addtogroup USER
* @{
*/

/** @defgroup USBH_USR 
* @brief    This file includes the user application layer
* @{
*/ 

/** @defgroup USBH_CORE_Exported_Types
* @{
*/ 



/** @defgroup USBH_USR_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Macros
* @{
*/ 
/**
* @}
*/ 

/** @defgroup USBH_USR_Private_Variables
* @{
*/
extern USB_OTG_CORE_HANDLE           USB_OTG_Core_dev;
/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_Callbacks =
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  NULL,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
};



/**
* @}
*/

/** @defgroup USBH_USR_Private_Constants
* @{
*/ 
/*--------------- LCD Messages ---------------*/
const uint8_t MSG_HOST_INIT[]          = "> Host Library Initialized\n";
const uint8_t MSG_DEV_ATTACHED[]       = "> Device Attached\n";
const uint8_t MSG_DEV_DISCONNECTED[]   = "> Device Disconnected\n";
const uint8_t MSG_DEV_ENUMERATED[]     = "> Enumeration completed\n";
const uint8_t MSG_DEV_HIGHSPEED[]      = "> High speed device detected\n";
const uint8_t MSG_DEV_FULLSPEED[]      = "> Full speed device detected\n";
const uint8_t MSG_DEV_LOWSPEED[]       = "> Low speed device detected\n";
const uint8_t MSG_DEV_ERROR[]          = "> Device fault \n";

const uint8_t MSG_MSC_CLASS[]          = "> Mass storage device connected\n";
const uint8_t MSG_HID_CLASS[]          = "> HID device connected\n";

const uint8_t USB_HID_MouseStatus[]    = "> HID Demo Device : Mouse\n";
const uint8_t USB_HID_KeybrdStatus[]   = "> HID Demo Device : Keyboard\n";
const uint8_t MSG_UNREC_ERROR[]        = "> UNRECOVERED ERROR STATE\n";
/**
* @}
*/



/** @defgroup USBH_USR_Private_FunctionPrototypes
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Functions
* @{
*/ 





/**
* @brief  USBH_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void)
{
  static uint8_t startup = 0;  
  
  if(startup == 0 )
  {
    startup = 1;
    /* Configure the LEDs */
//    STM_EVAL_LEDInit(LED1);
//    STM_EVAL_LEDInit(LED2);
//    STM_EVAL_LEDInit(LED3); 
//    STM_EVAL_LEDInit(LED4); 
    
    STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_GPIO);
    
//#if defined (USE_STM322xG_EVAL)
//  STM322xG_LCD_Init();
//#elif defined(USE_STM324xG_EVAL)
//  STM324xG_LCD_Init();
//#elif defined (USE_STM3210C_EVAL)
//  STM3210C_LCD_Init();
//#else
// #error "Missing define: Evaluation board (ie. USE_STM322xG_EVAL)"
//#endif

//    LCD_LOG_Init();

#ifdef USE_USB_OTG_HS 
//    LCD_LOG_SetHeader(" USB OTG HS HID Host"); 
#else
//    LCD_LOG_SetHeader(" USB OTG FS HID Host");
#endif
//    LCD_UsrLog("> USB Host library started.\n");  
//		UsbToPc_SendData('1');
//    LCD_LOG_SetFooter ("     USB Host Library v2.1.0" ); 
//		UsbToPc_SendData('2');
  }
}

/**
* @brief  USBH_USR_DeviceAttached 
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void)
{  
//	��⵽USB�豸����!
	printf("��⵽USB�豸����!\n");
	HostComInsert = 1;
}

/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
//�޷��ָ��Ĵ���!! 
void USBH_USR_UnrecoveredError (void)
{
	printf("�޷��ָ��Ĵ���!! \n");
}

/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval None
*/
void USBH_USR_DeviceDisconnected (void)
{
//	USB�豸�γ�!
	bDeviceState=0;	//USB�豸�γ���
	HostComConnected = 0;
	HostComInsert = 0;
	printf("USB�豸�γ�! \n");
}

/**
* @brief  USBH_USR_ResetUSBDevice 
*         Reset USB Device
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void)
{
  /* Users can do their application actions here for the USB-Reset */
//	��λ�豸
	printf("��λ�豸 \n");
}


/**
* @brief  USBH_USR_DeviceSpeedDetected 
*         Displays the message on LCD for device speed
* @param  Devicespeed : Device Speed
* @retval None
*/
//��⵽�ӻ��ٶ�
//DeviceSpeed:�ӻ��ٶ�(0,1,2 / ����)
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
  if(DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED)
  {
//		����(HS)USB�豸!
		printf("����(HS)USB�豸!\n");
  }  
  else if(DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED)
  {
//		ȫ��(FS)USB�豸!
		printf("ȫ��(FS)USB�豸!\n");
  }
  else if(DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED)
  {
//		����(LS)USB�豸!
		printf("����(LS)USB�豸!\n");
  }
  else
  {
//		�豸����!
  }
	
}

/**
* @brief  USBH_USR_Device_DescAvailable 
*         Displays the message on LCD for device descriptor
* @param  DeviceDesc : device descriptor
* @retval None
*/
//��⵽�ӻ���������
//DeviceDesc:�豸������ָ��
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{
  uint8_t temp[50];    
  USBH_DevDesc_TypeDef *hs;
  hs = DeviceDesc;  
  
  sprintf((char *)temp , "VID : %04Xh\n" , (uint32_t)(*hs).idVendor); 
  
  sprintf((char *)temp , "PID : %04Xh\n" , (uint32_t)(*hs).idProduct); 

	printf("VID:%04Xh\n",(*hs).idVendor);
	printf("PID:%04Xh\n",(*hs).idProduct);
}

/**
* @brief  USBH_USR_DeviceAddressAssigned 
*         USB device is successfully assigned the Address 
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void)
{
//  �ӻ���ַ����ɹ�!
	printf("�ӻ���ַ����ɹ�!\n");
}


/**
* @brief  USBH_USR_Conf_Desc 
*         Displays the message on LCD for configuration descriptor
* @param  ConfDesc : Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
  USBH_InterfaceDesc_TypeDef *id;
  
  id = itfDesc;  
  
  if((*id).bInterfaceClass  == 0x08)
  {
//		���ƶ��洢���豸!
			printf("���ƶ��洢���豸!\n");
  }
  else if((*id).bInterfaceClass  == 0x03)
  {
//		HID �豸!
			printf("HID �豸!\n");
  }
	else if((*id).bInterfaceClass==0x02)
	{
//		VCP �豸!
			printf("VCP �豸!\n");
	}
	
}

/**
* @brief  USBH_USR_Manufacturer_String 
*         Displays the message on LCD for Manufacturer String 
* @param  ManufacturerString : Manufacturer String of Device
* @retval None
*/
//��ȡ���豸Manufacturer String
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
  char temp[100];
  sprintf(temp, "Manufacturer : %s\n", (char *)ManufacturerString);
	
	printf("�豸Manufacturer String��%s\n",(char *)ManufacturerString);
  
}

/**
* @brief  USBH_USR_Product_String 
*         Displays the message on LCD for Product String
* @param  ProductString : Product String of Device
* @retval None
*/
//��ȡ���豸Product String 
void USBH_USR_Product_String(void *ProductString)
{
  char temp[100];
  sprintf((char *)temp, "Product : %s\n", (char *)ProductString);  
	
	printf("�豸Product String��%s\n",(char *)ProductString);
  
}

/**
* @brief  USBH_USR_SerialNum_String 
*         Displays the message on LCD for SerialNum_String 
* @param  SerialNumString : SerialNum_String of device
* @retval None
*/
//��ȡ���豸SerialNum String 
void USBH_USR_SerialNum_String(void *SerialNumString)
{
  uint8_t temp[100];
  sprintf((char *)temp, "Serial Number : %s\n", (char *)SerialNumString);    
	
	printf("�豸SerialNum String��%s\n",(char *)SerialNumString);
} 

/**
* @brief  EnumerationDone 
*         User response request is displayed to ask for
*         application jump to class
* @param  None
* @retval None
*/
//�豸USBö�����
void USBH_USR_EnumerationDone(void)
{
  /* Enumeration complete */ 

	printf("�豸USBö�����\n");
} 

/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
//�޷�ʶ���USB�豸
void USBH_USR_DeviceNotSupported(void)
{    
	printf("�޷�ʶ���USB�豸\n");
}  


/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
//�ȴ��û����밴��,ִ����һ������
USBH_USR_Status USBH_USR_UserInput(void)
{
//  �����û�ȷ�ϲ���!
  USBH_USR_Status usbh_usr_status;
  usbh_usr_status = USBH_USR_NO_RESP;  
  
	bDeviceState=1;

  usbh_usr_status = USBH_USR_RESP_OK;
  return usbh_usr_status;
  
} 

/**
* @brief  USBH_USR_OverCurrentDetected
*         Device Overcurrent detection event
* @param  None
* @retval None
*/
//USB�ӿڵ�������
void USBH_USR_OverCurrentDetected (void)
{
//   LCD_ErrLog ("Overcurrent detected.\n");
  
}

/**
* @brief  USR_MOUSE_Init
*         Init Mouse window
* @param  None
* @retval None
*/
void USR_MOUSE_Init	(void)
{
//	u8 buff[8] = "ccc";		//�����ӻ������ӳɹ��ź�
  printf("USR_MOUSE_Init\n");
	HostComConnected = 1;
}

/**
* @brief  USR_MOUSE_ProcessData
*         Process Mouse data
* @param  data : Mouse data to be displayed
* @retval None
*/
void USR_MOUSE_ProcessData(HID_MOUSE_Data_TypeDef *data)
{
  
  uint8_t idx = 1;   
  static uint8_t b_state[3] = { 0, 0 , 0};
  
  if ((data->x != 0) && (data->y != 0))
  {
//    HID_MOUSE_UpdatePosition(data->x , data->y);
  }
  
  for ( idx = 0 ; idx < 3 ; idx ++)
  {
    
    if(data->button & 1 << idx) 
    {
      if(b_state[idx] == 0)
      {
//        HID_MOUSE_ButtonPressed (idx);
        b_state[idx] = 1;
      }
    }
    else
    {
      if(b_state[idx] == 1)
      {
//        HID_MOUSE_ButtonReleased (idx);
        b_state[idx] = 0;
      }
    }
  }
  
//  UsbToPc_SendData('j');
}

/**
* @brief  USR_KEYBRD_Init
*         Init Keyboard window
* @param  None
* @retval None
*/
void  USR_KEYBRD_Init (void)
{

//  LCD_UsrLog((void*)USB_HID_KeybrdStatus); 
//  LCD_UsrLog("> Use Keyboard to tape characters: \n\n");   
//  LCD_UsrLog("\n\n\n\n\n\n");
//  LCD_DisplayStringLine( LCD_PIXEL_HEIGHT - 42, "                                   ");
//  LCD_DisplayStringLine( LCD_PIXEL_HEIGHT - 30, "                                   ");  
//    
//  LCD_SetTextColor(Green);

  
  KeybrdCharXpos = KYBRD_FIRST_LINE;
  KeybrdCharYpos = KYBRD_FIRST_COLUMN;
	
	printf("USR_KEYBRD_Init\n");
}


/**
* @brief  USR_KEYBRD_ProcessData
*         Process Keyboard data
* @param  data : Keyboard data to be displayed
* @retval None
*/
void  USR_KEYBRD_ProcessData (uint8_t data)
{
  if(data == '\n')
  {
    KeybrdCharYpos = KYBRD_FIRST_COLUMN;
    
    /*Increment char X position*/
    KeybrdCharXpos+=SMALL_FONT_LINE_WIDTH;
    
  }
  else if (data == '\r')
  {
    /* Manage deletion of charactter and upadte cursor location*/
    if( KeybrdCharYpos == KYBRD_FIRST_COLUMN) 
    {
      /*First character of first line to be deleted*/
      if(KeybrdCharXpos == KYBRD_FIRST_LINE)
      {  
        KeybrdCharYpos =KYBRD_FIRST_COLUMN; 
      }
      else
      {
        KeybrdCharXpos-=SMALL_FONT_LINE_WIDTH;
        KeybrdCharYpos =(KYBRD_LAST_COLUMN+SMALL_FONT_COLUMN_WIDTH); 
      }
    }
    else
    {
      KeybrdCharYpos +=SMALL_FONT_COLUMN_WIDTH;                  
      
    } 
//    LCD_DisplayChar(KeybrdCharXpos,KeybrdCharYpos, ' '); 
  }
  else
  {
//    LCD_DisplayChar(KeybrdCharXpos,KeybrdCharYpos, data);    
    /* Update the cursor position on LCD */
    
    /*Increment char Y position*/
    KeybrdCharYpos -=SMALL_FONT_COLUMN_WIDTH;
    
    /*Check if the Y position has reached the last column*/
    if(KeybrdCharYpos == KYBRD_LAST_COLUMN)
    {
      KeybrdCharYpos = KYBRD_FIRST_COLUMN;
      
      /*Increment char X position*/
      KeybrdCharXpos+=SMALL_FONT_LINE_WIDTH;
      
    }
  }
	
//	UsbToPc_SendData('l');
}

/**
* @brief  USBH_USR_DeInit
*         Deint User state and associated variables
* @param  None
* @retval None
*/
//���³�ʼ��
void USBH_USR_DeInit(void)
{
//	���³�ʼ��!!!
	printf("���³�ʼ��\n");
}

/**
* @}
*/ 
//////////////////////////////////////////////////////////////////////////////////////////
//������������,ΪALIENTEK���,�Է�ֹUSB����

//USBö��״̬�������,��ֹUSBö��ʧ�ܵ��µ�����
//phost:USB_HOST�ṹ��ָ��
//����ֵ:0,û������
//       1,������,�ⲿ������������USB����.
u8 USBH_Check_EnumeDead(USBH_HOST *phost)
{
	static u16 errcnt=0;
	//���״̬,�����������,��˵��USB������.
	if(phost->gState==HOST_CTRL_XFER&&(phost->EnumState==ENUM_IDLE||phost->EnumState==ENUM_GET_FULL_DEV_DESC))
	{
		errcnt++;
		if(errcnt>2000)//������
		{ 
			errcnt=0;
			RCC_AHB2PeriphClockCmd(RCC_AHB1Periph_OTG_HS,ENABLE);//USB OTG HS ��λ
			delay_ms(5);
			RCC_AHB2PeriphClockCmd(RCC_AHB1Periph_OTG_HS,DISABLE);	//��λ����  
			return 1;
		} 
	}else errcnt=0;
	return 0;
} 
//USB HIDͨ���������,��ֹUSBͨ������(��ʱ�����:DTERR,��Data toggle error)
//pcore:USB_OTG_Core_dev_HANDLE�ṹ��ָ��
//phidm:HID_Machine_TypeDef�ṹ��ָ�� 
//����ֵ:0,û������
//       1,������,�ⲿ������������USB����.
u8 USBH_Check_HIDCommDead(USB_OTG_CORE_HANDLE *pcore,HID_Machine_TypeDef *phidm)
{
 	if(pcore->host.HC_Status[phidm->hc_num_in]==HC_DATATGLERR)//��⵽DTERR����,ֱ������USB.
	{  
		return 1;
	}
	return 0;
}


/**
* @}
*/ 

/**
* @}
*/

/**
* @}
*/
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

