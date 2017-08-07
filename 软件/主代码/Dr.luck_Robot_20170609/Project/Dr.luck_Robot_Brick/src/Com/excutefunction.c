/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/excutefunction.c 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   
  ******************************************************************************
**/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "math.h"
#include "outputworkmode.h"
#include "stm32fxxx_it.h"
#include "input.h"
#include "rng.h"
#include "usart6.h"
#include "disassembly.h"
#include "excutefunction.h"
#include "cascade.h"

_ARMABI_FPEXCEPT float roundf(float /*x*/);

//void BriDisplay(uint8_t mode, uint32_t bufferrambase);
//void BriSound(uint8_t mode, uint32_t bufferrambase);
//void BriButton(uint8_t mode, uint32_t bufferrambase);
//void BriLight(uint8_t mode, uint32_t bufferrambase);
//void BriBt(uint8_t mode, uint32_t bufferrambase);
//void BriMessage(uint8_t mode, uint32_t bufferrambase);
//void BriFile(uint8_t mode, uint32_t bufferrambase);
//void BriWaveform(uint8_t mode, uint32_t bufferrambase);
//void BriRawSensor(uint8_t mode, uint32_t bufferrambase);
//void BriTimerSensor(uint8_t mode, uint32_t bufferrambase);		

#define   DegToRad(X)           (X * 0.0174532925f)
#define   RadToDeg(X)           (X * 57.2957795f)

/*!
 *  \param  (uint8_t)			par1   - �����ַ24~31λ
 *  \param  (uint8_t)			par2   - �����ַ16~23λ
 *  \param  (uint8_t)			par3   - �����ַ8~15λ
 *  \param  (uint8_t)			par4   - �����ַ0~7λ
 *	\return (uint32_t)		pc��ַ
 *	\brief  ���ȸߺ�͵��ĸ�Byte,��ȡ�����ַ
 */
 uint32_t Getpc(uint8_t par1, uint8_t par2, uint8_t par3, uint8_t par4)
{
	return (((uint32_t)par1<<24) + ((uint32_t)par2<<16) + ((uint32_t)par3<<8) + (((uint32_t)par4)));
}

/*!
 *  \param  (uint8_t)			par1   - ���ݵ�ַ24~31λ
 *  \param  (uint8_t)			par2   - ���ݵ�ַ16~23λ
 *  \param  (uint8_t)			par3   - ���ݵ�ַ8~15λ
 *  \param  (uint8_t)			par4   - ���ݵ�ַ0~7λ
 *	\return (uint32_t)		ram��ַ
 *	\brief	���ȸߺ�͵��ĸ�Byte,��ȡ���ݵ�ַ
 */
uint32_t GetRamBase(uint8_t par1, uint8_t par2, uint8_t par3, uint8_t par4)
{
	return (((uint32_t)par1<<24) + ((uint32_t)par2<<16) + ((uint32_t)par3<<8) + (((uint32_t)par4)));
}

/*!
 *  \param  (uint32_t)	rambase		-ram��ַ
 *	\return (uint8_t)		Byte����
 *	\brief	��ȡһ��Byte
 */
uint8_t GetByte(uint32_t rambase)
{
	return RAM_Buff[rambase];
}

/*! 
 *  \param  (uint32_t)	rambase - ram��ַ
 *	\return (uint32_t)  32λfloat����
 *	\brief	���ȵͺ�ߵ��ĸ�Byte,��ȡfloat
 */
float GetFloat(uint32_t rambase)
{
	float temp;
	uint8_t *p,i;
	p=(uint8_t *)(&temp);
	
	for(i=0; i<4; i++)
	{
		*(p+i) = RAM_Buff[rambase+i];
	}
	
	return temp;
}

/*! 
 *  \param  (uint32_t)    rambase		ram��ַ
 *  \param  (uint8_t)     par     	Byte����
 *  \return	
 *	\brief	д��Byte
 */
void SetByte(uint32_t rambase, uint8_t par)
{
	RAM_Buff[rambase] = par;
}

/*! 
 *  \param  (uint32_t)    rambase		ram��ַ
 *  \param  (float)    		par     	float����
 *  \return	
 *	\brief	���ȵͺ��д���ĸ�Byte, float��
 */
void SetFloat(uint32_t rambase, float par)
{
	uint8_t *p,i;
	p=(uint8_t *)(&par);
	
	for (i=0; i<4; i++) 
	{
		RAM_Buff[rambase+i] = *(p+i);
	}
}

/*! 
 *  \param  (float)			temp		float����
 *  \param  (uint8_t)		byte[]  Byte����
 *  \return	
 *	\brief	float����תΪByte����
 */
void FloatToByte4(float temp, uint8_t byte[],uint8_t len)
{
	uint8_t *p,i;
	p=(uint8_t *)(&temp);
	
	for (i=0; i<4; i++)
	{
		byte[3+len+i] = *(p+i);
	}
}

/*! 
 *  \param  (uint8_t)			byte[] 		Byte����
 *  \return	(float)				temp     	float����
 *	\brief	Byte����תΪfloat����
 */
float Byte4ToFloat(uint8_t byte[], uint8_t len)
{
	float temp;
	uint8_t *p,i;
	p=(uint8_t *)(&temp);
	
	for(i=0; i<4; i++)
	{
		*(p+i) = byte[3+len+i];
	}
	return temp;
}

/*! 
 *  \param  
 *  \return	
 *	\brief	�����κ���
 */
void OpNop(void)
{
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*! 
 *  \return		(uint32_t)		value��b��	���ݵ�ַ
 *  \param		(uint8_t)			ConstByte		Byte�ͳ���
 *	\brief	��Byte�ͳ���д��value��b����ַ��
 */
void OpMov_C_Byte(void)
{
	uint8_t i,Par[5];
	for(i=0; i<5; i++)
	{
		Par[i]	=	Flash_Buff[pc+1+i];
	}
	
	RAM_Buff[GetRamBase(Par[0],Par[1],Par[2],Par[3])] = Par[4];
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*! 
 *  \return		(uint32_t)		value��f��		���ݵ�ַ
 *  \param		(uint8_t)			ConstSingle		float�ͳ���
 *	\brief	��float�ͳ���д��value��f����ַ��
 */
void OpMov_C_Floart(void)
{
	uint8_t i,j,Par[8];
	for(i=0; i<8; i++)
	{
		Par[i]	=	Flash_Buff[pc+1+i];
	}

	for(j=0; j<4; j++)
	{
		RAM_Buff[GetRamBase(Par[0],Par[1],Par[2],Par[3])+j]	=	Par[4+j];
	}
	
//	printf("buffer:%x\r\n",GetRamBase(Par[0],Par[1],Par[2],Par[3]));
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*! 
 *  \return		(uint32_t)			value2��b��		���ݵ�ַ
 *  \param		(uint32_t)			value1��b��		���ݵ�ַ
 *	\brief	��value1��b����ַ����д��value2��b����ַ��
 */
void OpMov_R_Byte(void)
{
	uint8_t i,Par[8];
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	RAM_Buff[GetRamBase(Par[0],Par[1],Par[2],Par[3])]	 = RAM_Buff[GetRamBase(Par[4],Par[5],Par[6],Par[7])];
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*! 
 *  \return		(uint32_t)			value2��f��		���ݵ�ַ
 *  \param		(uint32_t)			value1��f��		���ݵ�ַ
 *	\brief	��value1��f����ַ����д��value2��f����ַ��
 */
void OpMov_R_Floart(void)
{
	uint8_t i,j,Par[8];
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	for(j=0; j<4; j++)
	{
		RAM_Buff[GetRamBase(Par[0],Par[1],Par[2],Par[3])+j]	 = RAM_Buff[GetRamBase(Par[4],Par[5],Par[6],Par[7])+j];
	}
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return	(uint32_t)    value(4b)		���ݵ�ַ
 *  \param  (uint32_t)    pc(4b)			�����ַ
 *  \param  (uint8_t)     LEN			���ȵͰ�λ
 *  \param  (uint8_t)     LEN			���ȸ߰�λ
 *	\brief	From RAM copy to ROM
 */
void OpMov_C_To_R(void)
{
	uint8_t i,Par[10];
	uint16_t j;
	for(i=0; i<10; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	for(j=0; j<(((uint16_t)Par[9]<<8) + ((uint16_t)Par[8])); j++)
	{
		RAM_Buff[GetRamBase(Par[0],Par[1],Par[2],Par[3])+j]	= Flash_Buff[GetRamBase(Par[4],Par[5],Par[6],Par[7])+j];
	}
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return	(uint32_t)    value2(4b)		���ݵ�ַ
 *  \param  (uint32_t)    value1(4b)		���ݵ�ַ
 *  \param  (uint8_t)     LEN				���ȵͰ�λ
 *  \param  (uint8_t)     LEN				���ȸ߰�λ
 *	\brief	From ROM copy to ROM
 */
void OpMov_R_To_R(void)
{
	uint8_t i,Par[10];
	uint16_t j;
	for(i=0; i<10; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	for(j=0; j<(((uint16_t)Par[9]<<8) + ((uint16_t)Par[8])); j++)
	{
		RAM_Buff[GetRamBase(Par[0],Par[1],Par[2],Par[3])+j]	 = RAM_Buff[GetRamBase(Par[4],Par[5],Par[6],Par[7])+j];
	}
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return	(uint8_t)    value2(b)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *	\brief	Change Float To Byte
 */
void OpChange_Float_To_Byte(void)
{
	uint8_t i,Par[8];
	float temp;
	
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	temp = GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7]));
	RAM_Buff[GetRamBase(Par[0],Par[1],Par[2],Par[3])] = (uint8_t) temp;
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return	(uint32_t)   value2(f)		���ݵ�ַ
 *  \param  (uint8_t)    value1(b)		���ݵ�ַ
 *	\brief	Change Byte To Float
 */
void OpChange_Byte_To_Float(void)
{
	uint8_t i,Par[8];
	uint8_t temp;
	
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	temp = GetByte(GetRamBase(Par[4],Par[5],Par[6],Par[7]));
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]),(float) temp);
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return	(uint32_t)   value3(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *  \param  (uint32_t)   value2(f)		���ݵ�ַ
 *	\brief	value3(f) = value1(f) + value2(f)
 */
void OpAdd(void)
{
	uint8_t i,Par[12];
	float temp;
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	temp = GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7]));
	temp += GetFloat(GetRamBase(Par[8],Par[9],Par[10],Par[11]));
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return	(uint32_t)   value3(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *  \param  (uint32_t)   value2(f)		���ݵ�ַ
 *	\brief	value3(f) = value1(f) - value2(f)
 */
void OpSub(void)
{
	uint8_t i,Par[12];
	float temp;
//	printf("%a ��float��\n");
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	temp = GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7]));
	temp -= GetFloat(GetRamBase(Par[8],Par[9],Par[10],Par[11]));
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return	(uint32_t)   value3(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *  \param  (uint32_t)   value2(f)		���ݵ�ַ
 *	\brief	value3(f) = value1(f) x value2(f)
 */
void OpMUL(void)
{
		uint8_t i,Par[12];
	float temp;
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	temp = GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7]));
	temp *= GetFloat(GetRamBase(Par[8],Par[9],Par[10],Par[11]));
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return	(uint32_t)   value3(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *  \param  (uint32_t)   value2(f)		���ݵ�ַ
 *	\brief	value3(f) = value1(f) / value2(f)
 */
void OpDiv(void)
{
	uint8_t i,Par[12];
	float temp;
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	temp = GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7]));
	temp /= GetFloat(GetRamBase(Par[8],Par[9],Par[10],Par[11]));
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return	(uint32_t)   value2(f)		���ݵ�ַ
 *  \param	(uint32_t)   value1(f)		���ݵ�ַ
 *	\brief	value2(f) = expf��value1(f)��
 */
void Exp(void)
{
	uint8_t i,Par[8];
	float temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	temp = expf(GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])));
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
}

