#include "bsp_ili9341_lcd.h"
#include "bsp_rtc.h"
#include "bsp_dht11.h"
#include "ff.h"
#include <stdio.h>
#include <string.h>

#include <even_process.h>

 //�ı���Ŀ
BYTE a[] = "PM2.5:54\n";

char s1[20];  //����װ�¶�
char s2[20];  //����װʪ��
char s3[20];  //������������ʱ����

extern FIL fnew;													/* file objects */
extern FATFS fs;													/* Work area (file system object) for logical drives */
extern FRESULT res; 
extern UINT br, bw;            					/* File R/W count */



extern DHT11_Data_TypeDef DHT11_Data;
/* ���жϱ�־���������ж�ʱ��1����ʱ�䱻ˢ��֮����0 */
__IO uint32_t TimeDisplay = 0;



/*���ڣ���Ф������ASCII��*/
uint8_t const *WEEK_STR[] = {"Sunday", "Monday ", "Tuesday","Wednesday", "Thursday", "Friday", "Saturday"};



uint8_t const *zodiac_sign[] = {"��", "��", "ţ", "��", "��", "��", "��", "��", "��", "��", "��", "��"};


/*
 * ��������NVIC_Configuration
 * ����  ������RTC���жϵ����ж����ȼ�Ϊ1�������ȼ�Ϊ0
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
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
 * ��������RTC_CheckAndConfig
 * ����  ����鲢����RTC
 * ����  �����ڶ�ȡRTCʱ��Ľṹ��ָ��
 * ���  ����
 * ����  ���ⲿ����
 */
void RTC_CheckAndConfig(struct rtc_time *tm)
{  
		/*�ȴ��Ĵ���ͬ��*/
		RTC_WaitForSynchro();
		
		/*����RTC���ж�*/
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		
		/*�ȴ��ϴ�RTC�Ĵ���д�������*/
		RTC_WaitForLastTask();
	 
	  /* Clear reset flags */
	  RCC_ClearFlag();

}



/*
 * ��������Time_Display
 * ����  ����ʾ��ǰʱ��ֵ
 * ����  ��-TimeVar RTC����ֵ����λΪ s
 * ���  ����
 * ����  ���ڲ�����
 */	







void Time_Display(uint32_t TimeVar,struct rtc_time *tm)
{
	   
	   uint32_t BJ_TimeVar;
	   /*  �ѱ�׼ʱ��ת��Ϊ����ʱ��*/
	   BJ_TimeVar =TimeVar + 8*60*60;
	   to_tm(BJ_TimeVar, tm);/*�Ѷ�ʱ����ֵת��Ϊ����ʱ��*/	
    // LCD_DispStr(5, 200, (uint8_t *)"Now Time :", BLACK);
	
	//��ʾ������
     LCD_DisNum(20,0,tm->tm_year, YELLOW);
	   LCD_DispChar(45, 0, '-', YELLOW);
     LCD_DisNum(50,0,tm->tm_mon, YELLOW);
	   LCD_DispChar(55, 0, '-', YELLOW);
	   LCD_DisNum(60,0,tm->tm_mday, YELLOW);
	
	//��ʾʱ����
	   LCD_DisNum(90,0,tm->tm_hour, YELLOW);
	   LCD_DispChar(105, 0, ':', YELLOW); 
     LCD_DisNum(110,0,tm->tm_min, YELLOW);
	   LCD_DispChar(125, 0, ':', YELLOW);
	   LCD_DispStr(130, 0, (uint8_t *)"  ", BLACK);
	   LCD_DisNum(130,0,tm->tm_sec, YELLOW);
	
	
	//��ʾ����
	   LCD_DispStr(170, 0, (uint8_t *)WEEK_STR[tm->tm_wday], YELLOW);
	
	
	
	     
	/************************************��ʾ��ʪ�� PMֵ**************/
	
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
	
	/************************************�������ݵ�SD��****************/
	
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
 * ��������Time_Show
 * ����  ���ڳ����ն�����ʾ��ǰʱ��ֵ
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */ 


void Time_Show(struct rtc_time *tm)
{	 
	  while (1)
	  {
		   even_process();
	    /* ÿ��1s */
			 Lcd_GramScan( 1 );                           //�ı���ʾģʽ
			
			
	    if (TimeDisplay == 1)
	    {
				/* Display current time */
	      Time_Display( RTC_GetCounter(),tm); 	
				
	      TimeDisplay = 0;
	    }
	  }
}



/************************END OF FILE***************************************/
