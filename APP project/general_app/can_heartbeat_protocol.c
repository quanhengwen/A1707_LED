
/***************************************************************************************************
*                    (c) Copyright 1992-2019 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          can_heartbeat_protocol.c
*\Description   
*\Note          
*\Log           2019.03.12    Ver 1.0    �ӹ���
*               �����ļ���
***************************************************************************************************/
#include "can_heartbeat_protocol.h"
#include "can_ota_protocol.h"
#include "iap_if.h"
#include "param.h"


/***************************************************************************************************
*\Function      CAN_Heartbeat_Package_Send
*\Description   //Ĭ����0x0101ƽ̨���ư巢��������
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.03.14    Ver 1.0    �ӹ���
*               ����������
***************************************************************************************************/
__weak void CAN_Heartbeat_Package_Send(void)
{
	u32 data_len = 8;
	u8 buf[8];
	u32 i = 0;
	
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID
	buf[i++] = realtime_param.heartbeat_count;//���
	buf[i++] = dev_param.version& 0xFF;//app�汾��
	buf[i++] = (u8)realtime_param.voltage;//��ѹ��
	buf[i++] = (u8)(realtime_param.voltage >> 8);//��ѹ��
	buf[i++] = (u8)realtime_param.current;//������
	buf[i++] = (u8)(realtime_param.current >> 8);//������
	canbus_middle_send(&CAN, 0x600, buf, data_len);
}

//can ������ ÿ10sһ��
void rt_thread_heartbeat_entry(void* parameter)
{	
	rt_thread_delay(1000);//�豸����10s����������

	while (1)
	{
		realtime_param.heartbeat_count+=1;
		if(realtime_param.heartbeat_count>=255)
		{
			realtime_param.heartbeat_count=0;
		}		
		CAN_Heartbeat_Package_Send();
		//������   10Sһ�� 
		rt_thread_delay(1000);
	}
}

/***************************************************************************************************
*\Function      rt_thread_heartbeat_init
*\Description   
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.03.14    Ver 1.0    �ӹ���
*               ����������
***************************************************************************************************/
void rt_thread_heartbeat_init(void)
{
	rt_thread_t heartbeat_init_thread;                    
	heartbeat_init_thread = rt_thread_create("heart",
		rt_thread_heartbeat_entry,RT_NULL,512,12,5);
	if (heartbeat_init_thread != RT_NULL)
		rt_thread_startup(heartbeat_init_thread);
}
/***************************************************************************************************
******************************************** END OF FILE *******************************************
***************************************************************************************************/


