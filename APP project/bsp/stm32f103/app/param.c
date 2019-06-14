/***************************************************************************************************
*                    (c) Copyright 2006-2016  Syncretize technologies co.,ltd.
*										All Rights Reserved
*	
*\File 			param.c
*\Description   
*\Note          
*\Log           2016.05.09    Ver 1.0   Job
*               �����ļ���
***************************************************************************************************/
#include "param.h"
#include "iap_if.h"
#include "flash_if.h"
#include "common_lib.h"
#include "manage_id.h"

//ip
u8 RT_LWIP_IPADDR0;
u8 RT_LWIP_IPADDR1;
u8 RT_LWIP_IPADDR2;
u8 RT_LWIP_IPADDR3;
//gw
u8 RT_LWIP_GWADDR0;
u8 RT_LWIP_GWADDR1;
u8 RT_LWIP_GWADDR2;
u8 RT_LWIP_GWADDR3;
//msk
u8 RT_LWIP_MSKADDR0;
u8 RT_LWIP_MSKADDR1;
u8 RT_LWIP_MSKADDR2;
u8 RT_LWIP_MSKADDR3;
/* DNS ��ַ*/
u8 RT_LWIP_DNSADDR0;
u8 RT_LWIP_DNSADDR1;
u8 RT_LWIP_DNSADDR2;
u8 RT_LWIP_DNSADDR3;

dev_param_type dev_param;//�豸����
realtime_param_type realtime_param;//ʵʱ������������
/***************************************************************************************************
*\Function      stm32_param_save
*\Description   ��������ӿ�
*\Parameter     
*\Return        void
*\Note          
*\Log           2015.07.06    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void stm32_param_save(void)
{
    u16 wdata; 
	 u32 addr;
	uint32_t addr_appparam =  FLASH_START_ADDR + Get_FlashSize() * 1024 - FLASH_APP_PARAM_ADDR*FLASH_PAGE_SIZE;//FLASH_APP_PARAM_ADDR;
	addr = addr_appparam;
	Flash_Unlock();
    /* ����������� */
	if (FLASH_ErasePage(addr) != FLASH_COMPLETE)
	{
		rt_kprintf("\r page 0x%08X erase fail!\n\r", addr);
	}	

    //IP��ַ
    wdata=RT_LWIP_IPADDR0;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_IPADDR1;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_IPADDR2;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_IPADDR3;
    FLASH_ProgramHalfWord(addr,wdata); 

    //NETMASK
    addr++;
    addr++;
    wdata=RT_LWIP_MSKADDR0;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_MSKADDR1;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_MSKADDR2;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_MSKADDR3;
    FLASH_ProgramHalfWord(addr,wdata);

    //GATEWAY
    addr++;
    addr++;
    wdata=RT_LWIP_GWADDR0;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_GWADDR1;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_GWADDR2;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_GWADDR3;
    FLASH_ProgramHalfWord(addr,wdata);

    //DNS
    addr++;
    addr++;
    wdata=RT_LWIP_DNSADDR0;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_DNSADDR1;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_DNSADDR2;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_DNSADDR3;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    Flash_Write(addr, (u8*)&dev_param, sizeof(dev_param));
    addr += sizeof(dev_param);

    FLASH_Lock();
}
/***************************************************************************************************
*\Function      stm32_param_default
*\Description   �ָ�Ĭ�ϲ����ӿ�
*\Parameter     
*\Return        void
*\Note          
*\Log           2015.07.06    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void stm32_param_default(void)
{
	int i;
    //IP��ַ
    RT_LWIP_IPADDR0 = 192;
    RT_LWIP_IPADDR1 = 168;
    RT_LWIP_IPADDR2 = 1;
    RT_LWIP_IPADDR3 = 100;

    //NETMASK
    RT_LWIP_MSKADDR0 = 255;
    RT_LWIP_MSKADDR1 = 255;
    RT_LWIP_MSKADDR2 = 255;
    RT_LWIP_MSKADDR3 = 0;

    //GATEWAY
    RT_LWIP_GWADDR0 = 192;
    RT_LWIP_GWADDR1 = 168;
    RT_LWIP_GWADDR2 = 1;
    RT_LWIP_GWADDR3 = 1;

    //DNS
    RT_LWIP_DNSADDR0 = 192;
    RT_LWIP_DNSADDR1 = 168;
    RT_LWIP_DNSADDR2 = 1;
    RT_LWIP_DNSADDR3 = 1;

    rt_memset(&dev_param, 0, sizeof(dev_param));

    dev_param.Power_First_Flag = DEV_APP_FIRST_POWER_FLAG;
	
	dev_param.mswork_mode = 0;//���ӹ���ģʽ ��0��1
	dev_param.version =0x01;//�汾
	dev_param.analog_dial_switch=0;//ģ�Ⲧ�뿪��
    stm32_param_save();
}

//ʵʱ������ʼ��
void realtime_param_init(void)
{
	realtime_param.voltage=0;
	realtime_param.current=0;
	realtime_param.heartbeat_count=0;
	realtime_param.can_debug_flag=0;
}

/***************************************************************************************************
*\Function      stm32_param_load
*\Description   ��ȡ�����ӿ�
*\Parameter     
*\Return        void
*\Note          
*\Log           2015.07.06    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
void stm32_param_load(void)
{
    u32 addr;
	uint32_t addr_appparam =  FLASH_START_ADDR + Get_FlashSize() * 1024 - FLASH_APP_PARAM_ADDR*FLASH_PAGE_SIZE;//FLASH_APP_PARAM_ADDR;
	addr=addr_appparam;
    dev_param.Power_First_Flag = *(vu16*)(addr);
    if (dev_param.Power_First_Flag != DEV_APP_FIRST_POWER_FLAG)
    {     
        rt_kprintf("Load Default Param[%04X]...\r\n", dev_param.Power_First_Flag);
        //2015��12��17�� �����һ���ϵ磬��Ĭ��ֵ
        stm32_param_default(); //����Ĭ�ϲ���,����FLASH֮�����
    }
    else
    {
        rt_kprintf("Load Param... \r\n");
    }

    //IP 
    RT_LWIP_IPADDR0 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_IPADDR1 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_IPADDR2 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_IPADDR3 = *(vu16*)(addr);
    addr++;
    addr++;

    //NETMASK
    RT_LWIP_MSKADDR0 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_MSKADDR1 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_MSKADDR2 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_MSKADDR3 = *(vu16*)(addr);
    addr++;
    addr++;

    //GATEWAY
    RT_LWIP_GWADDR0 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_GWADDR1 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_GWADDR2 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_GWADDR3 = *(vu16*)(addr);
    addr++;
    addr++;

    //DNS
    RT_LWIP_DNSADDR0 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_DNSADDR1 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_DNSADDR2 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_DNSADDR3 = *(vu16*)(addr);
    addr++;
    addr++;

    Flash_Read(addr, (u8*)&dev_param, sizeof(dev_param));
    addr += sizeof(dev_param);
	
	iap_param_load();//iap��������
	realtime_param_init();//ʵʱ������ʼ��
	device_id_set();//�豸ID����
}

