
//主要存放和正交编码器相关的处理代码
#include "system.h"
#include "app.h"


#define MAX_BYTE_LENS		3	



//保存楼层数据结构到对应flash位置里
void	Encode_Save(Floor_Data  *floor_data)
{

	Flash_WriteInfo((u8*)floor_data,FLOOR_ADD+(floor_data->floor_num-1)*sizeof(Floor_Data),sizeof(Floor_Data));

}
//读取对应楼层里的flash值到保存数据结构
void	Encode_Read(u8 floor_num,Floor_Data  *floor_data)
{
	Flash_ReadInfo((u8*)floor_data,FLOOR_ADD+(floor_num-1)*sizeof(Floor_Data),sizeof(Floor_Data));
}



//楼层标定处理函数
u8	Encoder_Demarcate(void)
{
//	char pcRet[MAX_BYTE_LENS];
	u8 nKey;
	Floor_Data  floor_data;
	
	u8 floor_num_tmp = 1;
	u32 encoder_set_2 = 0;
	
	

//	pcRet[0] = 0;
		
	uiLcdClear();
	uiLcd_1212_ch(UISTR_ENCODER_FLOOR,0,4,4);//显示楼层标定

	//uiLcd_1616_ch(6,2,4,1);//显示上翻图标
	uiLcd_1616_ch(7,2,6,1);//显示下翻图标
	
	uiLcd_1414_ch(UISTR_XXX_SAVE, 6, 4,2);
	uiLcdMediumString("OK", 3 ,4,0);


	uiLcd_1414_ch(UISTR_XXX_ESC, 6,76,2);
	uiLcdMediumString("ESC", 3, 13,0);
	
	while(1)
	{
		//调试接口
		uiLcdDecimal(floor_tmp[floor_num_tmp-1].floor_count ,0,9,0,7);//显示对应楼层保存的计数器值
		uiLcdDecimal(floor_tmp[floor_num_tmp-1].floor_diff,2,9,0,7);//显示对应楼层保存的计数器值


		//显示当前编码值
		encoder_set_2 = Floor_CurrentCount;
		uiLcdDecimal(encoder_set_2,1,9,0,7);
 
		uiLcdDecimal(floor_num_tmp,1,3,0,3);//显示楼层数

        		nKey = uiKeyGetKey();
		if(nKey == UIKEY_ESC)
			break;
		
		if(nKey == UIKEY_OK)//当按下OK键时，将对应楼层保存并发送给控制端更新
		{
			if(floor_num_tmp ==1)
			{
				floor_data.floor_flag = 1;//标志位使用了
				floor_data.floor_num = floor_num_tmp;//楼层设置正确
				floor_data.floor_diff = 0;
				floor_data.floor_count = encoder_set_2;//楼层对应的编码值

			}
			else
			{
				floor_data.floor_flag = 1;//标志位使用了
				floor_data.floor_num = floor_num_tmp;//楼层设置正确
				floor_data.floor_diff = encoder_set_2-floor_tmp[floor_num_tmp-2].floor_count;
				floor_data.floor_count = encoder_set_2;//楼层对应的编码值

			}

			//如果当前值小于前一楼层或大于后一楼层，标定无效

	
			Encode_Save(&floor_data);
			
			Encode_Read(floor_num_tmp,&floor_tmp[floor_num_tmp-1]);
	
			//提示保存正确
			uiLcd_1414_ch(UISTR_XXX_SAVE, 4,14*2+4,2);
			uiLcd_1414_ch(UISTR_XXX_SUCESS, 4,14*4+4,2);
			DelayMs(1000);
			uiLcdLineErase8x16(2,0,15,0);
			
		}

		if (nKey == UIKEY_NONE)
			continue;
		
		if(nKey == UIKEY_MENU)//删除
		{
			
		}
		if(nKey == UIKEY_DOWN)//下翻数字
		{
			floor_num_tmp--;
		}
		if(nKey == UIKEY_UP)//上翻数字
		{
			floor_num_tmp++;
		}

		if(floor_num_tmp < 1)
		{
			floor_num_tmp =App.Max_floor;
		}
	 	if(floor_num_tmp > App.Max_floor)
		{
			floor_num_tmp = 1;
		}

		if (uiKeyIsDigit(nKey))
		{
			
		}
		//System.Device.IO.SetBeep(0);
	
	}
	return FALSE;

}


