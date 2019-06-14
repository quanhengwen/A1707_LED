/***************************************************************************************************
*                    (c) Copyright 2006-2016  Syncretize technologies co.,ltd.
*										All Rights Reserved
*	
*\File 			drv_input.h
*\Description   
*\Note          
*\Log           2016.07.15    Ver 1.0   Job
*               �����ļ���
***************************************************************************************************/
#ifndef _DRV_INPUT_H
#define _DRV_INPUT_H
#include "public_type.h"

typedef struct input_type_st
{
    GPIO_TypeDef* gpio;
    u16 pin;
    /*�������ʼ����ֵ*/
}input_type;

void stm32_input_init(input_type* in);
bool stm32_input_get(input_type* in);

#endif /*_DRV_INPUT_H*/
