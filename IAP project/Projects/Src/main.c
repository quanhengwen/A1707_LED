/**
  ******************************************************************************
  * @file    CAN/CAN_Networking/Src/main.c
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    14-April-2017
  * @brief   This example shows how to configure the CAN peripheral
  *          to send and receive CAN frames in normal mode. The sent frames
  *          are used to control Leds by pressing KEY Push Button.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "iap_if.h"
#include "flash_if.h"
#include "app_board.h"
#include "usdl_calc.h"
#include "drv_watchdog.h"
#include "ota_protocol.h"
#include "zlib.h"
#include "canbus_middle.h"

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
void stm32_flash_RDP(void);

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;
iap_updata_param_type IapUpdataParam;//IAP����ʱ�ĸ�����Ϣ
static uint8_t check_buf[FLASH_PAGE_SIZE];//���黺����
extern void system_reset(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    int i = 0;
    uint32_t page_num = 0;//ҳ��
    uint32_t page_left = 0;//����ҳʣ����ֽ�
    CRC32_CTX CRC32;
    uint32_t Count = 0;
	uint32_t ComSizeCnt = 0, UncomSizeCnt = 0, ComSizeCntTmp = 0, UncomSizeCntTmp = 0;//��ѹǰ��С�ͽ�ѹ���С,��ѹǰÿ����С��ʱ��������ѹ������ÿ����С��ʱ����
	int err;	
	uint32_t addr_appinfo = FLASH_START_ADDR + Get_FlashSize() * 1024 - FLASH_APP_INFO_ADDR*FLASH_PAGE_SIZE;//FLASH_APP_INFO_ADDR;
	uint32_t addr_appcopy = FLASH_START_ADDR + Get_FlashSize() * 1024 - FLASH_APP_COPY_ADDR*FLASH_PAGE_SIZE;//FLASH_APP_COPY_ADDR;
	uint32_t addr_apprun = FLASH_APP_RUN_ADDR;
	/* STM32F103xG HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
	HAL_Init();
	/* Configure the system clock to 72 MHz */
	SystemClock_Config();
	//������
    stm32_flash_RDP();
	
    //��ȡ��־
    IAP_FLAG = ReadIAPMode();	
	
    if (IAP_FLAG == OTA_RUN_APP)
    {
RunAPP:
        /* Check if valid stack address (RAM address) then jump to user application */
        if (((*(__IO uint32_t*)addr_apprun) & 0x2FFE0000 ) == 0x20000000)
        {
//            printf("load app...\n");
            /* Jump to user application */
            JumpAddress = *(__IO uint32_t*) (addr_apprun + 4);
            Jump_To_Application = (pFunction) JumpAddress;
            /* Initialize user application's Stack Pointer */
            __set_MSP(*(__IO uint32_t*) addr_apprun);
            Jump_To_Application();
        }
        else
        {   /* Otherwise, do nothing */
            //��ת��ַ���Ϸ�������
//            printf("\n\r Addr is invalid! Jump IAP...\n\r");
            ForceUpdateAppMode();//�´�ֱ�ӽ���IAPģʽ
            goto OTAoE;
        }
    }
    else if (IAP_FLAG == OTA_Update_APP)
    {
		
		//��ȡIAP������Ϣ
		Flash_Read(addr_appinfo, (uint8_t*)&IapUpdataParam, sizeof(IapUpdataParam));
			
		//��У���������Ƿ���ȷ�������ǲ���ѹ�����ݣ�
        //�����ļ���У��
        page_num = IapUpdataParam.UpdatesTotalSize / sizeof(check_buf);
        page_left = IapUpdataParam.UpdatesTotalSize % sizeof(check_buf);
        if (IapUpdataParam.UpdatesTotalSize > OTA_BIN_MAX_SIZE)
        {
            //�ļ�����
            //���ı��
            RunAppMode();
            goto RunAPP;
        }
								
        CRC32_Init(&CRC32);
        for (i = 0; i < page_num; i++)
        {
            Flash_Read(addr_appcopy + i * sizeof(check_buf), check_buf, sizeof(check_buf));
            CRC32_Update(&CRC32, check_buf, sizeof(check_buf));
        }
        if (page_left)
        {
            Flash_Read(addr_appcopy + page_num * sizeof(check_buf), check_buf, page_left);
            CRC32_Update(&CRC32, check_buf, page_left);
        }
        //������� �ж�У���Ƿ���ȷ
        if (IapUpdataParam.UpdatesCRC32 != CRC32_Final(&CRC32))
        {
            //У����� ������ת��ԭ���ĳ�������
            //���ı��
            RunAppMode();			
            goto RunAPP;
        }
				
		//�ж��Ƿ���ѹ������
		if(IapUpdataParam.CompressFlag == 1)//�����ѹ�����ݣ���Ҫ�߽�ѹ��д��Falsh����
		{
			uint8_t *pSrcData = (uint8_t *)addr_appcopy;
			Flash_Unlock();
			Count = 0;
					
			//ѭ��д��ֱ��ȫ��д����ȷΪֹ
			while (1)
			{
				Count++;
				if (Count > 3)
				{
					//ʧ�ܴ���3�� �˳�����
					Flash_Lock();
					//д��3��ʧ��
					ForceUpdateAppMode();
					goto OTAoE;
				}
				//У����ȷ ��ʼcopy
				//�Ȳ��� ����������
				ComSizeCnt = 0, UncomSizeCnt = 0;
				Flash_Erase_Page(addr_apprun, IapUpdataParam.ComPackOriginalSize);
              
				for (; ComSizeCnt < IapUpdataParam.UpdatesTotalSize;)//�߽�ѹ�ߴ洢
				{
					ComSizeCntTmp = IapUpdataParam.PerComPackOriginalSize * 2;
					UncomSizeCntTmp = IapUpdataParam.PerComPackOriginalSize * 2;							
				    err = uncompress(check_buf, &UncomSizeCntTmp, pSrcData, &ComSizeCntTmp);									
					FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
					Flash_Write(addr_apprun + UncomSizeCnt, check_buf, UncomSizeCntTmp);
					//��ȡһ��ȷ�� ���ȷ���Ƿ�д����ȷ
//                	Flash_Read(addr_apprun + UncomSizeCnt, check_buf, UncomSizeCntTmp);
                
					pSrcData += ComSizeCntTmp;
					ComSizeCnt += ComSizeCntTmp;
					UncomSizeCnt += UncomSizeCntTmp;
				} 
				pSrcData = (uint8_t *)addr_apprun;
				CRC32_Init(&CRC32);
				CRC32_Update(&CRC32, pSrcData, UncomSizeCnt);
				if((IapUpdataParam.ComPackOriginalCRC32 == CRC32_Final(&CRC32)) && (IapUpdataParam.ComPackOriginalSize == UncomSizeCnt))
				{
					Flash_Lock();
					//д����ȷ ������ת
					RunAppMode();
					system_reset();
					goto RunAPP;
				}
			}
		}
		else
		{
			//����
			Flash_Unlock();			
			Count = 0;
			//ѭ��д��ֱ��ȫ��д����ȷΪֹ
			while (1)
			{
				Count++;
				if (Count > 3)
				{
					//ʧ�ܴ���3�� �˳�����
					Flash_Lock();
					//д��3��ʧ��
					ForceUpdateAppMode();
					goto OTAoE;
				}
				//У����ȷ ��ʼcopy
				//�Ȳ��� ����������
				Flash_Erase_Page(addr_apprun, IapUpdataParam.UpdatesTotalSize);
				CRC32_Init(&CRC32);
				for (i = 0; i < page_num; i++)
				{
					FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
					Flash_Read(addr_appcopy + i * sizeof(check_buf), check_buf, sizeof(check_buf));
					Flash_Write(addr_apprun + i * sizeof(check_buf), check_buf, sizeof(check_buf));
					//��ȡһ��ȷ�� ���ȷ���Ƿ�д����ȷ
					Flash_Read(addr_apprun + i * sizeof(check_buf), check_buf, sizeof(check_buf));
					CRC32_Update(&CRC32, check_buf, sizeof(check_buf));
				}
				if (page_left)
				{
					FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
					Flash_Read(addr_appcopy + i * sizeof(check_buf), check_buf, page_left);
					Flash_Write(addr_apprun + i * sizeof(check_buf), check_buf, page_left);
					//��ȡʣ��
					Flash_Read(addr_apprun + i * sizeof(check_buf), check_buf, page_left);
					CRC32_Update(&CRC32, check_buf, page_left);
				}
				if (IapUpdataParam.UpdatesCRC32 == CRC32_Final(&CRC32))
				{
					Flash_Lock();
					//д����ȷ ������ת
					RunAppMode();
					system_reset();
					goto RunAPP;
				}
			}
		}       
    }
    /* enter in IAP mode */
    else
    {
OTAoE:	
		iap_param_load();
        app_board_init();
		CAN_middle_init();
		
		/* Infinite loop */
        while (1)
        {			
			OTA_Periodic_Handle();//��������
			CAN_OTA_Periodic_Handle();
			

        }
    }
    return 0;	
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 1
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}

