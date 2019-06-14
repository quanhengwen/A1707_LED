/***************************************************************************************************
*                    (c) Copyright 1992-2014 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          stm32_18b20.c
*\Description   
*\Note          
*\Log           2014.12.04    Ver 1.0    �Ų�
*               �����ļ���
***************************************************************************************************/
#include "drv_18b20.h"

Ds18B20TempType Env_TMP;
Ds18B20TempType DC_TMP;

TempDs18B20Type Tempreture1,Tempreture2;

//��ȷһλС����
const unsigned char TempX_TAB[16]={0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x04,0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x09};

/***************************************************************************************************
*\Function      stm32_18b20_delay
*\Description   �˺�����ʱ������us����ģ�����Ӱ��ϵͳ����
*\Parameter     temp
*\Parameter     us
*\Return        void
*\Note          
*\Log           2014.12.04    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void stm32_ds18b20_delay(TempDs18B20Type* temp, u32 us)
{
    //u32 i = 0xFFFFFF;//����ʱ����ֹ��ʱ�޷�����

    (temp->Tim)->CNT = 0;
    (temp->Tim)->ARR = (us);
    TIM_Cmd(temp->Tim, ENABLE);
    while (((temp->Tim)->CR1) & TIM_CR1_CEN)//ֻҪʹ�ܣ�˵����ʱδ����
    {
        /*i--;
        if (!i)
        {
            break;
        }*/
    }
}