/*!
 *  \return	(uint32_t)   value3(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *  \param  (uint32_t)   value2(f)		���ݵ�ַ
 *	\brief	value3(f) = value1(f) \ value2(f)��10\3 = 1��
 */
void Mod(void)
{
	uint8_t i,Par[12];
	float temp;
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	temp = fmod(GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])),
								GetFloat(GetRamBase(Par[8],Par[9],Par[10],Par[11])));
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
}

/*!
 *  \return	(uint32_t)   value2(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *	\brief	value2(f) = floor��value1(f)��
 */
void Rej(void)
{
	uint8_t i,Par[8];
	float temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	temp = floorf(GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])));
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);

}

/*!
 *  \return	(uint32_t)   value2(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *	\brief	value2(f) = ceil��value1(f)��
 */
void Inj(void)
{
	uint8_t i,Par[8];
	float temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	temp = ceilf(GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])));
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
}

/*!
 *  \return	(uint32_t)   value2(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *	\brief	value2(f) = roundf��value1(f)��
 */
void Appr(void)
{
	uint8_t i,Par[8];
	float temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	temp = roundf(GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])));
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
}

/*!
 *  \return	(uint32_t)   value2(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *	\brief	value2(f) = fabs��value1(f)��
 */
void Abs(void)
{
	uint8_t i,Par[8];
	float temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	temp = fabs(GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])));
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
}

/*!
 *  \return	(uint32_t)   value2(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *	\brief	value2(f) = -��value1(f)��
 */
void Negate(void)
{
	uint8_t i,Par[8];
	float temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	temp = 0 - (GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])));
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
}

/*!
 *  \return	(uint32_t)   value2(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *	\brief	value2(f) = ��value1(f)
 */
void Sqrt(void)
{
	uint8_t i,Par[8];
	float temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	temp = GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7]));
	if(temp > 0)
	{
		temp = sqrtf(temp);
		SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	}

}

/*!
 *  \return	(uint32_t)   value2(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *	\brief	value2(f) = log10(value1(f))
 */
void Log(void)
{
	uint8_t i,Par[8];
	float temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	temp = log10(GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])));
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
}

/*!
 *  \return	(uint32_t)   value2(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *	\brief	value2(f) = log(value1(f))
 */
void Ln(void)
{
	uint8_t i,Par[8];
	float temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	temp = log(GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])));
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
}

/*!
 *  \return	(uint32_t)   value2(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *	\brief	value2(f) = sinf(value1(f))
 */
void Sin(void)
{
	uint8_t i,Par[8];
	float temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	temp = DegToRad(GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])));
	temp = sinf(temp);
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
}

/*!
 *  \return	(uint32_t)   value2(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *	\brief	value2(f) = cosf(value1(f))
 */
void Cos(void)
{
	uint8_t i,Par[8];
	float temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	temp = DegToRad(GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])));
	temp = cosf(temp);
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
}

/*!
 *  \return	(uint32_t)   value2(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *	\brief	value2(f) = tanf(value1(f))
 */
void Tan(void)
{
	uint8_t i,Par[8];
	float temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	temp = DegToRad(GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])));
	temp = tanf(temp);
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
}

/*!
 *  \return	(uint32_t)   value2(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *	\brief	value2(f) = asinf(value1(f))
 */
void Asin(void)
{
	uint8_t i,Par[8];
	float temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	temp = asinf(GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])));
	temp = RadToDeg(temp);
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
}

/*!
 *  \return	(uint32_t)   value2(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *	\brief	value2(f) = acosf(value1(f))
 */
void Acos(void)
{
	uint8_t i,Par[8];
	float temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	temp = acosf(GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])));
	temp = RadToDeg(temp);
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
}

/*!
 *  \return	(uint32_t)   value2(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *	\brief	value2(f) = atanf(value1(f))
 */
void Atan(void)
{
	uint8_t i,Par[8];
	float temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	temp = atanf(GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])));
	temp = RadToDeg(temp);
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
}

/*!
 *  \return	(uint32_t)   value3(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *  \param  (uint32_t)   value2(f)		���ݵ�ַ
 *	\brief	value3(f) = value1(f) ^ value2(f)
 */
void Pow(void)
{
	uint8_t i,Par[12];
	float temp;
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	temp = powf(GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])),
								GetFloat(GetRamBase(Par[8],Par[9],Par[10],Par[11])));
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
}

/*!
 *  \return	(uint32_t)   value3(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *  \param  (uint32_t)   value2(b)		���ݵ�ַ
 *	\brief	��ȡ��ЧС���� Value1(f)��������ֵ
 *												Value(b)������С����λ��
 */
void Trun(void)
{
  char s[20];
	uint8_t i,Par[9];
	int temp;
	double Value;
	for(i=0; i<9; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}

	temp = Par[8];
	if( 0 <= temp && temp<= 4)
	{
		temp = floorf(temp);
	}else if(temp < 0)
	{
		temp = 0;
	}else if(temp > 4)
	{
		temp = 4;
	}
	
	Value = GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7]));
	sprintf(s,"%.*lf",temp,Value);
  sscanf(s,"%lf",&Value);
	SetFloat(GetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3])),(float) Value);
	
}

/*!
 *  \return	(uint32_t)   value3(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *  \param  (uint32_t)   value2(f)		���ݵ�ַ
 *	\brief	value3(f) = value1(f) / value2(f)
 */
void Inside(void)
{
	uint8_t i,Par[16];
	for(i=0; i<16; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	if( GetFloat(GetRamBase(Par[8],Par[9],Par[10],Par[11])) <= GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])) && 
				GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7]))<= GetFloat(GetRamBase(Par[12],Par[13],Par[14],Par[15])) )
	{
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x01);
	}
	else
	{
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x00);
	}
			
}

/*!
 *  \return	(uint32_t)   value3(f)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *  \param  (uint32_t)   value2(f)		���ݵ�ַ
 *	\brief	value3(f) = value1(f) / value2(f)
 */
void Outside(void)
{
	uint8_t i,Par[16];
	for(i=0; i<16; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	if( GetFloat(GetRamBase(Par[8],Par[9],Par[10],Par[11])) <= GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])) && 
				GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7]))<= GetFloat(GetRamBase(Par[12],Par[13],Par[14],Par[15])) )
	{
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x00);
	}
	else
	{
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x01);
	}

}

/*!
 *  \return
 *  \param
 *	\brief	���ָ߼������
 */
