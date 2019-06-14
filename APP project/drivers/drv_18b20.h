/***************************************************************************************************
*                    (c) Copyright 1992-2014 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          stm32_18b20.h
*\Description   
*\Note          
*\Log           2014.12.04    Ver 1.0    �Ų�
*               �����ļ���
***************************************************************************************************/
#ifndef _DRV_18B20_H
#define _DRV_18B20_H
#include "public_type.h"

enum
{
    Accuracy_Ds18B20_9Bit, //ת��ʱ��93.75ms
    Accuracy_Ds18B20_10Bit,//ת��ʱ��187.5ms
    Accuracy_Ds18B20_11Bit,//ת��ʱ��375ms
    Accuracy_Ds18B20_12Bit,//ת��ʱ��750ms
};

enum
{
    DS18B20_ROM_CMD_SKIP = 0xCC,
};

enum
{
    DS18B20_REG_CMD_CONV = 0x44,
    DS18B20_REG_CMD_READ_RAM = 0xBE,
    DS18B20_REG_CMD_WRITE_RAM = 0x4E,
    DS18B20_REG_CMD_COPY_RAM = 0x48,
    DS18B20_REG_CMD_RESET_RAM = 0xB8,
    DS18B20_REG_CMD_READ_VDD = 0xB4,
};

typedef struct TempDs18B20Type_st
{
    GPIO_TypeDef* Gpio;//��Ӧ��gpio port
    u16           Pin; //��Ӧ��gpio_pin
    TIM_TypeDef*  Tim; //��ȷ��ʱ
}TempDs18B20Type;

//�¶ȸ�ʽ
typedef struct Ds18B20TempType_st
{
    u8 TmpH;//�¶���������
    u8 TmpL;//�¶�С������,֧ȡС����һλ����Ϊ18b20����Ϊ0.5������Ҳû������
    u8 Signal;//�¶ȷ���,0Ϊ����1Ϊ��
}Ds18B20TempType;

extern Ds18B20TempType Env_TMP;
extern Ds18B20TempType DC_TMP;

void stm32_ds18b20_thread_entry(void* para);

#endif /*_STM32_18B20_H*/
