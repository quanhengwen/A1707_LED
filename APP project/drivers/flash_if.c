/**
  ******************************************************************************
  * @file    flash_if.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-October-2011
  * @brief   This file provides high level routines to manage internal Flash 
  *          programming (erase and write). 
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "flash_if.h"
#include "stdio.h"
#include "string.h"
#include "iap_if.h"
#include "common_lib.h"

/***************************************************************************************************
*\Function      Flash_Unlock
*\Description   
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.01.18    Ver 1.0    Job               
				����������
***************************************************************************************************/
void Flash_Unlock(void)
{
    /* FLASH ���� */
    FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_EOP); 	
	FLASH_WaitForLastOperation((uint32_t)0x00002000);
}
/***************************************************************************************************
*\Function      Flash_Lock
*\Description   
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.01.18    Ver 1.0    Job               
				����������
***************************************************************************************************/
void Flash_Lock(void)
{
    FLASH_Lock();
}
/***************************************************************************************************
*\Function      Flash_Erase_Page
*\Description   
*\Parameter     addr
*\Parameter     bin_size
*\Return        uint_8
*\Note          
*\Log           2018.01.18    Ver 1.0    Job               
				����������
***************************************************************************************************/
uint8_t Flash_Erase_Page(uint32_t addr, uint32_t bin_size)
{
	uint32_t i = 0;
	uint32_t page_num=0;
	uint32_t FlashAddress;
    //����bin�ļ���С
    if (bin_size > OTA_BIN_MAX_SIZE || bin_size == 0)
    {
        return 1;
    }
    Flash_Unlock();	
	page_num = (bin_size + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
	for(i=0;i<page_num;i++)
	{
		FlashAddress=addr+i*FLASH_PAGE_SIZE;
		/* ����������� */
		if (FLASH_ErasePage(FlashAddress) != FLASH_COMPLETE)
		{
			rt_kprintf("\r page 0x%08X erase fail!\n\r", FlashAddress);
		}			
	}

    Flash_Lock();
    return 0;
}

/***************************************************************************************************
*\Function      flash_write
*\Description   ��flash��д����
*\Parameter     addr
*\Parameter     buf
*\Parameter     
*\Return        void
*\Note          
*\Log           2015.07.14    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
uint32_t Flash_Write(uint32_t addr, uint8_t* buf, uint32_t len)
{
    uint32_t left_num = 0;
    uint32_t i = 0;
    uint32_t n = 0;
    uint32_t word = 0;

    /*�ж��Ƿ�4�ı���*/
    left_num = len % 4;
    /*ֻ����˫��д��*/
    len = len - left_num;

    //�жϵ�ַ�Ƿ���4�ı���
    if (0 != (addr % 0x04))
    {
        /*���Ƕ���ĵ�ַ*/
        return 0;
    }
    
    Flash_Unlock();
    for (i = 0; i < len; i += 4)
    {
        word = ArrayU8ToU32(buf + i);
		FLASH_WaitForLastOperation((uint32_t)0x00002000);
		if (FLASH_ProgramWord(addr + i, word) != FLASH_COMPLETE)
        {
            Flash_Lock();
            return i;
        }
    }

    /* �Ƿ��ǵ��ֽ� */
    if (0 != left_num)
    {
        word = 0;

        for (n = 0; n < left_num; n++)
        {
            word |= (((uint32_t)buf[i + n]) << (8 * n));
        }
        for (n = left_num; n < 4; n++)
        {
            word |= (((uint32_t)0xFF) << (n * 8));
        }
		FLASH_WaitForLastOperation((uint32_t)0x00002000);
		if (FLASH_COMPLETE == FLASH_ProgramWord(addr + i, word))
        {
            /*���ϸղ�ȥ����*/
            len += left_num;
        }
    }

    Flash_Lock();
    return  len;
}
/***************************************************************************************************
*\Function      flash_read
*\Description   ��flash��ȡ����
*\Parameter     addr
*\Parameter     buf
*\Parameter     len
*\Return        uint32_t
*\Note          
*\Log           2015.07.14    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
uint32_t Flash_Read(uint32_t addr, uint8_t* buf, uint32_t len)
{
    memcpy(buf, (uint8_t*)(addr), len);
    return len;
}

/***************************************************************************************************
*\Function      Flash_Storage
*\Description   ���δ洢�Ƿ���ѹ������2�ֽڣ���lora�����ܴ�С��4�ֽڣ���lora�������С��2�ֽڣ���lora����CRC32У�飨4�ֽڣ���
*               BIN�ļ��ܴ�С��4�ֽڣ���ѹ�����ְ���Ŀ��2�ֽڣ���ѹ������������ԭʼ��С��2�ֽڣ���ѹ����CRC32У�飨4�ֽڣ�
*\Parameter     bin_size
*\Parameter     crc32
*\Return        uint32_t
*\Note          
*\Log           2018.01.19    Ver 1.0    Job
*               ����������
*               2018.05.03�޸�
***************************************************************************************************/
uint32_t Flash_Storage_Header(uint8_t *pData)
{
	uint32_t addr_appinfo = FLASH_START_ADDR + Get_FlashSize() * 1024 - FLASH_APP_INFO_ADDR*FLASH_PAGE_SIZE;//FLASH_APP_INFO_ADDR;
	uint32_t Tem = 0, i;
    Flash_Unlock();
	for(i = 0; i < 28/4; i ++)
	{

		Tem = *((uint32_t *)pData);
		pData += 4;
		FLASH_WaitForLastOperation((uint32_t)0x00002000);
		if (FLASH_ProgramWord((addr_appinfo + (i * 4)), Tem )!= FLASH_COMPLETE)
		{
			return 0;
		}
	}

    Flash_Lock();
    return 1;	
}


