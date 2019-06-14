/***************************************************************************************************
*                    (c) Copyright 2008-2017  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          drv_can.c
*\Description   
*\Note          
*\Log           2017.10.17    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#include "drv_can.h"
#include "rthw.h"

/***************************************************************************************************
*\Function      stm32_pin2pinsource
*\Description   
*\Parameter     src
*\Return        u32
*\Note          
*\Log           2015.11.12    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
static u32 stm32_pin2pinsource(u32 src)
{
    u32 result = 0;

    switch(src)
    {
    case GPIO_Pin_0:
        result = GPIO_PinSource0;
        break;
    case GPIO_Pin_1:
        result = GPIO_PinSource1;
        break;
    case GPIO_Pin_2:
        result = GPIO_PinSource2;
        break;
    case GPIO_Pin_3:
        result = GPIO_PinSource3;
        break;
    case GPIO_Pin_4:
        result = GPIO_PinSource4;
        break;
    case GPIO_Pin_5:
        result = GPIO_PinSource5;
        break;
    case GPIO_Pin_6:
        result = GPIO_PinSource6;
        break;
    case GPIO_Pin_7:
        result = GPIO_PinSource7;
        break;
    case GPIO_Pin_8:
        result = GPIO_PinSource8;
        break;
    case GPIO_Pin_9:
        result = GPIO_PinSource9;
        break;
    case GPIO_Pin_10:
        result = GPIO_PinSource10;
        break;
    case GPIO_Pin_11:
        result = GPIO_PinSource11;
        break;
    case GPIO_Pin_12:
        result = GPIO_PinSource12;
        break;
    case GPIO_Pin_13:
        result = GPIO_PinSource13;
        break;
    case GPIO_Pin_14:
        result = GPIO_PinSource14;
        break;
    case GPIO_Pin_15:
        result = GPIO_PinSource15;
        break;
    default:
        break;
    }

    return result;
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
*\Function      stm32_CAN_Filter_Add
*\Description   ��ӹ��˹���
*\Parameter     can_dev
*\Parameter     filter_num
*\Parameter     mode
*\Parameter     fifo
*\Parameter     id
*\Parameter     id_mask
*\Return        bool
*\Note          
*\Log           2017.10.30    Ver 1.0    Job               
				����������
***************************************************************************************************/
bool stm32_CAN_Filter_Add(CANBus_type* can_dev, u8 filter_num, u8 mode, u8 fifo, u32 id, u32 id_mask)
{
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;

    if (can_dev == RT_NULL)
    {
        return FALSE;
    }
    
    CAN_FilterInitStructure.CAN_FilterNumber = filter_num;
    CAN_FilterInitStructure.CAN_FilterMode = mode;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = fifo;

    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;

    CAN_FilterInitStructure.CAN_FilterIdLow = id;
    CAN_FilterInitStructure.CAN_FilterIdHigh = (id) >> 16;

    CAN_FilterInitStructure.CAN_FilterMaskIdLow = id_mask;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (id_mask) >> 16;

    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

    return TRUE;
}

/***************************************************************************************************
*\Function      stm32_CAN_init
*\Description   can��ʼ��
*\Parameter     can_dev
*\Return        void
*\Note          
*\Log           2017.10.30    Ver 1.0    Job               
				����������
***************************************************************************************************/
void stm32_CAN_init(CANBus_type* can_dev)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    CAN_InitTypeDef  CAN_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    if (can_dev == RT_NULL || can_dev->RxGpio == RT_NULL || can_dev->TxGpio == RT_NULL)
    {
        return;
    }

    can_dev->CanRx_Read = 0;
    can_dev->CanRx_Write = 0;

    //���ų�ʼ��
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /*rx���ų�ʼ��*/
    gpio_rcc_enable(can_dev->RxGpio->gpio);
    GPIO_InitStructure.GPIO_Pin = (can_dev->RxGpio)->gpio_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(can_dev->RxGpio->gpio, &GPIO_InitStructure);
	
	/*tx���ų�ʼ��*/
    gpio_rcc_enable(can_dev->TxGpio->gpio);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
    GPIO_InitStructure.GPIO_Pin = can_dev->TxGpio->gpio_pin;
    GPIO_Init(can_dev->TxGpio->gpio, &GPIO_InitStructure);
	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    //ʱ��ʹ��
    switch ((u32)(can_dev->can))
    {
    case CAN1_BASE:
        /* Enable CAN clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

        NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
        NVIC_Init(&NVIC_InitStructure);
        break;
    case CAN2_BASE:  
        /* Connect alternate function */

        break;
    default:
        break;
    }
   

    //can��ʼ��
    /* CAN register init */
    CAN_DeInit(can_dev->can);
    CAN_StructInit(&CAN_InitStructure);

    /* CAN cell init */
    CAN_InitStructure.CAN_TTCM = DISABLE;//��ֹʱ�䴥��
    CAN_InitStructure.CAN_ABOM = ENABLE;//ʹ���Զ����߹���
    CAN_InitStructure.CAN_AWUM = DISABLE;//��ֹ�Զ����ѹ���
    CAN_InitStructure.CAN_NART = ENABLE;//�Զ��ط��͡�
    CAN_InitStructure.CAN_RFLM = DISABLE;//��ֹFIFO����
    CAN_InitStructure.CAN_TXFP = ENABLE;//����˳���ɷ�������˳�����

    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;//CAN_SJW_3tq;
    /* CAN Baudrate = 1 MBps (CAN clocked at 36 MHz) 36Mhz / ((CAN_BS1 + CAN_BS2 + 1) * CAN_Prescaler)*/
    if (can_dev->baudrate == CAN_BAUDRATE_100K)
    {
        CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
        CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
        CAN_InitStructure.CAN_Prescaler = 40;
    }
    else if (can_dev->baudrate == CAN_BAUDRATE_200K)
    {
        CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
        CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
        CAN_InitStructure.CAN_Prescaler = 20;
    }
    else if (can_dev->baudrate == CAN_BAUDRATE_500K)
    {
        CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
        CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
        CAN_InitStructure.CAN_Prescaler = 8;
    }
    else if (can_dev->baudrate == CAN_BAUDRATE_1M)
    {
        CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
        CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
        CAN_InitStructure.CAN_Prescaler = 4;
    }
    else
    {
        //Ĭ��500K
        CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
        CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
        CAN_InitStructure.CAN_Prescaler = 8;
    }
    
    CAN_Init(can_dev->can, &CAN_InitStructure);

    //Ĭ����ɸѡ
    if (can_dev->can == CAN1)
    {
        stm32_CAN_Filter_Add(can_dev, 0, CAN_FilterMode_IdMask, CAN_Filter_FIFO0, 0, 0);
    }
    else
    {
        stm32_CAN_Filter_Add(can_dev, 14, CAN_FilterMode_IdMask, CAN_Filter_FIFO0, 0, 0);
    }

    /* Enable FIFO 0 message pending Interrupt */
    CAN_ITConfig(can_dev->can, CAN_IT_FMP0, ENABLE);
    /* Enable FIFO 0 message pending Interrupt */
    CAN_ITConfig(can_dev->can, CAN_IT_FMP1, ENABLE);
}

