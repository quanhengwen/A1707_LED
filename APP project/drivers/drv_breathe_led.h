/***************************************************************************************************
*                    (c) Copyright 2006-2016  Syncretize technologies co.,ltd.
*										All Rights Reserved
*	
*\File 			drv_breathe_led.h
*\Description   
*\Note          
*\Log           2016.08.23    Ver 1.0   Job
*               �����ļ���
***************************************************************************************************/
#ifndef _DRV_BREATHE_LED_H
#define _DRV_BREATHE_LED_H
#include "common_lib.h"
#include "drv_switch.h"

typedef struct BreatheMsgLedType_st
{
    switch_type*   sw_led;
    //�������ʼ��
    rt_timer_t    timer_msg;
    u32           count;//��������
    u32           count_light;//�Ѿ���������
    u32           on_cycle;//����ʱ��
    u32           off_cycle;//�ر�ʱ��
}BreatheMsgLedType;

bool breathe_msg_led_on(BreatheMsgLedType* msg, u32 count, bool steady);
bool breathe_msg_led_off(BreatheMsgLedType* msg);
bool breathe_msg_led_attr_set(BreatheMsgLedType* msg, u32 on_cycle, u32 off_cycle);
bool breathe_msg_led_init(BreatheMsgLedType* msg_auto, char* name, u32 on_cycle, u32 off_cycle);


#endif /*_DRV_BREATHE_LED_H*/
