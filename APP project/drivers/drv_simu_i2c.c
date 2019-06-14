/***************************************************************************************************
*                    (c) Copyright 2006-2016  Syncretize technologies co.,ltd.
*										All Rights Reserved
*	
*\File 			stm32_simu_i2c.c
*\Description   
*\Note          ʵ�֣�������
*\Log           2016.07.11    Ver 1.0   Job
*               �����ļ���
***************************************************************************************************/
#include "drv_simu_i2c.h"


/***************************************************************************************************
*\Function      SimuI2CDelay
*\Description   �ڲ���ʱ����
*\Parameter     
*\Return        void
*\Note          f4��1ԼΪ1us
*\Log           2011.10.10    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
static  void SimuI2CDelay(void)
{
    u32 i = 100;
    while(i--);
}

/***************************************************************************************************
*\Function      I2cBUSBusyRecovery
*\Description   i2c���������ָ�����������9��������ٷ���ֹͣλ
*\Parameter     simu_i2c
*\Return        void
*\Note          
*\Log           2011.10.11    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
static void I2cBUSBusyRecovery(stm32_simu_i2c_type* simu_i2c)
{
    u8 i = 0;

    /*SDA��©�����*/
    GPIO_SetBits(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin);
    /*�������9��ʱ������*/
    for (i = 0; i < 9; i++)
    {
        GPIO_ResetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
        SimuI2CDelay();
        GPIO_SetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
        SimuI2CDelay();
    }
    /*����һ��ֹͣλ������������sclΪ�ߵ�ƽʱ�ɵͱ��*/
    GPIO_ResetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
    GPIO_ResetBits(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin);
    SimuI2CDelay();
    GPIO_SetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
    SimuI2CDelay();
    GPIO_SetBits(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin);
}
/***************************************************************************************************
*\Function      SimuI2CStart
*\Description   ģ��i2c��ʼ����
*\Parameter     simu_i2c
*\Return        bool
*\Note          
*\Log           2011.10.10    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
static SimuI2CErrState SimuI2CStart(stm32_simu_i2c_type* simu_i2c)
{
    GPIO_SetBits(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin);
    GPIO_SetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
    SimuI2CDelay();
    /*��������Ƿ�Ϊæ��æ���˳�*/
    if (!(GPIO_ReadInputDataBit(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin)))
    {
        return SIMU_I2C_BUSY;
    }
    GPIO_ResetBits(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin);
    SimuI2CDelay();
    GPIO_ResetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
    SimuI2CDelay();
    return SIMU_I2C_OK;
}
/***************************************************************************************************
*\Function      SimuI2CStop
*\Description   ģ��i2cֹͣ����
*\Parameter     simu_i2c
*\Return        SimuI2CERR
*\Note          
*\Log           2011.10.10    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
static SimuI2CErrState SimuI2CStop(stm32_simu_i2c_type* simu_i2c)
{
    GPIO_ResetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
    SimuI2CDelay();
    GPIO_ResetBits(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin);
    SimuI2CDelay();
    GPIO_SetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
    SimuI2CDelay();
    GPIO_SetBits(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin);
    SimuI2CDelay();
    return SIMU_I2C_OK; 
}
/***************************************************************************************************
*\Function      SimuI2CACK
*\Description   ģ��i2c��Ӧack
*\Parameter     simu_i2c
*\Return        SimuI2CERR
*\Note          
*\Log           2011.10.10    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
static SimuI2CErrState SimuI2CACK(stm32_simu_i2c_type* simu_i2c)
{
    GPIO_ResetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
    SimuI2CDelay();
    GPIO_ResetBits(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin);
    SimuI2CDelay();
    GPIO_SetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
    SimuI2CDelay();
    GPIO_ResetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
    SimuI2CDelay();
    return SIMU_I2C_OK;
}
/***************************************************************************************************
*\Function      SimuI2CNOACK
*\Description   ģ��i2c����Ӧ
*\Parameter     simu_i2c
*\Return        SimuI2CERR
*\Note          
*\Log           2011.10.10    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
static SimuI2CErrState SimuI2CNOACK(stm32_simu_i2c_type* simu_i2c)
{
    GPIO_ResetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
    SimuI2CDelay();
    GPIO_SetBits(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin);
    SimuI2CDelay();
    GPIO_SetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
    SimuI2CDelay();
    GPIO_ResetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
    SimuI2CDelay();
    return SIMU_I2C_OK;
}
/***************************************************************************************************
*\Function      SimuI2CWaitACK
*\Description   ģ��i2c�ȴ�����ack
*\Parameter     simu_i2c
*\Return        
*\Note          
*\Log           2011.10.10    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
static SimuI2CErrState SimuI2CWaitACK(stm32_simu_i2c_type* simu_i2c)
{
    GPIO_ResetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
    SimuI2CDelay();
    GPIO_SetBits(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin);		
    SimuI2CDelay();
    GPIO_SetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
    SimuI2CDelay();
    if(GPIO_ReadInputDataBit(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin))
    {
        /*��ackΪ1����������*/
        GPIO_ResetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
        return SIMU_I2C_NOACK;
    }
    GPIO_ResetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
    return SIMU_I2C_OK;
}
/***************************************************************************************************
*\Function      SimuI2CByteSend
*\Description   ģ��i2cһ���ֽڷ��͹��̣���λ��ǰ����λ�ں�
*\Parameter     simu_i2c
*\Parameter     byte
*\Return        SimuI2CERR
*\Note          
*\Log           2011.10.10    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
static SimuI2CErrState SimuI2CByteSend(stm32_simu_i2c_type* simu_i2c, u8 byte)
{
    u8  i = 8;

    while (i--)
    {
        GPIO_ResetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
        SimuI2CDelay();
        if (byte & 0x80)
        {
            GPIO_SetBits(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin);
        }
        else
        {
            GPIO_ResetBits(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin);
        }
        byte <<= 1;
        SimuI2CDelay();
        GPIO_SetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
        SimuI2CDelay();
    }
    GPIO_ResetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
    return SIMU_I2C_OK;
}
/***************************************************************************************************
*\Function      SimuI2CByteRecv
*\Description   ģ��i2cһ���ֽڽ��չ��̣���λ��ǰ����λ�ں�
*\Parameter     simu_i2c
*\Parameter     byte
*\Return        SimuI2CERR
*\Note          
*\Log           2011.10.10    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
static SimuI2CErrState SimuI2CByteRecv(stm32_simu_i2c_type* simu_i2c, u8* byte)
{
    u8  i = 8;
    u8  tmp = 0;

    GPIO_SetBits(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin);				
    while (i--)
    {
        tmp <<= 1;      
        GPIO_ResetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
        SimuI2CDelay();
        GPIO_SetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
        SimuI2CDelay();	
        if(GPIO_ReadInputDataBit(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin))
        {
            tmp |= 0x01;
        }
    }
    GPIO_ResetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
    *byte = tmp;
    return SIMU_I2C_OK;
}
/***************************************************************************************************
*\Function      SimuI2CSend
*\Description   ģ��i2c���ͺ����ӿ�
*\Parameter     simu_i2c
*\Parameter     slave_addr ��������ַ
*\Parameter     buf
*\Parameter     len
*\Return        bool
*\Note          
*\Log           2011.10.10    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
static SimuI2CErrState SimuI2CSend(stm32_simu_i2c_type* simu_i2c, u8 slave_addr, u8* buf, u32 len)
{
    SimuI2CErrState result;
    u32 i  = 0;
    /*��ʼ����*/
    result = SimuI2CStart(simu_i2c);
    if (result != SIMU_I2C_OK)
    {
        return result;
    }
    /*���ʹ�������ַ + д*/
    result = SimuI2CByteSend(simu_i2c, slave_addr & 0xFE);
    if (result != SIMU_I2C_OK)
    {
        return result;
    }
    /*�ȴ���������Ӧ*/
    result = SimuI2CWaitACK(simu_i2c);
    if (result != SIMU_I2C_OK)
    {
        SimuI2CStop(simu_i2c);
        return result;
    }
    /*ѭ����������*/
    for (i = 0; i < len; i++)
    {
        SimuI2CByteSend(simu_i2c, buf[i]);
        result = SimuI2CWaitACK(simu_i2c);
        /*�������Ӧ����󣬷���ֹͣλ*/
        if (result != SIMU_I2C_OK)
        {
            SimuI2CStop(simu_i2c);
            return result;
        }
    }
    /*����ֹͣ����*/
    return SimuI2CStop(simu_i2c);
}
/***************************************************************************************************
*\Function      SimuI2CRecv
*\Description   ģ��i2c���սӿں���
*\Parameter     simu_i2c
*\Parameter     slave_addr
*\Parameter     word_addr
*\Parameter     buf
*\Parameter     len
*\Return        SimuI2CErrState
*\Note          
*\Log           2011.10.10    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
static SimuI2CErrState SimuI2CRecv(stm32_simu_i2c_type* simu_i2c, u8 slave_addr, u8 word_addr, u8* buf, u32 len)
{
    SimuI2CErrState result;
    u32 i  = 0;
    /*��ʼ����*/
    result = SimuI2CStart(simu_i2c);
    if (result != SIMU_I2C_OK)
    {
        return result;
    }
    /*���ʹ�������ַ + д*/
    result = SimuI2CByteSend(simu_i2c, slave_addr & 0xFE);
    if (result != SIMU_I2C_OK)
    {
        return result;
    }
    /*�ȴ���������Ӧ*/
    result = SimuI2CWaitACK(simu_i2c);
    if (result != SIMU_I2C_OK)
    {
        SimuI2CStop(simu_i2c);
        return result;
    }
    /*���ʹ������ڲ��Ĵ�����ַ*/
    result = SimuI2CByteSend(simu_i2c, word_addr);
    if (result != SIMU_I2C_OK)
    {
        return result;
    }
    /*�ȴ���������Ӧ*/
    result = SimuI2CWaitACK(simu_i2c);
    if (result != SIMU_I2C_OK)
    {
        SimuI2CStop(simu_i2c);
        return result;
    }
    /*���·�����ʼ����*/
    result = SimuI2CStart(simu_i2c);
    if (result != SIMU_I2C_OK)
    {
        return result;
    }
    /*���ʹ�������ַ + ��*/
    result = SimuI2CByteSend(simu_i2c, slave_addr | 0x01);
    if (result != SIMU_I2C_OK)
    {
        return result;
    }
    /*�ȴ���������Ӧ*/
    result = SimuI2CWaitACK(simu_i2c);
    if (result != SIMU_I2C_OK)
    {
        SimuI2CStop(simu_i2c);
        return result;
    }
    /*ѭ������len������*/
    for (i = 0; i < len; i++)
    {
        SimuI2CByteRecv(simu_i2c, buf++);
        if (i == len - 1)
        {
            result = SimuI2CNOACK(simu_i2c);
        }
        else
        {
            result = SimuI2CACK(simu_i2c);
        }
    }
    return SimuI2CStop(simu_i2c);
}
/***************************************************************************************************
*\Function      gpio_rcc_enable
*\Description   
*\Parameter     gpio
*\Return        void
*\Note          
*\Log           2016.07.11    Ver 1.0    Job
*               ����������
***************************************************************************************************/
static void gpio_rcc_enable(GPIO_TypeDef* gpio)
{
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
}
/***************************************************************************************************
*\Function      stm32_simu_i2c_init
*\Description   ģ��i2c��ʼ��
*\Parameter     scl��ʱ�ӽ�
*\Parameter     sda�����ݽ�
*\Return        stm32_simu_i2c_type*
*\Note          
*\Log           2016.07.11    Ver 1.0    Job
*               ����������
***************************************************************************************************/
void stm32_simu_i2c_init(stm32_simu_i2c_type* simu_i2c)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    if (simu_i2c == RT_NULL || simu_i2c->scl == RT_NULL || simu_i2c->sda == RT_NULL)
    {
        rt_kprintf("simu i2c null!\n");
        return;
    }
    //gpioʱ��ʹ��
    gpio_rcc_enable(simu_i2c->scl->gpio);
    gpio_rcc_enable(simu_i2c->sda->gpio);

    /*����i2cЭ�飬����״̬����Ϊ�ߵ�ƽ*/  
    /*ʱ�ӽų�ʼ��*/
    /*��ʼ��֮ǰ��������Ĵ���д��1����ֹ��ʼ����ɺ�Ĭ�����0������i2c��ʼʱ��*/
    GPIO_SetBits(simu_i2c->scl->gpio, simu_i2c->scl->gpio_pin);
    GPIO_InitStructure.GPIO_Pin   = simu_i2c->scl->gpio_pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(simu_i2c->scl->gpio, &GPIO_InitStructure);

    /*���ݽų�ʼ��*/
    /*��ʼ��֮ǰ��������Ĵ���д��1����ֹ��ʼ����ɺ�Ĭ�����0������i2c��ʼʱ��*/
    GPIO_SetBits(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin);
    GPIO_InitStructure.GPIO_Pin       = simu_i2c->sda->gpio_pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(simu_i2c->sda->gpio, &GPIO_InitStructure);

    /*�ڳ�ʼ����ɺ������⵽����æ������9������+stop�����
    *���߲�æ������start+stop��λm41t83 i2c״̬�������ֲ���������i2c������Ӱ�����֤*/
    /*�����⵽�����߱����ͣ�������æ��ִ�����߽�������*/
    if (!(GPIO_ReadInputDataBit(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin)))
    {
        I2cBUSBusyRecovery(simu_i2c);
    }
    else
    {   
        SimuI2CStart(simu_i2c);
        SimuI2CStop(simu_i2c);
    }   
    /*���뻥���ź���*/
    simu_i2c->simu_i2c_sem = rt_sem_create("simu_i2c", 1, RT_IPC_FLAG_FIFO);
    if (simu_i2c->simu_i2c_sem == RT_NULL)
    {
        rt_kprintf("simu_i2c sem create fail!\n");
        return;
    }
    return;
}
/***************************************************************************************************
*\Function      stm32_simu_i2c_send
*\Description   
*\Parameter     simu_i2c
*\Parameter     slave_addr
*\Parameter     buf
*\Parameter     len
*\Return        u32
*\Note          
*\Log           2016.07.11    Ver 1.0    Job
*               ����������
***************************************************************************************************/
u32 stm32_simu_i2c_send(stm32_simu_i2c_type* simu_i2c, u16 slave_addr, u8* buf, u32 len)
{
    SimuI2CErrState result;

    rt_sem_take(simu_i2c->simu_i2c_sem, RT_WAITING_FOREVER);

    result = SimuI2CSend(simu_i2c,(u8)slave_addr, buf, len);
    if (result != SIMU_I2C_OK)
    {
        /*����ʧ�ܽ��и�λ����*/
        GPIO_SetBits(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin);//��֮ǰ�����1
        if (!(GPIO_ReadInputDataBit(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin)))
        {
            I2cBUSBusyRecovery(simu_i2c);
        }
        else
        {   
            SimuI2CStart(simu_i2c);
            SimuI2CStop(simu_i2c);
        }  
        rt_sem_release(simu_i2c->simu_i2c_sem);
        return 0x00;
    }
    rt_sem_release(simu_i2c->simu_i2c_sem);
    return len;
}

