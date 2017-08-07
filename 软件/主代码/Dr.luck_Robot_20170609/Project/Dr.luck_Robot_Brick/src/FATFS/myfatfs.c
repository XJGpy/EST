/**
  ******************************************************************************
  * @file    EV3 Brick of Dr.luck/myfatfs.c 
  * @author  STU AI & Robot Lab
  * @version V1.0.0
  * @date    14-Nov-2015
  * @brief   
  ******************************************************************************
**/
#include "myfatfs.h"
#include <string.h>
#include <stdio.h>
#include "sys.h"

/**
  ******************************************************************************
  *                              �������
  ******************************************************************************
  */
FATFS fs;																									/* FatFs�ļ�ϵͳ���� */
FIL file1,file2,file3;																		/* �ļ�����*/  							//�����ơ�ͼƬ������
FRESULT res_flash;                												/* �ļ�������� */
UINT fnum;            					  												/* �ļ��ɹ���д���� */
u8 fpath[32];                  														/* ���浱ǰɨ��·�� */
u8 oldfpath[32];                 													/* ������һ��ɨ��·�� */

u8 myfilename[256]={0};																		/* �ӻ��ָ�����Ļѡ���ȡ��ǰ�ļ��� */

//��Ļ��ʾ��Ϣɨ��
u8 window_top=0;																					//���ڶ�������
u8 file_name_num=0;																				//��ǰ·���ļ���
le_file file_load[file_max];															/* ���浱ǰĿ¼�������ļ��� */ //һ��·���������ʾ256���ļ�

//�ļ�ϵͳ��Ϣ�ϴ�PC
u8 allfile[256][64];
float allfilesize[256];
u8 allfile_name_num=0;																		//�ļ�ϵͳ��ȫ���ļ���

//ͼƬ�������ļ�����
uint8_t PICTURE_Buff[1024*3];															//PICTURE����

//uint8_t SOUND_Buff[1024*3];																//SOUND����
u32 SOUND_Buff_index=0;
u8 SOUND_Buff_flag=0;
//u8 SOUND_Buff_frame=0;
u8 SOUND_UI=0;

u32 buttonsound_index=0;

u32 downloadsound_index=0;
u8 downloadsound_flag=0;

//����
BYTE myReadBuffer[1024]={0};      												/* �������� */
BYTE myWriteBuffer[] =            												/* д������*/
"�ֲ�ʿ�ļ�ϵͳ����1\r\n";

//char last_path[256][256];//��һ���ַ��� ����α���
//char mypath[256];//��ǰ·���ַ��� ���ַ�������
//unsigned char level = 0;//�ļ��в���
//unsigned char level_max = 255;//�ļ���������
//char scanned = 0;//�Ѿ�ɨ���

 //�ļ������б�
#define FILE_MAX_TYPE_NUM		5															//���FILE_MAX_TYPE_NUM������
u8*const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM]=
{
		{"dbf"},																							//ʮ������ִ���ļ�	0x01
		{"dpf"},																							//ͼƬ�ļ�					0x02
		{"mp3"},																							//�����ļ�					0x04
		{"ddf"},																							//�����ļ�					0x08
		{"dtf"},																							//�ı��ļ�					0x10
};
u8 f_typetell(char *fname);

/**
  * @brief  scan_all_files �ݹ�ɨ��FatFs�ڵ��ļ�
  * @param  path:��ʼɨ��·��
  * @retval result:�ļ�ϵͳ�ķ���ֵ
  */
