#include "even_process.h"
#include "bsp_led.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_touch.h"
#include "bsp_usart1.h"
#include "bsp_bmp.h"
#include <stdio.h>
#include "bsp_dht11.h"



extern DHT11_Data_TypeDef DHT11_Data;



EVEN even = E_GETCHAR;								//事件标志
BUTTON_STATE red_button 		= S_OFF;	//按键状态 红
BUTTON_STATE green_button 	= S_OFF;	//按键状态 绿
BUTTON_STATE yellow_button 	= S_OFF;	//按键状态 黄

//extern uint8_t recv_char;							//串口接收的字符串


void even_process(void)
{
	char lcd_char[20];
	switch(even)
	{
		case E_BUTTON1:									//LED1							
		
				if(red_button == S_OFF)		//原来为关状态
				{
					Lcd_show_bmp(38, 184,"/led/ui_r_button.bmp");			//显示开状态按钮
					red_button = S_ON;																//更新状态标志
					
				  Lcd_GramScan( 1 );                       //改变现实模式
					if(DHT11_Data.humi_int<=30)
							LCD_DispEnCh(10, 190,(uint8_t *)"湿度:较为干燥注意加湿",YELLOW);
					if(DHT11_Data.humi_int>30 &&  DHT11_Data.humi_int<50) 
							LCD_DispEnCh(10, 190,(uint8_t *)"湿度:较为舒适神情气爽",YELLOW);
					if(DHT11_Data.humi_int>=50)
							LCD_DispEnCh(10, 190,(uint8_t *)"湿度:较为潮湿注意通风",YELLOW);
				}
				else											//原来为开状态
				{
					Lcd_show_bmp(38, 184,"/led/ui_rb_button.bmp");		//显示关状态按钮
					red_button = S_OFF;																//更新状态标志
					
					Lcd_GramScan( 1 );                       //改变现实模式
					if(DHT11_Data.humi_int<=30)
							LCD_DispEnCh(10, 190,(uint8_t *)"湿度:较为干燥注意加湿",BLACK);
					if(DHT11_Data.humi_int>30 &&  DHT11_Data.humi_int<50) 
							LCD_DispEnCh(10, 190,(uint8_t *)"湿度:较为舒适神情气爽",BLACK);
					if(DHT11_Data.humi_int>=50)
							LCD_DispEnCh(10, 190,(uint8_t *)"湿度:较为潮湿注意通风",BLACK);
				}        
				
				LED1_TOGGLE;							//LED状态反转
					
				even = E_NULL;
				
	  break;
		
				
				
				
		case E_BUTTON2:										//LED2
			
				if(green_button == S_OFF)		//原来为关状态
				{
					Lcd_show_bmp(38, 95,"/led/ui_g_button.bmp");			//显示开状态按钮
					green_button = S_ON;															//更新状态标志
					
					Lcd_GramScan( 1 );                       //改变现实模式
					if(DHT11_Data.temp_int<=15)
					LCD_DispEnCh(10, 190,(uint8_t *)"温度:比较严寒加厚衣物",YELLOW);
					if(DHT11_Data.temp_int>15 && DHT11_Data.temp_int<=18)
					LCD_DispEnCh(10, 190,(uint8_t *)"温度:较为严寒注意保暖",YELLOW);
					if(DHT11_Data.temp_int>18 && DHT11_Data.temp_int<=28)
					LCD_DispEnCh(10, 190,(uint8_t *)"温度:较为舒适心情舒畅",YELLOW);
					if(DHT11_Data.temp_int>28)
					LCD_DispEnCh(10, 190,(uint8_t *)"温度:较为闷热注意避暑",YELLOW);
				}
				else											//原来为开状态
				{
					Lcd_show_bmp(38, 95,"/led/ui_gb_button.bmp");			//显示关状态按钮
					green_button = S_OFF;															//更新状态标志
					Lcd_GramScan( 1 );                       //改变现实模式
					
						Lcd_GramScan( 1 );                       //改变现实模式
					if(DHT11_Data.temp_int<=15)
					LCD_DispEnCh(10, 190,(uint8_t *)"温度:比较严寒加厚衣物",BLACK);
					if(DHT11_Data.temp_int>15 && DHT11_Data.temp_int<=18)
					LCD_DispEnCh(10, 190,(uint8_t *)"温度:较为严寒注意保暖",BLACK);
					if(DHT11_Data.temp_int>18 && DHT11_Data.temp_int<=28)
					LCD_DispEnCh(10, 190,(uint8_t *)"温度:较为舒适心情舒畅",BLACK);
					if(DHT11_Data.temp_int>28)
					LCD_DispEnCh(10, 190,(uint8_t *)"温度:较为闷热注意避暑",BLACK);
					}
		
				LED2_TOGGLE;
				
				even = E_NULL;
		
		break;
		
		case E_BUTTON3:											//LED3
			
				if(yellow_button == S_OFF)		//原来为关状态
				{
					Lcd_show_bmp(38, 11,"/led/ui_y_button.bmp");			//显示开状态按钮
					yellow_button = S_ON;															//更新状态标志
						Lcd_GramScan( 1 );                       //改变现实模式
					LCD_DispEnCh(10, 190,(uint8_t *)"颗粒:空气较好可以外出",YELLOW);
	
   
				}
				else											//原来为开状态
				{
					Lcd_show_bmp(38, 11,"/led/ui_yb_button.bmp");			//显示关状态按钮
					yellow_button = S_OFF;														//更新状态标志
						Lcd_GramScan( 1 );                       //改变现实模式
					LCD_DispEnCh(10, 190,(uint8_t *)"颗粒:空气较好可以外出",BLACK);
				
					}	
		
				LED3_TOGGLE;
		
				even = E_NULL;
		
		break;

		default:
			break;
	}
}


/**
  * @brief  touch_process中断服务函数，根据触摸位置更新触摸标志
  * @param  无  
  * @retval 无
  */
void touch_process(void)
{
	 if(touch_flag == 1)															/*如果触笔按下了*/
    {		
      /* 获取点的坐标 */
      if(Get_touch_point(&display, Read_2046_2(), &touch_para ) !=DISABLE)      
      {		
			//	printf("\r\n x=%d,y=%d",display.x,display.y);
				
				if(display.x>=34 && display.x<=76)						//控制条的范围
				{
						if(display.y>=15 && display.y<=46)				// LED1
						{
								even = E_BUTTON1;
					//			printf("\r\n LED1");
					  }
						else if(display.y>=100 && display.y<=135)	// LED2
						{
								even = E_BUTTON2;
							//	printf("\r\n LED2");
						}
						else if(display.y>=182 && display.y<=216)	// LED3
						{
								even = E_BUTTON3;
							//	printf("\r\n LED3");
						}	
				}
			}				
		}
}


/* ----------------------------------------end of file --------------------------------------------*/

