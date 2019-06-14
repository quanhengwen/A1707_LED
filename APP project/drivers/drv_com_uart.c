/***************************************************************************************************
*                    (c) Copyright 2008-2012 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          stm32_uart.c
*\Description   uart����
*\Note          
*\Log           2012.12.22    Ver 1.0    �Ų�
*               �����ļ���
***************************************************************************************************/
#include "drv_com_uart.h"

//uart�жϷ���ģʽʹ��
//#define UART_SENDIT_MODE_EN  1

/***************************************************************************************************
*\Function      stm32_uart_isr
*\Description   uart �жϴ�����
*\Parameter     num
*\Return        void
*\Note          
*\Log           2012.12.22    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void stm32_uart_isr(UartDevType* uart_dev)
{
    u8 data = 0;
    u32 uart = (u32)uart_dev->uart;

    /*�жϽ���*/
    if (UART_HAS_DATA_REC(uart))
    {
        /*���ջ��������գ�������ȡ�������ջ�����*/
        data = UART_READ_DATA(uart);
        AddByteToBuf(&uart_dev->rxbuf, data);
        //���յ�һ���ֽڵĻص�
        if (uart_dev->recv_hook.phook != RT_NULL)
        {
            uart_dev->recv_hook.phook(uart_dev->recv_hook.para);
        }
    }
    if (((UART_IS_TXEI_ENABLE(uart))) && (!(UART_HAS_NOT_DATA_SND(uart))) )
    {
        /*���ͻ��������һ�ֽ�*/
        UART_SEND_DATA(uart, (*(uart_dev->txbuf.TxBuf + uart_dev->txbuf.offset)));
        uart_dev->txbuf.offset++;
        if (uart_dev->txbuf.offset >= uart_dev->txbuf.len)
        {
            UART_DISABLE_TXEI(uart);
            /*�ȴ�������ȫ�������*/
            UART_ENABLE_TCEI(uart);
        }
    }

    if((UART_IS_TXCI_ENABLE(uart)) && (UART_HAS_ALL_SND(uart)))
    {
        UART_DISABLE_TCEI(uart);
        UART_CLR_DATA_TX_END(uart);
        //������ɵĻص�
        if (uart_dev->send_hook.phook != RT_NULL)
        {
            uart_dev->send_hook.phook(uart_dev->send_hook.para);
        }
        rt_sem_release(uart_dev->uart_sendIT); /*���ݷ�����ɣ��ͷ��ź���֪ͨ���ݷ��ͺ��������ݷ������*/
    }

    if (IS_RX_OVERFLOW(uart)) /*�������*/
    {
        UART_READ_DATA(uart);
    }
    else if(IS_NOISE_ERR(uart))/*��������*/
    {
        UART_READ_DATA(uart);
    }
    else if (IS_FRAME_ERR(uart))/*֡����*/
    {
        UART_READ_DATA(uart);
    }
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
*\Function      uart_rcc_enable
*\Description   uartʱ��
*\Parameter     uart
*\Return        void
*\Note          
*\Log           2016.07.26    Ver 1.0    Job
*               ����������
***************************************************************************************************/
static void uart_rcc_enable(USART_TypeDef* uart)
{
    switch ((u32)uart)
    {
    case USART1_BASE:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
        break;
    case USART2_BASE:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
        break;
    case USART3_BASE:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
        break;
    case UART4_BASE:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
        break;
    case UART5_BASE:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
        break;
    /*case USART6_BASE:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
        break;*/
    default:
        rt_kprintf("uart rcc error!\n");
        break;
    }
}
/***************************************************************************************************
*\Function      stm32_uart_init
*\Description   ��ʼ��uart����
*\Return        void
*\Note          1��uartĬ�ϳ�ʼ��Ϊ8����λ��1��ֹͣλ���޼���
*\              3��232���Ժ���ctrlgpio����
*\Log           2012.12.22    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void stm32_uart_init(UartDevType* uart_dev)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef	nvic_conf;

    if (uart_dev == RT_NULL)
    {
        return;
    }

    /*��ʼ��ʹ�õ�recv������*/
    RxBufInit(&(uart_dev->rxbuf), UART_RX_BUF_SIZE);
    /*�ź�������*/
    uart_dev->uart_sem = rt_sem_create("uart_sem", 1, RT_IPC_FLAG_FIFO);//uart�����ź���
    uart_dev->uart_sendIT = rt_sem_create("sendIT", 0, RT_IPC_FLAG_FIFO);//uart���͵ȴ��ź���

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //uart ʱ�� �ȳ�ʼ������ ���Է�ֹ��ӡ0xFF
    uart_rcc_enable(uart_dev->uart);

    if (uart_dev->baudrate == 0 || uart_dev->baudrate > 115200)//����������
    {
        rt_kprintf("uart baudrate:%d error!\n", uart_dev->baudrate);
        return;
    }

    USART_InitStructure.USART_BaudRate = uart_dev->baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(uart_dev->uart, &USART_InitStructure);

    if (uart_dev->uart == USART1 || uart_dev->uart == USART2 || uart_dev->uart == USART3)// || uart_dev->uart == USART6)
    {
        USART_ClockInitTypeDef USART_ClockInitStructure;

        USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
        USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
        USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
        USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
        USART_ClockInit(uart_dev->uart, &USART_ClockInitStructure);
    }

    //���ų�ʼ��
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    //tx���ų�ʼ��
    if (uart_dev->TxGpio != RT_NULL)
    {
        gpio_rcc_enable(uart_dev->TxGpio->gpio);

        /* Configure USART Tx as alternate function push-pull */
        GPIO_InitStructure.GPIO_Pin = uart_dev->TxGpio->gpio_pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(uart_dev->TxGpio->gpio, &GPIO_InitStructure);
    }
    /*rx���ų�ʼ��*/
    if (uart_dev->RxGpio != RT_NULL)
    {
        gpio_rcc_enable(uart_dev->RxGpio->gpio);

        /* Configure USART Rx as input floating */
        GPIO_InitStructure.GPIO_Pin = (uart_dev->RxGpio)->gpio_pin;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
        GPIO_Init(uart_dev->RxGpio->gpio, &GPIO_InitStructure);
    }
    
    //485���ƽ�
    if (uart_dev->Rs485Ctrl != RT_NULL)
    {
        stm32_switch_init(uart_dev->Rs485Ctrl);
    }

    //�ж�����
    nvic_conf.NVIC_IRQChannelSubPriority = 0;
    nvic_conf.NVIC_IRQChannelCmd = ENABLE;
    nvic_conf.NVIC_IRQChannelPreemptionPriority =  0;

    switch ((u32)(uart_dev->uart))
    {
        case USART1_BASE:
            nvic_conf.NVIC_IRQChannel = USART1_IRQn;
            break;
        case USART2_BASE:
            nvic_conf.NVIC_IRQChannel = USART2_IRQn;
            break;
        case USART3_BASE:
            nvic_conf.NVIC_IRQChannel = USART3_IRQn;
            break;
        /*case UART4_BASE:
            nvic_conf.NVIC_IRQChannel = UART4_IRQn;
            break;
        case UART5_BASE:
            nvic_conf.NVIC_IRQChannel = UART5_IRQn;
            break;
        case USART6_BASE:
            nvic_conf.NVIC_IRQChannel = USART6_IRQn;*/
        default:
            rt_kprintf("uart is err!\n");
            break;
    }
    NVIC_Init(&nvic_conf);

    USART_ClearFlag(uart_dev->uart, USART_FLAG_TC);
    /*������Ӧ�����ж�,���ջ���������*/
    UART_ENABLE_RXNEI((u32)(uart_dev->uart));
    /*ʹ��UART*/
    UART_ENABLE_UART((u32)(uart_dev->uart));//ʹ��uartʱ�Ὣtc��Ϊ��������һ��
    USART_ClearFlag(uart_dev->uart, USART_FLAG_TC);//�����һ�±�־�������һ���ֽڶ�ʧ
}
/***************************************************************************************************
*\Function      UARTSendByte
*\Description   uart�ֽڷ��ͺ�������ѯ����
*\Parameter     uart
*\Parameter     data
*\Return        void
*\Note
*\Log           2012.12.22    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool UARTSendByte(USART_TypeDef* uart, u8 data)
{
    u32 i = 0;

    UART_SEND_DATA((u32)uart, data);/*  д������*/

    while((UART_HAS_NOT_DATA_SND((u32)uart)) && (i++ < 0x9FFFFF));

    if (i >= 0x9FFFFF)
    {
        return FALSE;
    }
    return TRUE;
}
/***************************************************************************************************
*\Function      stm32_uart_send
*\Description   uart���ͺ���
*\Parameter     uart
*\Parameter     tx�����ͻ�����
*\Parameter     len���������ݳ���
*\Return        u32���ɹ����͵����ݸ���
*\Note
*\Log           2012.12.22    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 stm32_uart_send(UartDevType* uart_dev, u8* tx, u32 len)
{
    if (uart_dev == RT_NULL)
    {
        return 0;
    }

    //�ȴ��ź���
    rt_sem_take(uart_dev->uart_sem, RT_WAITING_FOREVER);
    //485����ƽ�����
    if (uart_dev->Rs485Ctrl != RT_NULL)
    {
        stm32_switch_on(uart_dev->Rs485Ctrl);
    }
#ifndef UART_SENDIT_MODE_EN
    {
        u32 j = 0;
        /*��ѯ��ʽ����*/
        for (j = 0; j < len; j++)
        {
            UARTSendByte(uart_dev->uart, *tx++);
        }
    }    
