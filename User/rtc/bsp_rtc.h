#ifndef __RTC_H
#define	__RTC_H

#include "stm32f10x.h"
#include "bsp_date.h"

void RTC_NVIC_Config(void);
void Time_Display(uint32_t TimeVar,struct rtc_time *tm);
void Time_Show(struct rtc_time *tm);
void RTC_CheckAndConfig(struct rtc_time *tm);

#endif 
