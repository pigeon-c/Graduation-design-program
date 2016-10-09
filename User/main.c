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



/*时间结构体*/
struct rtc_time systmtime;

//温湿度结构体
DHT11_Data_TypeDef DHT11_Data;

 //文本题目
BYTE textFileBuffer[] = "环境监测数据记录--曹鹏飞\n\n";
 



int main(void)
{
		uint8_t k;
	
		/* 初始化LCD */
		LCD_Init();	
		LCD_Clear(0, 0, 240, 320, BACKGROUND);		
		
		/* 初始化sd卡文件系统，因为汉字的字库放在了sd卡里面 */
		Sd_fs_init();	
	
		/* 初始化串口 */
	  USART1_Config();
	
		/* 系统定时器 1us 定时初始化 */
	 	SysTick_Init();				
	 
		/* 初始化LED */
		LED_GPIO_Config();	
	
		/* 初始化外部FLASH */
		SPI_FLASH_Init();
		//SPI_FLASH_SectorErase(0);
  
		/* 触摸屏IO和中断初始化 */
		Touch_Init();			
		
		/*初始化DTT11的引脚*/
	DHT11_GPIO_Config();
	
  /* 配置RTC秒中断优先级 */
  RTC_NVIC_Config();	
	//配置RTC
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
			/* 等待触摸屏校正完毕 */
			while(Touch_Calibrate() !=0);
		}
		#elif 0
		/* 等待触摸屏校正完毕 */
		while(Touch_Calibrate() !=0);
		#endif
	
		
		
		
		
		/* 显示BMP图片 */
		Lcd_show_bmp(0,0 ,"/myscreen.bmp");
			
		f_mount(0,&fs);
	 /* Create new file on the drive 0 */
		
		
	/*先创建文件 写进标题	*/
		res = f_open(&fnew, "0:file.txt", FA_CREATE_ALWAYS | FA_WRITE );
		if ( res == FR_OK )
		{
			res = f_write(&fnew, textFileBuffer, sizeof(textFileBuffer), &bw);
			f_close(&fnew);      
		}
		 f_mount(0, NULL);
		
		/*改变现实模式*/
		Lcd_GramScan( 1 );                      
		
		/*等待中段*/
	  Time_Show(&systmtime);
	 
			
	

}