FRESULT scan_all_files (char* path) 
{ 
  FRESULT res; 		//�����ڵݹ���̱��޸ĵı���������ȫ�ֱ���	
  FILINFO fno; 
  DIR dir; 
  int i,j;            
  char *fn;        // �ļ���	
	
#if _USE_LFN 
  /* ���ļ���֧�� */
  /* ����������Ҫ2���ֽڱ���һ�����֡�*/
  static char lfn[_MAX_LFN*2 + 1]; 	
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif 
  //��Ŀ¼
  res = f_opendir(&dir, path); 
  if (res == FR_OK) 
	{ 
    i = strlen(path); 
    for (j=0;;j++) 
		{ 
      //��ȡĿ¼�µ����ݣ��ٶ����Զ�����һ���ļ�
      res = f_readdir(&dir, &fno); 								
      //Ϊ��ʱ��ʾ������Ŀ��ȡ��ϣ�����
      if (res != FR_OK || fno.fname[0] == 0) break; 	
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      //���ʾ��ǰĿ¼������			
      if (*fn == '.') continue; 	
      //Ŀ¼���ݹ��ȡ      
      if (fno.fattrib & AM_DIR)         
			{ 			
        //�ϳ�����Ŀ¼��        
        sprintf(&path[i], "/%s", fn); 		
        //�ݹ����         
        res = scan_all_files(path);	
        path[i] = 0;         
        //��ʧ�ܣ�����ѭ��        
        if (res != FR_OK) 
					break; 
      } 
			else 
			{			
				sprintf(&allfile[allfile_name_num][strlen(allfile[allfile_name_num])], "%s",path); 
				sprintf(&allfile[allfile_name_num][strlen(allfile[allfile_name_num])], "/%s",fn); 
				allfilesize[allfile_name_num]=fno.fsize;
				allfile_name_num++;
//				printf("%s/%s--%d\r\n", path, fn, (int)fno.fsize);
				
//				printf("%s/%s\r\n", path, fn);								//����ļ���	
        /* ������������ȡ�ض���ʽ���ļ�·�� */        
      }//else
    } //for
  } 
  return res; 
}

//��ǰ·���ļ�ǰ��λ�õ��ã�
void inversion_file_load(void)
{
	u8 i;
	
	for(i=0; i<file_name_num/2; i++)
	{
		strcpy((char *)file_load[file_name_num].path,(char *)file_load[i].path);
		strcpy((char *)file_load[file_name_num].name,(char *)file_load[i].name);
		file_load[file_name_num].type = file_load[i].type;
		file_load[file_name_num].dir = file_load[i].dir;
		
		strcpy((char *)file_load[i].path,(char *)file_load[file_name_num-1-i].path);
		strcpy((char *)file_load[i].name,(char *)file_load[file_name_num-1-i].name);
		file_load[i].type = file_load[file_name_num-1-i].type;
		file_load[i].dir = file_load[file_name_num-1-i].dir;
		
		strcpy((char *)file_load[file_name_num-1-i].path,(char *)file_load[file_name_num].path);
		strcpy((char *)file_load[file_name_num-1-i].name,(char *)file_load[file_name_num].name);
		file_load[file_name_num-1-i].type = file_load[file_name_num].type;
		file_load[file_name_num-1-i].dir = file_load[file_name_num].dir;
		
	}
//	printf("\n\r");
//	for(i=0; i<file_name_num; i++)
//	{
//		printf("%d:%s\n\r",i,file_load[i].name);
//	}
}

/**
  * @brief  scan_files �ݹ�ɨ��FatFs�ڵ��ļ�
  * @param  path:��ʼɨ��·��
  * @retval result:�ļ�ϵͳ�ķ���ֵ
  */
	FRESULT scan_files (char* path, u8 mode) //0:�������0�ţ�ɨ��dbf�ļ�   1:�������1�ţ�ɨ��Ŀ¼���ļ�
{ 
  FRESULT res; 		//�����ڵݹ���̱��޸ĵı���������ȫ�ֱ���	
  FILINFO fno; 
  DIR dir; 
  int i,j;            
  char *fn;        // �ļ���	
	
#if _USE_LFN 
  /* ���ļ���֧�� */
  /* ����������Ҫ2���ֽڱ���һ�����֡�*/
  static char lfn[_MAX_LFN*2 + 1]; 	
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif 
  //��Ŀ¼
  res = f_opendir(&dir,path); 
  if (res == FR_OK) 
	{ 
    i = strlen(path); 
    for (j=0;;j++) 
		{ 
      //��ȡĿ¼�µ����ݣ��ٶ����Զ�����һ���ļ�
      res = f_readdir(&dir, &fno); 								
      //Ϊ��ʱ��ʾ������Ŀ��ȡ��ϣ�����
      if (res != FR_OK || fno.fname[0] == 0) 
			{
				res = f_closedir(&dir); 
				break;
			}
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      //���ʾ��ǰĿ¼������			
      if (*fn == '.') continue; 	

			if(mode == 1)
			{
				if (fno.fattrib & AM_DIR)         
				{
					//�ϳ�����Ŀ¼��        
					sprintf(&path[i], "/%s", fn); 	
					strcpy((char *)file_load[file_name_num].name,fn);								//��ȡĿ¼	
					strcpy((char *)file_load[file_name_num].path,path);
					file_load[file_name_num].dir = 1;
					file_load[file_name_num].type = 0;
					file_name_num++;
					path[i] = 0; 
        
					//��ʧ�ܣ�����ѭ��        
					if (res != FR_OK) 
						break; 	
				}
				else 
				{
					/* ������������ȡ�ض���ʽ���ļ�·�� */  
					strcpy((char *)file_load[file_name_num].path,path);					
					strcpy((char *)file_load[file_name_num].name,fn);										//��ȡ�ļ�
					file_load[file_name_num].dir = 0;
					file_load[file_name_num].type = f_typetell(file_load[file_name_num].name);	
					sprintf(&file_load[file_name_num].name[strlen(file_load[file_name_num].name)-4], "%s",' '); 			
					file_name_num++;
				}//else
			}else if(mode == 0)
			{
				//Ŀ¼���ݹ��ȡ      
				if (fno.fattrib & AM_DIR)         
				{ 			
					//�ϳ�����Ŀ¼��        
					sprintf(&path[i], "/%s", fn); 		
					//�ݹ����         
					res = scan_files(path,0);	
					path[i] = 0;         
					//��ʧ�ܣ�����ѭ��        
					if (res != FR_OK) 
						break; 
				} 
				else 
				{ 
					if(f_typetell(fn) == 0x01)
					{
						strcpy((char *)file_load[file_name_num].path,path);	
						strcpy((char *)file_load[file_name_num].name,fn);									//����ļ���	
						file_load[file_name_num].dir = 0;
						file_load[file_name_num].type = 0x01;
						sprintf(&file_load[file_name_num].name[strlen(file_load[file_name_num].name)-4], "%s",' '); 
						file_name_num++;
					}			
				}//else
			}
    } //for
  } 
  return res; 
}

