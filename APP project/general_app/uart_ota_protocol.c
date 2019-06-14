/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          uart_ota_protocol.c
*\Description   
*\Note          
*\Log           2018.01.19    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#include "uart_ota_protocol.h"
#include "flash_if.h"
#include "iap_if.h"
#include "app_board.h"
#include "iap_if.h"
#include "protocol_anylise_frame.h"
#include "string.h"

//OTAЭ���־
static u8 OTA_UpdateFlag = 0;// 0 δ������1 ��ʼ��2 �����У�3 �������

u16 OTA_Version = 0;//�汾
u64 OTA_TotalSize = 0;//�����ܳ��ȣ�ѹ����ʱ��ѹ������ܳ��ȣ�δѹ��ʱ��BIN�ļ����ܳ���
u32 OTA_SubSize = 0;//�ְ��̶���
u64 OTA_CRC32 = 0;//У��

u16 OTA_Frame_Num = 0;//���������

static comm_dev_type* UartCommDev;
u32 transport_send(u8* buf, u32 len)
{
    return stm32_uart_send(&UartTTL, buf, len);
}

static u32 transport_recv(u8* buf, u32 len, bool state)
{
	return stm32_uart_recv(&UartTTL, buf, len, state);
}
static u32 transport_del(u32 len)
{
	 return stm32_uart_del(&UartTTL, len);
}
static bool transport_clear(void)
{
	 return stm32_uart_clear(&UartTTL);
}


/***************************************************************************************************
*\Function      iap_param_read
*\Description   iap�����ͳ�
*\Parameter     buf
*\Parameter     len
*\Return        uint32_t
*\Note          
*\Log           2018.07.17    Ver 1.0    �ӹ���
*               ����������
***************************************************************************************************/
uint32_t iap_param_read(uint8_t* buf, uint32_t len)
{
	u32 data_len;
	int i=0;
	//��ѯ��ǰID �ŵ��Ƿ��flash�浵��һ�£�һ����ʼͨ�ţ���һ�¿�ʼ����flash
	data_len=29;
	if (len < data_len)
    {
        return 0;
    }

	buf[i++] = COMM_PROTOCOL_HEAD;
	buf[i++] = (data_len)%256;
	buf[i++] = (data_len)/256;
	buf[i++] = FUC_PARAM_IAP_READ;//������	
	buf[i++] = iap_param.ID%256;
	buf[i++] = iap_param.ID/256;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;	
	
	buf[i++] = iap_param.ChipUniqueID[0]&0xFF;
	buf[i++] = (iap_param.ChipUniqueID[0]>>8)&0xFF;
	buf[i++] = iap_param.ChipUniqueID[1]&0xFF;
	buf[i++] = (iap_param.ChipUniqueID[1]>>8)&0xFF;	
	buf[i++] = iap_param.ChipUniqueID[2]&0xFF;
	buf[i++] = (iap_param.ChipUniqueID[2]>>8)&0xFF;	
	buf[i++] = iap_param.ChipUniqueID[3]&0xFF;
	buf[i++] = (iap_param.ChipUniqueID[3]>>8)&0xFF;	
	buf[i++] = iap_param.ChipUniqueID[4]&0xFF;
	buf[i++] = (iap_param.ChipUniqueID[4]>>8)&0xFF;
	buf[i++] = iap_param.ChipUniqueID[5]&0xFF;
	buf[i++] = (iap_param.ChipUniqueID[5]>>8)&0xFF;
	buf[i++] = 0x00;//iap_param.Lora_fourth;
	buf[i++] = 0x00;//iap_param.Lora_six;
	buf[i++] = iap_param.IAP_flag;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = iap_param.swversion;
	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = COMM_PROTOCOL_TAIL;
	return i;
}


