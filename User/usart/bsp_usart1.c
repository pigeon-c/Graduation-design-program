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
  * @brief  USART1 GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
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
		
			/* ʹ�ܴ���1�����ж� */
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		
		NVIC_Configuration();
		
		USART_Cmd(USART1, ENABLE);
}


/// ����USART1�ж�
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
  * @brief  UsartReceive ���ڽ����жϷ�������������������
  * @param  ��
  * @retval ��
  */


void UsartReceive(void)
{
	//��ȡ״̬λ
	if(USART_GetFlagStatus(USART1,USART_IT_RXNE)==SET)
	{	 
		USART_ClearFlag(USART1, USART_FLAG_RXNE | USART_FLAG_ORE);
		//�����ݲ��� RXNE��OREλ      
		recv_char = USART_ReceiveData(USART1);	
	}
	//���ݽ��յ�����������ж�ִ����ص�����
	switch(recv_char)
	{
		/*���յ��ַ�0��������е��¶ȵ��ֻ��ͻ���*/
		case '0':
				printf("\r\n temperature= %d \r\n",DHT11_Data.temp_int);
			break;
			/*���յ��ַ�1��������е�ʪ�ȵ��ֻ��ͻ���*/
		case '1':
			printf("\r\n humidity= %d \r\n",DHT11_Data.humi_int);
			break;
			/*���յ��ַ�2��������еĿ���Ũ�ȵ��ֻ��ͻ���*/
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



///�ض���c�⺯��printf��USART1
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ�USART1 */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}
///�ض���c�⺯��scanf��USART1
int fgetc(FILE *f)
{
		/* �ȴ�����1�������� */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}
/*********************************************END OF FILE**********************/