#else
    {
        rt_err_t result;
        u32 max_send_tick = 0;//�����ʱ��tick
        max_send_tick = (len * 13 * RT_TICK_PER_SECOND) / (uart_dev->baudrate) + 2;//������20ms
        if (len)
        {
            uart_dev->txbuf.len = len;
            uart_dev->txbuf.offset = 0;
            uart_dev->txbuf.TxBuf = tx;

            rt_sem_trytake(uart_dev->uart_sendIT);//����sem������semΪ0
            /*ʹ��uart�����ж�*/
            UART_ENABLE_TXEI((u32)(uart_dev->uart));
            result = rt_sem_take(uart_dev->uart_sendIT, max_send_tick);
            if (result != RT_EOK)
            {
                len = uart_dev->txbuf.offset;//�˴�˵��Ϊ��ʱ���˳���ȥoffset��Ϊ�Ѿ����͵ĳ���
            }
        }
    }
#endif
    if (uart_dev->Rs485Ctrl != RT_NULL)
    {
        stm32_switch_off(uart_dev->Rs485Ctrl);
    }
    /*�ͷ��ź���*/
    rt_sem_release(uart_dev->uart_sem);
    return len;
}

/***************************************************************************************************
*\Function      stm32_uart_recv
*\Description   uart���պ���
*\Parameter     uart
*\Parameter     rx�����ݽ��ջ�����
*\Parameter     len���������ݳ���
*\Parameter     clear, TRUE,��ʾ����������е����ݣ�FALSE����ʾ������������е�����
*\Return        u32��ʵ�ʽ��յ����ݳ���
*\Note
*\Log           2012.12.22    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 stm32_uart_recv(UartDevType* uart_dev, u8* rx, u32 len, bool clear)
{
    u32 len_tmp = 0;

    if (uart_dev == RT_NULL)
    {
        return 0;
    }

    /*�ȴ��ź���������ʵ�ָýӿڵĻ���*/
    rt_sem_take(uart_dev->uart_sem, RT_WAITING_FOREVER);

    len_tmp = ReadDataFromBuf(&uart_dev->rxbuf, rx, len, clear);
    /*�ͷ��ź���*/
    rt_sem_release(uart_dev->uart_sem);
    return len_tmp;
}
/***************************************************************************************************
*\Function      stm32_uart_get_len
*\Description
*\Parameter     uart
*\Return        u32
*\Note
*\Log           2012.12.22    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 stm32_uart_get_len(UartDevType* uart_dev)
{
    u32 len = 0;

    if (uart_dev == RT_NULL)
    {
        return 0;
    }
    /*�ȴ��ź���������ʵ�ָýӿڵĻ���*/
    rt_sem_take(uart_dev->uart_sem, RT_WAITING_FOREVER);

    len = GetLenFromBuf(&uart_dev->rxbuf);
    /*�ͷ��ź���*/
    rt_sem_release(uart_dev->uart_sem);
    return len;
}
/***************************************************************************************************
*\Function      stm32_uart_del
*\Description   ɾ���������е��ֽ���
*\Parameter     uart
*\Parameter     len
*\Return        u32��ʵ��ɾ�����ֽ���
*\Note
*\Log           2012.08.06    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
u32 stm32_uart_del(UartDevType* uart_dev, u32 len)
{
    u32 len_tmp = 0;

    if (uart_dev == RT_NULL)
    {
        return 0;
    }

    /*�ȴ��ź���������ʵ�ָýӿڵĻ���*/
    rt_sem_take(uart_dev->uart_sem, RT_WAITING_FOREVER);

    len_tmp = DelDataFromBuf(&uart_dev->rxbuf, len);
    /*�ͷ��ź���*/
    rt_sem_release(uart_dev->uart_sem);
    return len_tmp;
}
/***************************************************************************************************
*\Function      stm32_uart_clear
*\Description   ����������е�����
*\Parameter     uart
*\Return        bool
*\Note
*\Log           2012.12.22    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool stm32_uart_clear(UartDevType* uart_dev)
{
    if (uart_dev == RT_NULL)
    {
        return FALSE;
    }

    /*�ȴ��ź���������ʵ�ָýӿڵĻ���*/
    rt_sem_take(uart_dev->uart_sem, RT_WAITING_FOREVER);
    uart_dev->rxbuf.Tail = uart_dev->rxbuf.Head;
    /*�ͷ��ź���*/
    rt_sem_release(uart_dev->uart_sem);
    return TRUE;
}
/***************************************************************************************************
*\Function      stm32_uart_hook_cfg
*\Description   ���ûص�����
*\Parameter     uart_dev
*\Parameter     htype
*\Parameter     phook
*\Parameter     para
*\Return        bool
*\Note          
*\Log           2016.08.29    Ver 1.0    Job
*               ����������
***************************************************************************************************/
bool stm32_uart_hook_cfg(UartDevType* uart_dev, UartHookType htype, pHookHandle phook, void* para)
{
    if (uart_dev == RT_NULL)
    {
        return FALSE;
    }

    if (htype == UART_TX_HOOK)
    {
        uart_dev->send_hook.phook = phook;
        uart_dev->send_hook.para  = para;
    }
    else if (htype == UART_RX_HOOK)
    {
        uart_dev->recv_hook.phook = phook;
        uart_dev->recv_hook.para  = para;
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}
/***************************************************************************************************
*\Function      stm32_uart_set_baudrate
*\Description   ���ô��ڲ�����
*\Parameter     uart_dev
*\Parameter     baudrate
*\Return        u32
*\Note          
*\Log           2017.06.05    Ver 1.0    Job               
				����������
***************************************************************************************************/
bool stm32_uart_set_baudrate(UartDevType* uart_dev, u32 baudrate)
{
    RCC_ClocksTypeDef clock_state;
    u32 apb_clock           = 0;                        /*��UART���ڵ����ߵ�ʱ��Ƶ��*/
    u32 integer_diverder    = 0x00;                     /*��������*/
    u32 fractional_diver    = 0x00;                     /*С������*/
    u32 temp_reg            = 0x00;
    u32 uart = (u32)(uart_dev->uart);

    if (baudrate < 1200 || baudrate > 115200)
    {
        return FALSE;
    }
    /* ����UART�Ĳ����� */
    RCC_GetClocksFreq(&clock_state);

    if ((((USART_TypeDef*)(uart)) == USART1))// || (((USART_TypeDef*)(uart)) == USART6))
    {
        apb_clock = clock_state.PCLK2_Frequency;
    }
    else
    {
        apb_clock = clock_state.PCLK1_Frequency;
    }

    /* Determine the integer part */
    if ((((USART_TypeDef*)(uart))->CR1 & USART_CR1_OVER8) != 0)
    {
        /* Integer part computing in case Oversampling mode is 8 Samples */
        integer_diverder = ((25 * apb_clock) / (2 * (baudrate)));
    }
    else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
    {
        /* Integer part computing in case Oversampling mode is 16 Samples */
        integer_diverder = ((25 * apb_clock) / (4 * (baudrate)));
    }
    temp_reg = (integer_diverder / 100) << 4;

    /* Determine the fractional part */
    fractional_diver = integer_diverder - (100 * (temp_reg >> 4));

    /* Implement the fractional part in the register */
    if ((((USART_TypeDef*)(uart))->CR1 & USART_CR1_OVER8) != 0)
    {
        temp_reg |= ((((fractional_diver * 8) + 50) / 100)) & ((uint8_t)0x07);
    }
    else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
    {
        temp_reg |= ((((fractional_diver * 16) + 50) / 100)) & ((uint8_t)0x0F);
    }

    /* Write to USART BRR register */
    ((USART_TypeDef*)(uart))->BRR = (uint16_t)temp_reg;

    uart_dev->baudrate = baudrate;

    return TRUE;
}
