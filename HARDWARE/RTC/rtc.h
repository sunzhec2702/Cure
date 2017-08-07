#ifndef __RTC_H__
#define __RTC_H__
#include "sys.h"

#define RTC_CLOCK_SOURCE_LSI
#define BKP_VALUE 0xF
#define BKP_VALUE_REG_VALUE 0
#define BKP_VALUE_OFFSET_VALUE 28
#define BKP_VALUE_MASK_VALUE 0xF

#define RTC_EXTI_LINE EXTI_LINE17
#define FEED_DOG_TIME 3 //s

extern void rtc_init(void);
extern void enable_rtc(u16 seconds);
extern void RTC_REENABLE_FEEDDOG(void);

typedef struct
{
    vu8 hour;
    vu8 min;
    vu8 sec;
    vu16 w_year;
    vu8 w_month;
    vu8 w_date;
    vu8 week;
} calendar_obj;
extern calendar_obj calendar;

extern u8 const mon_table[12];                //�·��������ݱ�
void Disp_Time(u8 x, u8 y, u8 size);          //���ƶ�λ�ÿ�ʼ��ʾʱ��
void Disp_Week(u8 x, u8 y, u8 size, u8 lang); //��ָ��λ����ʾ����
u8 RTC_Init(void);                            //��ʼ��RTC,����0,ʧ��;1,�ɹ�;
u8 Is_Leap_Year(u16 year);                    //ƽ��,�����ж�
u8 RTC_Get(void);                             //����ʱ��
u8 RTC_Get_Week(u16 year, u8 month, u8 day);
u8 RTC_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec); //����ʱ��

#endif