/***************************************************************************************************
*\Function      stm32_flash_RDP
*\Description   �����������ܺ����ϵ�����һ��
*\Parameter     
*\Return        void
*\Note          
*\Log           2017.01.09    Ver 1.0    Job               
����������
***************************************************************************************************/
void stm32_flash_RDP(void)
{
    FLASH_OBProgramInitTypeDef OBInit;

    HAL_FLASHEx_OBGetConfig(&OBInit);
    if (OBInit.RDPLevel != OB_RDP_LEVEL_0)
    {
        return;
    }
	/* Allow Access to Flash control registers and user Falsh */
    HAL_FLASH_Unlock();
    /* Allow Access to option bytes sector */
    HAL_FLASH_OB_Unlock();

    /* Enable FLASH_WRP_SECTORS write protection */
    OBInit.OptionType = OPTIONBYTE_RDP;
    OBInit.RDPLevel = OB_RDP_LEVEL_1;
    HAL_FLASHEx_OBProgram(&OBInit);

    /* Start the Option Bytes programming process */
//    if (HAL_FLASH_OB_Launch() != HAL_OK)
//    {

//    }

    /* Prevent Access to option bytes sector */
    HAL_FLASH_OB_Lock();

    /* Disable the Flash option control register access (recommended to protect
    the option Bytes against possible unwanted operations) */
    HAL_FLASH_Lock();
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void HAL_RTCEx_RTCEventCallback(RTC_HandleTypeDef *hrtc)
{   
	watchdog_refresh_and_count();	
	stm32_switch_evert(&MSGRED_LED);
	stm32_switch_evert(&MSGBLUE_LED);
	stm32_switch_evert(&MSGGREEN_LED);
}

//ϵͳ�ϵ�����
void system_reset(void)
{
	HAL_NVIC_SystemReset();//ϵͳ����	  
}



#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}


//static void Error_Handler(void)
//{
//	printf("error handle\n\r");
//	system_reset();
//	while (1)
//	{
//	}
//	
//}

#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