/***************************************************************************************************
*\Function      iap_param_analysis
*\Description   iap��������
*\Parameter     buf
*\Return        void
*\Note          
*\Log           2018.07.17    Ver 1.0    �ӹ���
*               ����������
***************************************************************************************************/
void iap_param_analysis(u8* buf)
{
	 u16 data = 0;

	data =  buf[5];
	data = (data << 8) | buf[4];
	iap_param.ID=data;
//	iap_param.Channel=buf[6]+410;
	data =  buf[8];
	data = (data << 8) | buf[7];
//	iap_param.GatewayID=data;
//	iap_param.Lora_fourth=buf[21];
//	iap_param.Lora_six=buf[22];		

}


/***************************************************************************************************
*\Function      iap_param_send_lora
*\Description   iap lora�ͳ�
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.07.17    Ver 1.0    �ӹ���
*               ����������
***************************************************************************************************/
void iap_param_send_lora(void)
{
//	u32 send_len = 0;
//	send_len = iap_param_read(OTASendBuf, sizeof(OTASendBuf));		
//	if (send_len)
//	{
//		stm32_uart_send(&UartTTL, OTASendBuf, send_len);
//	}
}



/***************************************************************************************************
*\Function      OTA_Start_ACK_Construct
*\Description   OTA��ʼACK����
*\Parameter     buf
*\Parameter     len
*\Return        u32
*\Note          
*\Log           2018.01.19    Ver 1.0    Job
*               ����������
***************************************************************************************************/
u32 OTA_Start_ACK_Construct(u8* buf, u32 len)
{
    u32 data_len = 8;
    u32 i = 0;

    if (len < data_len)
    {
        return 0;
    }
    buf[i++] = COMM_PROTOCOL_HEAD;
    buf[i++] = (u8)(data_len);
    buf[i++] = (u8)((data_len) >> 8);
    buf[i++] = OTA_Start;
    buf[i++] = 0x00;//
    buf[i++] = 0x00;//
    buf[i++] = Get_Sum8(buf + 1, data_len - 3);
    buf[i++] = COMM_PROTOCOL_TAIL;

    return i;
}

