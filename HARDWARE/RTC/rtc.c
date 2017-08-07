#include "rtc.h"

NVIC_InitTypeDef NVIC_InitStructure;

static calendar_obj current_calendar;

ErrorStatus rtc_set_time(calendar_obj cal)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	RTC_TimeTypeInitStructure.RTC_Hours = cal.hour;
	RTC_TimeTypeInitStructure.RTC_Minutes = cal.min;
	RTC_TimeTypeInitStructure.RTC_Seconds = cal.sec;
	RTC_TimeTypeInitStructure.RTC_H12 = RTC_H12_AM;
	return RTC_SetTime(RTC_Format_BIN, &RTC_TimeTypeInitStructure);
}

ErrorStatus rtc_set_date(calendar_obj cal)
{

	RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_DateTypeInitStructure.RTC_Date = cal.date;
	RTC_DateTypeInitStructure.RTC_Month = cal.month;
	RTC_DateTypeInitStructure.RTC_WeekDay = cal.week;
	RTC_DateTypeInitStructure.RTC_Year = cal.year;
	return RTC_SetDate(RTC_Format_BIN, &RTC_DateTypeInitStructure);
}

ErrorStatus rtc_set_cal(calendar_obj cal)
{
	if (RTC_Set_Time(cal) == ERROR)
		return ERROR;
	if (RTC_Set_Date(cal) == ERROR)
		return ERROR;
	PWR_BackupAccessCmd(ENABLE);
	RTC_WriteBackupRegister(RTC_BKP_DR1, 0xDEADBEEF); // RTC has been set already
	PWR_BackupAccessCmd(DISABLE);
	return SUCCESS;
}

ErrorStatus update_current_calendar()
{
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_GetDate(RTC_Format_BIN, &RTC_DateTypeInitStructure);
	current_calendar.date = RTC_DateTypeInitStructure.RTC_Date;
	current_calendar.month = RTC_DateTypeInitStructure.RTC_Month;
	current_calendar.week = RTC_DateTypeInitStructure.RTC_WeekDay;
	current_calendar.year = RTC_DateTypeInitStructure.RTC_Year;
	RTC_GsetTime(RTC_Format_BIN, &RTC_DateTypeInitStructure);
	current_calendar.hour = RTC_DateTypeInitStructure.RTC_Hours;
	current_calendar.min = RTC_DateTypeInitStructure.RTC_Minutes;
	current_calendar.sec = RTC_DateTypeInitStructure.RTC_Seconds;
}

calendar_obj* get_current_calendar()
{
	update_current_calendar();
	return &current_calendar;
}

u8 is_calendar_initialized()
{
	if (RTC_ReadBackupRegister(BTC_BKP_DR1) == 0xDEADBEEF)
		return TRUE;
	else
		return FALSE;
}

//RTC��ʼ��
//����ֵ:0,��ʼ���ɹ�;
//       1,LSE����ʧ��;
//       2,�����ʼ��ģʽʧ��;
u8 RTC_Init(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	u16 retry = 0X1FFF;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); //ʹ��PWRʱ��
	PWR_BackupAccessCmd(ENABLE);						//ʹ�ܺ󱸼Ĵ�������
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x5050)
	{
		RCC_LSEConfig(RCC_LSE_ON);							//LSE ����
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) //���ָ����RCC��־λ�������,�ȴ����پ������
		{
			retry++;
			delay_ms(10);
		}
		if (retry == 0)
			return 1; //LSE ����ʧ��.
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); //����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��
		RCC_RTCCLKCmd(ENABLE);					//ʹ��RTCʱ��
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;			  //RTC�첽��Ƶϵ��(1~0X7F)
		RTC_InitStructure.RTC_SynchPrediv = 0xFF;			  //RTCͬ����Ƶϵ��(0~7FFF)
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24; //RTC����Ϊ,24Сʱ��ʽ
		RTC_Init(&RTC_InitStructure);
		RTC_WriteBackupRegister(RTC_BKP_DR0, 0x5050); //����Ѿ���ʼ������
	} 
	return 0;
}

