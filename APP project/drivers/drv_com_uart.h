/***************************************************************************************************
*                    (c) Copyright 2008-2012 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          stm32_uart.h
*\Description   
*\Note          
*\Log           2012.12.22    Ver 1.0    �Ų�
*               �����ļ���
***************************************************************************************************/
#ifndef _DRV_COM_UART_H
#define _DRV_COM_UART_H
#include "public_type.h"
#include "bit_band_macro.h"
#include "rtthread.h"
#include "common_lib.h"
#include "drv_switch.h"

/***************************************************************************************************
*uart�궨����
***************************************************************************************************/
#define UART_O_DR   0x04
#define UART_O_SR   0x00
#define UART_O_CR1  0x0c
#define UART_O_CR3  0x14

/*ʹ�ܺͽ��ܽ��ջ��������ж�*/
#define UART_ENABLE_RXNEI(base)	SET_BIT_BAND(base + UART_O_CR1, 5)
#define UART_DISABLE_RXNEI(base)	CLR_BIT_BAND(base + UART_O_CR1, 5)
/*ʹ�ܺͽ��ܷ��ͻ��������ж�*/
#define UART_ENABLE_TXEI(base)	SET_BIT_BAND(base + UART_O_CR1, 7)
#define UART_IS_TXEI_ENABLE(base)  (HWREGBIT(base + UART_O_CR1, 7) != RESET)
#define UART_DISABLE_TXEI(base)	CLR_BIT_BAND(base + UART_O_CR1, 7)
/*ʹ�ܽ��ܷ�������ж�*/
#define UART_ENABLE_TCEI(base)	SET_BIT_BAND(base + UART_O_CR1, 6)
#define UART_DISABLE_TCEI(base)	CLR_BIT_BAND(base + UART_O_CR1, 6)
#define UART_IS_TXCI_ENABLE(base)  (HWREGBIT(base + UART_O_CR1, 6) != RESET)

/*ʹ�ܽ���uart*/
#define UART_ENABLE_UART(base)		SET_BIT_BAND((base) + UART_O_CR1, 13)
#define UART_DISABLE_UART(base)		CLR_BIT_BAND((base) + UART_O_CR1, 13)
#define IS_UART_ENABLE(base)            (HWREGBIT((base) + UART_O_CR1, 13) != RESET)

#define UART_HAS_NOT_DATA_SND(base)	(HWREGBIT(base + UART_O_SR, 7) == RESET)
#define UART_CLR_DATA_TX_END(base)                (CLR_BIT_BAND(base + UART_O_SR, 6))
#define UART_HAS_NOT_ALL_SND(base)      (HWREGBIT(base + UART_O_SR, 6) == RESET)
#define UART_HAS_ALL_SND(base)          (HWREGBIT(base + UART_O_SR, 6) != RESET)
#define UART_HAS_DATA_REC(base)	(HWREGBIT(base + UART_O_SR, 5) != RESET)

#define IS_RX_OVERFLOW(base)    (HWREGBIT(base + UART_O_SR, 3) != RESET)
#define IS_NOISE_ERR(base)    (HWREGBIT(base + UART_O_SR, 2) != RESET)
#define IS_FRAME_ERR(base)    (HWREGBIT(base + UART_O_SR, 1) != RESET)

#define UART_SEND_DATA(base,data)	    (HWREG((base) + UART_O_DR) = data)
#define UART_READ_DATA(base)	             (HWREG((base) + UART_O_DR))


#define UART_RX_BUF_SIZE  1024

typedef enum
{
    UART_TX_HOOK,
    UART_RX_HOOK,
}UartHookType;

/*uart����*/
typedef struct UartBufType_st
{
    USART_TypeDef* uart;
    GpioType*   TxGpio; //uart���ͽ�
    GpioType*   RxGpio; //uart���ս�
    switch_type*   Rs485Ctrl;//�����485�����п��ƽţ�����ΪRT_NULL
    u32         baudrate;//�����ʣ�����ʱ����ݲ����ʼ���ȴ�ʱ��
    /*�������ʼ����ֵ*/
    RingBufType   rxbuf;  //uart���ջ�����
    TxBufType   txbuf;  //uart���ͻ�����
    rt_sem_t    uart_sem;//�ź���������ʵ�ֽӿڵĻ��� 
    rt_sem_t    uart_sendIT;//������ɱ�־
    HookDevType send_hook;//������ɵ�hook
    HookDevType recv_hook;//���յ�һ���ֽڵ�hook
}UartDevType;

void stm32_uart_isr(UartDevType* uart_dev);

void stm32_uart_init(UartDevType* uart_dev);
u32 stm32_uart_send(UartDevType* uart_dev, u8* tx, u32 len);
u32 stm32_uart_recv(UartDevType* uart_dev, u8* rx, u32 len, bool clear);
u32 stm32_uart_get_len(UartDevType* uart_dev);
u32 stm32_uart_del(UartDevType* uart_dev, u32 len);
bool stm32_uart_clear(UartDevType* uart_dev);
bool stm32_uart_hook_cfg(UartDevType* uart_dev, UartHookType htype, pHookHandle phook, void* para);

bool stm32_uart_set_baudrate(UartDevType* uart_dev, u32 baudrate);
#endif /*_DRV_COM_UART_H*/