/***************************************************************************************************
*\Function      ds18B20_reset
*\Description   18b20��λ�ӿ�
*\Parameter     temp
*\Return        bool
*\Note          
*\Log           2014.12.04    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool ds18B20_reset(TempDs18B20Type* temp)
{
    bool result = FALSE;

    GPIO_SetBits(temp->Gpio, temp->Pin);
    stm32_ds18b20_delay(temp, 50);
    GPIO_ResetBits(temp->Gpio, temp->Pin);
    stm32_ds18b20_delay(temp, 500);
    GPIO_SetBits(temp->Gpio, temp->Pin);
    stm32_ds18b20_delay(temp, 40);
    if (GPIO_ReadInputDataBit(temp->Gpio, temp->Pin) == 0)//ds18b20����0����ʾ��λ�ɹ�
    {
        result = TRUE;
    }
    stm32_ds18b20_delay(temp, 500);
    return result;
}

/***************************************************************************************************
*\Function      ds18b20_write_byte
*\Description   дһ���ֽ�
*\Parameter     temp
*\Parameter     data
*\Return        void
*\Note          
*\Log           2014.12.04    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void ds18b20_write_byte(TempDs18B20Type* temp, u8 data)
{
    u8 i = 0;

    for (i = 0; i < 8; i++)
    {
        GPIO_ResetBits(temp->Gpio, temp->Pin);//��15u���������������ϣ�DS18B20��15-60u����
        stm32_ds18b20_delay(temp, 5);
        if (data & 0x01)
        {
            GPIO_SetBits(temp->Gpio, temp->Pin);
        }
        else
        {
            GPIO_ResetBits(temp->Gpio, temp->Pin);
        }
        stm32_ds18b20_delay(temp, 80);
        GPIO_SetBits(temp->Gpio, temp->Pin);
        stm32_ds18b20_delay(temp, 2);
        data >>= 1;
    }
}

/***************************************************************************************************
*\Function      ds18b20_read_byte
*\Description   ��һ���ֽ�
*\Parameter     temp
*\Return        u8
*\Note          
*\Log           2014.12.04    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u8 ds18b20_read_byte(TempDs18B20Type* temp)
{
    u8 i =0;
    u8 data = 0;

    GPIO_SetBits(temp->Gpio, temp->Pin);
    stm32_ds18b20_delay(temp, 5);
    for(i = 8; i > 0; i--)
    {
        data >>= 1;
        GPIO_ResetBits(temp->Gpio, temp->Pin);     //�Ӷ�ʱ��ʼ�������ź��߱�����15u�ڣ��Ҳ�������������15u�����
        stm32_ds18b20_delay(temp, 5);   //5us
        GPIO_SetBits(temp->Gpio, temp->Pin);
        stm32_ds18b20_delay(temp, 5);   //5us
        if(GPIO_ReadInputDataBit(temp->Gpio, temp->Pin))
        {
            data |= 0x80;
        }
        else
        {
            data &=0x7f;  
        }
        stm32_ds18b20_delay(temp, 65);   //65us
        GPIO_SetBits(temp->Gpio, temp->Pin);
    }
    return data;
}

/***************************************************************************************************
*\Function      stm32_ds18b20_init
*\Description   ��ʼ��ds18b20,
*\Parameter     temp
*\Parameter     gpio
*\Parameter     pin��io
*\Parameter     tim,��ȷ��ʱTIM
*\Return        void
*\Note          1��Ĭ��18b20�ķֱ���Ϊ12λ����0.0625��
*\Log           2014.12.05    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void stm32_ds18b20_init(TempDs18B20Type* temp, GPIO_TypeDef* gpio, u16 pin, TIM_TypeDef* tim)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    temp->Gpio = gpio;
    temp->Pin = pin;
    temp->Tim = tim;

    //�˿�ʱ��ʹ��
    switch((u32)(temp->Gpio))
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
    default:
        rt_kprintf("18b20 gpio is undefine!\n");
        break;
    }
    /*���ų�ʼ��,��©ģʽ*/
    GPIO_InitStructure.GPIO_Pin = temp->Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(temp->Gpio, &GPIO_InitStructure);
    GPIO_SetBits(temp->Gpio, temp->Pin);//����ߵ�ƽ

    //tim��ʼ��
    /*������ʱ����Ӧ��ʱ��*/
    switch ((u32)(temp->Tim))
    {
    case TIM1_BASE:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
        break;
    case TIM2_BASE:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
        break;
    case TIM3_BASE:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
        break;
    case TIM4_BASE:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
        break;
    case TIM5_BASE:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
        break;
    case TIM6_BASE:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
        break;
    case TIM7_BASE:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
        break;
        //     case TIM8_BASE:
        //         RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
        //         break;
    default:
        rt_kprintf("temp tim is undefine!\n");
        break;
    }
    TIM_DeInit(temp->Tim);
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;	
    TIM_TimeBaseStructure.TIM_Prescaler = 72-1;    	//Ԥ��Ƶ,��ֵ+1Ϊ��Ƶ�ĳ���,ÿ������1us
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0; 
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 	//���ϼ���	
    TIM_TimeBaseInit(temp->Tim, &TIM_TimeBaseStructure); 
    TIM_SelectOnePulseMode(temp->Tim, TIM_OPMode_Single);
}

