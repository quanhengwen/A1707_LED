/***************************************************************************************************
*                    (c) Copyright 1992-2015 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          iap_if.c
*\Description   iap��ؽӿ�
*\Note          
*\Log           2015.05.27    Ver 1.0    �Ų�
*               �����ļ���
***************************************************************************************************/
#include "iap_if.h"
#include "stdio.h"
#include "string.h"
#include "flash_if.h"
#include "ota_protocol.h"

char* IAPoE_Name = "stm32_ota";//IAP�̼�����
char* IAPoE_Ver = "V1.3";//IAP�̼��汾
char* IAPoE_Date = __DATE__;//IAP�̼���������

uint64_t IAP_FLAG;//iapģʽ��־
iap_param_type iap_param;
/***************************************************************************************************
*\Function      Get_FlashSize
*\Description   ��ȡflash����
*\Parameter     
*\Return        uint16_t
*\Note          ��λKB
*\Log           2015.05.29    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
uint16_t Get_FlashSize(void)
{
    return *(uint16_t*)(FLASH_SIZE_ADDR);//��ȡflash��С   
}
/***************************************************************************************************
*\Function      RunAppMode
*\Description   ��������ģʽ
*\Parameter     
*\Return        void
*\Note          0xFFFFFFFF FFFFFFFF
*\Log           2018.01.18    Ver 1.0    Job               
				����������
***************************************************************************************************/
void RunAppMode(void)
{
    /*Variable used for Erase procedure*/
    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t addr_iapflag = FLASH_START_ADDR + Get_FlashSize() * 1024 - FLASH_IAP_FLAG_ADDR*FLASH_PAGE_SIZE;//FLASH_IAP_FLAG_ADDR;
    uint32_t NbOfPages = 0;
    uint32_t PAGEError = 0;

    IAP_FLAG = OTA_RUN_APP;
    /* FLASH ���� */
    HAL_FLASH_Unlock();
    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR); 
    /* Get the 1st page to erase */
    /* Get the number of pages to erase from 1st page */
    NbOfPages = 1;
    /* Get the bank */
    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = addr_iapflag;
    EraseInitStruct.NbPages     = NbOfPages;

    /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
    you have to make sure that these data are rewritten before they are accessed during code
    execution. If this cannot be done safely, it is recommended to flush the caches by setting the
    DCRST and ICRST bits in the FLASH_CR register. */
	FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
    {
        printf("RunAppMode erase error!\n");
    }
    /* FLASH ���� */
    HAL_FLASH_Lock();
}
/***************************************************************************************************
*\Function      OutIAPMode
*\Description   �������Appģʽ
*\Parameter     
*\Return        void
*\Note          
*\Log           2015.05.27    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void UpdateAppMode(void)
{
    uint32_t addr_iapflag = FLASH_START_ADDR + Get_FlashSize() * 1024 - FLASH_IAP_FLAG_ADDR*FLASH_PAGE_SIZE;//FLASH_IAP_FLAG_ADDR;

    //����0xFFFFFFFFFFFF..�����
    if (ReadIAPMode() != OTA_RUN_APP)
    {
        RunAppMode();
    }
    /* FLASH ���� */
    HAL_FLASH_Unlock();
    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

    IAP_FLAG = OTA_Update_APP;
    FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr_iapflag, IAP_FLAG) != HAL_OK)
    {
        printf("UpdateAppMode erase error!\n");
    }

    /* FLASH ���� */
    HAL_FLASH_Lock();
}

