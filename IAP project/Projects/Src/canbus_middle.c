/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          canbus_middle.c
*\Description   
*\Note          
*\Log           2018.08.29    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#include "canbus_middle.h"
#include "iap_if.h"
#include "flash_if.h"

RingBufType CAN_BUF500={0};
u8 canbuf500[2048]={0};
u8 otacanbuf500[2048]={0};

static u16 OTA_Version = 0;//�汾
static u32 OTA_TotalSize = 0;//bin����
static u32 OTA_SubSize = 0;//�ְ��̶���
static u32 OTA_CRC32 = 0;//У��
static u16 OTA_Frame_Num = 0;//���������
//OTAЭ���־
static u8 OTA_UpdateFlag = 0;// 0 δ������1 ��ʼ��2 �����У�3 �������
u32 can500_buf_recv(u8* buf, u32 len, bool state)
{
    return ReadDataFromBuf(&CAN_BUF500, buf, len, state);
}
u32 can500_buf_del(u32 len)
{
    return DelDataFromBuf(&CAN_BUF500, len);  
}
bool can500_buf_clear(void)
{
    return ClearDateFromBuf(&CAN_BUF500);
}

extern void system_reset(void);
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
u32 CAN_OTA_Start_ACK_Construct(u8* buf, u32 len)
{
	u32 data_len = 10;
	u32 i = 0;

	if (len < data_len)
	{
		return 0;
	}

	buf[i++] = CAN_COMM_PROTOCOL_HEAD;
	buf[i++] = (u8)(data_len);
	buf[i++] = (u8)((data_len) >> 8);
	buf[i++] = CAN_OTA_Send_Start;
	buf[i++] = (u8)iap_param.remoteID;//remoteID
	buf[i++] = (u8)(iap_param.remoteID >> 8);//remoteID
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID
	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = CAN_COMM_PROTOCOL_TAIL;

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
u32 CAN_OTA_Update_ACK_Construct(u8* buf, u32 len, u8 state_code)
{
	u32 data_len = 13;
	u32 i = 0;

	if (len < data_len)
	{
		return 0;
	}

	buf[i++] = CAN_COMM_PROTOCOL_HEAD;
	buf[i++] = (u8)(data_len);
	buf[i++] = (u8)((data_len) >> 8);
	buf[i++] = CAN_OTA_Send_Update;
	buf[i++] = (u8)iap_param.remoteID;//remoteID
	buf[i++] = (u8)(iap_param.remoteID >> 8);//remoteID
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID

	buf[i++] = (u8)OTA_Frame_Num;
	buf[i++] = (u8)(OTA_Frame_Num >> 8);
	buf[i++] = state_code;

	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = CAN_COMM_PROTOCOL_TAIL;

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
u32 CAN_OTA_End_ACK_Construct(u8* buf, u32 len)
{
	u32 data_len = 10;
	u32 i = 0;

	if (len < data_len)
	{
		return 0;
	}

	buf[i++] = CAN_COMM_PROTOCOL_HEAD;
	buf[i++] = (u8)(data_len);
	buf[i++] = (u8)((data_len) >> 8);
	buf[i++] = CAN_OTA_Send_End;
	buf[i++] = (u8)iap_param.remoteID;//remoteID
	buf[i++] = (u8)(iap_param.remoteID >> 8);//remoteID
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID
	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = CAN_COMM_PROTOCOL_TAIL;

	return i;
}

/***************START***********�豸����/����/�Լ�baron****************************************/

/***************************************************************************************************
*\Function      CAN_Paramset_Read_Construct
*\Description   �豸�����Ķ�ȡ
*\Parameter     buf
*\Parameter     id_buf  Ŀ��id
*\Parameter     len
*\Return        u32
*\Note
*\Log           2019.03.07    Ver 1.0    baron
*               ����������
***************************************************************************************************/
__weak u32 CAN_Paramset_Read_Construct(u8* buf , u32 len)//��Ҫÿ�˸����Լ�����Ŀ���
{
	u32 data_len = 18;
	u32 i = 0;

	if (len < data_len)
	{
		return 0;
	}

	buf[i++] = CAN_COMM_PROTOCOL_HEAD;
	buf[i++] = (u8)(data_len);
	buf[i++] = (u8)((data_len) >> 8);
	buf[i++] = FUC_PARAMSET_READ;
	buf[i++] = (u8)iap_param.remoteID;//remoteID
	buf[i++] = (u8)(iap_param.remoteID >> 8);//remoteID
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID

	buf[i++] = 0x00;// param
	buf[i++] = 0x00;// 
	buf[i++] = 0x00;// 
	buf[i++] = 0x00;// 
	buf[i++] = 0x00;// 
	buf[i++] = 0x00;// 
	buf[i++] = 0x00;// 
	buf[i++] = 0x00;// 
	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = CAN_COMM_PROTOCOL_TAIL;

	return i;
}

/***************************************************************************************************
*\Function      CAN_AUTOMATIC_CHECK_ACK_Construct
*\Description   �Զ���ⷢ�ͻظ�
*\Parameter     buf
*\Parameter     id_buf  Ŀ��id
*\Parameter     len
*\Return        u32
*\Note
*\Log           2019.03.08    Ver 1.0    baron
*               ����������
***************************************************************************************************/
__weak u32 CAN_Automatic_Check_Construct(u8* buf, u32 len)//���ݲ�ͬ���豸�������
{
	u32 data_len = 24;
	u32 i = 0;

	if (len < data_len)
	{
		return 0;
	}

	buf[i++] = CAN_COMM_PROTOCOL_HEAD;
	buf[i++] = (u8)(data_len);
	buf[i++] = (u8)((data_len) >> 8);
	buf[i++] = FUC_DEVICE_SEARCH;
	buf[i++] = (u8)iap_param.remoteID;//remoteID
	buf[i++] = (u8)(iap_param.remoteID >> 8);//remoteID
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID

	buf[i++] = 0x00;     //������������������
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = CAN_COMM_PROTOCOL_TAIL;

	return i;
}
/***************************************************************************************************
*\Function      CAN_Paramset_Receive_Analyse
*\Description   ���ݽ���
*\Parameter     buf
*\Parameter     len
*\Return        u32
*\Note
*\Log           2019.03.08    Ver 1.0    baron
*               ����������
***************************************************************************************************/
__weak void CAN_Paramset_Receive_Analyse(u8* buf)//��Ҫ�����Լ���Э���������
{
	u32 data_len = ((buf[2] << 8) | buf[1]);
	u32 i = 0;

}

/***************************************************************************************************
*\Function      CAN_Paramset_write_Construct
*\Description   �豸����д��ظ�
*\Parameter     buf
*\Parameter     id_buf  Ŀ��id
*\Parameter     len
*\Return        u32
*\Note
*\Log           2019.03.07    Ver 1.0    baron
*               ����������
***************************************************************************************************/
u32 CAN_Paramset_Write_Construct(u8* buf, u32 len)
{
	u32 data_len = 10;
	u32 i = 0;

	if (len < data_len)
	{
		return 0;
	}

	buf[i++] = CAN_COMM_PROTOCOL_HEAD;
	buf[i++] = (u8)(data_len);
	buf[i++] = (u8)((data_len) >> 8);
	buf[i++] = FUC_PARAMSET_WRITE;
	buf[i++] = (u8)iap_param.remoteID;//remoteID
	buf[i++] = (u8)(iap_param.remoteID >> 8);//remoteID
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID

	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = CAN_COMM_PROTOCOL_TAIL;

	return i;
}
/***************************************************************************************************
*\Function      CAN_PARAMSET_SAVE_ACK_Construct
*\Description   ����д��flash�ظ�
*\Parameter     buf
*\Parameter     id_buf  Ŀ��id
*\Parameter     len
*\Return        u32
*\Note
*\Log           2019.03.07    Ver 1.0    baron
*               ����������
***************************************************************************************************/
u32 CAN_Paramset_Save_Construct(u8* buf, u32 len)
{
	u32 data_len = 10;
	u32 i = 0;

	if (len < data_len)
	{
		return 0;
	}

	buf[i++] = CAN_COMM_PROTOCOL_HEAD;
	buf[i++] = (u8)(data_len);
	buf[i++] = (u8)((data_len) >> 8);
	buf[i++] = FUC_PARAMSET_SAVE;
	buf[i++] = (u8)iap_param.remoteID;//remoteID
	buf[i++] = (u8)(iap_param.remoteID >> 8);//remoteID
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID

	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = CAN_COMM_PROTOCOL_TAIL;

	return i;
}
/***************************************************************************************************
*\Function      CAN_DEVICE_RESTART_ACK_Construct
*\Description   �����豸�ظ�
*\Parameter     buf
*\Parameter     id_buf  Ŀ��id
*\Parameter     len
*\Return        u32
*\Note
*\Log           2019.03.07    Ver 1.0    baron
*               ����������
***************************************************************************************************/
u32 CAN_Device_Restart_Construct(u8* buf, u32 len)
{
	u32 data_len = 10;
	u32 i = 0;

	if (len < data_len)
	{
		return 0;
	}

	buf[i++] = CAN_COMM_PROTOCOL_HEAD;
	buf[i++] = (u8)(data_len);
	buf[i++] = (u8)((data_len) >> 8);
	buf[i++] = FUC_DEVICE_RESTART;
	buf[i++] = (u8)iap_param.remoteID;//remoteID
	buf[i++] = (u8)(iap_param.remoteID >> 8);//remoteID
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID

	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = CAN_COMM_PROTOCOL_TAIL;

	return i;
}
/***************************************************************************************************
*\Function      CAN_PARAMSET_DEFAULT_ACK_Construct
*\Description   �ָ�Ĭ�ϻظ�
*\Parameter     buf
*\Parameter     id_buf  Ŀ��id
*\Parameter     len
*\Return        u32
*\Note
*\Log           2019.03.07    Ver 1.0    baron
*               ����������
***************************************************************************************************/
u32 CAN_Paramset_Default_Construct(u8* buf, u32 len)
{
	u32 data_len = 10;
	u32 i = 0;

	if (len < data_len)
	{
		return 0;
	}

	buf[i++] = CAN_COMM_PROTOCOL_HEAD;
	buf[i++] = (u8)(data_len);
	buf[i++] = (u8)((data_len) >> 8);
	buf[i++] = FUC_PARAMSET_DEFAULT;
	buf[i++] = (u8)iap_param.remoteID;//remoteID
	buf[i++] = (u8)(iap_param.remoteID >> 8);//remoteID
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID

	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = CAN_COMM_PROTOCOL_TAIL;

	return i;
}
/***************************************************************************************************
*\Function      CAN_DEVICE_SEARCH_ACK_Construct
*\Description   �豸�����ظ�
*\Parameter     buf
*\Parameter     id_buf  Ŀ��id
*\Parameter     len
*\Return        u32
*\Note
*\Log           2019.03.08    Ver 1.0    baron
*               ����������
***************************************************************************************************/
u32 CAN_Device_Search_Construct(u8* buf, u32 len)
{
	u32 data_len = 32;
	u32 i = 0;

	if (len < data_len)
	{
		return 0;
	}

	buf[i++] = CAN_COMM_PROTOCOL_HEAD;
	buf[i++] = (u8)(data_len);
	buf[i++] = (u8)((data_len) >> 8);
	buf[i++] = FUC_DEVICE_SEARCH;
	buf[i++] = (u8)iap_param.remoteID;//remoteID
	buf[i++] = (u8)(iap_param.remoteID >> 8);//remoteID
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID

	buf[i++] = iap_param.ChipUniqueID[0] & 0xFF;     //UUID0
	buf[i++] = (iap_param.ChipUniqueID[0] >> 8) & 0xFF;
	buf[i++] = iap_param.ChipUniqueID[1] & 0xFF;
	buf[i++] = (iap_param.ChipUniqueID[1] >> 8) & 0xFF;
	buf[i++] = iap_param.ChipUniqueID[2] & 0xFF;
	buf[i++] = (iap_param.ChipUniqueID[2] >> 8) & 0xFF;
	buf[i++] = iap_param.ChipUniqueID[3] & 0xFF;
	buf[i++] = (iap_param.ChipUniqueID[3] >> 8) & 0xFF;
	buf[i++] = iap_param.ChipUniqueID[4] & 0xFF;
	buf[i++] = (iap_param.ChipUniqueID[4] >> 8) & 0xFF;
	buf[i++] = iap_param.ChipUniqueID[5] & 0xFF;
	buf[i++] = (iap_param.ChipUniqueID[5] >> 8) & 0xFF;//UUID11
	buf[i++] = iap_param.swversion & 0xFF;             //iap�汾��
	buf[i++] = (iap_param.swversion >> 8) & 0xFF;        //iap�汾��
	buf[i++] = iap_param.hwversion;                  //Ӳ���汾
	buf[i++] = iap_param.IAP_flag;                 //iap��־λ
	buf[i++] = 0x00;//app�汾��
	buf[i++] = 0x00;//app�汾��
	buf[i++] = (u8)iap_param.ID;;//����ID��
	buf[i++] = (u8)(iap_param.ID >> 8);//����ID��
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = CAN_COMM_PROTOCOL_TAIL;

	return i;
}
/***************************************************************************************************
*\Function      CAN_DEVICE_PARAMSET_SEARCH_ACK_Construct
*\Description   �豸�����������ûظ�
*\Parameter     buf
*\Parameter     id_buf  Ŀ��id
*\Parameter     len
*\Return        u32
*\Note
*\Log           2019.03.08    Ver 1.0    baron
*               ����������
***************************************************************************************************/
u32 CAN_Device_Paramset_Search_Construct(u8* buf, u32 len)
{
	u32 data_len = 10;
	u32 i = 0;

	if (len < data_len)
	{
		return 0;
	}

	buf[i++] = CAN_COMM_PROTOCOL_HEAD;
	buf[i++] = (u8)(data_len);
	buf[i++] = (u8)((data_len) >> 8);
	buf[i++] = FUC_DEVICE_PARAMSET_SEARCH;
	buf[i++] = (u8)iap_param.remoteID;//remoteID
	buf[i++] = (u8)(iap_param.remoteID >> 8);//remoteID
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID

	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = CAN_COMM_PROTOCOL_TAIL;

	return i;
}


//�豸���� �����豸ID
void CAN_Device_Paramset_Search_SetID(u8* buf)
{
	u32 i=0;
	int check_sum=0;
	uint16_t RecvUUID[6];//uuid
	uint16_t RecvID;//�豸��Ҫ������id
	RecvID=((buf[27] << 8) | buf[26]);
	RecvUUID[0]=((buf[9] << 8) | buf[8]);
	RecvUUID[1]=((buf[11] << 8) | buf[10]);
	RecvUUID[2]=((buf[13] << 8) | buf[12]);
	RecvUUID[3]=((buf[15] << 8) | buf[14]);
	RecvUUID[4]=((buf[17] << 8) | buf[16]);
	RecvUUID[5]=((buf[19] << 8) | buf[18]);
	for(i=0;i<6;i++)
	{
		if(RecvUUID[i]==iap_param.ChipUniqueID[i])
		{
			check_sum+=1;
		}
		else
		{
			check_sum+=0;
		}
	}
	
	if(check_sum==6)
	{//uuidУ��ͨ�� д��id
		iap_param.ID=RecvID;
	}
	else
	{//uuidУ�鲻ͨ��
		
	}
	
}


/***************END****************************************************************************/

/***************************************************************************************************
*\Function      can500_protocol_anylise
*\Description   
*\Parameter     comm
*\Parameter     buf
*\Parameter     len
*\Return        del_info_type
*\Note          
*\Log           2018.08.30    Ver 1.0    Job               
				����������
***************************************************************************************************/
static can_del_info_type can500_protocol_anylise( u8* buf, u32 len)
{
    can_del_info_type info = {can_anylise_waiting, 0};
    u32 i = 0;
    u32 data_len = 0;
    u16 dev_ID = 0;

    u32 updata_len = 0;//������С������
    u32 send_len = 0;
    u8 send_buf[100];
    CRC32_CTX CRC32;
	iap_updata_param_type iap_updata_param_data;
	uint32_t addr_appcopy = FLASH_START_ADDR + Get_FlashSize() * 1024 - FLASH_APP_COPY_ADDR*FLASH_PAGE_SIZE;//FLASH_APP_COPY_ADDR;
    if (len >= CAN_COMM_PROTOCOL_MIN_LEN)
    {
        /*��ʼ����*/
        i = 0;
        while (1)
        {
            /*ͬ��ͷ��β����ȷ����У��Ͳ���ȷ*/
            if ((buf[i] !=  CAN_COMM_PROTOCOL_HEAD))
            {
                for (i += 1; i < len; i++)
                {
                    if (buf[i] == CAN_COMM_PROTOCOL_HEAD)
                    {
                        break;
                    }
                }
                if (len - i >=  CAN_COMM_PROTOCOL_MIN_LEN)
                {
                    continue;
                }
                else
                {
                    info.state = can_anylise_reject;
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
                if (data_len > CAN_COMM_PROTOCOL_MAX_LEN || data_len < CAN_COMM_PROTOCOL_MIN_LEN)
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
                        info.state = can_anylise_waiting;
                        info.del_len = 0;
                    }
                    else
                    {
                        info.state = can_anylise_reject;
                        info.del_len = i;
                    }
                    return info;
                }
                /*���֡β,֡β���Ϲ��� ����Ϊ����һ������������֡*/
                if (buf[i + data_len - 1] == CAN_COMM_PROTOCOL_TAIL && 
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
        info.state = can_anylise_accept;//��Ԥ��Ϊ����
        //��ɸѡ��ID ����
        dev_ID = buf[i + 5];
        dev_ID = (dev_ID << 8) | buf[i + 4];
		
        iap_param.remoteID = buf[i + 7];
        iap_param.remoteID = (dev_ID << 8) | buf[i + 6];
		
		if ((iap_param.ID == dev_ID) || (iap_param.ID == 0xFFFF))
		{
			switch (buf[i + 3])
			{
			case CAN_OTA_Recv_Start:
				//�����汾 ���� �ְ��� ��У�� �˴���ʼ�Ų�����Э������һλ6 �ŵ�
				OTA_UpdateFlag = 1;
				OTA_Version = buf[i + 9];
				OTA_Version = (OTA_Version << 8) | buf[i + 8];

				OTA_TotalSize = ArrayU8ToU32(buf + i + 12);

				OTA_SubSize = ArrayU8ToU16(buf + i + 16);

				OTA_CRC32 = ArrayU8ToU32(buf + i + 18);

				printf("\r\nCAN_OTA_Start...\r\n");

				memcpy(&iap_updata_param_data, buf + i + 10, sizeof(iap_updata_param_data));
				//			iap_updata_param_data.Version = OTA_Version;

				printf("Bin[%d], Sub[%d], CRC32[%08X]\r\n", OTA_TotalSize, OTA_SubSize, OTA_CRC32);

				//�洢bin����+crc32
				Flash_Storage_Header((uint8_t *)&iap_updata_param_data);
				//������д����
				Flash_Erase_Page(addr_appcopy, OTA_TotalSize);

				//�ظ�ACK
				send_len = CAN_OTA_Start_ACK_Construct(send_buf, sizeof(send_buf));
				if (send_len)
				{
					canbus_middle_send(&CAN, 0x500, send_buf, send_len);
				}
				printf("Update:");
				break;
			case CAN_OTA_Recv_Update:
				OTA_UpdateFlag = 2;
				//������ ����
				OTA_Frame_Num = ArrayU8ToU16(buf + i + 8);
				printf("%d,", OTA_Frame_Num + 1);
				if (data_len > 10 && OTA_Frame_Num * OTA_SubSize < OTA_BIN_MAX_SIZE)
				{
					//updata_lenӦ����4�ı���
					updata_len = data_len - 12;
					Flash_Write(addr_appcopy + (OTA_Frame_Num)* OTA_SubSize, buf + i + 10, updata_len);
					//ack
					send_len = CAN_OTA_Update_ACK_Construct(send_buf, sizeof(send_buf), 0);
					if (send_len)
					{
						canbus_middle_send(&CAN, 0x500, send_buf, send_len);
					}
				}
				else
				{
					send_len = CAN_OTA_Update_ACK_Construct(send_buf, sizeof(send_buf), 1);
					if (send_len)
					{
						canbus_middle_send(&CAN, 0x500, send_buf, send_len);
					}
				}

				break;
			case CAN_OTA_Recv_End:
				HAL_Delay(10);
				//������� ��������
				OTA_UpdateFlag = 3;
				//�ظ�ACK
				printf("\r\nCAN_OTA_End\r\n");
				send_len = CAN_OTA_End_ACK_Construct(send_buf, sizeof(send_buf));
				if (send_len)
				{
					canbus_middle_send(&CAN, 0x500, send_buf, send_len);
				}
				UpdateAppMode();
				printf("IAP_FLAG: 0x%X \n\r", (uint16_t)ReadIAPMode());
				HAL_Delay(100);
				NVIC_SystemReset();
				break;
				/*********************START**********�豸����/����/�Լ�baron***************************************/
			case FUC_PARAMSET_READ:            //�豸������ȡ			
				send_len = CAN_Paramset_Read_Construct(send_buf, sizeof(send_buf));
				if (send_len)
				{
					canbus_middle_send(&CAN, 0x500, send_buf, send_len);
				}			
				break;
			case FUC_PARAMSET_WRITE:           //�豸����д��
				//���������д�뵥������
				CAN_Paramset_Receive_Analyse(&buf[i]); //�����ݴ���receive_buf
				send_len = CAN_Paramset_Write_Construct(send_buf, sizeof(send_buf));
				if (send_len)
				{
					canbus_middle_send(&CAN, 0x500, send_buf, send_len);
				}		
				break;
			case FUC_PARAMSET_SAVE:            //����д��flash			
				//�������д��flash��������
				iap_param_save();
				send_len = CAN_Paramset_Save_Construct(send_buf, sizeof(send_buf));
				if (send_len)
				{
					canbus_middle_send(&CAN, 0x500, send_buf, send_len);
				}			
				break;
			case FUC_DEVICE_RESTART:           //�����豸 			
				send_len = CAN_Device_Restart_Construct(send_buf, sizeof(send_buf));
				if (send_len)
				{
					canbus_middle_send(&CAN, 0x500, send_buf, send_len);
				}
				HAL_Delay(10);
				system_reset();					
				break;
			case FUC_PARAMSET_DEFAULT:         //�ָ�Ĭ�ϲ���							
				send_len = CAN_Paramset_Default_Construct(send_buf, sizeof(send_buf));
				if (send_len)
				{
					canbus_middle_send(&CAN, 0x500, send_buf, send_len);
				}
				iap_param_default();
			break;
			case FUC_DEVICE_SEARCH:            //�豸����		
				send_len = CAN_Device_Search_Construct(send_buf, sizeof(send_buf));
				if (send_len)
				{
					
					if(iap_param.ID == 0xFFFF)
					{
						//�޵ȴ�
					}
					else
					{
						HAL_Delay(((iap_param.ID >> 8) & 0xFF) * 10 + (iap_param.ID & 0xFF) * 2);
					}
					canbus_middle_send(&CAN, 0x500, send_buf, send_len);
				}	
				break;
			case FUC_DEVICE_PARAMSET_SEARCH:   //�豸������������			
				//�豸�������̽������޸�id
				CAN_Device_Paramset_Search_SetID(&buf[i]); //�����ݴ���receive_buf
				send_len = CAN_Device_Paramset_Search_Construct(send_buf, sizeof(send_buf));
				if (send_len)
				{
					canbus_middle_send(&CAN, 0x500, send_buf, send_len);
				}		
			break;
			case FUC_AUTOMATIC_CHECK:          //�Զ����		
				send_len = CAN_Automatic_Check_Construct(send_buf, sizeof(send_buf));
				if (send_len)
				{
					canbus_middle_send(&CAN, 0x500, send_buf, send_len);
				}			
				break;
			/*********************END*******************************************************************/
			default:
				printf("the cmd[%d] is no support!\n", buf[i + 3]);
				break;
			}
		}
		
       

        info.del_len = data_len + i; 
        return info;
    }
    else
    {
        return info;
    } 
}
/***************************************************************************************************
*\Function      canbus_middle_send
*\Description   CAN���ݷ���
*\Parameter     can
*\Parameter     ID
*\Parameter     buf
*\Parameter     len
*\Return        u32
*\Note          
*\Log           2018.08.30    Ver 1.0    Job               
				����������
***************************************************************************************************/
u32 canbus_middle_send(CANBus_type* can, u32 stdID, u8* buf, u32 len)
{
    CANBusTx_type tx;
    u32 i = 0;
    u32 num = 0;
    u32 len_left = 0;

    if (!can || len == 0 || !buf)
    {
        return 0;
    }

    num = len / 8;
    len_left = len % 8;

    tx.CANTxHead.StdId = stdID;
    tx.CANTxHead.IDE = CAN_ID_STD;
    tx.CANTxHead.RTR = CAN_RTR_DATA;

    //ÿ�η���8�ֽ�
    for (i = 0; i < num; i++)
    {
        tx.CANTxHead.DLC = 8;
        memcpy(tx.Data, buf + i * 8, 8);
        if (!stm32_CAN_send(can, &tx))
        {
            return i * 8;
        }
        if (i % 10 == 0)
        {
            HAL_Delay(10);
        }
    }

    //���Ͷ�����ֽ�
    if (len_left)
    {
        tx.CANTxHead.DLC = len_left;
        memcpy(tx.Data, buf + num * 8, len_left);
        if (!stm32_CAN_send(can, &tx))
        {
            return num * 8;
        }
    }

    return len;
}



#include "usdl_buf.h"
//can ���ݽ�����س�ʼ��
void CAN_middle_init(void)
{
	RingBufInit(&CAN_BUF500,canbuf500, 1024);
    stm32_CAN_clear(&CAN);
}

static int CANOTAWaitTicks;
void CAN_OTA_Periodic_Handle(void)
{
    u32 len = 0;
	CANBusRx_type rx;
    u32 delay = 0;
	can_del_info_type info = {can_anylise_waiting, 0};
    int tick_diff;
	
	if (stm32_CAN_recv(&CAN, &rx))
	{
		
		delay++;
		if (delay % 10 == 0)
		{
			HAL_Delay(10);
			delay = 0;
		}
		
		if (rx.CANRxHead.IDE != CAN_ID_STD || rx.CANRxHead.RTR != CAN_RTR_DATA)
		{
//                continue;
		}
		switch (rx.CANRxHead.StdId)
		{
		case 0x500:
			AddDataToBuf(&CAN_BUF500, rx.Data, rx.CANRxHead.DLC);
			{
				u32  i = 0;
//				printf("LEN:%d\n", rx.CANRxHead.DLC);
//				for(i = 0; i< rx.CANRxHead.DLC;i++)
//				{
//					printf("%02X ", rx.Data[i]);
//				}
//				printf("\r\n");
			}


			break;
		default:
			printf("StdID[0x%03X] is no support!\n", rx.CANRxHead.StdId);
			break;
		}
	}
	else
	{
		delay = 0;
		HAL_Delay(10);
	}
	
	len = ReadDataFromBuf(&CAN_BUF500, otacanbuf500, sizeof(otacanbuf500), FALSE);
	if (len == 0)
	{
		CANOTAWaitTicks = HAL_GetTick();		
		return;
	}
//	printf("LEN:%d\r\n", len);
	info = can500_protocol_anylise(otacanbuf500, len);
	if (info.state != can_anylise_waiting)
	{
		//ɾ��
		DelDataFromBuf(&CAN_BUF500, info.del_len);
		CANOTAWaitTicks = HAL_GetTick();
	}
	else
	{
		tick_diff = HAL_GetTick() - CANOTAWaitTicks;
		if (Abs(tick_diff) > 10*100)//����5S�������ɹ�������ջ��������ݣ����߱���һ�㣬ɾ��һ���ֽڣ�������������Ƚ�����
		{
			stm32_CAN_clear(&CAN);
			CANOTAWaitTicks = HAL_GetTick();
		} 
	}
	
}
















