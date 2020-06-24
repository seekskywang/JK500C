#include "stm32f4xx.h"
#include "ff.h"
#include "jk508.h"
#include "./lcd/bsp_lcd.h"
#include "sdio/bsp_sdio_sd.h"


char filename[20];
char foldername[20];
FILINFO hisinfo;
DIR dir;
FATFS fs;													/* FatFs文件系统对象 */
FIL fnew;													/* 文件对象 */
FRESULT res_sd;                /* 文件操作结果 */
UINT fnum;            					/* 文件成功读写数量 */
u8 WriteBuffer[512];        /* 读缓冲区 */
//u8 ReadBuffer[512];        /* 读缓冲区 */
char Dirname[MAXDIR][13];
char Filelist[MAXFILE][13];
u8 maxhispage,dirflag,maxfilepage;
u16 foldernum,filenum;
//BYTE WriteBuffer[] =              /* 写缓冲区*/
BLOCK_REC BlockNum;
SAVE_SD SaveBuffer;
SAVE_SD ReadBuffer;
uint64_t sizewatch;
//加载SD卡
void Mount_SD(void)
{
	res_sd = f_mount(&fs,"0:",1);
	if(res_sd == FR_NO_FILESYSTEM)
	{
		DrawInstruction("加载文件系统失败，请重新格式化");
	}else{
		DrawInstruction("加载文件系统成功");
	}
}
void Read_Block_Rec(void)
{
	SD_ReadBlock((uint8_t *)&BlockNum,0,512);
	SD_WaitWriteOperation();
	while(SD_GetStatus() != SD_TRANSFER_OK);
}

void Write_His_Data(void)
{
	SD_WriteBlock((uint8_t *)&BlockNum,0,512);
	SD_WaitWriteOperation();	
	while(SD_GetStatus() != SD_TRANSFER_OK);
	SD_WriteMultiBlocks((uint8_t *)&SaveBuffer,512+sizeof(SaveBuffer)*BlockNum.Num[0],512,sizeof(SaveBuffer)/512);
	SD_WaitWriteOperation();	
	while(SD_GetStatus() != SD_TRANSFER_OK);
	BlockNum.Num[0]++;
}

void Search_Handle(void)
{
	u16 i;
	
}

void Read_His_Data(u32 block)
{	
//	sizewatch = 512+(sizeof(SaveBuffer)*block);
	SD_ReadMultiBlocks((uint8_t *)&ReadBuffer,512+(sizeof(SaveBuffer)*block),512,sizeof(SaveBuffer)/512);
	SD_WaitWriteOperation();
	while(SD_GetStatus() != SD_TRANSFER_OK);
	
}
//读取历史文件列表
void READ_HIS(void)
{
	static u8 i;
	char buf[5];
	
	res_sd = f_opendir(&dir,Dirname[(hispage-1)*10+(dirflag-1)]);
//	res_sd = f_opendir(&dir, "");
	if(res_sd == FR_OK)
	{
		for(i=0;i<100;i++)
		{
			res_sd = f_readdir(&dir,&hisinfo);
			if(hisinfo.fname[0] == 0)
			{		
				maxfilepage = i/10+1;
				DISP_HIS_FILE();
				filenum = i;
				break;
			}
			strncpy(Filelist[i],hisinfo.fname,13);
		}	
		DrawInstruction("读取历史数据成功");
	}else{
		DrawInstruction("读取历史数据失败");
	}
	DISP_HIS_FILE();
	i = 0;
}

//读取文件夹列表
void READ_HIS_FOLDER(void)
{
	static u16 i;	
	char buf[5];

	
	res_sd = f_opendir(&dir, "");
	if(res_sd == FR_OK)
	{					
		for(i=0;i<300;i++)
		{
			res_sd = f_readdir(&dir,&hisinfo);
			if(hisinfo.fname[0] == 0)
			{
				maxhispage = i/10+1;
				DISP_HIS_FOLDER();
				foldernum = i;
				break;
			}
			if(hisinfo.fattrib&AM_DIR)
			{
				strncpy(Dirname[i],hisinfo.fname,13);
			}
		}
		DrawInstruction("读取目录成功");
	}else{
		DrawInstruction("读取目录失败");
	}
	i = 0;
	Drawhishmenu();
	page_flag = history;
}

