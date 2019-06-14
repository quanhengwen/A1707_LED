/***************************************************************************************************
*                    (c) Copyright 2008-2017  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          drv_adc.c
*\Description   
*\Note          
*\Log           2017.05.11    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#include "drv_adc.h"
#include "common_lib.h"

/***************************************************************************************************
*\Function      stm32_adc_with_dma_init
*\Description   ��ʼ��adc dma���ã���û���������ݴ���
*\Parameter     adc_dev
*\Return        bool
*\Note          
*\Log           2016.11.16    Ver 1.0    Job               
����������
***************************************************************************************************/
bool stm32_adc_with_dma_init(ADC_DMA_Type* adc_dev)
{
    DMA_InitTypeDef DMA_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    if (adc_dev == RT_NULL || adc_dev->dma == RT_NULL || adc_dev->adc == RT_NULL
        || adc_dev->adc_data == RT_NULL)
    {
        return FALSE;
    }

    if (adc_dev->dma == DMA1_Channel1 || adc_dev->dma == DMA1_Channel2|| adc_dev->dma == DMA1_Channel3||
        adc_dev->dma == DMA1_Channel4 || adc_dev->dma == DMA1_Channel5 || adc_dev->dma == DMA1_Channel6|| 
        adc_dev->dma == DMA1_Channel7)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    }
    else if(adc_dev->dma == DMA2_Channel1 || adc_dev->dma == DMA2_Channel2|| adc_dev->dma == DMA2_Channel3||
        adc_dev->dma == DMA2_Channel4 || adc_dev->dma == DMA2_Channel5)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    }
    else
    {
        return FALSE;
    }

    // ����ADC
    RCC_ADCCLKConfig(RCC_PCLK2_Div4);   
    //adcʱ��ʹ��
    if (adc_dev->adc == ADC1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
    }
    else if (adc_dev->adc == ADC2)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2,ENABLE);
    }
    else if (adc_dev->adc == ADC3)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3,ENABLE);
    }
    else
    {
        return FALSE;
    }
    // DMA1 CHANNEL1
    DMA_DeInit(adc_dev->dma);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&adc_dev->adc->DR;            //ADC1�����ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)adc_dev->adc_data;                     //�ڴ��ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                                 //dma���䷽��
    DMA_InitStructure.DMA_BufferSize = adc_dev->data_len;                              //����DMA�ڴ���ʱ�������ĳ��� word
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                   //����DMA���������ģʽ��һ������
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                            //����DMA���ڴ����ģʽ��
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;        //���������ֳ�
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;                //�ڴ������ֳ�
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                                    //����DMA�Ĵ���ģʽ���������ϵ�ѭ��ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                                 //����DMA�����ȼ���
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                              
    DMA_Init(adc_dev->dma, &DMA_InitStructure);
    // Enable DMA1 channel1
    DMA_ITConfig(adc_dev->dma, DMA_IT_TC, DISABLE);//��ʱ����dma��������ж�
    DMA_Cmd(adc_dev->dma, ENABLE);

    // ADC1 configuration ------------------------------------------------------
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                   //��������ģʽ
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;                         //ɨ�跽ʽ
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                   //����ת��
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  //�ⲿ������ֹ
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;               //�����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = adc_dev->max_num;                //����ת����ͨ����
    ADC_Init(adc_dev->adc, &ADC_InitStructure);

    //ʹ���ڲ��¶ȴ������Ĳο���ѹ 
    ADC_TempSensorVrefintCmd(ENABLE); 

    //����ADC1��DMAģʽ
    ADC_DMACmd(adc_dev->adc, ENABLE);

    //����ADC1
    ADC_Cmd(adc_dev->adc, ENABLE);

    //����У׼�Ĵ���   
    ADC_ResetCalibration(adc_dev->adc);
    while(ADC_GetResetCalibrationStatus(adc_dev->adc));

    //��ʼУ׼״̬
    ADC_StartCalibration(adc_dev->adc);
    while(ADC_GetCalibrationStatus(adc_dev->adc));

    return TRUE;
}
/***************************************************************************************************
*\Function      stm32_adc_channel_add
*\Description   ���adcͨ��
*\Parameter     adc_dev
*\Parameter     seq_num ����� ��0 ��ʼ�ڲ�+1
*\Parameter     channel��ͨ��
*\Parameter     sample������ʱ��
*\Return        bool
*\Note          1��ע��rank�Ǵ�1��ʼ��
*\Log           2016.11.16    Ver 1.0    Job               
����������
***************************************************************************************************/
bool stm32_adc_channel_add(ADC_DMA_Type* adc_dev,u32 seq_num, u32 channel, u32 sample, GpioType* port)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    if (adc_dev == RT_NULL || adc_dev->adc_count >= 16)
    {
        return FALSE;
    }

    if (port != RT_NULL)
    {
        //ʹ�ܶ˿�ʱ��
        switch((u32)port->gpio)
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
            break;
        }

        //��ʼ��gpio����
        GPIO_InitStruct.GPIO_Pin   = port->gpio_pin;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

        GPIO_Init(port->gpio, &GPIO_InitStruct); 
    }
    /*##-2- Configure ADC regular channel ######################################*/
    ADC_RegularChannelConfig(adc_dev->adc, channel, seq_num + 1, sample);
    adc_dev->adc_count++;
    return TRUE;
}
/***************************************************************************************************
*\Function      stm32_adc_with_dma_start
*\Description   ����dma���ݴ���
*\Parameter     adc_dev
*\Return        bool
*\Note          
*\Log           2016.11.16    Ver 1.0    Job               
				����������
***************************************************************************************************/
bool stm32_adc_with_dma_start(ADC_DMA_Type* adc_dev)
{
    if (adc_dev == RT_NULL)
    {
        return FALSE;
    }

    //�˹���ADCת��
    ADC_SoftwareStartConvCmd(adc_dev->adc, ENABLE);
    return TRUE;
}