//楼层标定初始化处理函数
u8	Encoder_Demarcate_Init(void)
{

	u8 nKey;
	u8 floor_num_tmp;
	Floor_Data  floor_data;
//	u32 encoder_set_2 = 0;
	uiLcdClear();


	//是否进行自动平层标定?
	uiLcd_1414_ch(UISTR_XXX_SHIFOU, 0, 35,4);
	uiLcd_1414_ch(UISTR_XXX_ZIDONG, 2, 20,2);
	uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UI, 2, 20+14*2,4);
	uiLcdMediumString("?", 1 ,13,0);

	//请务必停至一楼
	uiLcdMediumString("*", 2 ,0,0);
	uiLcd_1414_ch(UISTR_XXX_TINGZHIYILOU, 4, 14,8);

	
	uiLcd_1414_ch(UISTR_XXX_SHIFOU, 6, 4+6,1);
	uiLcdMediumString("OK", 3 ,3,0);

	uiLcd_1414_ch(UISTR_XXX_SHIFOU+1, 6,76+6,1);
	uiLcdMediumString("ESC", 3, 12,0);

	//是否进行初始化
	
	while(1)
	{
		nKey = uiKeyGetKey();
		if(nKey == UIKEY_ESC)
		{
			return 0;
		}
		else if(nKey == UIKEY_OK)
		{
			
			break;
		}
		else
		{

		}
	}

	uiLcdClear();//清屏
	//楼层
	uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_MAX_FLOOR+2, 0,15,2);

	//标定
	uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UI+2, 0,76+14,2);

	//楼标定成功
	//uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_MAX_FLOOR+2,4,60,1);
	uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UI+2,4,4,2);
	uiLcd_1414_ch(UISTR_XXX_CHENGGONG,4,4+14*2,2);
	
	//初始化当前楼层
	Floor_CurrentCount = 20000;
	//保存
	Flash_WriteInfo((u8*)&Floor_CurrentCount,FLOOR_CUR_ADD,4);

	for(floor_num_tmp=0;floor_num_tmp<App.Max_floor;floor_num_tmp++)
	{
		if(floor_num_tmp==0)
		{
			floor_data.floor_flag = 1;//标志位使用了
			floor_data.floor_num = floor_num_tmp+1;//楼层设置正确
			floor_data.floor_diff = 0;//楼层之间的差值
			floor_data.floor_count = Floor_CurrentCount;//楼层对应的编码值

		}
		else
		{
			floor_data.floor_flag = 1;//标志位使用了
			floor_data.floor_num = floor_num_tmp+1;//楼层设置正确
			floor_data.floor_diff = floor_tmp[floor_num_tmp].floor_diff;//楼层之间的差值
			//楼层对应的编码值=前一层的编码值加上楼层差值
			floor_data.floor_count = floor_tmp[floor_num_tmp-1].floor_count + floor_tmp[floor_num_tmp].floor_diff;

		}

		Encode_Save(&floor_data);
		DelayMs(100);
		Encode_Read(floor_data.floor_num,&floor_tmp[floor_data.floor_num-1]);


		uiLcdDecimal(floor_data.floor_num,1,2,0,3);//显示楼层数
		uiLcdDecimal(floor_tmp[floor_data.floor_num -1].floor_count ,1,9,0,7);//显示对应楼层保存的计数器值

		//显示当前楼层更新完成
		//uiLcdDecimal(floor_data.floor_num,2,5,0,2);//显示楼层数
		
		DelayMs(1000);
		
	}

	//提示全部更新完成
	uiLcdClear();
	//自动平层成功  请退出
	uiLcd_1414_ch(UISTR_XXX_ZIDONG, 2, 20,2);
	uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UI, 2, 20+14*2,2);
	uiLcd_1414_ch(UISTR_XXX_CHENGGONG,2,20+14*4,2);

	uiLcd_1414_ch(UISTR_XXX_TINGZHIYILOU, 4, 40,1);
	uiLcd_1414_ch(UISTR_XXX_ESC, 4, 40+14*1,2);
	
	uiLcd_1414_ch(UISTR_XXX_ESC, 6,76,2);
	uiLcdMediumString("ESC", 3, 13,0);
	while(1)
	{
        		nKey = uiKeyGetKey();
		if(nKey == UIKEY_ESC)
			break;
	}
	return TRUE;

}







