#include "bsp_ili9341_lcd.h"
#include "bsp_rtc.h"
#include "bsp_dht11.h"
#include "ff.h"
#include <stdio.h>
#include <string.h>

#include <even_process.h>

 //文本题目
BYTE a[] = "PM2.5:54\n";

char s1[20];  //用来装温度
char s2[20];  //用来装湿度
char s3[20];  //用来放年月日时分秒

extern FIL fnew;													/* file objects */
extern FATFS fs;													/* Work area (file system object) for logical drives */
extern FRESULT res; 
extern UINT br, bw;            					/* File R/W count */



extern DHT11_Data_TypeDef DHT11_Data;
/* 秒中断标志，进入秒中断时置1，当时间被刷新之后清0 */
__IO uint32_t TimeDisplay = 0;



/*星期，生肖用文字ASCII码*/
uint8_t const *WEEK_STR[] = {"Sunday", "Monday ", "Tuesday","Wednesday", "Thursday", "Friday", "Saturday"};



uint8_t const *zodiac_sign[] = {"猪", "鼠", "牛", "虎", "兔", "龙", "蛇", "马", "羊", "猴", "鸡", "狗"};


/*
 * 函数名：NVIC_Configuration
 * 描述  ：配置RTC秒中断的主中断优先级为1，次优先级为0
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */

void RTC_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


/*
 * 函数名：RTC_CheckAndConfig
 * 描述  ：检查并配置RTC
 * 输入  ：用于读取RTC时间的结构体指针
 * 输出  ：无
 * 调用  ：外部调用
 */
void RTC_CheckAndConfig(struct rtc_time *tm)
{  
		/*等待寄存器同步*/
		RTC_WaitForSynchro();
		
		/*允许RTC秒中断*/
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		
		/*等待上次RTC寄存器写操作完成*/
		RTC_WaitForLastTask();
	 
	  /* Clear reset flags */
	  RCC_ClearFlag();

}



/*
 * 函数名：Time_Display
 * 描述  ：显示当前时间值
 * 输入  ：-TimeVar RTC计数值，单位为 s
 * 输出  ：无
 * 调用  ：内部调用
 */	







void Time_Display(uint32_t TimeVar,struct rtc_time *tm)
{
	   
	   uint32_t BJ_TimeVar;
	   /*  把标准时间转换为北京时间*/
	   BJ_TimeVar =TimeVar + 8*60*60;
	   to_tm(BJ_TimeVar, tm);/*把定时器的值转换为北京时间*/	
    // LCD_DispStr(5, 200, (uint8_t *)"Now Time :", BLACK);
	
	//显示年月日
     LCD_DisNum(20,0,tm->tm_year, YELLOW);
	   LCD_DispChar(45, 0, '-', YELLOW);
     LCD_DisNum(50,0,tm->tm_mon, YELLOW);
	   LCD_DispChar(55, 0, '-', YELLOW);
	   LCD_DisNum(60,0,tm->tm_mday, YELLOW);
	
	//显示时分秒
	   LCD_DisNum(90,0,tm->tm_hour, YELLOW);
	   LCD_DispChar(105, 0, ':', YELLOW); 
     LCD_DisNum(110,0,tm->tm_min, YELLOW);
	   LCD_DispChar(125, 0, ':', YELLOW);
	   LCD_DispStr(130, 0, (uint8_t *)"  ", BLACK);
	   LCD_DisNum(130,0,tm->tm_sec, YELLOW);
	
	
	//显示星期
	   LCD_DispStr(170, 0, (uint8_t *)WEEK_STR[tm->tm_wday], YELLOW);
	
	
	
	     
	/************************************显示温湿度 PM值**************/
	
	if( Read_DHT11(&DHT11_Data)==SUCCESS)
	{
		LCD_DispStr(10, 130, (uint8_t *)"humidity:   ", YELLOW);
		LCD_DisNum(64, 130, DHT11_Data.humi_int,YELLOW );
		
		LCD_DispStr(10, 150, (uint8_t *)"temperature:  ", YELLOW);
		
		LCD_DisNum(82, 150, DHT11_Data.temp_int, YELLOW);
	}
	else
	{
		//LCD_DispStr(10, 110, (uint8_t *)"Read DHT11 ERROR", WHITE);
	}
	
	
			LCD_DispStr(10, 170, (uint8_t *)"PM:   ", YELLOW);
		  LCD_DispStr(30, 170, (uint8_t *)"54   ", YELLOW);
	
	/************************************保存数据到SD卡****************/
	
		f_mount(0,&fs);
		res = f_open(&fnew, "0:file.txt",  FA_READ |  FA_WRITE); 	 	
		res = f_lseek(&fnew, f_size(&fnew));   

	  sprintf(s3,"%d/",tm->tm_year);
	  sprintf(s3+strlen(s3),"%d/",tm->tm_mon);
	  sprintf(s3+strlen(s3),"%d-",tm->tm_mday);
	  sprintf(s3+strlen(s3),"%d:",tm->tm_hour);
	  sprintf(s3+strlen(s3),"%d:",tm->tm_min);
	  sprintf(s3+strlen(s3),"%d\t",tm->tm_sec);
	
		res = f_write(&fnew, s3, sizeof(s3), &bw);
	  sprintf(s1,"temperature:%d\t",DHT11_Data.temp_int);
		res = f_write(&fnew, s1, sizeof(s1), &bw);     
		
	  sprintf(s2,"humidity:%d\t",DHT11_Data.humi_int);
		res = f_write(&fnew, s2, sizeof(s2), &bw);   
		
		res = f_write(&fnew, a, sizeof(a), &bw);   
		
		
		
		f_close(&fnew);	
	  f_mount(0, NULL);
	
}





/*
 * 函数名：Time_Show
 * 描述  ：在超级终端中显示当前时间值
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */ 


void Time_Show(struct rtc_time *tm)
{	 
	  while (1)
	  {
		   even_process();
	    /* 每过1s */
			 Lcd_GramScan( 1 );                           //改变显示模式
			
			
	    if (TimeDisplay == 1)
	    {
				/* Display current time */
	      Time_Display( RTC_GetCounter(),tm); 	
				
	      TimeDisplay = 0;
	    }
	  }
}



/************************END OF FILE***************************************/