/***************************************************************************************************
*\Function      OTA_Update_ACK_Construct
*\Description   ���������ACK
*\Parameter     buf
*\Parameter     len
*\Return        u32
*\Note          
*\Log           2018.01.19    Ver 1.0    Job
*               ����������
***************************************************************************************************/
u32 OTA_Update_ACK_Construct(u8* buf, u32 len, u8 state_code)
{
    u32 data_len = 11;
    u32 i = 0;

    if (len < data_len)
    {
        return 0;
    }
    buf[i++] = COMM_PROTOCOL_HEAD;
    buf[i++] = (u8)(data_len);
    buf[i++] = (u8)((data_len) >> 8);
    buf[i++] = OTA_Update;
    buf[i++] = 0x00;//
    buf[i++] = 0x00;//

    buf[i++] = (u8)OTA_Frame_Num;
    buf[i++] = (u8)(OTA_Frame_Num >> 8);

    buf[i++] = state_code;

    buf[i++] = Get_Sum8(buf + 1, data_len - 3);
    buf[i++] = COMM_PROTOCOL_TAIL;

    return i;
}
/***************************************************************************************************
*\Function      OTA_End_ACK_Construct
*\Description   
*\Parameter     buf
*\Parameter     len
*\Return        u32
*\Note          
*\Log           2018.01.19    Ver 1.0    Job
*               ����������
***************************************************************************************************/
u32 OTA_End_ACK_Construct(u8* buf, u32 len)
{
    u32 data_len = 8;
    u32 i = 0;

    if (len < data_len)
    {
        return 0;
    }
    buf[i++] = COMM_PROTOCOL_HEAD;
    buf[i++] = (u8)(data_len);
    buf[i++] = (u8)((data_len) >> 8);
    buf[i++] = OTA_End;
    buf[i++] = 0x00;//
    buf[i++] = 0x00;//
    buf[i++] = Get_Sum8(buf + 1, data_len - 3);
    buf[i++] = COMM_PROTOCOL_TAIL;

    return i;
}
/***************************************************************************************************
*\Function      OTA_Protocol_Anylise
*\Description   
*\Parameter     buf
*\Parameter     len
*\Return        del_info_type
*\Note          
*\Log           2018.01.19    Ver 1.0    Job               
				����������
***************************************************************************************************/
extern void system_reset(void);
static del_info_type UART_OTA_Protocol_Anylise(comm_dev_type* comm, u8* buf, u32 len)
{
    del_info_type info = {anylise_waiting, 0};
    u32 i = 0;
    u32 data_len = 0;
    u16 dev_ID = 0;
    u32 updata_len = 0;//������С������
	u8 send_buf[256];
    u32 send_len = 0;
	iap_updata_param_type iap_updata_param_data;
	uint32_t addr_appcopy = FLASH_START_ADDR + Get_FlashSize() * 1024 - FLASH_APP_COPY_ADDR*FLASH_PAGE_SIZE;//FLASH_APP_COPY_ADDR;

    if (len >= COMM_PROTOCOL_MIN_LEN)
    {
        /*��ʼ����*/
        i = 0;
        while (1)
        {
            /*ͬ��ͷ��β����ȷ����У��Ͳ���ȷ*/
            if ((buf[i] !=  COMM_PROTOCOL_HEAD))
            {
                for (i += 1; i < len; i++)
                {
                    if (buf[i] == COMM_PROTOCOL_HEAD)
                    {
                        break;
                    }
                }
                if (len - i >=  COMM_PROTOCOL_MIN_LEN)
                {
                    continue;
                }
                else
                {
                    info.state = anylise_reject;
                    info.del_len = i;
                    return info;
                }      
            }
            else
            {
                /*�������ݰ�����*/
                data_len = buf[i + 2];
                data_len = (data_len << 8) + buf[i + 1];
                 //���ȺϷ��ж� Ŀǰ��������8-1200
                if (data_len > COMM_PROTOCOL_MAX_LEN || data_len < COMM_PROTOCOL_MIN_LEN)
                {
                    i++;
                    continue;
                }
                /*����������ݳ���С��֡���ɳ��� �����ȴ�*/
                if (len - i < data_len)
                {
                    /*i == 0��ʾ��ȷ��֡ͷǰ��û�д������� ���صȴ�,����ɾ�����������*/
                    if (i == 0)
                    {
                        info.state = anylise_waiting;
                        info.del_len = 0;
                    }
                    else
                    {
                        info.state = anylise_reject;
                        info.del_len = i;
                    }
                    return info;
                }
                /*���֡β,֡β���Ϲ��� ����Ϊ����һ������������֡*/
                if (buf[i + data_len - 1] == COMM_PROTOCOL_TAIL && 
                    buf[i + data_len - 2] == Get_Sum8(buf + i + 1, data_len - 3))
                {
                    break;
                }
                else//֡βУ�鲻���� ɾ��һ���ֽ� ��������
                {
                    i += 1;
                    continue;
                }
            }
        }
        //��֤ͨ��������Э�鴦��
        info.state = anylise_accept;//��Ԥ��Ϊ����
        //��ɸѡ��ID ����
        dev_ID = buf[i + 5];
        dev_ID = (dev_ID << 8) | buf[i + 4];
        switch (buf[i + 3])
        {
        case OTA_Start:
			rt_thread_delay(1);
            //�����汾 ���� �ְ��� ��У��
            OTA_UpdateFlag = 1;

            OTA_Version = buf[i+6];
            OTA_Version = (OTA_Version << 8)|buf[i+7];

            OTA_TotalSize = ArrayU8ToU32(buf + i + 10);

            OTA_SubSize = ArrayU8ToU16(buf + i + 14);

            OTA_CRC32 = ArrayU8ToU32(buf + i + 16);
			rt_kprintf("\r\n OTA_Start...\r\n");
		
			memcpy(&iap_updata_param_data, buf + i + 6, sizeof(iap_updata_param_data));
			iap_updata_param_data.Version = OTA_Version;
			rt_kprintf("Bin[%d], Sub[%d], CRC32[%08X]\r\n", OTA_TotalSize, OTA_SubSize, OTA_CRC32);
		    
			//������д����
			Flash_Erase_Page(addr_appcopy, OTA_TotalSize + 64);
            //�洢bin����+crc32
            Flash_Storage_Header((uint8_t *)&iap_updata_param_data);
            //�ظ�ACK
            send_len = OTA_Start_ACK_Construct(send_buf, sizeof(send_buf));
            if (send_len)
            {
				stm32_uart_send(&UartTTL, send_buf, send_len);
            }
			rt_kprintf("Update:");
            break;
        case OTA_Update:
            OTA_UpdateFlag = 2;
            //������ ����
            OTA_Frame_Num = ArrayU8ToU16(buf + i + 6);
            rt_kprintf("%d,", OTA_Frame_Num + 1);
            if (data_len > 10 && OTA_Frame_Num * OTA_SubSize < OTA_BIN_MAX_SIZE)
            {
                //updata_lenӦ����8�ı���
                updata_len = data_len - 10;
                Flash_Write(addr_appcopy + (OTA_Frame_Num) * OTA_SubSize, buf + i + 8, updata_len);
                //ack
                send_len = OTA_Update_ACK_Construct(send_buf, sizeof(send_buf), 0);
                if (send_len)
                {
					stm32_uart_send(&UartTTL, send_buf, send_len);
                }
            }
            else
            {
                send_len = OTA_Update_ACK_Construct(send_buf, sizeof(send_buf), 1);
                if (send_len)
                {
					stm32_uart_send(&UartTTL, send_buf, send_len);
                }
            }
            
            break;
        case OTA_End:
            //������� ��������
            OTA_UpdateFlag = 3;
			rt_kprintf("\r\nCAN_OTA_End\r\n");
            //�ظ�ACK		
            send_len = OTA_End_ACK_Construct(send_buf, sizeof(send_buf));
            if (send_len)
            {
				stm32_uart_send(&UartTTL, send_buf, send_len);
				rt_thread_delay(20);
            }
#ifdef USE_IAP
			UpdateAppMode();
#endif
			rt_kprintf("IAP_FLAG: 0x%X \n\r", ReadIAPMode());
			system_reset();
            break;
		case FUC_PARAM_IAP_READ:
				send_len = iap_param_read(send_buf, sizeof(send_buf));		
				if (send_len)
				{
					stm32_uart_send(&UartTTL, send_buf, send_len);  
				}
		break;	
		case FUC_PARAM_IAP_WRITE://�޷��� ��Ҫ����
				iap_param_analysis(buf);
			break;					
		case FUC_PARAMSET_SAVE:
			iap_param_save();
			break;
		case FUC_PARAMSET_RESET:
			rt_thread_delay(20);
			system_reset();
			break;
		default:
			rt_kprintf("the cmd[%d] is no support!\n", buf[i + 4]);
			break;				
				
        }

        info.del_len = data_len + i; 
        return info;
    }
    else
    {
        return info;
    } 
}



void uart_ota_init(void)
{
	stm32_uart_init(&UartTTL);
    //���ڽ�������
    UartCommDev = comm_dev_create("UartComm", 1024, transport_recv, transport_del, transport_clear, RT_NULL, 1*RT_TICK_PER_SECOND);
    if (UartCommDev == RT_NULL)
    {
        rt_kprintf("UartCommDev creat fail\n!");
        return;
    }
    //��Ӿ���Э��
    protocol_anylise_add(UartCommDev, UART_OTA_Protocol_Anylise);
    //����Э����� ���ȼ�12
    protocol_anylise_startup(UartCommDev, 21, 2048);
}