/***************************************************************************************************
*\Function      stm32_adc_get_value
*\Description   ��ȡadc���ݣ���ȡ�˲��㷨֮�������
*\Parameter     adc_dev
*\Parameter     seq_num ��0��ʼ
*\Return        u16
*\Note          1����ʱ����ӹ����㷨������Ч��
*\Log           2016.11.16    Ver 1.0    Job               
				����������
***************************************************************************************************/
u16 stm32_adc_get_value(ADC_DMA_Type* adc_dev, u32 seq_num)
{
    u32 i = 0;
    u32 adc_sum = 0;
    u16 adc_avg = 0;

    if (adc_dev == RT_NULL)
    {
        return FALSE;
    }
    
    for (i = 0; i < adc_dev->data_len / adc_dev->max_num; i++)
    {
        adc_sum += adc_dev->adc_data[i * adc_dev->max_num + seq_num];
    }

    adc_avg = adc_sum / (adc_dev->data_len / adc_dev->max_num);

    return adc_avg;
}

/***************************************************************************************************
*\Function      get_chip_temp
*\Description   ת��оƬ�¶�
*\Parameter     
*\Return        double
*\Note          ���㹫ʽ��3.3*Vadc/4096 - 0.76)*1000/ 2.5 + 25
*\Log           2016.07.01    Ver 1.0    job
*               ����������
***************************************************************************************************/
u32 stm32_get_chip_temp(u16 value)
{
    double temp = 0.0;

    temp= (1.42 - value * 3.3 / 4096) * 1000 / 4.3 + 25;

    return (u32)temp;
}
/***************************************************************************************************
��ʹ��DMA�Ľӿ�
***************************************************************************************************/
#if 0
bool stm32_adc_init(ADC_DMA_Type* adc_dev)
{
    ADC_InitTypeDef ADC_InitStructure;

    if (adc_dev == RT_NULL || adc_dev->dma == RT_NULL || adc_dev->adc == RT_NULL
        || adc_dev->adc_data == RT_NULL)
    {
        return FALSE;
    }

    if (adc_dev->dma == DMA1_Channel1 || adc_dev->dma == DMA1_Channel2|| adc_dev->dma == DMA1_Channel3||
        adc_dev->dma == DMA1_Channel4 || adc_dev->dma == DMA1_Channel5 || adc_dev->dma == DMA1_Channel6|| 
        adc_dev->dma == DMA1_Channel7)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    }
    else if(adc_dev->dma == DMA2_Channel1 || adc_dev->dma == DMA2_Channel2|| adc_dev->dma == DMA2_Channel3||
        adc_dev->dma == DMA2_Channel4 || adc_dev->dma == DMA2_Channel5)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    }
    else
    {
        return FALSE;
    }

    // ����ADC
    RCC_ADCCLKConfig(RCC_PCLK2_Div4);   
    //adcʱ��ʹ��
    if (adc_dev->adc == ADC1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
    }
    else if (adc_dev->adc == ADC2)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2,ENABLE);
    }
    else if (adc_dev->adc == ADC3)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3,ENABLE);
    }
    else
    {
        return FALSE;
    }
    // ADC1 configuration ------------------------------------------------------
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                   //��������ģʽ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                         //ɨ�跽ʽ
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                   //����ת��
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  //�ⲿ������ֹ
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;               //�����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 1;                //����ת����ͨ����
    ADC_Init(adc_dev->adc, &ADC_InitStructure);

    //ʹ���ڲ��¶ȴ������Ĳο���ѹ 
    ADC_TempSensorVrefintCmd(ENABLE); 

    //����ADC1
    ADC_Cmd(adc_dev->adc, ENABLE);

    //����У׼�Ĵ���   
    ADC_ResetCalibration(adc_dev->adc);
    while(ADC_GetResetCalibrationStatus(adc_dev->adc));

    //��ʼУ׼״̬
    ADC_StartCalibration(adc_dev->adc);
    while(ADC_GetCalibrationStatus(adc_dev->adc));

    return TRUE;
}
/***************************************************************************************************
*\Function      stm32_adc_start_convert
*\Description   
*\Parameter     adc_dev
*\Parameter     channel
*\Parameter     sample
*\Parameter     port
*\Return        u16
*\Note          
*\Log           2017.05.12    Ver 1.0    Job               
				����������
***************************************************************************************************/
u16 stm32_adc_start_convert(ADC_DMA_Type* adc_dev, u32 channel, u32 sample, GpioType* port)
{
    u32 time_out = 0x00;
    GPIO_InitTypeDef GPIO_InitStruct;

    if (adc_dev == RT_NULL || adc_dev->adc_count >= 16)
    {
        return FALSE;
    }

    if (port != RT_NULL)
    {
        //ʹ�ܶ˿�ʱ��
        switch((u32)port->gpio)
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
            break;
        }

        //��ʼ��gpio����
        GPIO_InitStruct.GPIO_Pin   = port->gpio_pin;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

        GPIO_Init(port->gpio, &GPIO_InitStruct); 
    }
    /*##-2- Configure ADC regular channel ######################################*/
    ADC_RegularChannelConfig(adc_dev->adc, channel, 1, sample);   //оƬ�¶�

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);     //ʹ��ָ����ADC1�����ת����������
    while (time_out++ < RT_TICK_PER_SECOND)
    {
        rt_thread_delay(1);
        if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ))//�ȴ�ת������
        {
            break;
        }
    }

    return ADC_GetConversionValue(ADC1);    //�������һ��ADC1�������ת�����
}

///*MCU�ڲ��¶ȴ�����*/
static u16 ADC_InterT_Buf[16];
ADC_DMA_Type ADC_InterT = {DMA1_Channel1, ADC1, 1, (u16*)&ADC_InterT_Buf, countof(ADC_InterT_Buf)};

u32 Temp_Chip;
void chip_temp_thread_entry(void* p)
{
    //adc�ڲ��¶�
    stm32_adc_init(&ADC_InterT);

    while (1)
    {
//        Temp_Chip = stm32_get_chip_temp(stm32_adc_start_convert(&ADC_InterT, ADC_Channel_16, ADC_SampleTime_239Cycles5, RT_NULL));
        rt_thread_delay(100);
    }
}
#endif