void OpMath(void)
{
	uint8_t cmd, pclength = 0;
	
	cmd = Flash_Buff[pc+1];

	switch(cmd)
	{
		case 0x01:Exp();pclength = 8;
			break;
		case 0x02:Mod();pclength = 12;
			break;
		case 0x03:Rej();pclength = 8;
			break;
		case 0x04:Inj();pclength = 8;
			break;
		case 0x05:Appr();pclength = 8;
			break;
		case 0x06:Abs();pclength = 8;
			break;
		case 0x07:Negate();pclength = 8;
			break;
		case 0x08:Sqrt();pclength = 8;
			break;
		case 0x09:Log();pclength = 8;
			break;
		case 0x0A:Ln();pclength = 8;
			break;
		case 0x0B:Sin();pclength = 8;
			break;
		case 0x0C:Cos();pclength = 8;
			break;
		case 0x0D:Tan();pclength = 8;
			break;
		case 0x0E:Asin();pclength = 8;
			break;
		case 0x0F:Acos();pclength = 8;
			break;		
		case 0x10:Atan();pclength = 8;
			break;
		case 0x11:Pow();pclength = 12;
			break;
		case 0x12:Trun();pclength = 9;
			break;
		case 0x13:Inside();pclength = 16;
			break;
		case 0x14:Outside();pclength = 16;
			break;
		default:break;
	}
	
	pc += InstructParsMap[Flash_Buff[pc]]+pclength;
}

/*!
 *  \return	(uint32_t)   value(f)			�����
 *  \param  (uint32_t)   low(f)				����
 *  \param  (uint32_t)   up(f)				����
 *	\brief	���������
 */
void OpRan_f(void)
{
	uint8_t i,Par[12];
	int Min,Max,temp;
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	Min = (int) GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7]));
	Max = (int) GetFloat(GetRamBase(Par[8],Par[9],Par[10],Par[11]));
	
  temp  =  RNG_Get_RandomRange(Min,Max);
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), (float) temp);
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return	(uint32_t)   value(f)			���������
 *  \param  (uint32_t)   prob(f)			����
 *	\brief	�����ʲ����������ֵ
 */
void OpRan_b(void)
{
	uint8_t i,temp,Par[8];
	float pro;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	pro = GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7]));
	if(pro < 0)
	{
		temp = 0x00;
	}else if(pro > 100)
	{
		temp = 0x01;
	}else
	{
		if( RNG_Get_RandomRange(0,100) < pro)
		{
			temp = 0x01;
		}else
		{
			temp = 0x00;
		}
	}
	
	SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return	(uint32_t)   logic(b)			���
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *  \param  (uint32_t)   value2(f)		���ݵ�ַ
 *	\brief	�Ƚϣ� value1(f) == value2(f)
 */
void Equ(void)
{
	uint8_t i,Par[12];
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	if( GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])) 
		== GetFloat(GetRamBase(Par[8],Par[9],Par[10],Par[11])) )
	{
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x01);
	}
	else
	{
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x00);
	}
			
}

/*!
 *  \return	(uint32_t)   logic(b)			���
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *  \param  (uint32_t)   value2(f)		���ݵ�ַ
 *	\brief	�Ƚϣ� value1(f) != value2(f)
 */
void Nequ(void)
{
	uint8_t i,Par[12];
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	if( GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])) 
		!= GetFloat(GetRamBase(Par[8],Par[9],Par[10],Par[11])) )
	{
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x01);
	}
	else
	{
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x00);
	}

}

/*!
 *  \return	(uint32_t)   logic(b)			���
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *  \param  (uint32_t)   value2(f)		���ݵ�ַ
 *	\brief	�Ƚϣ� value1(f) > value2(f)
 */
void Gt(void)
{
	float value1,value2;
	uint8_t i,Par[12];
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	value1 = GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7]));
	value2 = GetFloat(GetRamBase(Par[8],Par[9],Par[10],Par[11]));
	
	if( value1 
		> value2 )
	{
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x01);
	}
	else
	{
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x00);
	}
	
//	printf("value1:%f\r\n",value1);
//	printf("value2:%f\r\n",value2);
//	printf("Channel_State:%f\n\r",Byte4ToFloat(Channel_State[0],0));
	
}

/*!
 *  \return	(uint32_t)   logic(b)			���
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *  \param  (uint32_t)   value2(f)		���ݵ�ַ
 *	\brief	�Ƚϣ� value1(f) >= value2(f)
 */
void Gte(void)
{
	uint8_t i,Par[12];
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	if( GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])) 
		>= GetFloat(GetRamBase(Par[8],Par[9],Par[10],Par[11])) )
	{
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x01);
	}
	else
	{
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x00);
	}
			
}

/*!
 *  \return	(uint32_t)   logic(b)			���
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *  \param  (uint32_t)   value2(f)		���ݵ�ַ
 *	\brief	�Ƚϣ� value1(f) < value2(f)
 */
void Lt(void)
{
	uint8_t i,Par[12];
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	if( GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])) 
		< GetFloat(GetRamBase(Par[8],Par[9],Par[10],Par[11])) )
	{
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x01);
	}
	else
	{
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x00);
	}

}

/*!
 *  \return	(uint32_t)   logic(b)			���
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *  \param  (uint32_t)   value2(f)		���ݵ�ַ
 *	\brief	�Ƚϣ� value1(f) <= value2(f)
 */
void Lte(void)
{
	uint8_t i,Par[12];
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	if( GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])) 
		<= GetFloat(GetRamBase(Par[8],Par[9],Par[10],Par[11])) )
	{
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x01);
	}
	else
	{
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x00);
	}
			
}

/*!
 *  \return	(uint32_t)   value3(b)		���ݵ�ַ
 *  \param  (uint32_t)   value1(f)		���ݵ�ַ
 *  \param  (uint32_t)   value2(f)		���ݵ�ַ
 *  \param  (uint32_t)   flag(b)			���ݵ�ַ
 *	\brief	ѡ����� flag == 1 ��  value3(b)	== value1(b);
 *								��� flag == 0 ��  value3(b)	== value2(b);
 */
void Select()
{
	uint8_t i,Par[16];
	for(i=0; i<16; i++)
	{
		Par[i]		= Flash_Buff[pc+2+i];
	}
	
	if( GetFloat(GetRamBase(Par[12],Par[13],Par[14],Par[15])) >= 0x01)
	{
		SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])));
	}
	else if( GetFloat(GetRamBase(Par[12],Par[13],Par[14],Par[15])) == 0x00)
	{
		SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]), GetFloat(GetRamBase(Par[8],Par[9],Par[10],Par[11])));
	}

}

/*!
 *  \return
 *  \param
 *	\brief	���ֱȽ�ָ��
 */
void OpCompare(void)
{
	uint8_t cmd,pclength = 0;
	
	cmd = Flash_Buff[pc+1];
	
	switch(cmd)
	{
		case 0x01:Equ();pclength = 12;
			break;
		case 0x02:Nequ();pclength = 12;
			break;
		case 0x03:Gt();pclength = 12;
			break;
		case 0x04:Gte();pclength = 12;
			break;
		case 0x05:Lt();pclength = 12;
			break;
		case 0x06:Lte();pclength = 12;
			break;
		case 0x07:Select();pclength = 16;
			break;
		default:break;
	}
	
	pc += InstructParsMap[Flash_Buff[pc]]+pclength;
}

/*!
 *  \return	(uint32_t)   value3(b)		���ݵ�ַ
 *  \param  (uint32_t)   value1(b)		���ݵ�ַ
 *  \param  (uint32_t)   value2(b)		���ݵ�ַ
 *	\brief	�߼��� value3(b) = value1(b) && value2(b)
 */
