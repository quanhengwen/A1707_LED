/***************************************************************************************************
*                    (c) Copyright 2008-2017  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          drv_InRTC.c
*\Description   
*\Note          ��Ϊʹ��RTC��rtc������Ϳ��Ա������ݣ����Խ�rtc��һ�γ�ʼ����־����rtc��������
*               RTC�������BKPSRAMд����ȥ���ǲ�һ����
*\Log           2017.06.01    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#include "drv_InRTC.h"

/***************************************************************************************************
*\Function      stm32_rtc_init
*\Description   rtcʱ�ӳ�ʼ��
*\Parameter     first,���ΪTRUE����ǿ�ƽ���һ�γ�ʼ��,��ʱ������Ϊ2000-1-1 0:0:0
*\Return        void
*\Note          1��ϵͳ�ڵ�һ���ϵ�ʱ���ʼ��
*\Log           2014.11.07    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void stm32_Inrtc_init(bool first)
{
    u32 count = 0;

    /*ʹ�ܵ�Դ�ͺ󱸽ӿ�ʱ��*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /*���üĴ���PWR_CR��DBPλ��ʹ�ܶԺ󱸼Ĵ�����RTC�ķ���*/
    PWR_BackupAccessCmd(ENABLE);
    /*��λ������*/
    if (first || BKP_ReadBackupRegister(BKP_DR1) != 0x1234)
    {
        BKP_DeInit();//ֻ�е�һ���ϵ縴λbkp
        /*ʹ��LSIʱ��*/
		RCC_LSICmd(ENABLE);
//        RCC_LSEConfig(RCC_LSE_ON);
        /*�ȴ�LSE�����Ч*/
        while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
        {
            if (count++ > 5)
            {
                rt_kprintf("InRTC init fail!\n");
                return;
            }
            rt_thread_delay(RT_TICK_PER_SECOND);
        }

        /*����RTC��ʱ��ԴΪLSI*/
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
        /*RTCʱ��ʹ��*/
        RCC_RTCCLKCmd(ENABLE);

        RTC_WaitForSynchro();
        /*�ȴ�RTC���һ�β������*/
        RTC_WaitForLastTask();

        /*����Ԥ��Ƶ��������ֵ*/
        RTC_SetPrescaler(0x7fff); /*RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(0x7fff+1)*/
        /*�ȴ��������*/
        RTC_WaitForLastTask();
        
        //��ʼʱ���趨
        RTC_SetCounter(0);
        /*�ȴ��������*/
        RTC_WaitForLastTask();

        BKP_WriteBackupRegister(BKP_DR1, 0x1234); 
        rt_kprintf("InRTC initialized Firstly...\n");
    }  
    else
    {
        rt_kprintf("InRTC initialized...\n");
    }
}

/***************************************************************************************************
*\Function      stm32_rtc_getTime
*\Description   ��ȡdecʱ��
*\Parameter     time
*\Return        bool
*\Note          
*\Log           2014.11.10    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool stm32_Inrtc_getTime(TIME* time)
{
    u32 stamp = 0;

    stamp = RTC_GetCounter();
    *time = TimeDECStampToTime(stamp);
    return TRUE;
}
/***************************************************************************************************
*\Function      stm32_rtc_setTime
*\Description   ����DECʱ��
*\Parameter     time
*\Return        bool
*\Note          
*\Log           2014.11.10    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool stm32_Inrtc_setTime(TIME time)
{
    u32 stamp = 0;

    stamp = TimeDECToStamp(time);
    RTC_SetCounter(stamp);
    /*�ȴ��������*/
    RTC_WaitForLastTask();
    return TRUE;
}

