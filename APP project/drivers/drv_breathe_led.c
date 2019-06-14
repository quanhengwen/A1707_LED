/***************************************************************************************************
*                    (c) Copyright 2006-2016  Syncretize technologies co.,ltd.
*										All Rights Reserved
*	
*\File 			drv_breathe_led.c
*\Description   
*\Note          
*\Log           2016.08.23    Ver 1.0   Job
*               �����ļ���
***************************************************************************************************/
#include "drv_breathe_led.h"

/***************************************************************************************************
*\Function      msg_control_handle
*\Description   timer�жϴ�����
*\Parameter     msg_node
*\Return        void
*\Note          
*\Log           2014.12.16    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
static void msg_control_handle(void* p)
{
    u32 interval = 0;
    BreatheMsgLedType* msg_auto = (BreatheMsgLedType*) p;

    stm32_switch_evert(msg_auto->sw_led);

    if (!(msg_auto->count_light % 2))
    {
        interval = msg_auto->off_cycle;
    }
    else
    {
        interval = msg_auto->on_cycle;
    }
    //�Ѿ���˸����
    msg_auto->count_light++;

    if (msg_auto->count == 0)//һֱ��˸
    {
        msg_auto->count_light %= 2;
        rt_timer_control(msg_auto->timer_msg, RT_TIMER_CTRL_SET_TIME, &interval);
        //rt_timer_start(msg_node->timer_msg);�����������ж��˳�����RT_TIMER_FLAG_PERIODIC���Զ�����
        return;
    }
    if (msg_auto->count_light >= msg_auto->count * 2)
    {
        rt_timer_stop(msg_auto->timer_msg);
        stm32_switch_off(msg_auto->sw_led);
        return;
    }
    rt_timer_control(msg_auto->timer_msg, RT_TIMER_CTRL_SET_TIME, &interval);
    //rt_timer_start(msg_node->timer_msg);�����������ж��˳�����RT_TIMER_FLAG_PERIODIC���Զ�����
}

/***************************************************************************************************
*\Function      msg_control_on
*\Description   ��˸����
*\Parameter     led
*\Parameter     count��0Ϊ������˸
*\Parameter     steady,trueΪ����
*\Return        void
*\Note          
*\Log           2014.12.16    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool breathe_msg_led_on(BreatheMsgLedType* msg, u32 count, bool steady)
{
    u32 interval = 0;

    if (msg == RT_NULL)
    {
        return FALSE;
    }

    msg->count = count;
    msg->count_light = 0;
    interval = msg->on_cycle;
    rt_timer_stop(msg->timer_msg);
    rt_timer_control(msg->timer_msg, RT_TIMER_CTRL_SET_TIME, &interval);
    stm32_switch_on(msg->sw_led);
    if (!steady)//���������������timer
    {
        rt_timer_start(msg->timer_msg);
    }    

    return TRUE;
}
/***************************************************************************************************
*\Function      msg_control_off
*\Description   
*\Parameter     msg
*\Return        bool
*\Note          
*\Log           2014.12.17    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool breathe_msg_led_off(BreatheMsgLedType* msg)
{
    if (msg == RT_NULL)
    {
        return FALSE;
    }

    rt_timer_stop(msg->timer_msg);
    stm32_switch_off(msg->sw_led);   
    return TRUE;
}
/***************************************************************************************************
*\Function      msg_control_attr_set
*\Description   
*\Parameter     msg
*\Parameter     on_cycle
*\Parameter     off_cycle
*\Return        bool
*\Note          
*\Log           2014.12.17    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool breathe_msg_led_attr_set(BreatheMsgLedType* msg, u32 on_cycle, u32 off_cycle)
{
    if (msg == RT_NULL)
    {
        return FALSE;
    }

    msg->on_cycle = on_cycle;
    msg->off_cycle = off_cycle;
    return TRUE;
}
/***************************************************************************************************
*\Function      msg_control_init
*\Description   
*\Parameter     led
*\Parameter     name,time����
*\Parameter     on_cycle������ʱ��
*\Parameter     off_cycle���ر�ʱ��
*\Return        bool
*\Note          1����ʼ��һ�������Զ����Ƶ���ʱ���led
*\Log           2014.12.17    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool breathe_msg_led_init(BreatheMsgLedType* msg_auto, char* name, u32 on_cycle, u32 off_cycle)
{
    if (msg_auto == RT_NULL)
    {
        return FALSE;
    }
    
    msg_auto->count = 0;
    msg_auto->count_light = 0;
    msg_auto->on_cycle = on_cycle;
    msg_auto->off_cycle = off_cycle;

    msg_auto->timer_msg = rt_timer_create(name, msg_control_handle, msg_auto, 0, RT_TIMER_FLAG_PERIODIC);
    if (msg_auto->timer_msg == RT_NULL)
    {
        rt_kprintf("msg is null!\n");
        return FALSE;
    }
    stm32_switch_init(msg_auto->sw_led);

    return TRUE;
}
