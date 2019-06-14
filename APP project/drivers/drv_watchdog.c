/***************************************************************************************************
*                    (c) Copyright 2008-2017  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          drv_watchdog.c
*\Description   
*\Note          
*\Log           2017.06.01    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#include "drv_watchdog.h"

rt_uint32_t Sys_Run_Time;//ϵͳ����ʱ��

/***************************************************************************************************
*\Function      rt_hw_watchdog_init
*\Description   ���Ź�ʹ��
*\Parameter     
*\Return        void
*\Note          
*\Log           2014.12.22    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void rt_hw_watchdog_init(void)
{
	//�����ڲ�LSI
	RCC_LSICmd(ENABLE);
	
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)==RESET);//�ȴ�ֱ��LSI�ȶ�

	/* д��0x5555,���������Ź��Ĵ���д�빦�� */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	/* ����ʱ�ӷ�Ƶ,40K/256=156HZ(6.4ms)*/
	IWDG_SetPrescaler(IWDG_Prescaler_256);

	/*ι��ʱ�� ����λms��= ��Ԥ��Ƶϵ��/4��*0.1*RLR����װ��ֵ��
	000 = ��256/4��*0.1*RLR
	RLR = 1000/(256/4*0.1) = 156 = 0x9C */	

	/* ι��ʱ�� 5s/6.4MS=781 .ע�ⲻ�ܴ���0xfff*/ 

	IWDG_SetReload(0x186);	  //2.5s
	
	/* ι��*/
	IWDG_ReloadCounter();
	
	/* ʹ�ܹ���*/
	IWDG_Enable(); 

}

//���Ź�����
void rt_watchdog_thread_entry(void* parameter)
{	
	Sys_Run_Time = 0;
   
	while (1)
	{	
		//��ʱι��   1Sһ�� 
		IWDG_ReloadCounter();
		Sys_Run_Time++;
		//delay
		rt_thread_delay(100);
	}
}

u8 SysRstFlag = 0;//��λ��־��¼
//�õ�ϵͳ��λ�ķ�ʽ,���Ź�&���&����
void GetRestFlag(void)
{
    //�ϵ縴λ��־
    if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
        SysRstFlag = 1;
        //���縴λ
        rt_kprintf("PowerDown RST!\n");
    }
    //���Ź�
    else if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
    {
        SysRstFlag = 2;
        //���Ź�
        rt_kprintf("WatchDog RST!\n");
    }
    //�����λ��־
    else if(RCC_GetFlagStatus(RCC_FLAG_SFTRST) != RESET)
    {
        SysRstFlag = 3;
        //software��λ
        rt_kprintf("SoftWare RST!\n");
    }	  	
    //NRST
    else if(RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
        SysRstFlag = 4;
        //NRst��λ
        rt_kprintf("PINRST RST!\n");
    }
    //WWDGRST
    else if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
    {
        SysRstFlag = 5;
        //WWDGRST��λ
        rt_kprintf("WWDGRST RST!\n");
    }
    //LPWRRST
    else if(RCC_GetFlagStatus(RCC_FLAG_LPWRRST) != RESET)
    {
        SysRstFlag = 6;
        //LPWRRST��λ
        rt_kprintf("Low Power RST!\n");
    }
    else
    {
        SysRstFlag = 7;
        rt_kprintf("Other RST!\n");
    }
    RCC_ClearFlag();//�����λ��־

}
void RstFlag(void)
{
    switch(SysRstFlag)
    {
    case 1:
        //���縴λ
        rt_kprintf("PowerDown RST!\n");
        break;
    case 2:
        //���Ź�
        rt_kprintf("WatchDog RST!\n");
        break;
    case 3:
        //software��λ
        rt_kprintf("SoftWare RST!\n");
        break;
    case 4:
        //NRst��λ
        rt_kprintf("PINRST RST!\n");
        break;
    case 5:
        //WWDGRST��λ
        rt_kprintf("WWDGRST RST!\n");
        break;
    case 6:
        //Low Power��λ
        rt_kprintf("Low Power RST!\n");
        break;
    case 7:
        rt_kprintf("Other RST!\n");
        break;
    default:
        rt_kprintf("RstFlag is Err!\n");
        break;
    }
}
void system_reset(void)
{   
    rt_kprintf("System restarting\n"); 

    rt_thread_delay(1);

    __set_PRIMASK(1);//�ر��ж�

    NVIC_SystemReset();
}

void runtime(void)
{
    rt_kprintf("SysRunTime = %d Sec", Sys_Run_Time);
}

#ifdef RT_USING_FINSH
#include <finsh.h>

FINSH_FUNCTION_EXPORT(RstFlag, system Reset Flag.)
FINSH_FUNCTION_EXPORT(system_reset, Reset system.)
FINSH_FUNCTION_EXPORT(runtime, sys run time.)

#endif



