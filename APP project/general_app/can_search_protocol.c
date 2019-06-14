
/***************************************************************************************************
*                    (c) Copyright 1992-2019 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          can_search_protocol.c
*\Description   
*\Note          
*\Log           2019.01.15    Ver 1.0    �ӹ���
*               �����ļ���
***************************************************************************************************/
#include "can_search_protocol.h"
#include "can_ota_protocol.h"
#include "param.h"
#include "iap_if.h"
#include "flash_if.h"

/***************START***********�豸����/����/�Լ�baron****************************************/
extern rt_uint32_t Sys_Run_Time;//ϵͳ����ʱ��
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
	u32 data_len = 32;
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

	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID
	buf[i++] = dev_param.mswork_mode;// ����ģʽ
	buf[i++] = realtime_param.can_debug_flag;// ������Ϣ
	buf[i++] = dev_param.analog_dial_switch;// ģ�Ⲧ�뿪��
	buf[i++] = (u8)realtime_param.voltage;// ��ѹ��
	buf[i++] = (u8)(realtime_param.voltage >> 8);// ��ѹ��
	buf[i++] = (u8)realtime_param.current;// ������
	buf[i++] = (u8)(realtime_param.current >> 8);// ������
	buf[i++] = (u8)Sys_Run_Time;// ����ʱ��0
	buf[i++] = (u8)(Sys_Run_Time >> 8);// ����ʱ��8
	buf[i++] = (u8)(Sys_Run_Time >> 16);// ����ʱ��16
	buf[i++] = (u8)(Sys_Run_Time >> 24);// ����ʱ��24
	buf[i++] = (u8)dev_param.version;//app�汾��;
	buf[i++] = (u8)(dev_param.version >> 8);// app�汾��
	buf[i++] = (u8)iap_param.swversion;// iap�汾��
	buf[i++] = (u8)(iap_param.swversion >> 8);// iap�汾��
	buf[i++] = iap_param.hwversion;// Ӳ���汾
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
	iap_param.ID=((buf[9] << 8) | buf[8]);//ID
	dev_param.mswork_mode=buf[10];// ����ģʽ
	realtime_param.can_debug_flag=buf[11];// ������Ϣ
	dev_param.analog_dial_switch=buf[12];// ģ�Ⲧ�뿪��
	
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

	buf[i++] = (u8)iap_param.ChipUniqueID[0];     //UUID0
	buf[i++] = (u8)(iap_param.ChipUniqueID[0] >> 8);
	buf[i++] = (u8)iap_param.ChipUniqueID[1];
	buf[i++] = (u8)(iap_param.ChipUniqueID[1] >> 8);
	buf[i++] = (u8)iap_param.ChipUniqueID[2];
	buf[i++] = (u8)(iap_param.ChipUniqueID[2] >> 8);
	buf[i++] = (u8)iap_param.ChipUniqueID[3];
	buf[i++] = (u8)(iap_param.ChipUniqueID[3] >> 8);
	buf[i++] = (u8)iap_param.ChipUniqueID[4];
	buf[i++] = (u8)(iap_param.ChipUniqueID[4] >> 8);
	buf[i++] = (u8)iap_param.ChipUniqueID[5];
	buf[i++] = (u8)(iap_param.ChipUniqueID[5] >> 8);//UUID11
	buf[i++] = (u8)iap_param.swversion;             //iap�汾��
	buf[i++] = (u8)(iap_param.swversion >> 8);        //iap�汾��
	buf[i++] = iap_param.hwversion;                  //Ӳ���汾
	buf[i++] = iap_param.IAP_flag;                 //iap��־λ
	buf[i++] = (u8)dev_param.version;//app�汾��
	buf[i++] = (u8)(dev_param.version >> 8);//app�汾��
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
/***************************************************************************************************
*\Function      CAN_Device_Paramset_Search_SetID
*\Description   
*\Parameter     buf
*\Return        void
*\Note          
*\Log           2019.03.14    Ver 1.0    �ӹ���
*               ����������
***************************************************************************************************/
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
******************************************** END OF FILE *******************************************
***************************************************************************************************/