/***************************************************************************************************
*\Function      stm32_ds18b20_conv_start
*\Description   �����¶�ת����ת������һ��ʱ�����ʱ����������ȥ��ȡ
*\Parameter     temp
*\Return        void
*\Note          
*\Log           2014.12.05    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void stm32_ds18b20_conv_start(TempDs18B20Type* temp)
{
    ds18B20_reset(temp);
    ds18b20_write_byte(temp, DS18B20_ROM_CMD_SKIP);
    ds18b20_write_byte(temp, DS18B20_REG_CMD_CONV);
}

/***************************************************************************************************
*\Function      stm32_ds18b20_read_rawdata
*\Description   ��ȡ18b20�Ĵ����е�ԭʼ����
*\Parameter     temp
*\Return        u16��Ĭ��12λ�ֱ����£���5λΪ����λ
*\Note          1����Ϊ����ʱΪ������ʽ
*\Log           2014.12.05    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u16 stm32_ds18b20_read_rawdata(TempDs18B20Type* temp)
{
    u16 data1 = 0;
    u16 data2 = 0;

    ds18B20_reset(temp);
    ds18b20_write_byte(temp, DS18B20_ROM_CMD_SKIP);
    ds18b20_write_byte(temp, DS18B20_REG_CMD_READ_RAM);
    data1 = ds18b20_read_byte(temp);
    data2 = ds18b20_read_byte(temp);
    data2 = (data2 << 8) | data1;

    return data2;
}

/***************************************************************************************************
*\Function      stm32_ds18b20_raw2reality
*\Description   ת��Ϊfloat��ʽ�����Ȳ���ʧ
*\Parameter     data
*\Return        float
*\Note          
*\Log           2014.12.05    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
float stm32_ds18b20_raw2reality(u16 data)
{
    float t = 0.0;

    if (data & 0x8000)//������ʽ������
    {
        data = ~data;
        data += 1;
        t = -(data * 0.0625);
    }
    else
    {
        t = data * 0.0625;
    }

    return t;
}

/***************************************************************************************************
*\Function      stm32_ds18b20_raw2Temp
*\Description   ԭʼ����ת��Ϊ������С����1λ��������
*\Parameter     data
*\Return        Ds18B20TempType
*\Note          
*\Log           2014.12.05    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
Ds18B20TempType stm32_ds18b20_raw2Temp(u16 data)
{
    Ds18B20TempType aTmp;
    u8 tmp_h = (u8)(data >> 8);
    u8 tmp_l = (u8)(data);
    u8 sign = 0;

    if(tmp_h & 0x80)
    {
        tmp_h = (~tmp_h) | 0x08;
        tmp_l = ~tmp_l + 1;
        if(tmp_l == 0)
        {
            tmp_h += 1;
        }
    }

    tmp_h = (tmp_h << 4) + ((tmp_l & 0xf0) >> 4);
    tmp_l = TempX_TAB[tmp_l & 0x0f];

    sign = tmp_h & 0x80;
    if(sign == 0x00)
    {
        aTmp.Signal = 0;//����
    }
    else
    {
        aTmp.Signal = 1;
    }

    tmp_h &= 0x7f;
    aTmp.TmpH = tmp_h;
    aTmp.TmpL = tmp_l;
    
    return aTmp;
}

/***************************************************************************************************
*\Function      stm32_ds18b20_read_reality
*\Description   ��ȡ��ȷ��ֵ
*\Parameter     temp
*\Return        float
*\Note          
*\Log           2014.12.05    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
float stm32_ds18b20_read_reality(TempDs18B20Type* temp)
{
    float t = 0.0;
    u16 data = 0;

    stm32_ds18b20_conv_start(temp);
    rt_thread_delay(80);//��ʱ800ms
    data = stm32_ds18b20_read_rawdata(temp);
    t = stm32_ds18b20_raw2reality(data);

    return t;
}

/***************************************************************************************************
*\Function      stm32_ds18b20_read_Temp
*\Description   ��ȡ���� С�� ���������¶�
*\Parameter     temp
*\Return        Ds18B20TempType
*\Note          
*\Log           2014.12.05    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
Ds18B20TempType stm32_ds18b20_read_Temp(TempDs18B20Type* temp)
{
    Ds18B20TempType aTmp;
    u16 data = 0;

    stm32_ds18b20_conv_start(temp);
    rt_thread_delay(80);//��ʱ800ms
    data = stm32_ds18b20_read_rawdata(temp);
    aTmp = stm32_ds18b20_raw2Temp(data);

    return aTmp;
}
void stm32_ds18b20_thread_entry(void* para)
{
//    stm32_ds18b20_init(&Tempreture1, GPIOC, GPIO_Pin_11, TIM5);
    //stm32_ds18b20_init(&Tempreture2, GPIOB, GPIO_Pin_6, TIM6);
    while(1)
    {  
        //DC_TMP =  stm32_ds18b20_read_Temp(&Tempreture1);
        Env_TMP = stm32_ds18b20_read_Temp(&Tempreture1);
        
        rt_thread_delay(100);
    }

}