//��ʾʣ������
//drv:�̷�
//����ֵ:ʣ������(�ֽ�)
u32 mf_showfree(u8 *drv, u32 *total)
{
	FATFS *fs1;
	u8 res;
  u32 fre_clust=0, fre_sect=0, tot_sect=0;
  //�õ�������Ϣ�����д�����
  res = f_getfree((const TCHAR*)drv,(DWORD*)&fre_clust, &fs1);	
  if(res==0)
	{											   
	    tot_sect = (fs1->n_fatent - 2) * fs1->csize;//�õ���������
	    fre_sect = fre_clust * fs1->csize;			//�õ�����������	   
		
#if _MAX_SS!=512
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
//		if(tot_sect<20480)//������С��10M
//		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		
//		    printf("\r\n����������:%d KB\r\n"
//		           "���ÿռ�:%d KB\r\n",
//		           tot_sect>>1,fre_sect>>1);
		
//		}else
//		{
//		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
//		    printf("\r\n����������:%d MB\r\n"
//		           "���ÿռ�:%d MB\r\n",
//		           tot_sect>>11,fre_sect>>11);
//		}
			(*total) = tot_sect>>1;
	}
	return (fre_sect>>1);
}

//����д��ĸתΪСд��ĸ,���������,�򱣳ֲ���.
u8 char_lower(u8 c)
{
	if(c<'A')return c;//����,���ֲ���.
	
	if(c>='A'&&c<='Z')return c+0x20;//��ΪСд.
		else return c;//Сд,���ֲ���
}	

//�����ļ�������
//fname:�ļ���
//����ֵ:0XFF,��ʾ�޷�ʶ����ļ����ͱ��.
//		 ����,����λ��ʾ��������,����λ��ʾ����С��.
u8 f_typetell(char *fname)
{
	
	u8 tbuf[5];
	char *attr='\0';//��׺��
	u8 i=0;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;//ƫ�Ƶ��������.
		fname++;
	}
	if(i==250)return 0XFF;//������ַ���.
 	for(i=0;i<5;i++)//�õ���׺��
	{
		fname--;
		if(*fname=='.')
		{
			fname++;
			attr=fname;
			break;
		}
  }
	strcpy((char *)tbuf,(const char*)attr);//copy
 	for(i=0;i<4;i++)tbuf[i]=char_lower(tbuf[i]);//ȫ����ΪСд 
	for(i=0;i<FILE_MAX_TYPE_NUM;i++)	//�Ա�
	{
		if(*FILE_TYPE_TBL[i]==0)break;//�����Ѿ�û�пɶԱȵĳ�Ա��.
		if(strcmp((const char *)FILE_TYPE_TBL[i],(const char *)tbuf)==0)//�ҵ���
		{
			return 0x01<<i;
		}
	}
	return 0XFF;//û�ҵ�		 			   
}	 