void OpAnd(void)
{
	uint8_t i,Par[12];
	uint8_t  temp,temp1,temp2;
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	temp1 = GetByte(GetRamBase(Par[4],Par[5],Par[6],Par[7]));
//	if( temp1 >= 0x01)
//	{
//		temp1 = 0x01;
//	}else if(temp1 == 0x00)
//	{
//		temp1 = 0x00;
//	}
	
	temp2 = GetByte(GetRamBase(Par[8],Par[9],Par[10],Par[11]));
//	if( temp2 >= 0x01)
//	{
//		temp2 = 0x01;
//	}else if(temp2 == 0x00)
//	{
//		temp2 = 0x00;
//	}
//	
	temp = temp1 & temp2;
//	printf("temp1:%x\n\r",temp1);
//	printf("temp2:%x\n\r",temp2);
//	printf("temp:%x\n\r",temp);
//	printf("OpAnd\n\r");
	SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return	(uint32_t)   value3(b)		���ݵ�ַ
 *  \param  (uint32_t)   value1(b)		���ݵ�ַ
 *  \param  (uint32_t)   value2(b)		���ݵ�ַ
 *	\brief	�߼��� value3(b) = value1(b) || value2(b)
 */
void OpOr(void)
{
	uint8_t i,Par[12];
	uint8_t  temp,temp1,temp2;
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	temp1 = GetByte(GetRamBase(Par[4],Par[5],Par[6],Par[7]));
//	if( temp1 >= 0x01)
//	{
//		temp1 = 0x01;
//	}else if(temp1 == 0x00)
//	{
//		temp1 = 0x00;
//	}
	
	temp2 = GetByte(GetRamBase(Par[8],Par[9],Par[10],Par[11]));
//	if( temp2 >= 0x01)
//	{
//		temp2 = 0x01;
//	}else if(temp2 == 0x00)
//	{
//		temp2 = 0x00;
//	}
	
	temp = temp1 | temp2;
	
//	printf("OpOr\n\r");
	SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return	(uint32_t)   value2(b)		���ݵ�ַ
 *  \param  (uint32_t)   value1(b)		���ݵ�ַ
 *	\brief	�߼��� value2(b) = !value1(b)
 */
void OpNot(void)
{
	uint8_t i,Par[8];
	uint8_t  temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	temp = GetByte(GetRamBase(Par[4],Par[5],Par[6],Par[7]));
//	if( temp >= 0x01)
//	{
//		temp = 0x00;
//	}else if(temp == 0x00)
//	{
//		temp = 0x01;
//	}
	
	SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), ~temp);
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return	(uint32_t)   value3(b)		���ݵ�ַ
 *  \param  (uint32_t)   value1(b)		���ݵ�ַ
 *  \param  (uint32_t)   value2(b)		���ݵ�ַ
 *	\brief	�߼��� value3(b) = value1(b) ^ value2(b)
 */
void OpXor(void)
{
	uint8_t i,Par[12];
	uint8_t  temp,temp1,temp2;
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	temp1 = GetByte(GetRamBase(Par[4],Par[5],Par[6],Par[7]));
//	if( temp1 >= 0x01)
//	{
//		temp1 = 0x01;
//	}else if(temp1 == 0x00)
//	{
//		temp1 = 0x00;
//	}
	
	temp2 = GetByte(GetRamBase(Par[8],Par[9],Par[10],Par[11]));
//	if( temp2 >= 0x01)
//	{
//		temp2 = 0x01;
//	}else if(temp2 == 0x00)
//	{
//		temp2 = 0x00;
//	}
	
	temp = temp1^temp2;
	
	SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), temp);
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return	(uint32_t)   value3(2b)		���ݵ�ַ
 *  \param  (uint32_t)   value1(2b)		���ݵ�ַ
 *  \param  (uint32_t)   value2(2b)		���ݵ�ַ
 *	\brief	�߼��� value3(2b) = value1(2b) && value2(2b)
 */
void OpAnd_2b(void)
{
	uint8_t i,Par[12];
	uint16_t  temp,temp1,temp2;
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	temp1 = ( (((uint16_t) GetByte(GetRamBase(Par[4],Par[5],Par[6],Par[7])+1))<<8) + 
						(((uint16_t) GetByte(GetRamBase(Par[4],Par[5],Par[6],Par[7])  )))     );
//	if( temp1 >= 0x01)
//	{
//		temp1 = 0x01;
//	}else if(temp1 == 0x00)
//	{
//		temp1 = 0x00;
//	}
	
	temp2 = ( (((uint16_t) GetByte(GetRamBase(Par[8],Par[9],Par[10],Par[11])+1))<<8) + 
						(((uint16_t) GetByte(GetRamBase(Par[8],Par[9],Par[10],Par[11])  )))     );
//	if( temp2 >= 0x01)
//	{
//		temp2 = 0x01;
//	}else if(temp2 == 0x00)
//	{
//		temp2 = 0x00;
//	}
	
	temp = temp1 & temp2;
//	printf("temp1:%x\n\r",temp1);
//	printf("temp2:%x\n\r",temp2);
//	printf("temp:%x\n\r",temp);
//	printf("OpAnd_2b\n\r");

	if(temp > 0){
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x01);
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3])+1, 0x00);
	}else{
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x00);
		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3])+1, 0x00);
	}

//	printf("And_2b1:%x\n\r",temp1);
//	printf("And_2b1:%x\n\r",(u8)(temp1>>8));
//	printf("And_2b2:%x\n\r",temp2);
//	printf("And_2b2:%x\n\r",(u8)(temp2>>8));
	printf("And_2b3:%x\n\r",temp);
//	printf("And_2b3:%x\n\r",(u8)(temp>>8));
	
//	SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), (uint8_t)temp);
//	SetByte((GetRamBase(Par[0],Par[1],Par[2],Par[3])+1),(uint8_t)(temp>>8));
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return	(uint32_t)   value3(2b)		���ݵ�ַ
 *  \param  (uint32_t)   value1(2b)		���ݵ�ַ
 *  \param  (uint32_t)   value2(2b)		���ݵ�ַ
 *	\brief	�߼��� value3(2b) = value1(2b) || value2(2b)
 */
void OpOr_2b(void)
{
	uint8_t i,Par[12];
	uint16_t  temp,temp1,temp2;
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	temp1 = ( (((uint16_t) GetByte(GetRamBase(Par[4],Par[5],Par[6],Par[7])+1))<<8) + 
						(((uint16_t) GetByte(GetRamBase(Par[4],Par[5],Par[6],Par[7])  )))     );
//	if( temp1 >= 0x01)
//	{
//		temp1 = 0x01;
//	}else if(temp1 == 0x00)
//	{
//		temp1 = 0x00;
//	}
//	
	temp2 = ( (((uint16_t) GetByte(GetRamBase(Par[8],Par[9],Par[10],Par[11])+1))<<8) + 
						(((uint16_t) GetByte(GetRamBase(Par[8],Par[9],Par[10],Par[11])  )))     );
//	if( temp2 >= 0x01)
//	{
//		temp2 = 0x01;
//	}else if(temp2 == 0x00)
//	{
//		temp2 = 0x00;
//	}
	
	temp = temp1 | temp2;
//	printf("temp1:%x\n\r",temp1);
//	printf("temp2:%x\n\r",temp2);
//	printf("temp:%x\n\r",temp);
//	printf("OpOr_2b\n\r");
//	if(temp > 0){
//		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x01);
//	}else{
//		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x00);
//	}

	SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), (uint8_t)temp);
	SetByte((GetRamBase(Par[0],Par[1],Par[2],Par[3])+1),(uint8_t)(temp>>8));
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return	(uint32_t)   value2(2b)		���ݵ�ַ
 *  \param  (uint32_t)   value1(2b)		���ݵ�ַ
 *	\brief	�߼��� value2(2b) = !value1(2b) 
 */
void OpNot_2b(void)
{
	uint8_t i,Par[8];
	uint16_t temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	temp = ( (((uint16_t) GetByte(GetRamBase(Par[4],Par[5],Par[6],Par[7])+1))<<8) + 
						(((uint16_t) GetByte(GetRamBase(Par[4],Par[5],Par[6],Par[7])  )))     );
//	if( temp >= 0x01)
//	{
//		temp = 0x00;
//	}else if(temp == 0x00)
//	{
//		temp = 0x01;
//	}
//	printf("temp:%x\n\r",temp);
	temp = ~temp;
//	printf("temp:%x\n\r",temp);
	SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), (uint8_t)temp);
	SetByte((GetRamBase(Par[0],Par[1],Par[2],Par[3])+1),(uint8_t)(temp>>8));
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return	(uint32_t)   value3(2b)		���ݵ�ַ
 *  \param  (uint32_t)   value1(2b)		���ݵ�ַ
 *  \param  (uint32_t)   value2(2b)		���ݵ�ַ
 *	\brief	�߼��� value3(2b) = value1(2b) ^ value2(2b)
 */
