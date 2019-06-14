/***************************************************************************************************
*                    (c) Copyright 1992-2015 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          protocol_anylise_frame.c
*\Description   Э��������
*\Note          1���������ɹ�ʱ Ӧ������wait_tick,�����ж�������֡����ʱ�����ڽ����ɹ������һ֡
*               ���ݿ��ܻ�δ����������³�ʱ
*\Log           2015.07.11    Ver 1.0    �Ų�
                2017��11��9�� Ver 1.1    Job
*               �����ļ���
***************************************************************************************************/
#include "protocol_anylise_frame.h"

//����Э�������Ϣ����
#define protocol_printf   rt_kprintf

/***************************************************************************************************
*\Function      comm_dev_create
*\Description   ͨѶ�豸���������ӿ�
*\Parameter     name
*\Parameter     len ͨѶ�豸��Ҫ�Ļ���������
*\Parameter     recv   ͨѶ�豸�������ݺ���
*\Parameter     del    ͨѶ�豸ɾ�����ݺ���
*\Parameter     clear  ͨѶ�豸������ݺ���
*\Parameter     send   ͨѶ�豸�������ݺ���
*\Return        comm_dev_type*
*\Note          
*\Log           2015.07.11    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
comm_dev_type* comm_dev_create(char* name, u32 len, \
                     u32 (*recv)(u8* buf, u32 len, bool state), \
                     u32 (*del)(u32 len),\
                     bool (*clear)(),\
                     u32 (*send)(u8* buf, u32 len),\
                     u32 out_time
                     )
{
    comm_dev_type* comm_dev = RT_NULL;

    comm_dev = (comm_dev_type*)rt_malloc(sizeof(comm_dev_type));
    if (comm_dev == RT_NULL)
    {
        protocol_printf("comm_dev creat fail!\n");
        return comm_dev;
    }
    comm_dev->name = name;
    comm_dev->buf_len = len;
    comm_dev->buf = rt_malloc(comm_dev->buf_len);
    if (comm_dev->buf == RT_NULL)
    {
        rt_kprintf("comm_dev buf create fail!\n");
        rt_free(comm_dev);
        comm_dev = RT_NULL;
        return comm_dev;
    }
    comm_dev->recv = recv;
    comm_dev->send = send;
    comm_dev->del = del;
    comm_dev->clear = clear;
    comm_dev->wait_ticks = 0;
    comm_dev->ticks_out = out_time;//2018��3��5�� By Job

    comm_dev->protocol_num = 0;
    rt_memset(comm_dev->protocol_anylise_fun, 0, sizeof(comm_dev->protocol_anylise_fun));

    comm_dev->tid = RT_NULL;

    return comm_dev;
}

/***************************************************************************************************
*\Function      protocol_anylise_add
*\Description   ��Э������������ʵ�ʽ�������
*\Parameter     protocol_anylise
*\Return        bool�� TRUE������ӳɹ�
*\Note          
*\Log           2015.07.13    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool protocol_anylise_add(comm_dev_type* comm_dev, protocol_anylise_handle protocol_anylise)
{
    u32 i = 0;

    if (comm_dev == RT_NULL)
    {
        protocol_printf("comm_dev is NULL\n");
        return FALSE;
    }
    /*Э�����������޷������*/
    if (comm_dev->protocol_num >= PROTOCOL_FUN_MAX)
    {
        protocol_printf("protocol fun is max!\n");
        return FALSE;
    }
    //�ȹ����߳�
    if (comm_dev->tid != RT_NULL)
    {
        rt_thread_suspend(comm_dev->tid);
    }
    for (i = 0; i < PROTOCOL_FUN_MAX; i++)
    {
        //���ҵ��գ����
        if (comm_dev->protocol_anylise_fun[i] == RT_NULL)
        {
            comm_dev->protocol_anylise_fun[i] = protocol_anylise;
            comm_dev->protocol_num++;
            //�ȹ����߳�
            if (comm_dev->tid != RT_NULL)
            {
                rt_thread_resume(comm_dev->tid);
            }
            return TRUE;
        }
    }

    if (comm_dev->tid != RT_NULL)
    {
        rt_thread_resume(comm_dev->tid);
    }
    return FALSE;
}