//��������ʱ��(����������,24Сʱ��)
//week:���ڼ�(1~7) @ref  RTC_Alarm_Definitions
//hour,min,sec:Сʱ,����,����
void RTC_Set_AlarmA(u8 week, u8 hour, u8 min, u8 sec)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmTypeInitStructure;
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;

	RTC_AlarmCmd(RTC_Alarm_A, DISABLE); //�ر�����A

	RTC_TimeTypeInitStructure.RTC_Hours = hour;  //Сʱ
	RTC_TimeTypeInitStructure.RTC_Minutes = min; //����
	RTC_TimeTypeInitStructure.RTC_Seconds = sec; //��
	RTC_TimeTypeInitStructure.RTC_H12 = RTC_H12_AM;

	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay = week;								  //����
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay; //��������
	RTC_AlarmTypeInitStructure.RTC_AlarmMask = RTC_AlarmMask_None;						  //��ȷƥ�����ڣ�ʱ����
	RTC_AlarmTypeInitStructure.RTC_AlarmTime = RTC_TimeTypeInitStructure;
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmTypeInitStructure);

	RTC_ClearITPendingBit(RTC_IT_ALRA);  //���RTC����A�ı�־
	EXTI_ClearITPendingBit(EXTI_Line17); //���LINE17�ϵ��жϱ�־λ

	RTC_ITConfig(RTC_IT_ALRA, ENABLE); //��������A�ж�
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE); //��������A

	EXTI_InitStructure.EXTI_Line = EXTI_Line17;			   //LINE17
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;			   //ʹ��LINE17
	EXTI_Init(&EXTI_InitStructure);						   //����

	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		 //�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 //ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);								 //����
}

//�����Ի��Ѷ�ʱ������
/*wksel:  @ref RTC_Wakeup_Timer_Definitions
#define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
#define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
#define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
#define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
#define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)
#define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)
*/
//cnt:�Զ���װ��ֵ.����0,�����ж�.
void RTC_Set_WakeUp(u32 wksel, u16 cnt)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	RTC_WakeUpCmd(DISABLE); //�ر�WAKE UP

	RTC_WakeUpClockConfig(wksel); //����ʱ��ѡ��

	RTC_SetWakeUpCounter(cnt); //����WAKE UP�Զ���װ�ؼĴ���

	RTC_ClearITPendingBit(RTC_IT_WUT);   //���RTC WAKE UP�ı�־
	EXTI_ClearITPendingBit(EXTI_Line22); //���LINE22�ϵ��жϱ�־λ

	RTC_ITConfig(RTC_IT_WUT, ENABLE); //����WAKE UP ��ʱ���ж�
	RTC_WakeUpCmd(ENABLE);			  //����WAKE UP ��ʱ����

	EXTI_InitStructure.EXTI_Line = EXTI_Line22;			   //LINE22
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;			   //ʹ��LINE22
	EXTI_Init(&EXTI_InitStructure);						   //����

	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		 //�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 //ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);								 //����
}

u8 Is_Leap_Year(u16 year)
{
	if (year % 4 == 0) //�����ܱ�4����
	{
		if (year % 100 == 0)
		{
			if (year % 400 == 0)
				return 1; //�����00��β,��Ҫ�ܱ�400����
			else
				return 0;
		}
		else
			return 1;
	}
	else
		return 0;
}

//������������ڼ�
//��������:���빫�����ڵõ�����(ֻ����1901-2099��)
//�������������������
//����ֵ�����ں�
u8 RTC_Get_Week(u16 year, u8 month, u8 day)
{
	u16 temp2;
	u8 yearH, yearL;

	yearH = year / 100;
	yearL = year % 100;
	// ���Ϊ21����,�������100
	if (yearH > 19)
		yearL += 100;
	// ����������ֻ��1900��֮���
	temp2 = yearL + yearL / 4;
	temp2 = temp2 % 7;
	temp2 = temp2 + day + table_week[month - 1];
	if (yearL % 4 == 0 && month < 3)
		temp2--;
	return (temp2 % 7);
}
