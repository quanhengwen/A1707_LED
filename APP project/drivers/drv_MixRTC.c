/***************************************************************************************************
*                    (c) Copyright 2008-2017  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          drv_MixRTC.c
*\Description   
*\Note          
*\Log           2017.06.01    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#include "drv_MixRTC.h"

//#define USE_RT8025T

static rt_mutex_t stm32_rtc_mutex;
static u8 InitFlag = 0;

#ifdef USE_RT8025T
/***************************************************************************************************
*\Function      stm32_rtc_timer_handle
*\Description   ÿ��һ��ʱ�佫Ƭ��Ƭ��ʱ��ͬ��
*\Parameter     p
*\Return        void
*\Note          10s���
*\Log           2015.09.28    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
static void stm32_rtc_sync_thread_entry(void* p)
{
    TIME time;

    while (1)
    {
        rt_thread_delay(5 * 100);
        /*��ȡ��ǰʱ��*/
        stm32_rt8025t_getTime(&time);
        /*Ƭ����Ƭ��ͬ��ʱ��*/
        stm32_Inrtc_setTime(time);
    } 
}
#endif
/***************************************************************************************************
*\Function      stm32_rtc_init
*\Description   ʱ�ӳ�ʼ��
*\Parameter     
*\Return        void
*\Note          
*\Log           2015.09.28    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void stm32_rtc_init(void)
{
#ifdef USE_RT8025T
    TIME time;
    rt_thread_t tid;
#endif
    stm32_rtc_mutex = rt_mutex_create("rtc_mutex", RT_IPC_FLAG_FIFO);
    if (stm32_rtc_mutex == RT_NULL)
    {
        rt_kprintf("rtc mutex create fail!\n");
        return;
    }
#ifdef USE_RT8025T
    /*rt8025t*/
    stm32_rt8025t_init();
#endif
    /*Ƭ��*/
    stm32_Inrtc_init(FALSE);

#ifdef USE_RT8025T
    /*��ȡ��ǰʱ��*/
    stm32_rt8025t_getTime(&time);
    /*Ƭ����Ƭ��ͬ��ʱ��*/
    stm32_Inrtc_setTime(time);

    tid = rt_thread_create("rtc_sync", stm32_rtc_sync_thread_entry, RT_NULL, 512, 28, 5);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }
#endif
    InitFlag = 1;
}

/***************************************************************************************************
*\Function      stm32_rtc_getTime
*\Description   ��ȡʱ��
*\Parameter     time
*\Return        bool
*\Note          
*\Log           2015.09.28    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool stm32_rtc_getTime(TIME* time)
{
    bool result;

    if(!InitFlag)
    {
        return FALSE;
    }
    rt_mutex_take(stm32_rtc_mutex, RT_WAITING_FOREVER);
    result = stm32_Inrtc_getTime(time);
    rt_mutex_release(stm32_rtc_mutex);

    return result;
}
/***************************************************************************************************
*\Function      stm32_rtc_setTime
*\Description   ����ʱ��
*\Parameter     time
*\Return        bool
*\Note          
*\Log           2015.09.28    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool stm32_rtc_setTime(TIME time)
{
    if(!InitFlag)
    {
        return FALSE;
    }
    rt_mutex_take(stm32_rtc_mutex, RT_WAITING_FOREVER);
#ifdef USE_RT8025T
    if (!stm32_rt8025t_setTime(time))
    {
        rt_mutex_release(stm32_rtc_mutex);
        return FALSE;
    }
#endif
    if (!stm32_Inrtc_setTime(time))
    {
        rt_mutex_release(stm32_rtc_mutex);
        return FALSE;
    }

    rt_mutex_release(stm32_rtc_mutex);
    return TRUE;
}
/***************************************************************************************************
*\Function      date_get
*\Description   ��ӡrtcʱ��
*\Parameter     
*\Return        void
*\Note          
*\Log           2014.11.11    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void date_get(void)
{
    TIME time = INIT_TIME;

    stm32_rtc_getTime(&time);
    rt_kprintf("20%02d-%02d-%02d %02d:%02d:%02d\n",time.Year, time.Month, time.Day, time.Hour,time.Minute, time.Second);
}
/***************************************************************************************************
*\Function      date_set
*\Description   ����rtcʱ��
*\Parameter     year
*\Parameter     month
*\Parameter     day
*\Parameter     hour
*\Parameter     min
*\Parameter     sec
*\Return        void
*\Note          
*\Log           2014.11.11    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void date_set(u8 year, u8 month, u8 day, u8 hour, u8 min, u8 sec)
{
    TIME time = INIT_TIME;

    time.Year = year % 100;
    time.Month = month;
    time.Day = day;
    time.Hour = hour;
    time.Minute = min;
    time.Second = sec;
    if (!TimeDECCheck(&time))
    {
        rt_kprintf("the format of date is err! eg.date_set(2014,10,1,12,10,30).\n");
        return;
    }
#ifdef USE_RT8025T
    stm32_rt8025t_setTime(time);
#endif
    stm32_rtc_setTime(time);
}
#ifdef RT_USING_FINSH
#include <finsh.h>

FINSH_FUNCTION_EXPORT(date_get, list the date and time.)
FINSH_FUNCTION_EXPORT(date_set, set the date and time. eg.date_set(2014,10,1,12,10,30).)
#endif


