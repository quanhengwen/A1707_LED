/***************************************************************************************************
*                    (c) Copyright 2006-2016  Syncretize technologies co.,ltd.
*										All Rights Reserved
*	
*\File 			stm32_simu_i2c.h
*\Description   
*\Note          
*\Log           2016.07.11    Ver 1.0   Job
*               �����ļ���
***************************************************************************************************/
#ifndef _DRV_SIMU_I2C_H
#define _DRV_SIMU_I2C_H
#include "common_lib.h"

/*i2c���ߴ����־*/
typedef enum 
{
    SIMU_I2C_OK,
    SIMU_I2C_BUSY, //����æ
    SIMU_I2C_NOACK,//����Ӧ
}SimuI2CErrState;


/*ģ��i2cʹ�õ��Ľṹ*/
typedef struct stm32_simu_i2c_type
{
    GpioType* scl;/*i2cʱ�������*/
    GpioType* sda;/*i2c���������*/
    /*�������ʼ����ֵ*/
    rt_sem_t simu_i2c_sem;/*�豸������*/
}stm32_simu_i2c_type;

void stm32_simu_i2c_init(stm32_simu_i2c_type* simu_i2c);
u32 stm32_simu_i2c_send(stm32_simu_i2c_type* simu_i2c, u16 slave_addr, u8* buf, u32 len);
u32 stm32_simu_i2c_recv(stm32_simu_i2c_type* simu_i2c, u16 slave_addr, u8 word_addr, u8* buf, u32 len);

u32 stm32_simu_i2c_send_ch45x(stm32_simu_i2c_type* simu_i2c, u8* buf, u32 len);
u32 stm32_simu_i2c_recv_ch45x(stm32_simu_i2c_type* simu_i2c, u8 cmd, u8* buf, u32 len);

#endif /*_DRV_SIMU_I2C_H*/
