/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          app_board.c
*\Description   
*\Note          
*\Log           2018.01.19    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#include "app_board.h"

//LED
switch_type MSGRED_LED = {GPIOB, GPIO_PIN_5, FALSE};
switch_type MSGBLUE_LED = {GPIOB, GPIO_PIN_1, FALSE};
switch_type MSGGREEN_LED = {GPIOB, GPIO_PIN_13, FALSE};

//����
static GpioType UartTx = {GPIOA, GPIO_PIN_9};
static GpioType UartRx = {GPIOA, GPIO_PIN_10};
static u8 UartRxBuf[1024];

UartDevType UartTTL = {USART1, &UartTx, &UartRx, NULL, 115200, UartRxBuf, sizeof(UartRxBuf)};

//CAN  TX PA12 RX PA11
static GpioType CAN_Tx = {GPIOA, GPIO_PIN_12};
static GpioType CAN_Rx = {GPIOA, GPIO_PIN_11};
CANBus_type CAN = {CAN1, &CAN_Tx, &CAN_Rx, CAN_BAUDRATE_500K};

/***************************************************************************************************
*\Function      stm32_board_lowpower_cfg
*\Description   �͹�������
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.05.25    Ver 1.0    Job               
				����������
***************************************************************************************************/
void stm32_board_lowpower_cfg(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
  /* Note: Debug using ST-Link is not possible during the execution of this   */
  /*       example because communication between ST-link and the device       */
  /*       under test is done through UART. All GPIO pins are disabled (set   */
  /*       to analog input mode) including  UART I/O pins.           */
  GPIO_InitStructure.Pin = GPIO_PIN_All;
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;

  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure); 
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* Disable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOE_CLK_DISABLE();

}
/***************************************************************************************************
*\Function      acs_app_board_init
*\Description   
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.01.19    Ver 1.0    Job               
				����������
***************************************************************************************************/
extern  void sensor_gpio_Init(void);
void app_board_init(void)
{
    //ȫ����������һ��
    stm32_board_lowpower_cfg();
    //���Ź��ȳ�ʼ��
    watchdog_init();
    //���� ���ʹ�øô��ڵ��� �����ȳ�ʼ��
    stm32_uart_init(&UartTTL);
    stm32_switch_init(&MSGRED_LED);
	stm32_switch_init(&MSGBLUE_LED);
	stm32_switch_init(&MSGGREEN_LED);
	show_version();
    GetRestFlag();
	//RTC
    stm32_Inrtc_init(FALSE);   
	stm32_CAN_init(&CAN);
}


/* Private function prototypes -----------------------------------------------*/
#ifdef USING_USART_PRINTF
int fputc(int ch, FILE *f)
{
    stm32_uart_send(&UartTTL, (u8*)&ch, 1);
    return ch;
}
#endif /* __GNUC__ */
