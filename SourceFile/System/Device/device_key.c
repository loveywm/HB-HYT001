/*
*******************
********************************************************************************
*文件名     : device_key.c
*作用       : 按键设备
*原理       : 基于SystemTick 100/秒 10mS扫描获取按键值，按键值分为长按与短按两种
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
********************************************************************************
*/



#include "drive.h"
#include "system.h"

//loveywm 20140910
#define	Key_Out1_1	GPIO_SetBits(GPIOD,GPIO_Pin_3);
#define	Key_Out2_1	GPIO_SetBits(GPIOD,GPIO_Pin_2);
#define	Key_Out3_1	GPIO_SetBits(GPIOD,GPIO_Pin_1);
#define	Key_Out4_1	GPIO_SetBits(GPIOD,GPIO_Pin_0);

#define	Key_Out1_0	GPIO_ResetBits(GPIOD,GPIO_Pin_3);
#define	Key_Out2_0	GPIO_ResetBits(GPIOD,GPIO_Pin_2);
#define	Key_Out3_0	GPIO_ResetBits(GPIOD,GPIO_Pin_1);
#define	Key_Out4_0	GPIO_ResetBits(GPIOD,GPIO_Pin_0);

#define 	Key_In_1  	GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_7)
#define 	Key_In_2 	GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)
#define 	Key_In_3 	GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_5)
#define 	Key_In_4  	GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_4)


/////////////////////////////////////////////////////////////////////////////
u8 HB_ScanPin(void)
{
    
	u8 scanValue;
	scanValue = UIKEY_NONE;
	

	Key_Out1_1
	Key_Out2_0
	Key_Out3_0
    	Key_Out4_0
    	DelayUs(10);
	if(Key_In_1 == 1)
	{
		scanValue = UIKEY_1;
		return scanValue;
	}
	else if(Key_In_2 == 1)
	{
		scanValue = UIKEY_2;
		return scanValue;
	}
	else if(Key_In_3 == 1)
	{
		scanValue = UIKEY_3;
		return scanValue;
	}
	else if(Key_In_4 == 1)
	{
		scanValue = UIKEY_MENU;
		return scanValue;
	}
	
	
	Key_Out1_0
	Key_Out2_0
	Key_Out3_0
    	Key_Out4_0
    	Key_Out2_1
    	DelayUs(10);
	if(Key_In_1 == 1)
	{
		scanValue = UIKEY_4;
		return scanValue;
	}
	else if(Key_In_2 == 1)
	{
		scanValue = UIKEY_5;
		return scanValue;
	}
	else if(Key_In_3 == 1)
	{
		scanValue = UIKEY_6;
		return scanValue;
	}
	else if(Key_In_4 == 1)
	{
		scanValue = UIKEY_ESC;
		return scanValue;
	}

	//3
	Key_Out1_0
	Key_Out2_0
	Key_Out3_0
    	Key_Out4_0
    	Key_Out3_1
    	DelayUs(10);
	if(Key_In_1 == 1)
	{
		scanValue = UIKEY_7;
		return scanValue;
	}
	else if(Key_In_2 == 1)
	{
		scanValue = UIKEY_8;
		return scanValue;
	}
	else if(Key_In_3 == 1)
	{
		scanValue = UIKEY_9;
		return scanValue;
	}
	else if(Key_In_4 == 1)
	{
		scanValue = UIKEY_UP;
		return scanValue;
	}

	//4
	Key_Out1_0
	Key_Out2_0
	Key_Out3_0
    	Key_Out4_1
    	DelayUs(10);
	if(Key_In_1 == 1)
	{
		scanValue = UIKEY_DEL;
		return scanValue;
	}
	else if(Key_In_2 == 1)
	{
		scanValue = UIKEY_0;
		return scanValue;
	}
	else if(Key_In_3 == 1)
	{
		scanValue = UIKEY_OK;
		return scanValue;
	}
	else if(Key_In_4 == 1)
	{
		scanValue = UIKEY_DOWN;
		return scanValue;
	}
	
	return scanValue;

}



void InitializeKey(void)
{

	//loveywm 20140910
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);

	//GPIOD0~3对应out1~4
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure);
	Key_Out1_0
	Key_Out2_0
	Key_Out3_0
    	Key_Out4_0


	//GPIOD4~7对应in1~4
         GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//下拉输入
 	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//System.Device.HB_Key.uiKeyGetKey = uiKeyGetKey;
	System.Device.HB_Key.HB_ScanPin = HB_ScanPin;

}

