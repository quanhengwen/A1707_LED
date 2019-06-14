/***************************************************************************************************
*                    (c) Copyright 2006-2016  Syncretize technologies co.,ltd.
*										All Rights Reserved
*	
*\File 			drv_switch.c
*\Description   ������������ƽṹ
*\Note          
*\Log           2016.07.08    Ver 1.0   Job
*               �����ļ���
***************************************************************************************************/
#include "drv_switch.h"

/***************************************************************************************************
*\Function      rt_switch_on
*\Description   ��switch
*\Parameter     sw
*\Return        void
*\Note          
*\Log           2016.07.08    Ver 1.0    Job
*               ����������
***************************************************************************************************/
void stm32_switch_on(switch_type* sw)
{
    if (sw == RT_NULL)
    {
        return;
    }
    if (sw->valid)
    {
        GPIO_SetBits(sw->gpio, sw->pin);
    }
    else
    {
        GPIO_ResetBits(sw->gpio, sw->pin);
    }
}
/***************************************************************************************************
*\Function      rt_switch_off
*\Description   �ر����
*\Parameter     sw
*\Return        void
*\Note          
*\Log           2016.07.08    Ver 1.0    Job
*               ����������
***************************************************************************************************/
void stm32_switch_off(switch_type* sw)
{
    if (sw == RT_NULL)
    {
        return;
    }
    if (sw->valid)
    {
        GPIO_ResetBits(sw->gpio, sw->pin);
    }
    else
    {
        GPIO_SetBits(sw->gpio, sw->pin);
    }
}
/***************************************************************************************************
*\Function      rt_switch_evert
*\Description   ��ת���
*\Parameter     sw
*\Return        void
*\Note          
*\Log           2016.07.08    Ver 1.0    Job
*               ����������
***************************************************************************************************/
void stm32_switch_evert(switch_type* sw)
{
    if (sw == RT_NULL)
    {
        return;
    }
    GPIO_ToggleBits(sw->gpio, sw->pin);
}
/***************************************************************************************************
*\Function      rt_switch_init
*\Description   
*\Parameter     gpio,�˿�
*\Parameter     pin������
*\Parameter     valid,onʱ����Ч��ƽ
*\Return        void
*\Note          
*\Log           2016.07.08    Ver 1.0    Job
*               ����������
***************************************************************************************************/
void stm32_switch_init(switch_type* sw)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_TypeDef* gpio = sw->gpio;
    u16 pin = sw->pin;

    if (sw == RT_NULL)
    {
        return;
    }

    //ʹ�ܶ˿�ʱ��
    switch((u32)gpio)
    {
    case GPIOA_BASE:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        break;
    case GPIOB_BASE:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        break;
    case GPIOC_BASE:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        break;
    case GPIOD_BASE:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
        break;
    case GPIOE_BASE:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
        break;
    case GPIOF_BASE:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
        break;
    case GPIOG_BASE:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
        break;
    default:
        rt_kprintf("gpio clk error!\n");
        break;
    }

    //Ĭ�Ϲر����
    stm32_switch_off(sw);

    /*���ų�ʼ��,ppģʽ*/
    GPIO_InitStructure.GPIO_Pin = pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(gpio, &GPIO_InitStructure);

    //Ĭ�Ϲر����
    stm32_switch_off(sw);
}
