#ifndef __EVEN_PROCESS_H
#define __EVEN_PROCESS_H

#include "stm32f10x.h"

typedef enum{E_NULL,E_BUTTON1,E_BUTTON2,E_BUTTON3,E_GETCHAR}EVEN;
						//���¼�����ť1����ť2����ť3����������

typedef enum{S_ON,S_OFF}BUTTON_STATE;
						//״̬����״̬��

void even_process(void);
void touch_process(void);

#endif