/***************************************************************************************************
*\Function      stm32_simu_i2c_recv
*\Description   
*\Parameter     simu_i2c
*\Parameter     slave_addr
*\Parameter     word_addr
*\Parameter     buf
*\Parameter     len
*\Return        u32
*\Note          
*\Log           2016.07.11    Ver 1.0    Job
*               ����������
***************************************************************************************************/
u32 stm32_simu_i2c_recv(stm32_simu_i2c_type* simu_i2c, u16 slave_addr, u8 word_addr, u8* buf, u32 len)
{
    SimuI2CErrState result;

    rt_sem_take(simu_i2c->simu_i2c_sem, RT_WAITING_FOREVER);

    result = SimuI2CRecv(simu_i2c,(u8)slave_addr, word_addr, buf, len);
    if (result != SIMU_I2C_OK)
    {
        /*����ʧ�ܽ��и�λ����*/
        GPIO_SetBits(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin);//��֮ǰ�����1
        if (!(GPIO_ReadInputDataBit(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin)))
        {
            I2cBUSBusyRecovery(simu_i2c);
        }
        else
        {   
            SimuI2CStart(simu_i2c);
            SimuI2CStop(simu_i2c);
        } 
        rt_sem_release(simu_i2c->simu_i2c_sem);
        return 0x00;
    }

    rt_sem_release(simu_i2c->simu_i2c_sem);

    return len;
}
/**************************************************************************************************/
/*ch45xר��ʱ��                                                                                   */
/**************************************************************************************************/
/***************************************************************************************************
*\Function      SimuI2CSendCh45x
*\Description   ch45xоƬר��ʱ��
*\Parameter     simu_i2c
*\Parameter     buf
*\Parameter     len
*\Return        SimuI2CErrState
*\Note          
*\Log           2016.07.28    Ver 1.0    Job
*               ����������
***************************************************************************************************/
static SimuI2CErrState SimuI2CSendCh45x(stm32_simu_i2c_type* simu_i2c, u8* buf, u32 len)
{
    SimuI2CErrState result;
    u32 i  = 0;

    /*��ʼ����*/
    result = SimuI2CStart(simu_i2c);
    if (result != SIMU_I2C_OK)
    {
        return result;
    }
    /*ѭ����������*/
    for (i = 0; i < len; i++)
    {
        SimuI2CByteSend(simu_i2c, buf[i]);
        SimuI2CNOACK(simu_i2c);
        //����ch45x��ack�����Բ��ù�ack
    }
    /*����ֹͣ����*/
    return SimuI2CStop(simu_i2c);
}
/***************************************************************************************************
*\Function      stm32_simu_i2c_send_ch45x
*\Description   
*\Parameter     simu_i2c
*\Parameter     buf
*\Parameter     len
*\Return        u32
*\Note          
*\Log           2016.07.28    Ver 1.0    Job
*               ����������
***************************************************************************************************/
u32 stm32_simu_i2c_send_ch45x(stm32_simu_i2c_type* simu_i2c, u8* buf, u32 len)
{
    SimuI2CErrState result;

    rt_sem_take(simu_i2c->simu_i2c_sem, RT_WAITING_FOREVER);

    result = SimuI2CSendCh45x(simu_i2c, buf, len);
    if (result != SIMU_I2C_OK)
    {
        /*����ʧ�ܽ��и�λ����*/
        GPIO_SetBits(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin);//��֮ǰ�����1
        if (!(GPIO_ReadInputDataBit(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin)))
        {
            I2cBUSBusyRecovery(simu_i2c);
        }
        else
        {   
            SimuI2CStart(simu_i2c);
            SimuI2CStop(simu_i2c);
        }  
        rt_sem_release(simu_i2c->simu_i2c_sem);
        return 0x00;
    }
    rt_sem_release(simu_i2c->simu_i2c_sem);
    return len;
}
/***************************************************************************************************
*\Function      SimuI2CRecvCh45x
*\Description   ch45xоƬר��
*\Parameter     simu_i2c
*\Parameter     cmd
*\Parameter     buf
*\Parameter     len
*\Return        SimuI2CErrState
*\Note          
*\Log           2016.07.28    Ver 1.0    Job
*               ����������
***************************************************************************************************/
static SimuI2CErrState SimuI2CRecvCh45x(stm32_simu_i2c_type* simu_i2c, u8 cmd, u8* buf, u32 len)
{
    SimuI2CErrState result;
    u32 i  = 0;
    /*��ʼ����*/
    result = SimuI2CStart(simu_i2c);
    if (result != SIMU_I2C_OK)
    {
        return result;
    }
    /*����������*/
    result = SimuI2CByteSend(simu_i2c, cmd);
    if (result != SIMU_I2C_OK)
    {
        return result;
    }
    /*�ȴ���������Ӧ ���ù�ack���*/
    SimuI2CNOACK(simu_i2c);

    /*ѭ������len������*/
    for (i = 0; i < len; i++)
    {
        SimuI2CByteRecv(simu_i2c, buf++);
        SimuI2CNOACK(simu_i2c);
    }
    return SimuI2CStop(simu_i2c);
}
/***************************************************************************************************
*\Function      stm32_simu_i2c_recv
*\Description   
*\Parameter     simu_i2c
*\Parameter     slave_addr
*\Parameter     buf
*\Parameter     len
*\Return        u32
*\Note          
*\Log           2016.07.28    Ver 1.0    Job
*               ����������
***************************************************************************************************/
u32 stm32_simu_i2c_recv_ch45x(stm32_simu_i2c_type* simu_i2c, u8 cmd, u8* buf, u32 len)
{
    SimuI2CErrState result;

    rt_sem_take(simu_i2c->simu_i2c_sem, RT_WAITING_FOREVER);

    result = SimuI2CRecvCh45x(simu_i2c, cmd, buf, len);
    if (result != SIMU_I2C_OK)
    {
        /*����ʧ�ܽ��и�λ����*/
        GPIO_SetBits(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin);//��֮ǰ�����1
        if (!(GPIO_ReadInputDataBit(simu_i2c->sda->gpio, simu_i2c->sda->gpio_pin)))
        {
            I2cBUSBusyRecovery(simu_i2c);
        }
        else
        {   
            SimuI2CStart(simu_i2c);
            SimuI2CStop(simu_i2c);
        } 
        rt_sem_release(simu_i2c->simu_i2c_sem);
        return 0x00;
    }

    rt_sem_release(simu_i2c->simu_i2c_sem);

    return len;
}
