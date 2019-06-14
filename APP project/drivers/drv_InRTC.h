/***************************************************************************************************
*                    (c) Copyright 2008-2017  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          drv_InRTC.h
*\Description   
*\Note          
*\Log           2017.06.01    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#ifndef _DRV_INRTC_H
#define _DRV_INRTC_H
#include "common_lib.h"

#define HOUR_FORMAT     0   /*Ϊ0:24Сʱ��ʽ ����0:Ϊ12Сʱ��ʽ*/


void stm32_Inrtc_init(bool force);
bool stm32_Inrtc_getTime(TIME* time);
bool stm32_Inrtc_setTime(TIME time);
#endif /*_DRV_INRTC_H*/