/***************************************************************************************************
*\Function      stm32_CAN_recv
*\Description   ����һ֡����
*\Parameter     can_dev
*\Parameter     rx
*\Return        bool
*\Note          
*\Log           2017.10.30    Ver 1.0    Job               
				����������
***************************************************************************************************/
bool stm32_CAN_recv(CANBus_type* can_dev, CanRxMsg* rx)
{
    if (can_dev == RT_NULL || rx == RT_NULL)
    {
        return FALSE;
    }
    
    if (can_dev->CanRx_Read == can_dev->CanRx_Write)
    {
        return FALSE;
    }

    rt_memcpy(rx, &can_dev->CANRxBuf[can_dev->CanRx_Read++], sizeof(CanRxMsg));
    can_dev->CanRx_Read %= CAN_RX_BUFFER_SIZE;

    return TRUE;
}
/***************************************************************************************************
*\Function      stm32_CAN_clear
*\Description   
*\Parameter     can_dev
*\Return        bool
*\Note          
*\Log           2017.11.18    Ver 1.0    Job               
				����������
***************************************************************************************************/
bool stm32_CAN_clear(CANBus_type* can_dev)
{
    if (can_dev == RT_NULL)
    {
        return FALSE;
    }

    can_dev->CanRx_Read = can_dev->CanRx_Write;

    return TRUE;
}
/***************************************************************************************************
*\Function      stm32_CAN_send
*\Description   ����һ֡����
*\Parameter     can_dev
*\Parameter     tx
*\Return        bool
*\Note          
*\Log           2017.10.30    Ver 1.0    Job               
				����������
***************************************************************************************************/
bool stm32_CAN_send(CANBus_type* can_dev, CanTxMsg* tx)
{
	uint8_t transmit_mailbox;
	rt_size_t fail_times = 0;
	u32 uwCounter = 0;

	if (can_dev == RT_NULL || tx == RT_NULL)
	{
		return FALSE;
	}

	/* polling mode */
	while (1)
	{
		transmit_mailbox = CAN_Transmit(can_dev->can, tx);
		uwCounter = 0;
		while (uwCounter++ <= 0xFFFF)
		{
			if (CAN_TransmitStatus(can_dev->can, transmit_mailbox) == CANTXOK)
			{
				return TRUE;
			}
		}
		fail_times++;
		if (fail_times >= CAN_TX_TRY_TIMES)
		{
			return FALSE;
		}
	}
}

/***************************************************************************************************
*\Function      stm32_CAN_recv_isr
*\Description   �жϴ���
*\Parameter     can_dev
*\Parameter     can_fifo
*\Return        void
*\Note          
*\Log           2017.10.30    Ver 1.0    Job               
				����������
***************************************************************************************************/
void stm32_CAN_recv_isr(CANBus_type* can_dev, u8 can_fifo)
{
    u32 it_status;

    if(can_fifo == CAN_FIFO0)
    {
        it_status = CAN_IT_FMP0;
    }
    else if(can_fifo == CAN_FIFO1)
    {
        it_status = CAN_IT_FMP1;
    }

    if(CAN_GetITStatus(can_dev->can, it_status) != RESET)
    {
        /* save on rx buffer */
        while (CAN_MessagePending(can_dev->can, can_fifo) > 0)
        {
            rt_base_t level;

            /* disable interrupt */
            level = rt_hw_interrupt_disable();

            /* save character */
            CAN_Receive(can_dev->can, can_fifo, &(can_dev->CANRxBuf[can_dev->CanRx_Write++]));
            can_dev->CanRx_Write %= CAN_RX_BUFFER_SIZE;

            if (can_dev->CanRx_Write == can_dev->CanRx_Read)
            {
                can_dev->CanRx_Read++;
                can_dev->CanRx_Read %= CAN_RX_BUFFER_SIZE;
            }
            /* enable interrupt */
            rt_hw_interrupt_enable(level);
        }
    }
}


