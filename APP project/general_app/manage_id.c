
/***************************************************************************************************
*                    (c) Copyright 1992-2019 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          manage_id.c
*\Description   
*\Note          
*\Log           2019.03.14    Ver 1.0    �ӹ���
*               �����ļ���
***************************************************************************************************/
#include "manage_id.h"
#include "param.h"
#include "iap_if.h"

//����PCB��param.h ʹ�ú궨��
//#define VARI3101_SCU_ID 	//*1 ƽ̨���ư�
//#define VARI3101_DCG_ID 	//*1 ������������A*1
//#define VARI3101_RCG_ID 	//*1 ң������*1
//#define VARI3101_PMU_ID 	//*1 ��ع�������*1
//#define VARI3101_PXU_ID 	//*2 �������Ӱ�*4
//#define VARI3101_GNSS_ID 	//*1 GNSS����*1
//#define VARI3101_ACCG_ID 	//*2 ��̬��������*4
//#define VARI3101_BMLG_ID 	//*2 �߲յ�������*4 ���Ҳ�����������*4
//#define VARI3101_MLDG_ID 	//*2 ���Ҳ���װ�ε���*4
//#define VARI3101_WSG_ID 	//*1 �����������
//#define VARI3101_OAG_ID 	//*2 ��������*4
//#define VARI3101_DWSM_ID 	//*2 �Ӷ��ּ��*2
//#define VARI3101_OWSM_ID 	//*2 OES�ٶȴ�����*2



/***************************************************************************************************
*\Function      device_id_set
*\Description   //�豸ID����
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.03.14    Ver 1.0    �ӹ���
*               ����������
***************************************************************************************************/
void device_id_set(void)
{
	u16 dial_switch_flag = 0;//���뿪�ر�־λ ����������Ϊ0��Զ��������Ϊ1������Ĭ��Ϊ�������ͬ���İ���
	u16 id_increase = 0;
	//Ԥ�����뿪������
	
	//��pcbδʹ�ò��뿪��ǰʹ��ģ�Ⲧ�뿪��
	dial_switch_flag=dev_param.analog_dial_switch;
	
	//���ݲ�ͬ�Ĺ���ģʽ���ò�ͬ��id����ģʽ ��+0���ӵ����豸+1,����豸+2
	if (dev_param.mswork_mode == 0)
	{
		id_increase = 0;
	}
	else
	{
		id_increase = 1;
	}

#ifdef VARI3101_SCU_ID
	iap_param.ID = 0x0101 + dial_switch_flag + id_increase;
#endif	

#ifdef VARI3101_DCG_ID
	iap_param.ID = 0x0201 + dial_switch_flag + id_increase;
#endif	

#ifdef VARI3101_RCG_ID
	iap_param.ID = 0x0301 + dial_switch_flag + id_increase;
#endif	

#ifdef VARI3101_PMU_ID
	iap_param.ID = 0x0401 + dial_switch_flag + id_increase;
#endif	

#ifdef VARI3101_PXU_ID
	if (dev_param.mswork_mode == 0)
	{
		id_increase = 0;
	}
	else
	{
		id_increase = 2;
	}
	iap_param.ID = 0x0501 + dial_switch_flag + id_increase;
#endif	

#ifdef VARI3101_GNSS_ID
	iap_param.ID = 0x0601 + dial_switch_flag + id_increase;
#endif	

#ifdef VARI3101_ACCG_ID
	if (dev_param.mswork_mode == 0)
	{
		id_increase = 0;
	}
	else
	{
		id_increase = 2;
	}
	iap_param.ID = 0x0701 + dial_switch_flag + id_increase;
#endif	

#ifdef VARI3101_BMLG_ID
	if (dev_param.mswork_mode == 0)
	{
		id_increase = 0;
	}
	else
	{
		id_increase = 2;
	}
	iap_param.ID = 0x0801 + dial_switch_flag + id_increase;
#endif	

#ifdef VARI3101_MLDG_ID
	if (dev_param.mswork_mode == 0)
	{
		id_increase = 0;
	}
	else
	{
		id_increase = 2;
	}
	iap_param.ID = 0x0901 + dial_switch_flag + id_increase;
#endif	

#ifdef VARI3101_WSG_ID
	iap_param.ID = 0x0A01 + dial_switch_flag + id_increase;
#endif	

#ifdef VARI3101_OAG_ID
	if (dev_param.mswork_mode == 0)
	{
		id_increase = 0;
	}
	else
	{
		id_increase = 2;
	}
	iap_param.ID = 0x0B01 + dial_switch_flag + id_increase;
#endif	

#ifdef VARI3101_DWSM_ID
	if (dev_param.mswork_mode == 0)
	{
		id_increase = 0;
	}
	else
	{
		id_increase = 2;
	}
	iap_param.ID = 0x0C01 + dial_switch_flag + id_increase;
#endif	

#ifdef VARI3101_OWSM_ID
	if (dev_param.mswork_mode == 0)
	{
		id_increase = 0;
	}
	else
	{
		id_increase = 2;
	}
	iap_param.ID = 0x0D01 + dial_switch_flag + id_increase;
#endif	

}

/***************************************************************************************************
******************************************** END OF FILE *******************************************
***************************************************************************************************/


