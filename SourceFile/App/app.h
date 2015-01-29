/*
********************************************************************************
********************************************************************************
*�ļ���     : app.h
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/13       ��ʼ�汾
********************************************************************************
*/


#ifndef __APP_H
#define __APP_H

/*******************************************************************************
* �ṹ����	: AppStruct
* ����	    : msOS������Ҫ��һ���ṹ�壬��װ��App����������ݼ����ڿؼ�
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/11       ��ʼ�汾
*******************************************************************************/
typedef unsigned short T_U16;
typedef unsigned short u16;
typedef unsigned char T_U8;
typedef unsigned char u8;
typedef unsigned char BOOL;
typedef unsigned char UINT8,u8;
typedef unsigned short UINT16;
typedef unsigned int UINT32,u32;
#define 	APP_SETUP_ADD 		0*1024  //ϵͳ������ŵ�flashλ��1
#define 	APP_SETUP_ADD2 		4*1024  //ϵͳ������ŵ�flashλ��2
#define 	APP_SETUP_ADD3 		8*1024  //ϵͳ������ŵ�flashλ��3

#define 	FLOOR_ADD 		12*1024  //¥�����ݴ�ŵ�flashλ��

#define 	CALL_ADD 		28*1024  //¥�����ݴ�ŵ�flashλ��

#define 	FLOOR_CUR_ADD 	32*1024  //¥�����ݴ�ŵ�flashλ��

#define 	PWD_MAP_ADD 	36*1024  //�����ŵ�ַ

#define   MAX_FRAM_LEN    32   /*��֡���ȴ��ڴ�ֵ������Ϊ��Ч����������֡ͷ*/
#define     MAX_FLOOR_NUM   50//��ʱ����¥�����߶�

typedef struct{
        u8  floor_flag;//Ϊ1ʱ��ʾ��ʹ�ã�Ϊ0ʱ��ʾ��ֵ������
        u8  floor_num;//��ֵ¥��
        u16	floor_diff;//¥��֮��Ĳ�ֵ����¥�㵽��һ��֮���ֵ������һ���궨
        u32     floor_count;//¥���Ӧ�ı���������ֵ
}Floor_Data;

typedef struct{
        u8  Call_flag;//Ϊ1ʱ��ʾ��ʹ�ã�Ϊ0ʱ��ʾ��ֵ������
        u8  Call_num;//��ֵ¥��
        u32     Call_Time_Count;//��Ӧ�ڽ���ʱ��¼�Ľ���ʱ�䣻Ҳ����Ϊ¥��洢��ֵ
}Call_Data;

typedef struct
{
	u16 	wLicCheckSum;					/*�����(�������ݵ��ֽں�)*/

	struct Weight
	{
		u32	Rated_weight;//�����
		u32	Warning_weight;//Ԥ������
		u32	Alarm_weight;//��������
		u32  Empty_weight;//��������
		u32	calibrate_sub_value;//����У׼����ֵ��ֵ
		u32	calibrate_ad_sub_value;//����У׼����ֵ��Ӧ��AD���ֵ��ֵ
		u32  weight_clear_ad_value;////����ʱ��������Ӧ��AD���ֵ��ʵ�����������Ӧ��AD���ֵ����ΪֻУ׼ʱҲ�����һ�����ֵ�������ֵ��
		u32  weight_clear_ad_value_sign;////����ʱ����ֵ�������Ŵ�������0Ϊ����1Ϊ����

		
		u32  calibrate_one_suffer;//�궨��һ
		u32  calibrate_one_ad_suffer;//�궨��һ��Ӧ��adֵ
		u32  calibrate_two_suffer;//�궨���
		u32  calibrate_two_ad_suffer;//�궨�����Ӧ��adֵ
		u32  calibrate_suffer;//ֱ��б��*�궨��һ,����

		//float	  calibrate_slope;//ֱ��б��
		u8 	weight_display_change_flg;//�����Ƿ���ʾ�ܿ��Ʊ�־
		
	}Weight;

	u32  Up_buchang;//��������
	u32	Down_buchang;//�½�����
	u32	Other_buchang;//��������//�ɸ����л������ٵ�ƽ�������������ֵ
	//u32	Floor_Count_Max;
	u8	Max_floor;//�������¥��
 
}AppStruct;

extern AppStruct App;
extern AppStruct App_Flash;
extern u32	Target_F;
extern unsigned char fram_len;
extern unsigned char  fram_data_buff[MAX_FRAM_LEN];
extern Floor_Data   floor_tmp[MAX_FLOOR_NUM];

//extern u32 tmpxxx;//���ڼ���
//extern u8 master_pre_value;//ɨ�谴��ֵ��ǰ��
extern signed int Floor_CurrentCount;//���ڼ����������ĵ�ֵ
extern u32	Floor_TargetCount;

extern u8  HB_Start_Flag;
extern u8  HB_RELAY_Flag;
extern u8  HB_RELAY_Close_Flag;


extern u8  HB_Floor_Call_Num;
extern Call_Data    HB_Floor_Call_Buff[3];
extern Call_Data    HB_Floor_Call[MAX_FLOOR_NUM];

extern u16  Voltage_Tmp;//��ѹ������ADֵ
extern u16  Weight_Tmp;//�������Ĳ�׽������ֵ
extern u16  Weight_Value;////ʵ�ʲɼ������������ADֵ

#include "logic.h"

#include "menu.h"

#include 	"Voice.h"
#include "KeyBoard.h"

#include "uiPross.h"
#include "uiLCD_12864.h"
#include "uiFont.h"

#include "password.h"
#include "SFlashApi.h"
#include "Uart.h"

#include "uiLCD_tft.h"
#include "load.h"//������ش����ļ�
#include "Encoder.h"
#include "Call.h"

#endif /*__APP_H*/

