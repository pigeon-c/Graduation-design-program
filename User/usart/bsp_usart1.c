#include "bsp_usart1.h"
#include "even_process.h"
#include "bsp_led.h"
#include "stdio.h"
#include "bsp_dht11.h"

extern DHT11_Data_TypeDef DHT11_Data;

extern EVEN even;
uint8_t recv_char=0;

static void NVIC_Configuration(void);

 /**
  * @brief  USART1 GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void USART1_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		
		/* config USART1 clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
		
		/* USART1 GPIO config */
		/* Configure USART1 Tx (PA.09) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);    
		/* Configure USART1 Rx (PA.10) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
			
		/* USART1 mode config */
		USART_InitStructure.USART_BaudRate = 9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No ;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART1, &USART_InitStructure); 
		
			/* 使能串口1接收中断 */
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		
		NVIC_Configuration();
		
		USART_Cmd(USART1, ENABLE);
}


/// 配置USART1中断
static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  UsartReceive 串口接收中断服务函数，用来控制音量
  * @param  无
  * @retval 无
  */


void UsartReceive(void)
{
	//读取状态位
	if(USART_GetFlagStatus(USART1,USART_IT_RXNE)==SET)
	{	 
		USART_ClearFlag(USART1, USART_FLAG_RXNE | USART_FLAG_ORE);
		//读数据并清 RXNE、ORE位      
		recv_char = USART_ReceiveData(USART1);	
	}
	//根据接收到的命令进行判断执行相关的内容
	switch(recv_char)
	{
		/*接收到字符0传输空气中的温度到手机客户端*/
		case '0':
				printf("\r\n temperature= %d \r\n",DHT11_Data.temp_int);
			break;
			/*接收到字符1传输空气中的湿度到手机客户端*/
		case '1':
			printf("\r\n humidity= %d \r\n",DHT11_Data.humi_int);
			break;
			/*接收到字符2传输空气中的颗粒浓度到手机客户端*/
		case '2':
			printf("\r\n PM2.5= %d \r\n",54);
			break;

		case '3':
				LED1(ON);
			break;
			
		case '4':
				LED2(ON);
			break;
			
		case '5':
				LED3(ON);
			break;
			
		case '6':
				LED1(OFF);
			break;
			
		case '7':
				LED2(OFF);
			break;
			
		case '8':
				LED3(OFF);
			break;
		
		default:
			break;
			
	}
}



///重定向c库函数printf到USART1
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到USART1 */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}
///重定向c库函数scanf到USART1
int fgetc(FILE *f)
{
		/* 等待串口1输入数据 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}
/*********************************************END OF FILE**********************/
