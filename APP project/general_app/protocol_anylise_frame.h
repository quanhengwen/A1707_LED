/***************************************************************************************************
*                    (c) Copyright 1992-2015 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          protocol_anylise_frame.h
*\Description   Э��������
*\Note          
*\Log           2015.07.11    Ver 1.0    �Ų�
*               �����ļ���
***************************************************************************************************/
#ifndef _PROTOCOL_ANYLISE_FRAME_H
#define _PROTOCOL_ANYLISE_FRAME_H

#include "common_lib.h"

/*�����ýṹ*/
typedef enum
{
    anylise_waiting,
    anylise_accept,
    anylise_reject,   
}anylise_state;

typedef struct del_info_type_st 
{
    anylise_state state;
    u32 del_len;
}del_info_type;


/*Э��������֧���飬��ͬһ������Դ�ɽ�����Э�����͸���*/
#define PROTOCOL_FUN_MAX    3
typedef struct comm_dev_type_st comm_dev_type;
//Э�鴦����ָ�룬Э�鴦��ֱ���ٺ����У���Ҫ�ⲿ������ͨ������ͨ��
typedef del_info_type (*protocol_anylise_handle)(comm_dev_type* comm, u8* buf, u32 len);

/*Э�����ͨѶ�豸����*/
typedef struct comm_dev_type_st
{
    char* name;
    u8* buf;//Э������û�����
    u32 buf_len;//���������� 
    u32 (*recv)(u8* buf, u32 len, bool state);//�������ݺ���
    u32 (*del)(u32 len);//ɾ�����ݺ���
    bool (*clear)();//������ݺ���
    u32 (*send)(u8* buf, u32 len);
    u32 ticks_out;//��ʱɾ��ʱ�� ÿ�������������ܻ᲻һ�� by Job 2018��1��29��
    protocol_anylise_handle  protocol_anylise_fun[PROTOCOL_FUN_MAX];
    u32 protocol_num;//ʵ����ӵ�Э�����
    rt_thread_t tid;
    u32 wait_ticks;//�ȴ���ʱ��ʱ
}comm_dev_type;


comm_dev_type* comm_dev_create(char* name, u32 len, \
                               u32 (*recv)(u8* buf, u32 len, bool state), \
                               u32 (*del)(u32 len),\
                               bool (*clear)(),\
                               u32 (*send)(u8* buf, u32 len),\
                               u32 out_time
                               );
bool protocol_anylise_add(comm_dev_type* comm_dev, protocol_anylise_handle protocol_anylise);
bool protocol_anylise_del(comm_dev_type* comm_dev, protocol_anylise_handle protocol_anylise);
bool protocol_anylise_startup(comm_dev_type* comm_dev, rt_uint8_t priority, rt_uint32_t stack);
bool protocol_anylise_destroy(comm_dev_type* comm_dev);
#endif /*_PROTOCOL_ANYLISE_FRAME_H*/

