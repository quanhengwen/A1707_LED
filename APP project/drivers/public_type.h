/***************************************************************************************************
*                    (c) Copyright 1992-2015 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          public_type.h
*\Description   �������ݽṹ���Ͷ����ļ�
*\Note          
*\Log           2015.07.06    Ver 1.0    �Ų�
*               �����ļ���
***************************************************************************************************/
#ifndef _PUBLIC_TYPE_H
#define _PUBLIC_TYPE_H

#include "rtthread.h"
#include "stm32f10x.h"

typedef int64_t  s64;
typedef const int32_t sc64;
typedef __IO int64_t  vs64;
typedef __I int64_t vsc64;
typedef uint64_t  u64;
typedef __IO uint64_t  vu64;
typedef __I uint64_t vuc64;
typedef const uint64_t uc64;

/**************************************************************************************************/
/* �Զ������Ͷ���                                           �Ų�                                  */
/**************************************************************************************************/
typedef enum
{
    FALSE = 0,
    TRUE = !0,
}bool;

#define Max(a,b)    (((a) > (b)) ? (a) : (b))
#define Min(a,b)    (((a) < (b)) ? (a) : (b))
#define Abs(a)      ((a) > 0 ? (a) : -(a))
#define Swap(a, b) {(a)^=(b); (b)^=(a); (a)^=(b);}

/*gpio����*/
typedef struct GpioType_st
{
    GPIO_TypeDef* gpio;
    u16           gpio_pin;
    u32           Alternate;
}GpioType;

/*�жϴ�����*/
typedef void(*pHookHandle)(void *);

typedef struct HookDevType_st
{
    pHookHandle phook;
    void*       para;
}HookDevType;

#endif /*_PUBLIC_TYPE_H*/

