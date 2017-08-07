/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/stm32fxxx_it.H
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32Fxxx_IT_H
#define __STM32Fxxx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "disassembly.h"
	 
/* Includes ------------------------------------------------------------------*/
#include "usb_conf.h"
#include "lego_include.h"

/* Exported types ------------------------------------------------------------*/
	 
/*****ϵͳʱ��*****/
extern uint32_t SystemTime,ProgramTime,SystemCountTime,SoundTime;
extern uint32_t GetSystemTimeMs(void);
extern uint32_t GetProgramTimeMs(void);

/*****ϵͳ�δ�ʱ���ж�*****/
extern UBYTE SysTick_1ms;		//ʱ��Ƭʱ��1ms
extern unsigned int SysTick_50us_my;
	 
///*****ϵͳʱ��*****/
//extern uint32_t SystemTime,ProgramTime;
//extern uint32_t GetSystemTimeMs(void);
//extern uint32_t GetProgramTimeMs(void);

///*****ϵͳ�δ�ʱ���ж�*****/
//extern UBYTE SysTick_1ms;		//ʱ��Ƭʱ��1ms
//extern UBYTE SysTick_2ms;		//ʱ��Ƭʱ��2ms
//extern UBYTE SysTick_10ms;	//ʱ��Ƭʱ��10ms
//extern UBYTE SysTick_50ms;	//ʱ��Ƭʱ��50ms
//extern UBYTE SysTick_100ms;	//ʱ��Ƭʱ��100ms
//extern UBYTE SysTick_1s;	//ʱ��Ƭʱ��1s

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32Fxxx_IT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