void OpXor_2b(void)
{
	uint8_t i,Par[12];
	uint16_t  temp,temp1,temp2;
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	temp1 = ( (((uint16_t) GetByte(GetRamBase(Par[4],Par[5],Par[6],Par[7])+1))<<8) + 
						(((uint16_t) GetByte(GetRamBase(Par[4],Par[5],Par[6],Par[7])  )))     );
//	if( temp1 >= 0x01)
//	{
//		temp1 = 0x01;
//	}else if(temp1 == 0x00)
//	{
//		temp1 = 0x00;
//	}
	
	temp2 = ( (((uint16_t) GetByte(GetRamBase(Par[8],Par[9],Par[10],Par[11])+1))<<8) + 
						(((uint16_t) GetByte(GetRamBase(Par[8],Par[9],Par[10],Par[11])  )))     );
//	if( temp2 >= 0x01)
//	{
//		temp2 = 0x01;
//	}else if(temp2 == 0x00)
//	{
//		temp2 = 0x00;
//	}
	
	temp = temp1 ^ temp2;
	
//	if(temp > 0){
//		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x01);
//	}else{
//		SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), 0x00);
//	}
	
	SetByte(GetRamBase(Par[0],Par[1],Par[2],Par[3]), (uint8_t)temp);
	SetByte((GetRamBase(Par[0],Par[1],Par[2],Par[3])+1),(uint8_t)(temp>>8));
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \param  (uint32_t)   pc(4b)				�����ַ
 *	\brief	jump
 */
void OpJmp(void)
{
	uint8_t i,Par[4];
	for(i=0; i<4; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	pc = Getpc(Par[0],Par[1],Par[2],Par[3]);
}

/*!
 *  \param  (uint32_t)   pc(4b)				�����ַ
 *  \param  (uint32_t)   log(b)				������־
 *	\brief	���logicΪ��ʱ��ת  Ϊ��ʱ������ָ��
 */
void OpJmp_True(void)
{
	uint8_t i,Par[8];
	uint8_t temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	temp = GetByte(GetRamBase(Par[4],Par[5],Par[6],Par[7]));
	if( temp >= 0x01)
	{
		temp = 0x01;
	}else if(temp == 0x00)
	{
		temp = 0x00;
	}
	
	if(temp == 0x01)
	{
		pc = Getpc(Par[0],Par[1],Par[2],Par[3]);
	}else
	{
		pc += InstructParsMap[Flash_Buff[pc]];
	}
}

/*!
 *  \param  (uint32_t)   pc(4b)				�����ַ
 *  \param  (uint32_t)   log(b)				������־
 *	\brief	���logicΪ��ʱ��ת  Ϊ��ʱ������ָ��
 */
void OpJmp_False(void)
{
	uint8_t i,Par[8];
	uint8_t temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	temp = GetByte(GetRamBase(Par[4],Par[5],Par[6],Par[7]));
	if( temp >= 0x01)
	{
		temp = 0x01;
	}else if(temp == 0x00)
	{
		temp = 0x00;
	}
	
	if(temp == 0x00)
	{
		pc = Getpc(Par[0],Par[1],Par[2],Par[3]);
	}else
	{
		pc += InstructParsMap[Flash_Buff[pc]];
	}
}

/*!
 *  \param  (uint32_t)   pc(4b)				�����ַ
 *  \param  (uint32_t)   value(f)			ѭ������
 *	\brief	ѭ����ת
 */
void OpDjmp(void)
{
	uint8_t i,Par[8];
	float temp;
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	temp = floorf(GetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7])));
	if( temp > 0.000000f)
	{
		temp = temp - 1.000000f;
		SetFloat(GetRamBase(Par[4],Par[5],Par[6],Par[7]),temp);
		pc = Getpc(Par[0],Par[1],Par[2],Par[3]);
	}else if(temp <= 0.000000f)
	{
		pc += InstructParsMap[Flash_Buff[pc]];
	}
}

/*!
 *  \param  (uint32_t)   pc(4b)				�����ַ
 *  \param  (uint8_t)   ID-L(b)			�߳�ID��λ
 *  \param  (uint8_t)   ID-H(b)			�߳�ID��λ
 *	\brief	�����߳�
 */
