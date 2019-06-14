/***************************************************************************************************
*                    (c) Copyright 1992-2015 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          iap_if.h
*\Description   
*\Note          
*\Log           2016.06.28    Ver 1.0    �Ų�
*               �����ļ���
***************************************************************************************************/
#ifndef _IAP_IF_H
#define _IAP_IF_H
#include "stm32f1xx.h"
#include "public_type.h"
extern char* IAPoE_Name;//IAP�̼�����
extern char* IAPoE_Ver;//IAP�̼��汾
extern char* IAPoE_Date;//IAP�̼���������

extern uint64_t IAP_FLAG;//iapģʽ��־


#define FLASH_SIZE_ADDR     (0x1FFFF7E0)//flash������ŵ�ַ
#define FLASH_START_ADDR    (0x08000000)//flash��ʼ��ַ

//�̼�����С
#define OTA_BIN_MAX_SIZE     52* FLASH_PAGE_SIZE//(FLASH_SIZE - 64 * FLASH_PAGE_SIZE)
#define OTA_RUN_APP          ((uint64_t)0xFFFFFFFFFFFFFFFF)
#define OTA_Update_APP       ((uint64_t)0xFFFF5678FFFF5678)
#define OTA_Force_Update_APP ((uint64_t)0x1234567812345678)

//��������ַ
#define FLASH_APP_RUN_ADDR  (0x0800A000)

//IAP��־��ַ
#define FLASH_IAP_FLAG_ADDR  1//(FLASH_BASE + FLASH_SIZE - FLASH_PAGE_SIZE)
//IAP �豸�����洢��ַ
#define FLASH_IAP_PARAM_ADDR  2//(FLASH_BASE + FLASH_SIZE - FLASH_PAGE_SIZE*2)
//APP �豸�����洢��ַ
#define FLASH_APP_PARAM_ADDR  3//(FLASH_BASE + FLASH_SIZE - FLASH_PAGE_SIZE*3)
#define FLASH_APP_INFO_ADDR  4//(FLASH_BASE + FLASH_SIZE)
#define FLASH_APP_FACTORY_ADDR  5//(FLASH_BASE + FLASH_SIZE)
//app�̼��洢��ʼ��ַ
#define FLASH_APP_COPY_ADDR  57//(FLASH_BASE + FLASH_SIZE + FLASH_PAGE_SIZE)δ���� ������ʱ����

uint16_t Get_FlashSize(void);
void RunAppMode(void);
void UpdateAppMode(void);
void ForceUpdateAppMode(void);
uint64_t ReadIAPMode(void);
void Get_ChipID(uint16_t* chipID);
	
#define DEV_FIRST_POWER_FLAG 0x1234
typedef struct iap_param_type_st
{
	uint16_t Power_First_Flag;//��һ���ϵ��־
	uint16_t ChipUniqueID[6];
	uint16_t swversion;//����汾��Ϣ
	uint16_t hwversion;//Ӳ���汾��Ϣ
	uint16_t IAP_flag;//IAP��־ iap 1 app 2
	uint16_t remoteID;//�ⲿͨ���豸ID 
	uint16_t ID;//
}iap_param_type;

typedef struct
{
	uint16_t CompressFlag;//ѹ����־��0δѹ����1ѹ��
	uint32_t UpdatesTotalSize;//�������ܴ�С
	uint16_t UpdatesSubSize;//�������ְ���С
	uint32_t UpdatesCRC32;//�������ļ�У��
	uint32_t ComPackOriginalSize;//ѹ��֮ǰԭʼ�ļ��ܴ�С
	uint16_t ComPackNo;//ѹ��������
	uint16_t PerComPackOriginalSize;//ѹ��֮ǰÿ��ѹ����ԭʼ���ݴ�С
	uint32_t ComPackOriginalCRC32;//lora���õ���λ
}__attribute__((packed)) iap_updata_param_type;
extern iap_param_type iap_param;
void iap_param_save(void);
void iap_param_load(void);
extern void iap_param_default(void);

#endif /*_IAP_IF_H*/

