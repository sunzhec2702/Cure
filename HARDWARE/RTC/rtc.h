#ifndef __RTC_H__
#define __RTC_H__
#include "sys.h"

#define RTC_CLOCK_SOURCE_LSI

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
extern calendar_obj* get_current_calendar(void);
extern ErrorStatus update_current_calendar(void);
extern u8 is_calendar_initialized(void);

extern u8 rtc_init(void);
extern void rtc_set_alarmA(u8 week, u8 hour, u8 min, u8 sec);
extern void rtc_set_wake_up(u32 wksel, u16 cnt);
extern u8 is_leap_year(u16 year);

#endif
