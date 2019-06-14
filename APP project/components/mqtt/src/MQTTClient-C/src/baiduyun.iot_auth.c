/***************************************************************************************************
*                    (c) Copyright 2008-2017  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          baiduyun.iot_auth.c
*\Description   
*\Note          1/һֱ�����ڰٶ���
*\Log           2017.05.05    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "aliyun_iot_platform_stdio.h"
#include "aliyun_iot_platform_network.h"
#include "aliyun_iot_platform_persistence.h"
#include "aliyun_iot_platform_memory.h"
#include "aliyun_iot_common_datatype.h"
#include "aliyun_iot_common_error.h"
#include "aliyun_iot_common_log.h"
#include "aliyun_iot_common_config.h"
#include "aliyun_iot_common_base64.h"
#include "aliyun_iot_common_json.h"
#include "aliyun_iot_common_md5.h"
#include "aliyun_iot_common_httpclient.h"
#include "aliyun_iot_common_hmac.h"
#include "aliyun_iot_auth.h"
#include "aliyun_iot_mqtt_internal.h"

#include "iap_if.h"
#include "param.h"

#ifndef USING_ALIYUN_SERV
/***********************************************************
* ȫ�ֱ���: g_userInfo
* ��       ��: �û���Ϣ
* ˵       ��:
************************************************************/
ALIYUN_IOT_USER_INFO_S g_userInfo;
/***********************************************************
* ȫ�ֱ���: g_authInfo
* ��       ��: ��Ȩ��Ϣ�ڴ�ָ��
* ˵       ��: ��ʼ��ʱmalloc�õ�authinfo�ڴ汣���ַ�ڴ˱�����
************************************************************/
AUTH_INFO_S *g_authInfo = NULL;

/***********************************************************
* ��������: aliyun_iot_set_auth_state
* ��       ��: ���ü�Ȩģʽ
* �������: USER_AUTH_STATE_E authState
* �������: VOID
* �� ��  ֵ: VOID
* ˵       ��: EVERY_TIME_AUTH    ÿ�ε���aliyun_iot_auth�ӿڶ������»�ȡ֤���Ȩ
*           FIRST_CONNECT_AUTH ֻ���豸��һ������ʱ��Ȩ���Ժ�ʹ�ü�¼�ļ�Ȩ��Ϣ
************************************************************/
void aliyun_iot_set_auth_state(USER_AUTH_STATE_E authState)
{
    
}

int set_usr_info(AUTH_INFO_S * authInfo)
{
    ALIYUN_IOT_USER_INFO_S *userInfo = &g_userInfo;
    u32 chip_id[3];

    memset(&g_userInfo,0x0,sizeof(ALIYUN_IOT_USER_INFO_S));
    //����client ID
    Get_ChipID(chip_id);
    aliyun_iot_stdio_snprintf(userInfo->clientId, CLIENT_ID_LEN, "%08X%08X%08X", chip_id[0], chip_id[1], chip_id[2]);
    WRITE_IOT_DEBUG_LOG("userInfo-clientId:%s",userInfo->clientId);

    //����username
    memcpy(userInfo->userName, dev_param.MQTT_User, strlen((const char*)dev_param.MQTT_User));
    WRITE_IOT_DEBUG_LOG("userInfo-userName:%s",userInfo->userName);

    //password
    memcpy(userInfo->passWord, dev_param.MQTT_Password, strlen((const char*)dev_param.MQTT_Password));
    WRITE_IOT_DEBUG_LOG("userInfo-passWord:%s",userInfo->passWord);

    //host address
    memcpy(userInfo->hostAddress, dev_param.DstAddr, strlen((const char*)dev_param.DstAddr));
    memcpy(userInfo->port, dev_param.DstPort, strlen((const char*)dev_param.DstPort));

    WRITE_IOT_DEBUG_LOG("userInfo->port:%s",userInfo->port);
    WRITE_IOT_DEBUG_LOG("userInfo-hostAddress:%s",userInfo->hostAddress);

    return SUCCESS_RETURN;
}
INT32 aliyun_iot_verify_certificate(AUTH_INFO_S *authInfo,SERVER_PARAM_TYPE_E pullType,SIGN_DATA_TYPE_E signDataType)
{
     return SUCCESS_RETURN;
}
INT32 pull_server_param(SIGN_DATA_TYPE_E signDataType,SERVER_PARAM_TYPE_E pullType,AUTH_INFO_S *info)
{
    return SUCCESS_RETURN;
}
/***********************************************************
* ��������: aliyun_iot_auth_init
* ��       ��: auth��ʼ������
* �������: VOID
* �������: VOID
* �� ��  ֵ: 0 �ɹ���-1 ʧ��
* ˵       ��: ��ʼ����־�����豸��Ϣ����Ȩ��Ϣ�ļ��ı���·��
************************************************************/
INT32 aliyun_iot_auth_init()
{
    aliyun_iot_common_set_log_level(ERROR_IOT_LOG);
    aliyun_iot_common_log_init();

    WRITE_IOT_NOTICE_LOG("auth init success!");
    return SUCCESS_RETURN;
}

/***********************************************************
* ��������: aliyun_iot_auth_release
* ��       ��: auth�ͷź���
* �������: VOID
* �������: VOID
* �� ��  ֵ: 0:�ɹ� -1:ʧ��
* ˵      ��: �ͷ�authInfo�ڴ�
************************************************************/
INT32 aliyun_iot_auth_release()
{
    WRITE_IOT_NOTICE_LOG("auth release!");
    aliyun_iot_common_log_release();
    return SUCCESS_RETURN;
}

/***********************************************************
* ��������: aliyun_iot_set_device_info
* ��       ��: �����豸��Ϣ
* �������: IOT_DEVICEINFO_SHADOW_S*deviceInfo
* �������: VOID
* �� ��  ֵ: 0���ɹ�  -1��ʧ��
* ˵       ��: ����aliyunע����豸��Ϣ���õ�SDK�е��豸������
************************************************************/
INT32 aliyun_iot_set_device_info(IOT_DEVICEINFO_SHADOW_S*deviceInfo)
{
    WRITE_IOT_DEBUG_LOG("set device info success!");

    return SUCCESS_RETURN;
}

/***********************************************************
* ��������: aliyun_iot_auth
* ��       ��: sdk�û���Ȩ����
* �������: SIGN_DATA_TYPE_E signDataType ǩ������
*          IOT_BOOL_VALUE_E haveFilesys �Ƿ����ļ�ϵͳ
* �� ��  ֵ: 0���ɹ�  -1��ʧ��
* ˵       ��: ��Ȩ�õ���Կ֤�鲢�����û���Ϣ
************************************************************/
INT32 aliyun_iot_auth(SIGN_DATA_TYPE_E signDataType,IOT_BOOL_VALUE_E haveFilesys)
{
    int rc = SUCCESS_RETURN;

    set_usr_info(NULL);
    return rc;
}
#endif


