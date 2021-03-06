#include "system.h"
#include "app.h"


#define MAX_BYTE_LENS		3	



void	Call_Save(Call_Data  *call_data)
{

	Flash_WriteInfo((u8*)call_data,CALL_ADD+(call_data->Call_num-1)*sizeof(Call_Data),sizeof(Call_Data));

}
void	Call_Read(u8 call_num,Call_Data  *call_data)
{

	Flash_ReadInfo((u8*)call_data,CALL_ADD+(call_num-1)*sizeof(Call_Data),sizeof(Call_Data));

}
//楼层呼叫器的删除处理函数
u8 Call_Del(void)
{
	u8 nKey;
	u16 floor_num_tmp = 1;
	u32 Call_num_set_tmp= 0;
	//u32 Call_num_set= 0;

	uiLcdClear();
	uiLcd_1212_ch(UISTR_INPUT_PASSWORD,0,4,3);//请输入
	uiLcd_1212_ch(UISTR_ENCODER_FLOOR+2,0,4+36,2);//楼层

	uiLcd_1212_ch(UISTR_CALL_SET_FENJI,0,4+12*6,2);//分机
	uiLcd_1212_ch(UISTR_ZAIZHONG_DAODAYUJINGZHI+4,0,4+12*8,1);//值

	//uiLcd_1616_ch(6,2,4,1);//显示上翻图标
	uiLcd_1616_ch(7,2,6,1);//显示下翻图标
	
	uiLcd_1414_ch(UISTR_XXX_SHANCHU, 6, 4,2);
	uiLcdMediumString("OK", 3 ,4,0);


	uiLcd_1414_ch(UISTR_XXX_ESC, 6,76,2);
	uiLcdMediumString("ESC", 3, 13,0);

	
	uiLcdDecimal(1,1,3,0,2);//显示楼层数
	
	while(1)
	{
		if(floor_num_tmp < MAX_FLOOR_NUM && floor_num_tmp>0)
		{
			Call_num_set_tmp = HB_Floor_Call[floor_num_tmp-1].Call_Time_Count;
			uiLcdDecimal(Call_num_set_tmp,1, 2+6,0,8);
		}
		
        		nKey = uiKeyGetKey();
		if(nKey == UIKEY_ESC)
		{
			break;
		}	
		
		if(nKey == UIKEY_OK)//当按下OK键时，将对应楼层保存并发送给控制端更新
		{

			Call_Data  call_data;

			if((floor_num_tmp == 0) || (floor_num_tmp > MAX_FLOOR_NUM))
			{
				continue;
			}
			
			call_data.Call_flag =0;//标志位使用了
			call_data.Call_num = floor_num_tmp;//楼层设置正确
			call_data.Call_Time_Count = 0;//楼层对应无线编码的编码值

	
			Call_Save(&call_data);
			
			Flash_ReadInfo((UINT8 *)HB_Floor_Call, CALL_ADD, sizeof(Call_Data)*MAX_FLOOR_NUM);
			
			
			//提示删除成功
			uiLcd_1414_ch(UISTR_XXX_SHANCHU, 4,14*2+4,2);
			uiLcd_1414_ch(UISTR_XXX_SUCESS, 4,14*4+4,2);
			DelayMs(1000);
			uiLcdLineErase8x16(2,0,15,0);
			
		}

		if (nKey == UIKEY_NONE)
			continue;
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

/*			for(ywm_tmp=0;ywm_tmp<MAX_FLOOR_NUM;ywm_tmp++)
			{
				if(Call_num_set == HB_Floor_Call[ywm_tmp].Call_Time_Count)
				{
					//分机已学习
					uiLcd_1212_ch(UISTR_CALL_SET_FENJI, 4,12*4+4,2);
					uiLcd_1212_ch(UISTR_START, 4,12*6+4,1);
					uiLcd_1212_ch(UISTR_CALL_SET, 4,12*7+4,2);
					DelayMs(1000);
					uiLcdLineErase8x16(2,0,15,0);

					//Call_num_set = 0;
					
					break;
				}

			}
*/			


//楼层呼叫器的学习处理函数
u8 Call_Learn(void)
{
	u8 nKey;
	u8 ywm_tmp;
	u16 floor_num_tmp = 0;
	u32 Call_num_set_tmp= 0;
	u32 Call_num_set= 0;
	u32 Call_num_set2= 0;
	u32 Call_num_set3= 0;
	u32 Call_Time;

	uiLcdClear();
	uiLcd_1212_ch(UISTR_CALL_SET,0,4,2);//学习
	uiLcd_1212_ch(UISTR_ENCODER_FLOOR+2,0,4+24,2);//楼层

	uiLcd_1212_ch(UISTR_CALL_SET,0,4+12*6,2);//学习
	uiLcd_1212_ch(UISTR_ZAIZHONG_DAODAYUJINGZHI+4,0,4+12*8,1);//值

	//uiLcd_1616_ch(6,2,4,1);//显示上翻图标
	uiLcd_1616_ch(7,2,6,1);//显示下翻图标
	
	uiLcd_1414_ch(UISTR_XXX_SAVE, 6, 4,2);
	uiLcdMediumString("OK", 3 ,4,0);

	uiLcd_1414_ch(UISTR_XXX_ESC, 6,76,2);
	uiLcdMediumString("ESC", 3, 13,0);

	uiLcdDecimal(0,1,3,0,2);//显示楼层数
	Call_Time =uiTimeGetTickCount();
	while(1)
	{
		if((floor_num_tmp >0) && (floor_num_tmp < MAX_FLOOR_NUM))
		{
			Call_num_set_tmp = System.Device.Call_Floor.Remote_Scan();
			if((Call_num_set_tmp != 0))//&&(Call_num_set_tmp < 0x01000000) &&((Call_num_set_tmp&0xf)==0xf)&&((Call_num_set_tmp&0xffffff)!= 0xffffff))
			{
				uiLcdDecimal(Call_num_set_tmp,1, 2+6,0,8);
				Call_num_set=Call_num_set_tmp;
				
				DelayMs(800);
					
				Call_num_set_tmp = System.Device.Call_Floor.Remote_Scan();
				if((Call_num_set_tmp != 0))//&&(Call_num_set_tmp < 0x01000000) &&((Call_num_set_tmp&0xf)==0xf)&&((Call_num_set_tmp&0xffffff)!= 0xffffff))
				{
					uiLcdDecimal(Call_num_set_tmp,2, 2+6,0,8);
					Call_num_set2=Call_num_set_tmp;

					if(Call_num_set != Call_num_set2)
					{
						continue;
					}

					DelayMs(800);
					
					Call_num_set_tmp = System.Device.Call_Floor.Remote_Scan();
					if((Call_num_set_tmp != 0))//&&(Call_num_set_tmp < 0x01000000) &&((Call_num_set_tmp&0xf)==0xf)&&((Call_num_set_tmp&0xffffff)!= 0xffffff))
					{
						//uiLcdDecimal(Call_num_set_tmp,2, 2+6,0,8);
						Call_num_set=Call_num_set_tmp;

						if(Call_num_set != Call_num_set2)
						{
							continue;
						}
						Call_num_set3=Call_num_set;
						Voice_Call(floor_num_tmp);
					}
				}

			}
		}

        		nKey = uiKeyGetKey();
		if (nKey == UIKEY_NONE)
			continue;
		if(nKey == UIKEY_ESC)
		{
			break;
		}	
		
		if(nKey == UIKEY_OK)//当按下OK键时，将对应楼层保存并发送给控制端更新
		{
			Call_Data  call_data;
			if((floor_num_tmp == 0) || (floor_num_tmp > MAX_FLOOR_NUM))
			{
				continue;
			}
			//删除之前学习过的
			for(ywm_tmp=0;ywm_tmp<MAX_FLOOR_NUM;ywm_tmp++)
			{
				if(Call_num_set3 == HB_Floor_Call[ywm_tmp].Call_Time_Count)
				{
					call_data.Call_flag = 0;//标志位置0
					call_data.Call_num = ywm_tmp+1;//楼层设置正确
					call_data.Call_Time_Count = 0;//楼层对应无线编码的编码�
					Call_Save(&call_data);
					HB_Floor_Call[ywm_tmp].Call_flag=0;
					HB_Floor_Call[ywm_tmp].Call_Time_Count =0;
					//Flash_ReadInfo((UINT8 *)HB_Floor_Call, CALL_ADD, sizeof(Call_Data)*MAX_FLOOR_NUM);
					break;
				}

			}		
			
			call_data.Call_flag = 1;//标志位使用了
			call_data.Call_num = floor_num_tmp;//楼层设置正确
			call_data.Call_Time_Count = Call_num_set3;//楼层对应无线编码的编码值

			Call_Save(&call_data);
			Flash_ReadInfo((UINT8 *)HB_Floor_Call, CALL_ADD, sizeof(Call_Data)*MAX_FLOOR_NUM);
			
			//提示保存正确
			uiLcd_1414_ch(UISTR_XXX_SAVE, 4,4,2);
			uiLcd_1414_ch(UISTR_XXX_SUCESS, 4,4+14*2,2);
			DelayMs(1000);
			uiLcdLineErase8x16(2,0,16,0);
			
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