/***************************************************************************************************
*\Function      protocol_anylise_del
*\Description   �ӽ�����������ɾ��Ҫ������Э��
*\Parameter     comm_dev
*\Parameter     protocol_anylise
*\Return        bool
*\Note          
*\Log           2015.07.13    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool protocol_anylise_del(comm_dev_type* comm_dev, protocol_anylise_handle protocol_anylise)
{
    u32 i = 0;

    if (comm_dev == RT_NULL)
    {
        protocol_printf("comm_dev is NULL\n");
        return FALSE;
    }
    //Э�����Ϊ0,��������
    if (comm_dev->protocol_num == 0)
    {
        return TRUE;
    }
    //�����߳�
    if (comm_dev->tid != RT_NULL)
    {
        rt_thread_suspend(comm_dev->tid);
    }
    for (i = 0; i < PROTOCOL_FUN_MAX; i++)
    {
        //���ҵ���ɾ��
        if (comm_dev->protocol_anylise_fun[i] == protocol_anylise)
        {
            comm_dev->protocol_anylise_fun[i] = RT_NULL;
            comm_dev->protocol_num--;
            if (comm_dev->tid != RT_NULL)
            {
                rt_thread_resume(comm_dev->tid);
            }
            return TRUE;
        }
    }

    if (comm_dev->tid != RT_NULL)
    {
        rt_thread_resume(comm_dev->tid);
    }

    return FALSE;
}

/***************************************************************************************************
*\Function      protocol_anylise_process
*\Description   Э�������ܺ���
*\Parameter     comm
*\Return        void
*\Note          
*\Log           2015.7.13    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
static void protocol_anylise_process(comm_dev_type* comm)
{
    del_info_type info[PROTOCOL_FUN_MAX];
    u32 i = 0;
    u32 len = 0;
    u32 del_min = 0;
    int time_diff = 0;

    /*��ȡͨѶ�豸����*/
    len = comm->recv(comm->buf, comm->buf_len, FALSE);
    if (len == 0)
    {
        comm->wait_ticks = rt_tick_get();
        return;
    }
    for (i = 0; i < PROTOCOL_FUN_MAX; i++)
    {
        /*�����������Ϊ�� ����Ϊ�������Ϊ�ܾ���ɾ������Ϊ���*/
        if (comm->protocol_anylise_fun[i] == RT_NULL)
        {
            info[i].state = anylise_reject;
            info[i].del_len = len;
            continue;
        }
        info[i] = comm->protocol_anylise_fun[i](comm, comm->buf, len);
        if (info[i].state == anylise_accept)
        {
            /*ɾ���ɹ���������������*/
            comm->del(info[i].del_len);
            //���ճɹ��� Ӧ��Ҳ����һ��ʱ�� �����кü�֡���ݣ����ܻᵼ��wait_tick��ʱɾ�� 
            comm->wait_ticks = rt_tick_get();
            return;//�����ɹ�����ν�������
        }
        else
        {
            continue;
        }
    }
    //���е��˴�ʱ��������accept����Ϊacceptֱ�ӷ�����
    /*������������������Ǿܾ� ��ɾ����С�ܾ�����*/
    for (i = 0; i < PROTOCOL_FUN_MAX; i++)
    {
        if (info[i].state == anylise_reject)
        {
            if (del_min == 0)
            {
                del_min = info[i].del_len;
            }
            else
            {
                del_min = Min(del_min, info[i].del_len);
            }         
        }
        else
        {
            break;
        }
    }
    if (i >= PROTOCOL_FUN_MAX)//������������˵������reject
    {
        comm->del(del_min);   
        comm->wait_ticks = rt_tick_get();
        return;
    }
    else
    {  
        time_diff = rt_tick_get()  - comm->wait_ticks;//ֱ�ӷŵ�Abs�л�������� ����ʱ��
        if (Abs(time_diff) > comm->ticks_out)//����200ms�������ɹ�������ջ��������ݣ����߱���һ�㣬ɾ��һ���ֽڣ�������������Ƚ�����
        {
            comm->clear();
            comm->wait_ticks = rt_tick_get();
        }    
    }
}

/***************************************************************************************************
*\Function      protocol_anylise_thread_entry
*\Description   Э������߳�
*\Parameter     para
*\Return        void
*\Note          
*\Log           2015.07.13    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
static void protocol_anylise_thread_entry(void* para)
{
    comm_dev_type* comm_dev = (comm_dev_type*)para;

    if (comm_dev == RT_NULL)
    {
        protocol_printf("the para of procotol anylise thread is NULL\n");
        return;
    }
    
    while (1)
    {
        protocol_anylise_process(comm_dev);
        rt_thread_delay(RT_TICK_PER_SECOND / 50);
    }
}
/***************************************************************************************************
*\Function      protocol_anylise_startup
*\Description   ����Э�����
*\Parameter     comm_dev
*\Parameter     priority�������߳����ȼ�
*\Parameter     stack,�̶߳�ջ
*\Return        bool
*\Note          
*\Log           2015.07.13    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool protocol_anylise_startup(comm_dev_type* comm_dev, rt_uint8_t priority, rt_uint32_t stack)
{
    if (comm_dev == RT_NULL)
    {
        protocol_printf("comm_dev is NULL\n");
        return FALSE;
    }
    //ֻ�����Э���� �ſ��������߳�
    if (comm_dev->protocol_num == 0)
    {
        protocol_printf("protocol fun is Null\n");
        return FALSE;
    }
    //�����߳��Ѿ�����
    if (comm_dev->tid != RT_NULL)
    {
        protocol_printf("protocol thread has startup!\n");
        return TRUE;
    }
    comm_dev->tid = rt_thread_create(comm_dev->name, protocol_anylise_thread_entry, comm_dev, stack, priority, 5);
    if (comm_dev->tid != RT_NULL)
    {
        rt_thread_startup(comm_dev->tid);
        return TRUE;
    }
    return FALSE;
}

/***************************************************************************************************
*\Function      protocol_anylise_destroy
*\Description   ����comm_dev
*\Parameter     comm_dev
*\Return        bool
*\Note          
*\Log           2015.07.13    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
bool protocol_anylise_destroy(comm_dev_type* comm_dev)
{
    if (comm_dev == RT_NULL)
    {
        protocol_printf("comm_dev is NULL\n");
        return FALSE;
    }

    if (comm_dev->tid != RT_NULL)
    {
        rt_thread_delete(comm_dev->tid);
    }
    
    if (comm_dev->buf != RT_NULL)
    {
        rt_free(comm_dev->buf);
    }

    rt_free(comm_dev);

    return TRUE;
}
