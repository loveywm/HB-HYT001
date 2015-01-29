/*
********************************************************************************
********************************************************************************
*文件名     : app.h
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/13       初始版本
********************************************************************************
*/


#ifndef __APP_H
#define __APP_H

/*******************************************************************************
* 结构体名	: AppStruct
* 描述	    : msOS中最重要的一个结构体，封装了App层的所有数据及窗口控件
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
typedef unsigned short T_U16;
typedef unsigned short u16;
typedef unsigned char T_U8;
typedef unsigned char u8;
typedef unsigned char BOOL;
typedef unsigned char UINT8,u8;
typedef unsigned short UINT16;
typedef unsigned int UINT32,u32;
#define 	APP_SETUP_ADD 		0*1024  //系统参数存放的flash位置1
#define 	APP_SETUP_ADD2 		4*1024  //系统参数存放的flash位置2
#define 	APP_SETUP_ADD3 		8*1024  //系统参数存放的flash位置3

#define 	FLOOR_ADD 		12*1024  //楼层数据存放的flash位置

#define 	CALL_ADD 		28*1024  //楼层数据存放的flash位置

#define 	FLOOR_CUR_ADD 	32*1024  //楼层数据存放的flash位置

#define 	PWD_MAP_ADD 	36*1024  //密码存放地址

#define   MAX_FRAM_LEN    32   /*若帧长度大于此值，则认为无效，重新搜索帧头*/
#define     MAX_FLOOR_NUM   50//此时定义楼层最大高度

typedef struct{
        u8  floor_flag;//为1时表示在使用，为0时表示数值有问题
        u8  floor_num;//数值楼层
        u16	floor_diff;//楼层之间的差值，低楼层到上一层之间的值，用于一键标定
        u32     floor_count;//楼层对应的编码器计数值
}Floor_Data;

typedef struct{
        u8  Call_flag;//为1时表示在使用，为0时表示数值有问题
        u8  Call_num;//数值楼层
        u32     Call_Time_Count;//对应于接受时记录的接受时间；也复用为楼层存储的值
}Call_Data;

typedef struct
{
	u16 	wLicCheckSum;					/*检验和(后面数据的字节和)*/

	struct Weight
	{
		u32	Rated_weight;//额定重量
		u32	Warning_weight;//预警重量
		u32	Alarm_weight;//报警重量
		u32  Empty_weight;//空载重量
		u32	calibrate_sub_value;//两个校准重量值差值
		u32	calibrate_ad_sub_value;//两个校准重量值对应的AD检测值差值
		u32  weight_clear_ad_value;////清零时重量所对应的AD检测值；实际是零点所对应的AD检测值，因为只校准时也会计算一个零点值替代清零值；
		u32  weight_clear_ad_value_sign;////清零时重量值的正负号存贮区，0为正，1为负；

		
		u32  calibrate_one_suffer;//标定点一
		u32  calibrate_one_ad_suffer;//标定点一对应的ad值
		u32  calibrate_two_suffer;//标定点二
		u32  calibrate_two_ad_suffer;//标定点二对应的ad值
		u32  calibrate_suffer;//直线斜率*标定点一,待定

		//float	  calibrate_slope;//直线斜率
		u8 	weight_display_change_flg;//重量是否显示受控制标志
		
	}Weight;

	u32  Up_buchang;//上升补偿
	u32	Down_buchang;//下降补偿
	u32	Other_buchang;//其他补偿//由高速切换到低速的平层编码器计数器值
	//u32	Floor_Count_Max;
	u8	Max_floor;//设置最高楼层
 
}AppStruct;

extern AppStruct App;
extern AppStruct App_Flash;
extern u32	Target_F;
extern unsigned char fram_len;
extern unsigned char  fram_data_buff[MAX_FRAM_LEN];
extern Floor_Data   floor_tmp[MAX_FLOOR_NUM];

//extern u32 tmpxxx;//用于计数
//extern u8 master_pre_value;//扫描按键值的前面
extern signed int Floor_CurrentCount;//用于计数编码器的的值
extern u32	Floor_TargetCount;

extern u8  HB_Start_Flag;
extern u8  HB_RELAY_Flag;
extern u8  HB_RELAY_Close_Flag;


extern u8  HB_Floor_Call_Num;
extern Call_Data    HB_Floor_Call_Buff[3];
extern Call_Data    HB_Floor_Call[MAX_FLOOR_NUM];

extern u16  Voltage_Tmp;//电压采样的AD值
extern u16  Weight_Tmp;//传感器的捕捉的重量值
extern u16  Weight_Value;////实际采集到的最后重量AD值

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
#include "load.h"//载重相关代码文件
#include "Encoder.h"
#include "Call.h"

#endif /*__APP_H*/

