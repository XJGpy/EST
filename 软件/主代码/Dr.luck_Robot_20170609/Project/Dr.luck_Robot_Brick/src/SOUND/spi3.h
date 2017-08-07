/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/spi3.h 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief  
  ******************************************************************************
**/
#ifndef __SPI3_H
#define __SPI3_H

#ifdef  SPI_GLOBALS
#define SPI_EXT 
#else   
#define SPI_EXT    extern
#endif
 
#include "sys.h"

SPI_EXT void SPI3_Init(void);			 //��ʼ��SPI��
SPI_EXT void SPI3_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
SPI_EXT u8 SPI3_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�

//SPI_EXT void SPI1_Init(void);			 //��ʼ��SPI1��
//SPI_EXT void SPI1_SetSpeed(u8 SpeedSet); //����SPI1�ٶ�  		 
//SPI_EXT u8 SPI1_ReadWriteByte(u8 TxData);//SPI1���߶�дһ���ֽ�

#define	SPI_MP3_BUFNUM		32
SPI_EXT u8 SPI_Mp3_TxBuf[SPI_MP3_BUFNUM];

void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx,u32 par,u32 mar,u16 ndtr);//����DMAx_CHx
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr);	//ʹ��һ��DMA����		   

void MYDMA_Disable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr);
	
#endif
