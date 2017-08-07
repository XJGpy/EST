#include "wkup.h"
#include "delay.h"					
#include "delay.h"		
#include "stm32f429i_discovery.h"												   

 /**
  * @brief  ͣ�����Ѻ�����ϵͳʱ��: ʹ�� HSE, PLL
  *         ����ѡ��PLL��Ϊϵͳʱ��.
  * @param  None
  * @retval None
  */
static void SYSCLKConfig_STOP(void)
{
  /* After wake-up from STOP reconfigure the system clock */
  /* ʹ�� HSE */
  RCC_HSEConfig(RCC_HSE_ON);
  
  /* �ȴ� HSE ׼������ */
  while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  {
  }
  
  /* ʹ�� PLL */ 
  RCC_PLLCmd(ENABLE);
  
  /* �ȴ� PLL ׼������ */
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {
  }
  
  /* ѡ��PLL��Ϊϵͳʱ��Դ */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
  /* �ȴ�PLL��ѡ��Ϊϵͳʱ��Դ */
  while (RCC_GetSYSCLKSource() != 0x08)
  {
  }
}

//ϵͳ�������ģʽ
void Sys_Enter_Stopby(void)
{		
		RCC_ClocksTypeDef clock_status_wakeup,clock_status_config;
		uint8_t clock_source_wakeup,clock_source_config; 

  //ʹ�ú��ָʾ������ֹͣ״̬
//		LED_RED;
		STM_EVAL_LEDOn(LED3);
		STM_EVAL_LEDOff(LED4);
	
		/*����ֹͣģʽʱ��FLASH�������״̬*/
		PWR_FlashPowerDownCmd (ENABLE);
		/* ����ֹͣģʽ�����õ�ѹ������Ϊ�͹���ģʽ���ȴ��жϻ��� */
		PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);	
		
	
		//�ȴ��жϻ���  K1��K2�����ж�	
		
		//��ȡ�ձ�����ʱ��ʱ��״̬	
		//ʱ��Դ
		clock_source_wakeup = RCC_GetSYSCLKSource();
		//ʱ��Ƶ��
		RCC_GetClocksFreq(&clock_status_wakeup);
		
		//��ֹͣģʽ�±����Ѻ�ʹ�õ���HSIʱ�ӣ��˴�����HSEʱ��,ʹ��PLLCLK
		SYSCLKConfig_STOP();
		
		//��ȡ�������ú��ʱ��״̬	
		//ʱ��Դ
		clock_source_config = RCC_GetSYSCLKSource ();
		//ʱ��Ƶ��
		RCC_GetClocksFreq(&clock_status_config);
	
//		//��Ϊ�ջ��ѵ�ʱ��ʹ�õ���HSIʱ�ӣ���Ӱ�촮�ڲ����ʣ�������ԣ���������������ʱ��Դ���ʹ�ô��������
//		printf("\r\n�������ú��ʱ��״̬��\r\n");
//		printf(" SYSCLKƵ��:%d,\r\n HCLKƵ��:%d,\r\n PCLK1Ƶ��:%d,\r\n PCLK2Ƶ��:%d,\r\n ʱ��Դ:%d (0��ʾHSI��8��ʾPLLCLK)\n", 
//			clock_status_config.SYSCLK_Frequency, 
//			clock_status_config.HCLK_Frequency, 
//			clock_status_config.PCLK1_Frequency, 
//			clock_status_config.PCLK2_Frequency, 
//			clock_source_config);
//			
//		printf("\r\n�ջ��ѵ�ʱ��״̬��\r\n");	
//		printf(" SYSCLKƵ��:%d,\r\n HCLKƵ��:%d,\r\n PCLK1Ƶ��:%d,\r\n PCLK2Ƶ��:%d,\r\n ʱ��Դ:%d (0��ʾHSI��8��ʾPLLCLK)\n", 
//			clock_status_wakeup.SYSCLK_Frequency, 
//			clock_status_wakeup.HCLK_Frequency, 
//			clock_status_wakeup.PCLK1_Frequency, 
//			clock_status_wakeup.PCLK2_Frequency, 
//			clock_source_wakeup);
		
		/*ָʾ��*/
//		LED_BLUE;	
		STM_EVAL_LEDOff(LED3);
		STM_EVAL_LEDOn(LED4);

		EXTI->PR=1<<4;
		EXTI->IMR&=~(1<<4);
}

 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* ����NVICΪ���ȼ���1 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* �����ж�Դ������1 */
  NVIC_InitStructure.NVIC_IRQChannel = WKUP_INT_EXTI_IRQ;
  /* ������ռ���ȼ���1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* ���������ȼ���1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* ʹ���ж�ͨ�� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* �����ж�Դ������2������ʹ������������� */  
  NVIC_InitStructure.NVIC_IRQChannel = WKUP_INT_EXTI_IRQ;
  NVIC_Init(&NVIC_InitStructure);
}

//PA0 WKUP���ѳ�ʼ��
void WKUP_Init(void)
{	  
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
  
	/*��������GPIO�ڵ�ʱ��*/
	RCC_AHB1PeriphClockCmd(WKUP_INT_GPIO_CLK ,ENABLE);
  
  /* ʹ�� SYSCFG ʱ�� ��ʹ��GPIO�ⲿ�ж�ʱ����ʹ��SYSCFGʱ��*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* ���� NVIC */
  NVIC_Configuration();
  
	/* ѡ�񰴼�1������ */
  GPIO_InitStructure.GPIO_Pin = WKUP_INT_GPIO_PIN;
  /* ��������Ϊ����ģʽ */ 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	    		
  /* �������Ų�����Ҳ������ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  /* ʹ������Ľṹ���ʼ������ */
  GPIO_Init(WKUP_INT_GPIO_PORT, &GPIO_InitStructure); 

	/* ���� EXTI �ж�Դ ��key1���� */
  SYSCFG_EXTILineConfig(WKUP_INT_EXTI_PORTSOURCE,WKUP_INT_EXTI_PINSOURCE);

  /* ѡ�� EXTI �ж�Դ */
  EXTI_InitStructure.EXTI_Line = WKUP_INT_EXTI_LINE;
  /* �ж�ģʽ */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  /* �½��ش��� */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  /* ʹ���ж�/�¼��� */
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
}

