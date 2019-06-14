/***************************************************************************************************
*                    (c) Copyright 2006-2016  Syncretize technologies co.,ltd.
*										All Rights Reserved
*	
*\File 			drv_input.c
*\Description   
*\Note          
*\Log           2016.07.15    Ver 1.0   Job
*               �����ļ���
***************************************************************************************************/
#include "drv_input.h"

/***************************************************************************************************
*\Function      stm32_input_init
*\Description   �������ͳ�ʼ��
*\Parameter     in
*\Parameter     gpio
*\Parameter     pin
*\Return        void
*\Note          
*\Log           2016.07.15    Ver 1.0    Job
*               ����������
***************************************************************************************************/
void stm32_input_init(input_type* in)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    if (in == RT_NULL)
    {
        return;
    }

    //ʹ�ܶ˿�ʱ��
    switch((u32)in->gpio)
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

    //��ʼ��gpio����
    GPIO_InitStructure.GPIO_Pin = in->pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(in->gpio, &GPIO_InitStructure);
}

/***************************************************************************************************
*\Function      stm32_input_get
*\Description   ��ȡ״̬
*\Parameter     in
*\Return        bool
*\Note          �ߵ�ƽΪ TRUE
*\Log           2016.07.15    Ver 1.0    Job
*               ����������
***************************************************************************************************/
bool stm32_input_get(input_type* in)
{
    if (in == RT_NULL)
    {
        return FALSE;
    }

    if (GPIO_ReadInputDataBit(in->gpio, in->pin))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
