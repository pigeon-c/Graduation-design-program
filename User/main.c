#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_sdfs_app.h"
#include "bsp_bmp.h"
#include "bsp_SysTick.h"
#include "bsp_dht11.h"
#include "bsp_sdio_sdcard.h"
#include "ff.h"
#include "bsp_rtc.h"
#include "bsp_touch.h"
#include "bsp_spi_flash.h"
#include "bsp_led.h" 
#include "even_process.h"



FIL fnew;													/* file objects */
FATFS fs;													/* Work area (file system object) for logical drives */
FRESULT res; 
UINT br, bw;            					/* File R/W count */



/*ʱ��ṹ��*/
struct rtc_time systmtime;

//��ʪ�Ƚṹ��
DHT11_Data_TypeDef DHT11_Data;

 //�ı���Ŀ
BYTE textFileBuffer[] = "����������ݼ�¼--������\n\n";
 



int main(void)
{
		uint8_t k;
	
		/* ��ʼ��LCD */
		LCD_Init();	
		LCD_Clear(0, 0, 240, 320, BACKGROUND);		
		
		/* ��ʼ��sd���ļ�ϵͳ����Ϊ���ֵ��ֿ������sd������ */
		Sd_fs_init();	
	
		/* ��ʼ������ */
	  USART1_Config();
	
		/* ϵͳ��ʱ�� 1us ��ʱ��ʼ�� */
	 	SysTick_Init();				
	 
		/* ��ʼ��LED */
		LED_GPIO_Config();	
	
		/* ��ʼ���ⲿFLASH */
		SPI_FLASH_Init();
		//SPI_FLASH_SectorErase(0);
  
		/* ������IO���жϳ�ʼ�� */
		Touch_Init();			
		
		/*��ʼ��DTT11������*/
	DHT11_GPIO_Config();
	
  /* ����RTC���ж����ȼ� */
  RTC_NVIC_Config();	
	//����RTC
	  RTC_CheckAndConfig(&systmtime);
	
	
	
		/*-------------------------------------------------------------------------------------------------------*/  
		#if 1
		SPI_FLASH_BufferRead(&cal_flag, 0, 1);
		if( cal_flag == 0x45 )
		{
			SPI_FLASH_BufferRead((void*)cal_p, 1, sizeof(cal_p));
			SPI_FLASH_CS_HIGH();
			for( k=0; k<6; k++ )
						printf("\r\n rx = %LF \r\n",cal_p[k]);
		}
		else
		{
			/* �ȴ�������У����� */
			while(Touch_Calibrate() !=0);
		}
		#elif 0
		/* �ȴ�������У����� */
		while(Touch_Calibrate() !=0);
		#endif
	
		
		
		
		
		/* ��ʾBMPͼƬ */
		Lcd_show_bmp(0,0 ,"/myscreen.bmp");
			
		f_mount(0,&fs);
	 /* Create new file on the drive 0 */
		
		
	/*�ȴ����ļ� д������	*/
		res = f_open(&fnew, "0:file.txt", FA_CREATE_ALWAYS | FA_WRITE );
		if ( res == FR_OK )
		{
			res = f_write(&fnew, textFileBuffer, sizeof(textFileBuffer), &bw);
			f_close(&fnew);      
		}
		 f_mount(0, NULL);
		
		/*�ı���ʵģʽ*/
		Lcd_GramScan( 1 );                      
		
		/*�ȴ��ж�*/
	  Time_Show(&systmtime);
	 
			
	

}