//显示历史文件
void DISP_HIS_FILE(void)
{
	char buf[10],num[5];
	u8 i;
	
//	LCD_SetColors(LCD_COLOR_BACK,LCD_COLO R_BACK);
//	LCD_DrawFullRect(10,40,100,400);
	
	for(i=0;i<10;i++)
	{
		LCD_SetColors(LCD_COLOR_BACK,LCD_COLOR_BACK);
		LCD_DrawFullRect(70,40+i*35,200,32);
		LCD_SetColors(LCD_COLOR_LIGHTBLUE,LCD_COLOR_BACK);
		sprintf(num,"%d",i+1);
		LCD_DisplayStringLine(40+i*35,10,(uint8_t *)num);
		LCD_DisplayStringLine(40+i*35,50,(uint8_t *)".");
		if(i+1 == dirflag)
		{
			LCD_SetColors(LCD_COLOR_BLACK,LCD_COLOR_YELLOW);			
		}else{
			LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_BACK);
		}
		LCD_DisplayStringLine(40+i*35,70,(uint8_t *)Filelist[i+10*(hispage-1)]);
	}
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_BACK);
	sprintf(buf,"%d/%d",hispage,maxfilepage);
	LCD_DisplayStringLine(400,500,(uint8_t *)buf);
	page_flag = hisfile;
	Drawfilemenu();
}

//显示历史数据文件夹
void DISP_HIS_FOLDER(void)
{
	char buf[10],num[5];
	u8 i;
	
	for(i=0;i<10;i++)
	{

		LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_BACK);
		sprintf(num,"%d",i+1);
		LCD_DisplayStringLine(40+i*35,10,(uint8_t *)num);
		LCD_DisplayStringLine(40+i*35,50,(uint8_t *)".");
		if(i+1 == dirflag)
		{
			LCD_SetColors(LCD_COLOR_BLACK,LCD_COLOR_YELLOW);			
		}else{
			LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_BACK);
		}
		LCD_DisplayStringLine(40+i*35,70,(uint8_t *)Dirname[i+10*(hispage-1)]);
	}
	LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_BACK);
	sprintf(buf,"%d/%d",hispage,maxhispage);
	LCD_DisplayStringLine(400,500,(uint8_t *)buf);
}


//历史数据下一页
void His_Npage(void)
{
	if(hispage < maxhispage)
	{
		hispage ++;
	}
	dirflag = 1;
	DISP_HIS_FOLDER();	
}

//历史数据下一页
void His_Ppage(void)
{
	if(hispage > 1)
	{
		hispage --;
	}
	dirflag = 1;
	DISP_HIS_FOLDER();
}

//建立文件存储数据
void Create_His_File(void)
{
	static char namebuf[10];
	static u8 i;
	
	memcpy ((void *)filename,"0:",2);
	strcat((char *)filename,(char *)Dirname[(hispage-1)*10+(dirflag-1)]);
	sprintf(namebuf,"/%02d%02d%02d%d", 
			usbreadtime[4],
			usbreadtime[5],
			usbreadtime[6],
			i);
	strcat((char *)filename,(char *)namebuf);
	strcat((char *)filename,(char *)".xls");	
	res_sd = f_open(&fnew, (char *)filename,FA_CREATE_ALWAYS | FA_WRITE );
	if ( res_sd == FR_OK )
	{
		DrawInstruction("创建文件成功");
	}
	else
	{
		DrawInstruction("创建失败");
	}
	READ_HIS();
}

//格式化SD卡
void Format_SD(void)
{
	f_mkfs("0:",0,0);
}


//新建文件夹	
void Creat_New_Folder(void)
{
//	res_sd = f_mount(&fs,"0:",1);
	memset(filename,0,100);
	static u8  testcount;
	if(res_sd == FR_NO_FILESYSTEM)
	{
		DrawInstruction("加载文件系统失败，请重新格式化");
	}else{
//		DrawInstruction("加载文件系统成功");
	}
	sprintf(foldername,"0:/%02d%02d%02d", 
			usbreadtime[1],
			usbreadtime[2],
			usbreadtime[3]);
	res_sd = f_mkdir((char *)foldername);
	if(res_sd == FR_OK)
	{
		DrawInstruction("成功");
	}else if(res_sd == FR_EXIST){
		DrawInstruction("目录已存在");
	}else{
		DrawInstruction("失败");
	}
	testcount++;
	READ_HIS_FOLDER();
}

//自动存储数据
void SD_Save_His(void)
{
	
}
////打开指定文件夹
//void Open_Dir(void)
//{
//	memset(filename,0,100);
//	
//	sprintf(foldername,"0:/%02d%02d%02d", 
//			usbreadtime[1],
//			usbreadtime[2],
//			usbreadtime[3]);
//	res_sd = f_opendir(&dir,foldername);
//}

//删除指定文件夹
void Delete_file(char name)
{
	f_unlink(&name);
	
}