//ɾ���ļ��м�����·���ļ�
FRESULT f_deldir(const TCHAR *path)
{
    FRESULT res;
    DIR   dir;     /* �ļ��������� */ //36  bytes
    FILINFO fno;   /* �ļ����� */   //32  bytes
    TCHAR file[_MAX_LFN + 2] = {0};
#if _USE_LFN
    TCHAR lname[_MAX_LFN + 2] = {0};
#endif
    
#if _USE_LFN
    fno.lfsize = _MAX_LFN;
    fno.lfname = lname;    //���븳��ֵ
#endif
    //���ļ���
    res = f_opendir(&dir, path);
    
    //������ȡ�ļ�������
    while((res == FR_OK) && (FR_OK == f_readdir(&dir, &fno)))
    {
        //����"."��".."�ļ��У�����
        if(0 == strlen(fno.fname))          break;      //���������ļ���Ϊ��
        if(0 == strcmp(fno.fname, "."))     continue;   //���������ļ���Ϊ��ǰ�ļ���
        if(0 == strcmp(fno.fname, ".."))    continue;   //���������ļ���Ϊ��һ���ļ���
        
        memset(file, 0, sizeof(file));
#if _USE_LFN
        sprintf((char*)file, "%s/%s", path, (*fno.lfname) ? fno.lfname : fno.fname);
#else
        sprintf((char*)file, "%s/%s", path, fno.fname);
#endif
        if (fno.fattrib & AM_DIR)
        {//�����ļ��У��ݹ�ɾ��
            res = f_deldir(file);
        }
        else
        {//�����ļ���ֱ��ɾ��
            res = f_unlink(file);
        }
    }
    
    //ɾ������
    if(res == FR_OK)    res = f_unlink(path);
    
    return res;
}

