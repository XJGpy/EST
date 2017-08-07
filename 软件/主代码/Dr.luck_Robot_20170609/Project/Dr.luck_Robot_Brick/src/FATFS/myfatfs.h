/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/myfatfs.h
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   
  ******************************************************************************
**/
#ifndef __MYFATFS_H
#define __MYFATFS_H

#include "ff.h"
#include "sys.h"

#define file_max 256

/**
  ******************************************************************************
  *                              �������
  ******************************************************************************
  */
extern FATFS fs;								/* FatFs�ļ�ϵͳ���� */
extern FIL file1,file2,file3;		/* �ļ����� */
extern FRESULT res_flash;				/* �ļ�������� */
extern UINT fnum;								/* �ļ��ɹ���д���� */
//extern BYTE ReadBuffer[];				/* �������� */
//extern BYTE WriteBuffer[];			/* д������*/
extern u8 fpath[32];					/* ���浱ǰɨ��·�� */
extern u8 oldfpath[32];                  /* ���浱ǰɨ��·�� */

extern u8 allfile[256][64];
extern float allfilesize[256];
extern u8 allfile_name_num;			//ȫ���ļ���

extern u8 window_top;					//���ڶ�������
extern u8 file_name_num;			//�ļ���

typedef struct 
{
  u8 path[32];					//�ļ�·��
	u8 name[32];			//�ļ���
	u8 type;					//0���ޡ�1���������ļ���2��ͼƬ�ļ���3�������ļ�
	u8 dir;						//0����Ŀ¼ 1��Ŀ¼
}le_file;
extern le_file file_load[];//һ��·���������ʾ256���ļ�

extern uint8_t PICTURE_Buff[1024*3];					//PICTURE����
//extern uint8_t SOUND_Buff[1024*3];					//SOUND����
extern u32 buttonsound_index;
extern u32 SOUND_Buff_index;
extern u8 SOUND_Buff_flag;
//extern u8 SOUND_Buff_frame;
extern u8 SOUND_UI;

extern u32 downloadsound_index;
extern u8 downloadsound_flag;

extern u8 myfilename[];

extern void filetest(u8 button);
extern u32 mf_showfree(u8 *drv, u32 *total);
extern FRESULT scan_all_files(char* path); 
extern FRESULT scan_files(char* path, u8 mode);
extern FRESULT f_deldir(const TCHAR *path);
extern void inversion_file_load(void);

#endif