void OpThread_Start(void)
{
	uint8_t i,Par[6];
	uint16_t Par_16;
	for(i=0; i<6; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	Par_16 = ((uint16_t)Par[5]<<8) + ((uint16_t)Par[4]);
	Thread[Par_16].Threadpc = Getpc(Par[0],Par[1],Par[2],Par[3]);
	Thread[Par_16].state = 1;
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \param  (uint32_t)   pc(4b)				�����ַ
 *  \param  (uint32_t)   log(b)				������־
 *  \param  (uint8_t)   ID-L(b)			�߳�ID��λ
 *  \param  (uint8_t)   ID-H(b)			�߳�ID��λ
 *	\brief	���logicΪ��ʱ�����߳� Ϊ��ʱ������ָ��
 */
void OpThread_True_Start(void)
{
	uint8_t i,Par[10];
	uint16_t Par_16;
	for(i=0; i<10; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	if(RAM_Buff[GetRamBase(Par[4],Par[5],Par[6],Par[7])] != 0)
	{
		Par_16 = ((uint16_t)Par[9]<<8) + ((uint16_t)Par[8]);
		Thread[Par_16].Threadpc = Getpc(Par[0],Par[1],Par[2],Par[3]);
		Thread[Par_16].state = 1;
	}
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \param  (uint32_t)   pc(4b)				�����ַ
 *  \param  (uint32_t)   log(b)				������־
 *  \param  (uint8_t)   ID-L(b)			�߳�ID��λ
 *  \param  (uint8_t)   ID-H(b)			�߳�ID��λ
 *	\brief	���logicΪ��ʱ�����߳� Ϊ��ʱ������ָ��
 */
void OpThread_False_Start(void)
{
	uint8_t i,Par[10];
	uint16_t Par_16;
	for(i=0; i<10; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	if(RAM_Buff[GetRamBase(Par[4],Par[5],Par[6],Par[7])] == 0)
	{
		Par_16 = ((uint16_t)Par[9]<<8) + ((uint16_t)Par[8]);
		Thread[Par_16].Threadpc = Getpc(Par[0],Par[1],Par[2],Par[3]);
		Thread[Par_16].state = 1;
	}
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \param  (uint8_t)   ID-L(b)			�߳�ID��λ
 *  \param  (uint8_t)   ID-H(b)			�߳�ID��λ
 *	\brief	�̵߳ȴ��������������Զ���תָ������û������һֱ�ڴ�ָ��ͣ���ȴ�
 */
void OpThread_End_IF(void)
{
	uint8_t i,Par[2];
	uint16_t Par_16;
	for(i=0; i<2; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	Par_16 = ((uint16_t)Par[1]<<8) + ((uint16_t)Par[0]);
	if( Thread[Par_16].Threadpc == 0x0000)
	{
		pc += InstructParsMap[Flash_Buff[pc]];
	}
}

/*!
 *  \param  (uint8_t)   ID-L(b)			�߳�ID��λ
 *  \param  (uint8_t)   ID-H(b)			�߳�ID��λ
 *	\brief	�������߳�
 */
void OpThread_End(void)
{
	uint8_t i,Par[2];
	uint16_t Par_16;
	for(i=0; i<2; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	Par_16 = ((uint16_t)Par[1]<<8) + ((uint16_t)Par[0]);
	Thread[Par_16].Threadpc = 0x0000;
	Thread[Par_16].state = 0;
	
	if( Par_16 == ThreadNow)
	{
		pc = 0x0000;
	}else
	{
		pc += InstructParsMap[Flash_Buff[pc]];
	}
}

/*!
 *	\brief	������
 */
void OpStartprogram(void)
{
	if(flashbuff_flag == 1 )  //��flash��д�ɹ��������г���
	{
		programflag = 1;
		pc += InstructParsMap[Flash_Buff[pc]];
	}
}

/*!
 *  \param  (uint32_t)   time(f)			ʱ��
 *	\brief	��ʱ
 */
void OpDelay(void)
{
	uint8_t i,Par[4];
	uint32_t temp;
	for(i=0; i<4; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
//#ifdef INSTRUCT_DEBUG
//	printf("Delay:%d-%d-%d\r\n",ThreadNow,Thread[ThreadNow].state,temp);
//#endif
	
	if( Thread[ThreadNow].state == 1)
	{
		Thread[ThreadNow].state = 2;
		Thread[ThreadNow].timetmp = GetProgramTimeMs();
//		printf("Delay:%d-%d\r\n",ThreadNow,Thread[ThreadNow].timetmp);
	}
	temp = GetProgramTimeMs() - Thread[ThreadNow].timetmp;
	if( Thread[ThreadNow].state == 2
				&& temp >= 10*GetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3])))
	{
//		printf("Delay:%d-%d\r\n",ThreadNow,temp);
		Thread[ThreadNow].state = 1;
		pc += InstructParsMap[Flash_Buff[pc]];
	}
}

/*!
 *	\brief	�������
 */
void OpEndprogram(void)//�ֲ�ʿ����
{
	EndProgram();
}

/*!
 *  \param  (uint32_t)   time(f)
 *	\brief	
 */
void OpGet_Program_Time(void)
{
	uint8_t i,Par[4];
	
	for(i=0; i<4; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]),ProgramTime/10);
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return  (uint32_t)   time(f)		ʱ��
 *  \param  (uint32_t)   	ID(b)			��ʱ��ID
 *	\brief	��ȡ��ʱ����ʱ��
 */
void OpGet_Time(void)
{
	uint8_t i,Par[5];
	uint8_t temp;
	for(i=0; i<5; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	temp = Par[4];
	if(0 < temp && temp <= 8)
	{
		SetFloat(GetRamBase(Par[0],Par[1],Par[2],Par[3]),(Timer[temp - 1].time)/10);
	}

	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \param  (uint32_t)   	ID(b)			��ʱ��ID
 *	\brief	���ö�ʱ����ʱ��
 */
void OpReset_Time(void)
{
	uint8_t i,Par[1];
	uint8_t temp;
	for(i=0; i<2; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	temp = Par[0];
	if(0 < temp && temp <= 8)
	{
		Timer[temp - 1].time = 0;
	}
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*! 
 *  \param  (uint8_t)    Par[0]   LAY				������
 *  \param  (uint8_t)    Par[1]   PORT			�˿ں����ݵ�ַ24~31λ
 *  \param  (uint8_t)    Par[2]   PORT			�˿ں����ݵ�ַ16~23λ
 *  \param  (uint8_t)    Par[3]   PORT			�˿ں����ݵ�ַ8~15λ
 *  \param  (uint8_t)    Par[4]   PORT			�˿ں����ݵ�ַ0~7λ
 *  \param  (uint8_t)    Par[5]   ID
 *  \param  (uint8_t)    Par[6]   MODE
 *  \param  (uint8_t)    Par[7]   BufferSize
 *  \param  (uint8_t)    Par[8]   Buffer			�˿ں����ݵ�ַ24~31λ
 *  \param  (uint8_t)    Par[9]   Buffer			�˿ں����ݵ�ַ16~23λ
 *  \param  (uint8_t)    Par[10]  Buffer			�˿ں����ݵ�ַ8~15λ
 *  \param  (uint8_t)    Par[11]  Buffer			�˿ں����ݵ�ַ0~7λ
 *	\brief		���ж˿ڶ�ȡָ��
 */
void OpRead_Port(void)
{
	uint8_t i,Par[12];
	uint8_t lay,port,id,mode,buffersize,*buffer;
	uint32_t bufferAdress;
//	static u8 cascade_funt_send[4][8];

	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	lay 					= Par[0];
	port 					= RAM_Buff[GetRamBase(Par[1],Par[2],Par[3],Par[4])];
	id      			= Par[5];
	mode    			= Par[6];
	buffersize		= Par[7];
	bufferAdress 	= GetRamBase(Par[8],Par[9],Par[10],Par[11]);
	
	buffer = (u8 *) malloc(buffersize * sizeof(u8));//u8 ������*buffer������
	for(i=0; i<buffersize; i++)
	{
		buffer[i] = GetByte(bufferAdress+i);
	}
	
	if(lay > 0) //����
	{
		HostSend_DDataGet_Packet(lay, port, id, mode);

		
	}
	else if(lay == 0)//�Ǽ���
	{
		if(id == 0x01) 				//��ɫ
		{
			Color(port,mode,bufferAdress);
		}
		if(id == 0x02)				//������
		{
			Gyro(port,mode,bufferAdress);
		}
		if(id == 0x03) 				//���⴫��
		{
			Infrared(port,mode,bufferAdress);
		}
		if(id == 0x04) 				//���͵��
		{
			Motor(port,mode,bufferAdress);
		}
		if(id == 0x05) 				//���͵��
		{
			Motor(port,mode,bufferAdress);
		}
		if(id == 0x06) 				//�¶�
		{
			Temperature(port,mode,bufferAdress);
		}
		if(id == 0x07) 				//����
		{
			Touch(port,mode,bufferAdress);
		}
		if(id == 0x08) 				//������
		{
			Ultrasonic(port,mode,bufferAdress);
		}
		if(id == 0x09) 				//����
		{
			Recharge(port,mode,bufferAdress);
		}
		if(id == 0x0a) 				//NXT����
		{
			Sound(port,mode,bufferAdress);
		}
	}
	free(buffer);
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*! 
 *  \param  (uint8_t)    Par[0]   LAY				������
 *  \param  (uint8_t)    Par[1]   PORT			�˿ں����ݵ�ַ24~31λ
 *  \param  (uint8_t)    Par[2]   PORT			�˿ں����ݵ�ַ16~23λ
 *  \param  (uint8_t)    Par[3]   PORT			�˿ں����ݵ�ַ8~15λ
 *  \param  (uint8_t)    Par[4]   PORT			�˿ں����ݵ�ַ0~7λ
 *  \param  (uint8_t)    Par[5]   ID
 *  \param  (uint8_t)    Par[6]   MODE
 *  \param  (uint8_t)    Par[7]   BufferSize
 *  \param  (uint8_t)    Par[8]   Buffer			�˿ں����ݵ�ַ24~31λ
 *  \param  (uint8_t)    Par[9]   Buffer			�˿ں����ݵ�ַ16~23λ
 *  \param  (uint8_t)    Par[10]  Buffer			�˿ں����ݵ�ַ8~15λ
 *  \param  (uint8_t)    Par[11]  Buffer			�˿ں����ݵ�ַ0~7λ
 *	\brief		����˿�д��ָ��
 */
void OpWrite_Port(void)
{
	uint8_t i,Par[12],result=0;
	uint8_t lay,port,port_1,port_2,id,mode,buffersize,*buffer;
	uint32_t bufferAdress;
	static u8 cascade_funt_send[4][8];
	
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	lay 					= Par[0];
	port 					= RAM_Buff[GetRamBase(Par[1],Par[2],Par[3],Par[4])];
	port_1 				= (RAM_Buff[GetRamBase(Par[1],Par[2],Par[3],Par[4])] & 0x30) >> 4;
	port_2 				= (RAM_Buff[GetRamBase(Par[1],Par[2],Par[3],Par[4])] & 0xc0) >> 6;
	id      			= Par[5];
	mode    			= Par[6];
	buffersize		= Par[7];
	bufferAdress 	= GetRamBase(Par[8],Par[9],Par[10],Par[11]);
	
	buffer = (u8 *) malloc(buffersize * sizeof(u8));//u8 ������*buffer������
	for(i=0; i<buffersize; i++)
	{
		buffer[i] = GetByte(bufferAdress+i);
	}
	
	if(lay > 0) //����
	{
		if(cascade_funt_send[lay][port] == 0 )
		{
			HostSend_DControl_Packet(lay, port, id, mode, buffer, buffersize);
			cascade_funt_send[lay][port] = 1;
		}
		
		if(Cascade_Function.Cascade_flag[lay][port].flag == 1)
		{
			result = 1;
			cascade_funt_send[lay][port] = 0;
			Cascade_Function.Cascade_flag[lay][port].flag = 0;
		}
		
	}else if(lay == 0)//�Ǽ���
	{
//		printf("port%x \n\r",port);

		if(id == 0x80) 	//���͵��   //����޸�12.12
		{
			if(mode == 1)							//���͵��ģʽ1 off
			{
				result = Meduim_Motor_Off(port, GetByte(bufferAdress));
			}
			if(mode == 2)							//���͵��ģʽ2 on
			{
				result = Meduim_Motor_On(port, GetFloat(bufferAdress),Motor_Invert[port-5]);																			
			}
			if(mode == 3)							//���͵��ģʽ3 on for second
			{
				result = Meduim_Motor_OnForSecond(port,
																						GetFloat(bufferAdress),
																							GetFloat(bufferAdress+4),
																								GetByte(bufferAdress+8),
																									Motor_Invert[port-5]);
			}
			if(mode == 4)							//���͵��ģʽ4 on for degrees
			{
				result = Meduim_Motor_OnForDegrees(port,
																						GetFloat(bufferAdress),
																							GetFloat(bufferAdress+4),
																								GetByte(bufferAdress+8),
																									Motor_Invert[port-5]);
			}
			if(mode == 5)							//���͵��ģʽ5 on for rotation
			{
				result = Meduim_Motor_OnForRotations(port,
																							GetFloat(bufferAdress),
																								GetFloat(bufferAdress+4),
																									GetByte(bufferAdress+8),
																										Motor_Invert[port-5]);
			}
		}
		if(id == 0x81)	//���͵��
		{
			if(mode == 1)							//���͵��ģʽ1 off
			{
				result = Large_Motor_Off(port, GetByte(bufferAdress));
			}
			if(mode == 2)							//���͵��ģʽ2 on
			{
				result = Large_Motor_On(port, GetFloat(bufferAdress),Motor_Invert[port-5]);
			}
			if(mode == 3)							//���͵��ģʽ3 on for second
			{
				result = Large_Motor_OnForSecond(port,
																					GetFloat(bufferAdress),
																						GetFloat(bufferAdress+4),
																							GetByte(bufferAdress+8),
																								Motor_Invert[port-5]);
			}
			if(mode == 4)							//���͵��ģʽ4 on for degrees
			{
				result = Large_Motor_OnForDegrees(port,
																						GetFloat(bufferAdress),
																							GetFloat(bufferAdress+4),
																								GetByte(bufferAdress+8),
																									Motor_Invert[port-5]);
		
	//			printf("INSTRUCT:%s %d\r\n","result",result);

			}
			if(mode == 5)							//���͵��ģʽ5 on for rotation
			{
				result = Large_Motor_OnForRotations(port,
																							GetFloat(bufferAdress),
																								GetFloat(bufferAdress+4),
																									GetByte(bufferAdress+8),
																										Motor_Invert[port-5]);
			}
		}
		if(id == 0x82)	//��ʻ����               
		{
			if(mode == 1)							//��ʻ����ģʽ1 off
			{
				result = Move_Steering_Off(port_1+5, port_2+5, GetByte(bufferAdress));
			}
			if(mode == 2)							//��ʻ����ģʽ2 on
			{
				result = Move_Steering_On(port_1+5,port_2+5,
																		GetFloat(bufferAdress),
																			GetFloat(bufferAdress+4),
																				Motor_Invert[port_1-5]);
			}
			if(mode == 3)							//��ʻ����ģʽ3 on for second
			{
				result = Move_Steering_OnForSecond(port_1+5,port_2+5,
																						GetFloat(bufferAdress),
																							GetFloat(bufferAdress+4),
																								GetFloat(bufferAdress+8),
																									GetByte(bufferAdress+12),
																										Motor_Invert[port_1-5]);
			}
			if(mode == 4)							//��ʻ����ģʽ4 on for degrees
			{
				result = Move_Steering_OnForDegrees(port_1+5,port_2+5,
																							GetFloat(bufferAdress),
																								GetFloat(bufferAdress+4),
																									GetFloat(bufferAdress+8),
																										GetByte(bufferAdress+12),
																											Motor_Invert[port_1-5]);

			}
			if(mode == 5)							//��ʻ����ģʽ5 on for rotation
			{
				result = Move_Steering_OnForRotations(port_1+5,port_2+5,
																								GetFloat(bufferAdress),
																									GetFloat(bufferAdress+4),
																										GetFloat(bufferAdress+8),
																											GetByte(bufferAdress+12),
																												Motor_Invert[port_1-5]);
			}
		}
		if(id == 0x83)	//̹�˿���
		{
			if(mode == 1)							//̹�˿���ģʽ1 off
			{
				result = Move_Tank_Off(port_1+5, port_2+5, GetByte(bufferAdress));
			}
			if(mode == 2)							//̹�˿���ģʽ2 on
			{
				result = Move_Tank_On(port_1+5,port_2+5,
																GetFloat(bufferAdress),
																	GetFloat(bufferAdress+4),
																		Motor_Invert[port_1-5]);
			}
			if(mode == 3)							//̹�˿���ģʽ3 on for second
			{
				result = Move_Tank_OnForSecond(port_1+5,port_2+5,
																				GetFloat(bufferAdress),
																					GetFloat(bufferAdress+4),
																						GetFloat(bufferAdress+8),
																							GetByte(bufferAdress+12),
																								Motor_Invert[port_1-5]);
			}
			if(mode == 4)							//̹�˿���ģʽ4 on for degrees
			{
				result = Move_Tank_OnForDegrees(port_1+5,port_2+5,
																					GetFloat(bufferAdress),
																						GetFloat(bufferAdress+4),
																							GetFloat(bufferAdress+8),
																								GetByte(bufferAdress+12),
																									Motor_Invert[port_1-5]);
	//			printf("INSTRUCT:%s %d\r\n","result",result);

			}
			if(mode == 5)							//̹�˿���ģʽ5 on for rotation
			{
				result = Move_Tank_OnForRotations(port_1+5,port_2+5,
																						GetFloat(bufferAdress),
																							GetFloat(bufferAdress+4),
																								GetFloat(bufferAdress+8),
																									GetByte(bufferAdress+12),
																										Motor_Invert[port_1-5]);
			}
		}
		if(id == 0x84) 				//δУ׼���
		{
			if(mode == 1)				
			{
				result = Unregulated_Motor_On(port, GetFloat(bufferAdress), Motor_Invert[port-5]);
			}
			
		}
		if(id == 0x85) 				//��ת���
		{
			if(mode == 1)
			{
				if(GetByte(bufferAdress) == 1)
				{
					Motor_Invert[port-5] = 0x01;
				}else if(GetByte(bufferAdress) == 0)
				{
					Motor_Invert[port-5] = 0x00;
				}
				result = 1;
			}
		}
	}
	free(buffer);
	
	if(result == 1)
	{
//		printf("buffer:%x\r\n",bufferAdress);
//		printf("iii:%d \r\n",Pluse[port-5]++);
//		printf("iii:%d \r\n",SumPluseForDisplay[port-5]++);
//		Pluse[port-5]=0;
//		SumPluseForDisplay[port-5]=0;
		pc += InstructParsMap[Flash_Buff[pc]];
	}
}

/*! 
 *  \param  (uint8_t)    Par[0]   DEVICE
 *  \param  (uint8_t)    Par[1]   MODE
 *  \param  (uint8_t)    Par[2]   BufferSize
 *  \param  (uint8_t)    Par[3]   Buffer			�˿ں����ݵ�ַ24~31λ
 *  \param  (uint8_t)    Par[4]   Buffer			�˿ں����ݵ�ַ16~23λ
 *  \param  (uint8_t)    Par[5]  	Buffer			�˿ں����ݵ�ַ8~15λ
 *  \param  (uint8_t)    Par[6]  	Buffer			�˿ں����ݵ�ַ0~7λ
 *	\brief		�豸��ȡָ��
 */
void OpRead_Device(void)
{
	uint8_t i,Par[7];
	uint8_t device,mode;
	uint32_t bufferAdress;
//	uint8_t BufferSize;
	for(i=0; i<7; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	device				= Par[0];
	mode 					= Par[1];
//	BufferSize 		= Par[2];
	bufferAdress 				= GetRamBase(Par[3],Par[4],Par[5],Par[6]);
	
	if(device == 0x01) 				//��ʾ��
	{
	}
	if(device == 0x02) 				//����
	{
	}
	if(device == 0x03) 				//����
	{
		BriButton(mode,bufferAdress);
	}
	if(device == 0x04) 				//ָʾ��
	{
	}
	if(device == 0x05) 				//��������
	{
	}
	if(device == 0x06) 				//��Ϣ����
	{
	}
	if(device == 0x07) 				//�ļ�����
	{
	}
	if(device == 0x08) 				//�����ϴ�
	{
	}
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*! 
 *  \param  (uint8_t)    Par[0]   DEVICE
 *  \param  (uint8_t)    Par[1]   MODE
 *  \param  (uint8_t)    Par[2]   BufferSize
 *  \param  (uint8_t)    Par[3]   Buffer			�˿ں����ݵ�ַ24~31λ
 *  \param  (uint8_t)    Par[4]   Buffer			�˿ں����ݵ�ַ16~23λ
 *  \param  (uint8_t)    Par[5]  	Buffer			�˿ں����ݵ�ַ8~15λ
 *  \param  (uint8_t)    Par[6]  	Buffer			�˿ں����ݵ�ַ0~7λ
 *	\brief		�豸д��ָ��
 */
void OpWrite_Device(void)
{
	uint8_t i,result,Par[7];
	uint8_t device,mode,BufferSize;
	uint32_t bufferAdress;
	for(i=0; i<7; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	device				= Par[0];
	mode 					= Par[1];
	BufferSize 		= Par[2];
	bufferAdress 	= GetRamBase(Par[3],Par[4],Par[5],Par[6]);
	
	if(device == 0x01) 				//��ʾ��
	{
		BriDisplay(mode,BufferSize,bufferAdress);
		LCD_flag = 1;
		LCD_refresh();
		result = 1;
	}
	if(device == 0x02) 				//����
	{
		result = BriSound(mode,bufferAdress);
	}
	if(device == 0x03) 				//����
	{
		result = 1;
	}
	if(device == 0x04) 				//ָʾ��
	{
		BriLight(mode,bufferAdress);result = 1;
	}
	if(device == 0x05) 				//��������
	{
		BriBt(mode,bufferAdress);result = 1;
	}
	if(device == 0x06) 				//��Ϣ����
	{
		BriMessage(mode,bufferAdress);result = 1;
	}
	if(device == 0x07) 				//�ļ�����
	{
		BriFile(mode,bufferAdress);result = 1;
	}
	if(device == 0x08) 				//�����ϴ�
	{
		BriWaveform(mode,bufferAdress);result = 1;
	}
	if(device == 0x09) 			//��ʱ��
	{
		BriTimerSensor(mode,bufferAdress);result = 1;
	}
	if(device == 0x0a) 				//ԭʼֵ
	{
		BriRawSensor(mode,bufferAdress);result = 1;
	}
		
	if(result == 1)
	{
		pc += InstructParsMap[Flash_Buff[pc]];
	}
}

/*!
 *  \return
 *  \param
 *	\brief	�����ļ�������
 */
void OpFilename(void)
{
	uint8_t cmd;
	
	cmd = Flash_Buff[pc+1];
	
	switch(cmd)
	{
		case 0x01:
			break;
		case 0x02:
			break;
		case 0x03:
			break;
		case 0x04:
			break;
		case 0x05:
			break;
		case 0x06:
			break;
		default:break;
	}
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return
 *  \param
 *	\brief	
 */
void File_Byte(void)
{
	uint8_t i,Par[16];
	for(i=0; i<16; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	pc += 16;
}

/*!
 *  \return
 *  \param
 *	\brief	
 */
void File_picture(void)
{
	uint8_t i,Par[16];
	for(i=0; i<16; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	pc += 16;
}	

/*!
 *  \return
 *  \param
 *	\brief	
 */
void File_Sound(void)
{
	uint8_t i,Par[16];
	for(i=0; i<16; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	pc += 16;
}
	
/*!
 *  \return
 *  \param
 *	\brief	
 */
void File_Datalog(void)
{
	uint8_t i,Par[16];
	for(i=0; i<16; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	pc += 16;
}	

/*!
 *  \return
 *  \param
 *	\brief	
 */
void File_Txt(void)
{
	uint8_t i,Par[16];
	for(i=0; i<16; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	pc += 16;
}	

/*!
 *  \return
 *  \param
 *	\brief	
 */
void File_close(void)
{
	uint8_t i,Par[16];
	for(i=0; i<16; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	pc += 16;
}

/*!
 *  \return
 *  \param
 *	\brief	�����ļ�����
 */
void OpFile(void)
{
	uint8_t cmd;
	
	cmd = Flash_Buff[pc+1];
	
	switch(cmd)
	{
		case 0x01:File_Byte();
			break;
		case 0x02:File_picture();
			break;
		case 0x03:File_Sound();
			break;
		case 0x04:File_Datalog();
			break;
		case 0x05:File_Txt();
			break;
		case 0x06:File_close();
			break;
		default:break;
	}
	
	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return	(uint8_t)		size			�ַ�������
 *  \param	(uint8_t)		string		�ַ���
 *	\brief	��ȡ�ַ����ĳ���
 */
void Sting_Getsize(void)
{
	uint8_t i,Par[8];
	uint16_t  temp=0x0000;
	uint32_t address;
	
	for(i=0; i<8; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	address = Getpc(Par[4],Par[5],Par[6],Par[7]);
	while(Flash_Buff[address] <= '~' && Flash_Buff[address] >= ' ' )
	{
		address++;
		temp++;
	}
	SetByte(Getpc(Par[0],Par[1],Par[2],Par[3]), (uint8_t) temp>>8);
	SetByte(Getpc(Par[0],Par[1],Par[2],Par[3])+1, (uint8_t) temp);
	
	pc += 8;
}

/*!
 *  \return
 *  \param
 *	\brief	
 */
void Sting_Add(void)
{
	uint8_t i,Par[12];
//	uint16_t  temp=0x0000;
	uint32_t address1,address2,address3;
	
	for(i=0; i<12; i++)
	{
		Par[i]		= Flash_Buff[pc+1+i];
	}
	
	address1 = Getpc(Par[0],Par[1],Par[2],Par[3]);
	address2 = Getpc(Par[4],Par[5],Par[6],Par[7]);
	address3 = Getpc(Par[8],Par[9],Par[10],Par[11]);
	
//	while(Flash_Buff[address1] <= '~' && Flash_Buff[address1] >= ' ' )
//	{
//		address++;
//		temp++;
//	}
	
	
	pc += 12;
}

/*!
 *  \return
 *  \param
 *	\brief	
 */
void Sting_Compare(void)
{
	
}

/*!
 *  \return
 *  \param
 *	\brief	
 */
void Sting_Duplicate(void)
{
	
}

/*!
 *  \return
 *  \param
 *	\brief	
 */
void Sting_FloatToString(void)
{
	
}

/*!
 *  \return
 *  \param
 *	\brief	
 */
void Sting_StringToFloat(void)
{
	
}

/*!
 *  \return
 *  \param
 *	\brief	
 */
void Sting_Strip(void)
{
	
}

/*!
 *  \return
 *  \param
 *	\brief	
 */
void Sting_Sub(void)
{
	
}

/*!
 *  \return
 *  \param
 *	\brief	�����ַ�������
 */
void OpString(void)
{
	uint8_t cmd;
	
	cmd = Flash_Buff[pc+1];
	
	switch(cmd)
	{
		case 0x01:Sting_Getsize();
			break;
		case 0x02:Sting_Add();
			break;
		case 0x03:Sting_Compare();
			break;
		case 0x04:Sting_Duplicate();
			break;
		case 0x05:Sting_FloatToString();
			break;
		case 0x06:Sting_StringToFloat();
			break;
		case 0x07:Sting_Strip();
			break;
		case 0x08:Sting_Sub();
			break;
		default:break;
	}

	pc += InstructParsMap[Flash_Buff[pc]];
}

/*!
 *  \return ��uint8_t��		return				���
 *  \param ��uint8_t��		port					�˿�
 *  \param ��uint8_t��		DeviceId			�豸ID
 *  \param ��uint8_t��		mode					�豸ģʽ
 *  \param ��uint8_t��		buffersize		
 *  \param	(uint8_t��		Buffer[]			
 *	\brief	�������������
 */
u8 daisy_Write_Port(u8 port, u8 DeviceId, u8 mode,	u8 buffersize, u8 buff[13])  //����������ƺ���
{
	u8 result=0;
	if(DeviceId == 0x80) 	//���͵��
		{
			if(mode == 1)							//���͵��ģʽ1 off
			{
				result = Meduim_Motor_Off(port, GetByte(buff[0]));
			}
			if(mode == 2)							//���͵��ģʽ2 on
			{
				result = Meduim_Motor_On(port, GetFloat(buff[0]),Motor_Invert[port-5]);																			
			}
			if(mode == 3)							//���͵��ģʽ3 on for second
			{
				result = Meduim_Motor_OnForSecond(port,
																						60,//GetFloat(GetRamBase(buff[0],buff[1],buff[2],buff[3])),
																							10,//GetFloat(GetRamBase(buff[4],buff[5],buff[6],buff[7])),
																								buff[8],
																									Motor_Invert[port-5]);
//				result =1;
//					printf("GetFloat %f  \r\n",GetFloat(GetRamBase(buff[0],buff[1],buff[2],buff[3])));
//					printf("GetFloat %f  \r\n",GetFloat(GetRamBase(buff[4],buff[5],buff[6],buff[7])));
//					printf("GetByte %x  \r\n",buff[8]);
			}
			if(mode == 4)							//���͵��ģʽ4 on for degrees
			{
				result = Meduim_Motor_OnForDegrees(port,
																						GetFloat(buff[0]),
																							GetFloat(buff[4]),
																								GetByte(buff[8]),
																									Motor_Invert[port-5]);
			}
			if(mode == 5)							//���͵��ģʽ5 on for rotation
			{
				result = Meduim_Motor_OnForRotations(port,
																							GetFloat(buff[0]),
																								GetFloat(buff[4]),
																									GetByte(buff[8]),
																										Motor_Invert[port-5]);
			}
		}
		if(DeviceId == 0x81)	//���͵��
		{
			if(mode == 1)							//���͵��ģʽ1 off
			{
				result = Large_Motor_Off(port, GetByte(buff[0]));
			}
			if(mode == 2)							//���͵��ģʽ2 on
			{
				result = Large_Motor_On(port, GetFloat(buff[0]),Motor_Invert[port-5]);
			}
			if(mode == 3)							//���͵��ģʽ3 on for second
			{
				result = Large_Motor_OnForSecond(port,
																							GetFloat(buff[0]),
																								GetFloat(buff[4]),
																									GetByte(buff[8]),
																										Motor_Invert[port-5]);
			}
			if(mode == 4)							//���͵��ģʽ4 on for degrees
			{
				result = Large_Motor_OnForDegrees(port,
																						GetFloat(buff[0]),
																							GetFloat(buff[4]),
		//																						180,
																								GetByte(buff[8]),
																									Motor_Invert[port-5]);
	//			printf("INSTRUCT:%s %d\r\n","result",result);

			}
			if(mode == 5)							//���͵��ģʽ5 on for rotation
			{
				result = Large_Motor_OnForRotations(port,
																							GetFloat(buff[0]),
																								GetFloat(buff[4]),
																									GetByte(buff[8]),
																										Motor_Invert[port-5]);
			}
		}
		return result;
}

/*!
 *  \return ��uint8_t��		return				���
 *  \param ��uint8_t��		port					�˿�
 *  \param ��uint8_t��		DeviceId			�豸ID
 *  \param ��uint8_t��		mode					�豸ģʽ
 *  \param ��uint8_t��		buffersize		
 *  \param	(uint8_t��		Buffer[]			
 *	\brief	�������������ƺ���
 */
u8 daisy_sensor_control(u8 Port, u8 DeviceId, u8 mode,	u8 bufflength, u8 buff[13])  //����������ģʽ��������
{
	if(DeviceId == 0x01)
	{
		if(mode == 0x01)
		{
			ChangeMode(Port,2);
		}
		if(mode == 0x02)
		{
			ChangeMode(Port,0);
		}
		if(mode == 0x03)
		{
			ChangeMode(Port,1);
		}
	}
	if(DeviceId == 0x02)
	{
		if(mode == 0x01)
		{
			ChangeMode(Port,0);
		}
		if(mode == 0x02)
		{
			ChangeMode(Port,1);
		}
	}
	if(DeviceId == 0x03)
	{
		if(mode == 0x01)
		{
			ChangeMode(Port,0);
		}
	}
	if(DeviceId == 0x08)
	{
		if(mode == 0x03)
		{
			ChangeMode(Port,0);
		}
		if(mode == 0x04)
		{
			ChangeMode(Port,1);
		}
		if(mode == 0x05)
		{
			ChangeMode(Port,2);
		}
	}
	else
	{
		Channel_State[Port][2] = mode;
	}
	
	return 1;
}

/*!
 *  \return ��uint8_t��		return				���
 *  \param ��uint8_t��		port					�˿�
 *  \param ��uint8_t��		DeviceId			�豸ID
 *  \param ��uint8_t��		mode					�豸ģʽ
 *  \param ��uint8_t��		buffersize		
 *  \param	(uint8_t��		Buffer[]			
 *	\brief	����ָ���
 */
void deal_daisy_Chain(u8 Port, u8 DeviceId, u8 mode,	u8 bufflength, u8 buff[13])
{
	u8 result=0;
	
	if(Port > 4)
	{
		result = daisy_Write_Port(Port,DeviceId,mode,bufflength,buff);
	}
	else
	{
		result = daisy_sensor_control(Port,DeviceId,mode,bufflength,buff);
	}
	
//		result = 1;
	
	if(result == 1)
	{
		//����ָ�����
		SlaveReply_DControl_Packet(CascadeLevel,
																Port,
																	Cascade_Function.Cascade_Order[Port].id,
																		Cascade_Function.Cascade_Order[Port].mode);	
		Cascade_Function.Cascade_Order[Port-1].flag = 0;
	}
}