//�ļ�ϵͳ����
void filetest(u8 button)
{
	u8 i;
	u32 total;
//	FATFS *pfs;
//	FILINFO fno;
	DIR dir;
//  DWORD fre_clust, fre_sect, tot_sect;
	
//	printf("button:%x\r\n",button);
	
	switch(button)
	{
		case 1:                                                                  //ɨ���ļ�
			printf("%s \r\n", fpath);
		
			strcpy(fpath,"1:");
			for(i=0; i<file_name_num; i++)
			{
				strcpy(file_load[i].name,"");
				file_load[i].dir=0;
				file_load[i].type=0;
			}
			file_name_num = 0;
			scan_files(fpath,0);
		
			printf("%s \r\n", fpath);
		
//			sprintf(&fpath[strlen(fpath)], "/%s", file_load[0].name); 
//			scan_files(fpath,1);
//		
//			printf("%s \r\n", fpath);
			break;
		
		case 2:                                                                  //FLASH����
			mf_showfree("1:",&total);
			break;
		
		case 3:                                                                  //ɾ���ļ�
			res_flash = f_unlink ("1:Project/�ֲ�ʿ�����ļ�.dbf");
			if(res_flash==FR_OK)
			{
				printf("��ɾ���ɹ���\n");
			}else
			{
				printf("��ɾ��ʧ�ܡ�\r\n");
			}
			break;
		
		case 4:                                                                  //������
			res_flash=f_rename("1:Project/�ֲ�ʿ�����ļ�3.dsf","1:Project/�ֲ�ʿ�����ļ�3.mp3");      
			if(res_flash==FR_OK)
			{
				printf("���������ļ��ɹ���\r\n");
			}else
			{
				printf("���������ļ�ʧ�ܡ�\r\n");
			}
			break;
		
		case 5:                                                                  //

			break;
		
		case 6:                                                                  //д���ļ�
			res_flash = f_open(&file1, "1:Project/�ֲ�ʿ�����ļ�1.dbf",FA_CREATE_ALWAYS | FA_WRITE );
			if ( res_flash == FR_OK )
			{
				printf("����/�����ļ��ɹ������ļ�д�����ݡ�\r\n");
				/* ��ָ���洢������д�뵽�ļ��� */
				res_flash=f_write(&file1,myWriteBuffer,sizeof(myWriteBuffer),&fnum);
				if(res_flash==FR_OK)
				{
					printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
					printf("�����ļ�д�������Ϊ��\r\n%s\r\n",myWriteBuffer);
				}
				else
				{
					printf("�����ļ�д��ʧ�ܣ�(%d)\n",res_flash);
				}    
				/* ���ٶ�д���ر��ļ� */
				f_close(&file1);
			}
			else
			{	
				printf("������/�����ļ�ʧ�ܡ�\r\n");
			}
			res_flash = f_open(&file1, "1:Project/�ֲ�ʿ�����ļ�2.dpf",FA_CREATE_ALWAYS | FA_WRITE );
			if ( res_flash == FR_OK )
			{
				printf("����/�����ļ��ɹ������ļ�д�����ݡ�\r\n");
				/* ��ָ���洢������д�뵽�ļ��� */
				res_flash=f_write(&file1,myWriteBuffer,sizeof(myWriteBuffer),&fnum);
				if(res_flash==FR_OK)
				{
					printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
					printf("�����ļ�д�������Ϊ��\r\n%s\r\n",myWriteBuffer);
				}
				else
				{
					printf("�����ļ�д��ʧ�ܣ�(%d)\n",res_flash);
				}    
				/* ���ٶ�д���ر��ļ� */
				f_close(&file1);
			}
			else
			{	
				printf("������/�����ļ�ʧ�ܡ�\r\n");
			}
			res_flash = f_open(&file1, "1:Project/�ֲ�ʿ�����ļ�3.mp3",FA_CREATE_ALWAYS | FA_WRITE );
			if ( res_flash == FR_OK )
			{
				printf("����/�����ļ��ɹ������ļ�д�����ݡ�\r\n");
				/* ��ָ���洢������д�뵽�ļ��� */
				res_flash=f_write(&file1,myWriteBuffer,sizeof(myWriteBuffer),&fnum);
				if(res_flash==FR_OK)
				{
					printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
					printf("�����ļ�д�������Ϊ��\r\n%s\r\n",myWriteBuffer);
				}
				else
				{
					printf("�����ļ�д��ʧ�ܣ�(%d)\n",res_flash);
				}    
				/* ���ٶ�д���ر��ļ� */
				f_close(&file1);
			}
			else
			{	
				printf("������/�����ļ�ʧ�ܡ�\r\n");
			}
			res_flash = f_open(&file1, "1:Project/�ֲ�ʿ�����ļ�4.dbf",FA_CREATE_ALWAYS | FA_WRITE );
			if ( res_flash == FR_OK )
			{
				printf("����/�����ļ��ɹ������ļ�д�����ݡ�\r\n");
				/* ��ָ���洢������д�뵽�ļ��� */
				res_flash=f_write(&file1,myWriteBuffer,sizeof(myWriteBuffer),&fnum);
				if(res_flash==FR_OK)
				{
					printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
					printf("�����ļ�д�������Ϊ��\r\n%s\r\n",myWriteBuffer);
				}
				else
				{
					printf("�����ļ�д��ʧ�ܣ�(%d)\n",res_flash);
				}    
				/* ���ٶ�д���ر��ļ� */
				f_close(&file1);
			}
			else
			{	
				printf("������/�����ļ�ʧ�ܡ�\r\n");
			}
			res_flash = f_open(&file1, "1:Project/�ֲ�ʿ�����ļ�5.dbf",FA_CREATE_ALWAYS | FA_WRITE );
			if ( res_flash == FR_OK )
			{
				printf("����/�����ļ��ɹ������ļ�д�����ݡ�\r\n");
				/* ��ָ���洢������д�뵽�ļ��� */
				res_flash=f_write(&file1,myWriteBuffer,sizeof(myWriteBuffer),&fnum);
				if(res_flash==FR_OK)
				{
					printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
					printf("�����ļ�д�������Ϊ��\r\n%s\r\n",myWriteBuffer);
				}
				else
				{
					printf("�����ļ�д��ʧ�ܣ�(%d)\n",res_flash);
				}    
				/* ���ٶ�д���ر��ļ� */
				f_close(&file1);
			}
			else
			{	
				printf("������/�����ļ�ʧ�ܡ�\r\n");
			}
			res_flash = f_open(&file1, "1:Project/�ֲ�ʿ�����ļ�6.dbf",FA_CREATE_ALWAYS | FA_WRITE );
			if ( res_flash == FR_OK )
			{
				printf("����/�����ļ��ɹ������ļ�д�����ݡ�\r\n");
				/* ��ָ���洢������д�뵽�ļ��� */
				res_flash=f_write(&file1,myWriteBuffer,sizeof(myWriteBuffer),&fnum);
				if(res_flash==FR_OK)
				{
					printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
					printf("�����ļ�д�������Ϊ��\r\n%s\r\n",myWriteBuffer);
				}
				else
				{
					printf("�����ļ�д��ʧ�ܣ�(%d)\n",res_flash);
				}    
				/* ���ٶ�д���ر��ļ� */
				f_close(&file1);
			}
			else
			{	
				printf("������/�����ļ�ʧ�ܡ�\r\n");
			}
			res_flash = f_open(&file1, "1:Project/�ֲ�ʿ�����ļ�7.dbf",FA_CREATE_ALWAYS | FA_WRITE );
			if ( res_flash == FR_OK )
			{
				printf("����/�����ļ��ɹ������ļ�д�����ݡ�\r\n");
				/* ��ָ���洢������д�뵽�ļ��� */
				res_flash=f_write(&file1,myWriteBuffer,sizeof(myWriteBuffer),&fnum);
				if(res_flash==FR_OK)
				{
					printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
					printf("�����ļ�д�������Ϊ��\r\n%s\r\n",myWriteBuffer);
				}
				else
				{
					printf("�����ļ�д��ʧ�ܣ�(%d)\n",res_flash);
				}    
				/* ���ٶ�д���ر��ļ� */
				f_close(&file1);
			}
			else
			{	
				printf("������/�����ļ�ʧ�ܡ�\r\n");
			}
			res_flash = f_open(&file1, "1:Project/�ֲ�ʿ�����ļ�8.dbf",FA_CREATE_ALWAYS | FA_WRITE );
			if ( res_flash == FR_OK )
			{
				printf("����/�����ļ��ɹ������ļ�д�����ݡ�\r\n");
				/* ��ָ���洢������д�뵽�ļ��� */
				res_flash=f_write(&file1,myWriteBuffer,sizeof(myWriteBuffer),&fnum);
				if(res_flash==FR_OK)
				{
					printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
					printf("�����ļ�д�������Ϊ��\r\n%s\r\n",myWriteBuffer);
				}
				else
				{
					printf("�����ļ�д��ʧ�ܣ�(%d)\n",res_flash);
				}    
				/* ���ٶ�д���ر��ļ� */
				f_close(&file1);
			}
			else
			{	
				printf("������/�����ļ�ʧ�ܡ�\r\n");
			}
			break;
		
		case 7:                                                                  //��ȡ�ļ�
				res_flash = f_open(&file1, "1:Project/�ֲ�ʿ�����ļ�.dbf", FA_OPEN_EXISTING | FA_READ); 	 
				if(res_flash == FR_OK)
				{
					printf("�����ļ��ɹ���\r\n");
					res_flash = f_read(&file1, myReadBuffer,  file1.fsize, &fnum); 
					if(res_flash==FR_OK)
					{
						printf("���ļ���ȡ�ɹ�,�����ֽ����ݣ�%d\r\n",fnum);
						printf("����ȡ�õ��ļ�����Ϊ��\r\n%s \r\n", myReadBuffer);	
					}
					else
					{
						printf("�����ļ���ȡʧ�ܣ�(%d)\n",res_flash);
					}		
					/* ���ٶ�д���ر��ļ� */
					f_close(&file1);	
				}
				else
				{
					printf("�������ļ�ʧ�ܡ�\r\n");
				}
			break;
		
		case 8:                                                                  //����Ŀ¼
			/* ���Դ�Ŀ¼ */
			res_flash=f_opendir(&dir,"1:Project");
			if(res_flash!=FR_OK)
			{
				/* ��Ŀ¼ʧ�ܣ�����Ŀ¼ */
				res_flash=f_mkdir("1:Project");
				printf("����Ŀ¼ʧ�ܣ��ʹ���Ŀ¼��\n");
			}
			else
			{
				/* ���Ŀ¼�Ѿ����ڣ��ر��� */
				res_flash=f_closedir(&dir);
				printf("��Ŀ¼�Ѵ��ڣ��ر�Ŀ¼��\n");
			}
			/* ���Դ�Ŀ¼ */
			res_flash=f_opendir(&dir,"1:Project1");
			if(res_flash!=FR_OK)
			{
				/* ��Ŀ¼ʧ�ܣ�����Ŀ¼ */
				res_flash=f_mkdir("1:Project1");
				printf("����Ŀ¼ʧ�ܣ��ʹ���Ŀ¼��\n");
			}
			else
			{
				/* ���Ŀ¼�Ѿ����ڣ��ر��� */
				res_flash=f_closedir(&dir);
				printf("��Ŀ¼�Ѵ��ڣ��ر�Ŀ¼��\n");
			}
			/* ���Դ�Ŀ¼ */
			res_flash=f_opendir(&dir,"1:Project2");
			if(res_flash!=FR_OK)
			{
				/* ��Ŀ¼ʧ�ܣ�����Ŀ¼ */
				res_flash=f_mkdir("1:Project2");
				printf("����Ŀ¼ʧ�ܣ��ʹ���Ŀ¼��\n");
			}
			else
			{
				/* ���Ŀ¼�Ѿ����ڣ��ر��� */
				res_flash=f_closedir(&dir);
				printf("��Ŀ¼�Ѵ��ڣ��ر�Ŀ¼��\n");
			}
			/* ���Դ�Ŀ¼ */
			res_flash=f_opendir(&dir,"1:Project3");
			if(res_flash!=FR_OK)
			{
				/* ��Ŀ¼ʧ�ܣ�����Ŀ¼ */
				res_flash=f_mkdir("1:Project3");
				printf("����Ŀ¼ʧ�ܣ��ʹ���Ŀ¼��\n");
			}
			else
			{
				/* ���Ŀ¼�Ѿ����ڣ��ر��� */
				res_flash=f_closedir(&dir);
				printf("��Ŀ¼�Ѵ��ڣ��ر�Ŀ¼��\n");
			}
			/* ���Դ�Ŀ¼ */
			res_flash=f_opendir(&dir,"1:Project4");
			if(res_flash!=FR_OK)
			{
				/* ��Ŀ¼ʧ�ܣ�����Ŀ¼ */
				res_flash=f_mkdir("1:Project4");
				printf("����Ŀ¼ʧ�ܣ��ʹ���Ŀ¼��\n");
			}
			else
			{
				/* ���Ŀ¼�Ѿ����ڣ��ر��� */
				res_flash=f_closedir(&dir);
				printf("��Ŀ¼�Ѵ��ڣ��ر�Ŀ¼��\n");
			}/* ���Դ�Ŀ¼ */
			res_flash=f_opendir(&dir,"1:Project5");
			if(res_flash!=FR_OK)
			{
				/* ��Ŀ¼ʧ�ܣ�����Ŀ¼ */
				res_flash=f_mkdir("1:Project5");
				printf("����Ŀ¼ʧ�ܣ��ʹ���Ŀ¼��\n");
			}
			else
			{
				/* ���Ŀ¼�Ѿ����ڣ��ر��� */
				res_flash=f_closedir(&dir);
				printf("��Ŀ¼�Ѵ��ڣ��ر�Ŀ¼��\n");
			}/* ���Դ�Ŀ¼ */
			res_flash=f_opendir(&dir,"1:Project6");
			if(res_flash!=FR_OK)
			{
				/* ��Ŀ¼ʧ�ܣ�����Ŀ¼ */
				res_flash=f_mkdir("1:Project6");
				printf("����Ŀ¼ʧ�ܣ��ʹ���Ŀ¼��\n");
			}
			else
			{
				/* ���Ŀ¼�Ѿ����ڣ��ر��� */
				res_flash=f_closedir(&dir);
				printf("��Ŀ¼�Ѵ��ڣ��ر�Ŀ¼��\n");
			}
			break;
		
		case 9:                                                                  //��ʽ��		
			printf("�����ڸ�ʽ���ļ�ϵͳ��\r\n");
			res_flash=f_mkfs("1:",0,0);							
			if(res_flash == FR_OK)
			{
				printf("��FLASH�ѳɹ���ʽ���ļ�ϵͳ��\r\n");
				/* ��ʽ������ȡ������ */
				res_flash = f_mount(NULL,"1:",1);			
				/* ���¹���	*/			
				res_flash = f_mount(&fs,"1:",1);
				printf("�����¹����ļ�ϵͳ��\r\n");
			}else
			{
				printf("��FLASH��ʽ���ļ�ϵͳʧ�ܡ�\r\n");
			}
			break;
		
		default:
			break;
	}
}


