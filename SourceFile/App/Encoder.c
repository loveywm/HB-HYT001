
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
	u8 nKey;
	Floor_Data  floor_data;
	u16 floor_num_tmp = 1;
	u32 encoder_set_2 = 0;
		
	uiLcdClear();
	uiLcd_1212_ch(UISTR_ENCODER_FLOOR,0,4,4);//显示楼层标定

	//uiLcd_1616_ch(6,2,4,1);//显示上翻图标
	uiLcd_1616_ch(7,2,6,1);//显示下翻图标
	
	uiLcd_1414_ch(UISTR_XXX_SAVE, 6, 4,2);
	uiLcdMediumString("OK", 3 ,4,0);


	uiLcd_1414_ch(UISTR_XXX_ESC, 6,76,2);
	uiLcdMediumString("ESC", 3, 13,0);

	uiLcdDecimal(1,1,3,0,2);//显示楼层数
	
	while(1)
	{
		if(floor_num_tmp < MAX_FLOOR_NUM && floor_num_tmp>0)
		{
			//调试接口
			uiLcdDecimal(floor_tmp[floor_num_tmp-1].floor_count ,0,9,0,7);//显示对应楼层保存的计数器值
			//LcdDecimal(floor_tmp[floor_num_tmp-1].floor_diff,2,9,0,7);//显示对应楼层保存的计数器值
		}

		//显示当前编码值
		encoder_set_2 = Floor_CurrentCount;
		uiLcdDecimal(encoder_set_2,1,9,0,7);

        		nKey = uiKeyGetKey();
				
		if (nKey == UIKEY_NONE)
			continue;
		if(nKey == UIKEY_ESC)
			break;
		
		if(nKey == UIKEY_OK)//当按下OK键时，将对应楼层保存并发送给控制端更新
		{
			if(floor_num_tmp < 1)
			{	
				continue;	
			}
			else if(floor_num_tmp > App.Max_floor)
			{
				//界面提示错误值"标定楼层过大"

				uiLcd_1212_ch(UISTR_ENCODER_FLOOR,4,42,4);
				//uiLcd_1212_ch(UISTR_AIM_LOU_CHENG+3,4,66+12*2,1);
				uiLcd_1212_ch(UISTR_UI_GUODA,4,42+12*4,2);
				DelayMs(1500);
				uiLcdLineErase8x16(2,2,17,0);//将先前状态擦除
				floor_num_tmp = 0;
				
			}
			else
			{
				floor_data.floor_flag = 1;//标志位使用了
				floor_data.floor_num = floor_num_tmp;//楼层设置正确
				floor_data.floor_count = encoder_set_2;//楼层对应的编码值
				if(floor_num_tmp ==1)
				{
					floor_data.floor_diff = 0;
					if(floor_tmp[1].floor_flag == 1)
					{
						floor_data.floor_diff = 0;
					}
				}
				else if(floor_num_tmp == 2)//标定第二层时，差值是第二层和第一层差值
				{
					floor_data.floor_diff = encoder_set_2-floor_tmp[0].floor_count;
				}
				else
				{
					
					//floor_data.floor_diff = encoder_set_2-floor_tmp[floor_num_tmp-2].floor_count;
					//差值为当前楼层到第二层的差值
					floor_data.floor_diff = encoder_set_2-floor_tmp[1].floor_count;


					//如果当前值小于前一楼层或大于后一楼层，标定无效
					//if(floor_data.floor_count <floor_tmp[floor_num_tmp-2].floor_count)
					//{
						//continue;
					//}

				}

				Encode_Save(&floor_data);
				Encode_Read(floor_num_tmp,&floor_tmp[floor_num_tmp-1]);
		
				//提示保存正确
				uiLcd_1414_ch(UISTR_XXX_SAVE, 4,14*2+4,2);
				uiLcd_1414_ch(UISTR_XXX_SUCESS, 4,14*4+4,2);
				DelayMs(1000);
				uiLcdLineErase8x16(2,0,15,0);
			}
			
		}
		if (uiKeyIsDigit(nKey))
		{
			floor_num_tmp = floor_num_tmp * 10 + (int)nKey - '0';
			if(floor_num_tmp > App.Max_floor)
			{
				floor_num_tmp =  ((floor_num_tmp%100)/10)*10 +(nKey - '0');
			}
			if(floor_num_tmp < 1)
			{
				floor_num_tmp = 0;
			}
		}
		if(nKey == UIKEY_DOWN)//下翻数字
		{
			if(floor_num_tmp == 0)
			{
				
			}else
			{
				floor_num_tmp--;
			}
		}
		if(nKey == UIKEY_UP)//上翻数字
		{
			floor_num_tmp++;
			if(floor_num_tmp > App.Max_floor)
			{
				floor_num_tmp =  App.Max_floor;
			}
		}
		uiLcdDecimal(floor_num_tmp,1,3,0,2);//显示楼层数
		
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
	//uiLcd_1414_ch(UISTR_XXX_TINGZHIYILOU, 4, 14,8);
	uiLcd_1414_ch(UISTR_XXX_TINGZHIYILOU, 4, 14,5);
	uiLcd_1414_ch(UISTR_XXX_ZAIZHONG_SET_BIAODINGDIAN_TWO+5, 4, 14+14*5,1);
	uiLcd_1414_ch(UISTR_XXX_TINGZHIYILOU+6, 4, 14+14*6,2);

	
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
			//floor_data.floor_flag = 1;//标志位使用了
			//floor_data.floor_num = floor_num_tmp+1;//楼层设置正确
			//floor_data.floor_diff = 0;//楼层之间的差值
			//floor_data.floor_count = Floor_CurrentCount;//楼层对应的编码值


			//第一层等于第二层减去两者差值
			floor_data.floor_flag = 1;//标志位使用了
			floor_data.floor_num = floor_num_tmp+1;//楼层设置正确
			floor_data.floor_diff = 0;//楼层之间的差值
			floor_data.floor_count = Floor_CurrentCount-floor_tmp[1].floor_diff;//楼层对应的编码值

		}
		else if(floor_num_tmp==1)
		{
			floor_data.floor_flag = 1;//标志位使用了
			floor_data.floor_num = floor_num_tmp+1;//楼层设置正确
			floor_data.floor_diff = floor_tmp[1].floor_diff;//楼层之间的差值
			floor_data.floor_count = Floor_CurrentCount;
		}
		else
		{
			if(floor_tmp[floor_num_tmp].floor_diff ==0)//如果楼层编码差值未改变，那说明此层为用
			{
				floor_data.floor_flag = 0;//标志位使用了
				floor_data.floor_num = floor_num_tmp+1;//楼层设置正确
				floor_data.floor_diff = 0;//楼层之间的差值
				//楼层对应的编码值=前一层的编码值加上楼层差值
				floor_data.floor_count =0;
			}else
			{
				floor_data.floor_flag = 1;//标志位使用了
				floor_data.floor_num = floor_num_tmp+1;//楼层设置正确
				floor_data.floor_diff = floor_tmp[floor_num_tmp].floor_diff;//楼层之间的差值
				//楼层对应的编码值=前一层的编码值加上楼层差值
				floor_data.floor_count =  floor_tmp[1].floor_count + floor_tmp[floor_num_tmp].floor_diff;
			}
			
			
			

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




//供主界面调用平层
u8	Auto_Encoder_Demarcate(void)
{
	u8 nKey;
	//u8 floor_num_tmp = Target_F;
	u8 floor_num_tmp = 0;
	//Floor_Data  floor_data;
	u32 encoder_count= 0;
	u32 encoder_diff= 0;
	//u8 	encoder_flag=0;
	u8 ywm=0;
	
	uiLcdClear();
	
	uiLcd_1414_ch(UISTR_XXX_SHIFOU, 6, 4+6,1);
	uiLcdMediumString("OK", 3 ,3,0);

	uiLcd_1414_ch(UISTR_XXX_SHIFOU+1, 6,76+6,1);
	uiLcdMediumString("ESC", 3, 12,0);

	//是否进行自动补偿?
	uiLcd_1414_ch(UISTR_XXX_SHIFOU, 4, 35,2);
	uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UP_BUCHANG+2, 4, 35+14*2,2);
	uiLcdMediumString("?", 2 ,13,0);

	uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UI+2, 0, 4,2);
	uiLcd_1414_ch(UISTR_XXX_CHAOZHI+1, 0, 4+14*2,1);

	//计算当前楼层
	for(ywm=0;ywm<App.Max_floor-1;ywm++)
	{
		if(Floor_CurrentCount < (floor_tmp[ywm].floor_count+(floor_tmp[1].floor_diff/2)))
		{
			//uiLcdDecimal(ywm+1,0+2,3,0,2);
			floor_num_tmp = ywm+1;
			break;
		}
	}

	while(1)
	{
		encoder_count = Floor_CurrentCount;

		//对应平层楼层保存的值
		uiLcdDecimal(floor_tmp[floor_num_tmp-1].floor_count ,0,9,0,7);//显示对应楼层保存的计数器值
		//uiLcdDecimal(floor_tmp[floor_num_tmp-1].floor_diff,2,9,0,7);//显示对应楼层保存的差值

		//显示当前平层值
		uiLcdDecimal(encoder_count,1,9,0,7);

		nKey = uiKeyGetKey();
		if(nKey == UIKEY_ESC)
		{
			break;
		}
		else if(nKey == UIKEY_OK)
		{
			if(encoder_count >floor_tmp[floor_num_tmp-1].floor_count )//上升
			{
				encoder_diff = encoder_count - floor_tmp[floor_num_tmp-1].floor_count+App.Up_buchang;

				if(encoder_diff > 5000)
				{
					uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UP_BUCHANG+2, 2, 4,2);
					uiLcd_1414_ch(UISTR_XXX_CHAOZHI, 2, 4+14*2,2);
					//DelayMs(1000);
					
					continue;
				}
				App.Up_buchang = encoder_diff;
			}
			else if(encoder_count <floor_tmp[floor_num_tmp-1].floor_count)//下降
			{
				encoder_diff =  floor_tmp[floor_num_tmp-1].floor_count -encoder_count +App.Down_buchang;
				if(encoder_diff > 5000)
				{
					uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UP_BUCHANG+2, 2, 4,2);
					uiLcd_1414_ch(UISTR_XXX_CHAOZHI, 2, 4+14*2,2);
					continue;
				}
				
				App.Down_buchang= encoder_diff;
			}
			else
			{
				encoder_diff  = 0;
				
			}
			//保存系统参数
			SaveSystemInfo();
			break;
	
		}
		else
		{

		}
	}

	uiLcdClear();//清屏
	return 0;
}