/***************************************************************************************************
*\Function      ForceUpdateAppMode
*\Description   ǿ�Ƹ���ģʽ
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.01.18    Ver 1.0    Job               
				����������
***************************************************************************************************/
void ForceUpdateAppMode(void)
{
    uint32_t addr_iapflag = FLASH_START_ADDR + Get_FlashSize() * 1024 - FLASH_IAP_FLAG_ADDR*FLASH_PAGE_SIZE;//FLASH_IAP_FLAG_ADDR;

    //����0xFFFFFFFFFFFF..�����
    if (ReadIAPMode() != OTA_RUN_APP || ReadIAPMode() != OTA_Update_APP)
    {
        RunAppMode();
    }
    /* FLASH ���� */
    HAL_FLASH_Unlock();
    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

     IAP_FLAG = OTA_Force_Update_APP;

		FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr_iapflag, IAP_FLAG) != HAL_OK)
    {
        printf("ForceAppMode erase error!\n");
    }
    /* FLASH ���� */
    HAL_FLASH_Lock();
}
/***************************************************************************************************
*\Function      ReadIAPMode
*\Description   ��ȡIAP_FLAG��־
*\Parameter     
*\Return        uint16_t
*\Note          
*\Log           2015.05.29    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
uint64_t ReadIAPMode(void)
{
    return *(uint64_t*)(FLASH_START_ADDR + Get_FlashSize() * 1024 - FLASH_IAP_FLAG_ADDR*FLASH_PAGE_SIZE);
}

/***************************************************************************************************
*\Function      Get_ChipID
*\Description   ��ȡuuid
*\Parameter     chipID
*\Return        void
*\Note          
*\Log           2018.07.17    Ver 1.0    �ӹ���
*               ����������
***************************************************************************************************/
void Get_ChipID(uint16_t* chipID)
{
    chipID[0] = *(__IO uint16_t *)(0X1FFFF7E8);  // ���ֽ�
    chipID[1] = *(__IO uint16_t *)(0X1FFFF7EA); //  
    chipID[2] = *(__IO uint16_t *)(0X1FFFF7EC); // ���ֽ�
	chipID[3] = *(__IO uint16_t *)(0X1FFFF7EE);  // ���ֽ�
    chipID[4] = *(__IO uint16_t *)(0X1FFFF7F0); //  
    chipID[5] = *(__IO uint16_t *)(0X1FFFF7F2); // ���ֽ�
}


/***************************************************************************************************
*\Function      iap_param_save
*\Description   iap�����洢
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.07.17    Ver 1.0    �ӹ���
*               ����������
***************************************************************************************************/
void iap_param_save(void)
{
    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t addr_iapparam = FLASH_START_ADDR + Get_FlashSize() * 1024 - FLASH_IAP_PARAM_ADDR*FLASH_PAGE_SIZE;//FLASH_IAP_PARAM_ADDR;
    uint32_t NbOfPages = 0;
    uint32_t PAGEError = 0;

    HAL_FLASH_Unlock();
		/* ����������� */
	/* Clear OPTVERR bit set on virgin samples */
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR); 

	/* Get the bank */
	NbOfPages =  1;
	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress      = addr_iapparam;
	EraseInitStruct.NbPages	  = NbOfPages;
	FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
		printf("erase error\n");
	}	
	else
	{
//		printf("erase ok\n");
	}
	
    Flash_Write(addr_iapparam, (uint8_t*)&iap_param,sizeof(iap_param));
    addr_iapparam += sizeof(iap_param);

    /* FLASH ���� */
    HAL_FLASH_Lock();

}

/***************************************************************************************************
*\Function      iap_param_default
*\Description   iapĬ�ϲ���
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.07.17    Ver 1.0    �ӹ���
*               ����������
***************************************************************************************************/
void iap_param_default(void)
{
    memset(&iap_param, 0, sizeof(iap_param));
	Get_ChipID(iap_param.ChipUniqueID);	
	iap_param.Power_First_Flag = DEV_FIRST_POWER_FLAG;   
	iap_param.swversion = 0x01;
	iap_param.hwversion = 0x01;
	iap_param.IAP_flag = 0x01;
	iap_param.remoteID =0x00;
	iap_param.ID=0xFFFF;
    iap_param_save();
}


/***************************************************************************************************
*\Function      iap_param_load
*\Description   iap��������
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.07.17    Ver 1.0    �ӹ���
*               ����������
***************************************************************************************************/
void iap_param_load(void)
{
    uint32_t addr_iapparam =  FLASH_START_ADDR + Get_FlashSize() * 1024 - FLASH_IAP_PARAM_ADDR*FLASH_PAGE_SIZE;//FLASH_IAP_PARAM_ADDR;

    iap_param.Power_First_Flag = *(__IO uint16_t*)(addr_iapparam);
    if (iap_param.Power_First_Flag != DEV_FIRST_POWER_FLAG)
    {     
//		printf("IAP Load Default Param...\r\n");
        //�����һ���ϵ磬��Ĭ��ֵ
        iap_param_default(); //����Ĭ�ϲ���,����FLASH֮�����
    }
    else
    {
//        printf("IAP Load Param... \r\n");
    }
	
//	printf("length %d \r\n", Get_FlashSize());

    addr_iapparam =   FLASH_START_ADDR + Get_FlashSize() * 1024 - FLASH_IAP_PARAM_ADDR*FLASH_PAGE_SIZE;//FLASH_IAP_PARAM_ADDR;

    Flash_Read(addr_iapparam, (uint8_t*)&iap_param, sizeof(iap_param));
    addr_iapparam += sizeof(iap_param);
}





