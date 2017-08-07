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
    vu8 ampm;
    vu16 year;
    vu8 month;
    vu8 date;
    vu8 week;
} calendar_obj;

void Disp_Time(u8 x, u8 y, u8 size);          //在制定位置开始显示时间
void Disp_Week(u8 x, u8 y, u8 size, u8 lang); //在指定位置显示星期

extern ErrorStatus rtc_set_time(calendar_obj cal);
extern ErrorStatus rtc_set_date(calendar_obj cal);
extern ErrorStatus rtc_set_cal(calendar_obj cal);
extern u8 RTC_Init(void);
extern void RTC_Set_AlarmA(u8 week, u8 hour, u8 min, u8 sec);
extern void RTC_Set_WakeUp(u32 wksel, u16 cnt);
extern u8 Is_Leap_Year(u16 year);
extern u8 RTC_Get_Week(u16 year, u8 month, u8 day);
extern calendar_obj* get_current_calendar(void);
extern ErrorStatus update_current_calendar(void);

#endif