//楼层标定全部删除
u8	Encoder_Demarcate_Del(void)
{

	u8 nKey;
	u8 floor_num_tmp;
	Floor_Data  floor_data;
	
	uiLcdClear();


	//是否进行自动平层标定?
	uiLcd_1414_ch(UISTR_XXX_SHIFOU, 2, 20,2);
	uiLcd_1414_ch(UISTR_XXX_QUANBU, 2, 20+14*2,2);
	uiLcd_1414_ch(UISTR_XXX_SHANCHU, 2, 20+14*4,2);
	uiLcdMediumString("?", 1 ,13,0);

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

	for(floor_num_tmp=0;floor_num_tmp<App.Max_floor;floor_num_tmp++)
	{
		
		floor_data.floor_flag = 0;//标志位使用了
		floor_data.floor_num = floor_num_tmp+1;//楼层设置正确
		floor_data.floor_diff = 0;//楼层之间的差值
		floor_data.floor_count = 0;//楼层对应的编码值

		Encode_Save(&floor_data);
		DelayMs(100);
		Encode_Read(floor_data.floor_num,&floor_tmp[floor_data.floor_num-1]);

		uiLcdDecimal(floor_data.floor_num,1,2,0,3);//显示楼层数
		uiLcdDecimal(floor_tmp[floor_data.floor_num -1].floor_count ,1,9,0,7);//显示对应楼层保存的计数器值
		
		DelayMs(1000);
		
	}

	//提示全部更新完成
	uiLcdClear();

	//删除成功
	//uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_MAX_FLOOR+2,4,60,1);
	uiLcd_1414_ch(UISTR_XXX_SHANCHU,2,30,2);
	uiLcd_1414_ch(UISTR_XXX_CHENGGONG,2,30+14*2,2);


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




