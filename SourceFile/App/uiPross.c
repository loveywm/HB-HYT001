
#include "system.h"
#include "app.h"

#define HEAD_LEN  16	//菜单栏目每行显示预留
#define FLAG_ROW	6
#define FLAG_COL_FOUR	0//四个字时中间显示
#define FLAG_COL_FIVE	44//五个字时显示
#define FLAG_COL_SIX	44//六个字时显示


static u8 fuck_count =0;//连续三次超重就会关闭

extern const T_UI_MENUITEM g_uiMenuItemsSrc[MAX_UI];

T_UI_MENUITEM	g_uiMenuItems[sizeof(g_uiMenuItemsSrc) / sizeof(g_uiMenuItemsSrc[0])];


//用于状态自检显示信息
void Machine_Info(void)
{
	u8 nKey;
	uiLcdClear();
	//显示设备信息
	//uiLcdSmallString("Kg", row*2+1, 19, 0);
	uiLcdMediumString("V1.0.150204", 0, 0, 0);//20150204批次

	uiLcdSmallString("TEL:0579-85232071", 7, 0, 0);//20150204批次

	while(1)
	{
        		nKey = uiKeyGetKey();
		if(nKey == UIKEY_ESC)
			break;
	}

}



u8 uiKeyIsDigit(u8 nKey)
{
	return (UIKEY_0 <= nKey && nKey <= UIKEY_9);
}


////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int uiProcBoxNumber(BOOL *pbRet, int row, int col, int nInitial, int nMin, int nMax, int nMaxLen,
						BOOL bZeroPadding, BOOL b0isNo)
{
	int nRet = nInitial;
	int nPerRet = nInitial;
	int dwPrevTime = uiTimeGetTickCount() - 501;
	BOOL bPrevBlack = FALSE;
	UINT8 nKey;
	BOOL bStartNew = TRUE;
	char pnBuf[16];

	//printf("nInittial==%d\r\n",nInitial);
	
	*pbRet = FALSE;
	while(1)
	{
		if ((uiTimeGetTickCount() - dwPrevTime > 50) ||(nRet != nPerRet))
		//if(nRet != nPerRet)
		{
			//MinMax check
			if (nRet < nMin)
			{
				//uiBuzzerError();
				nRet = nMin;
				bStartNew = TRUE;
			}
			if (nRet > nMax)
			{
				//uiBuzzerError();
				nRet = nMax;
				bStartNew = TRUE;
			}
			sprintf(pnBuf, "%d", nRet);

			//draw
			dwPrevTime = uiTimeGetTickCount();
			bPrevBlack = !bPrevBlack;
			//bPrevBlack = TRUE;
			uiLcdLineErase8x16(row, col, col + nMaxLen, bPrevBlack);

			if (b0isNo && nRet == 0 && col + nMaxLen == 16)
			{
				//uiLcdMultiStringRightAlign(UISTR_NO, row, bPrevBlack);
			}
			else
			{
				if (bZeroPadding)
					uiLcdDecimal(nRet, row, col, bPrevBlack, nMaxLen);
				else
					uiLcdDecimal(nRet, row, col + nMaxLen - strlen(pnBuf), bPrevBlack, 0);
			}

			nPerRet = nRet;
		}
		else
		{
			//uiLcdDecimal(nRet, row, col, 1, nMaxLen);
		}

		nKey = uiKeyGetKey();
		if( nKey == UIKEY_ESC)
		{
			nRet = nInitial;
			break;
		}

		if (nKey == UIKEY_OK)
		{
			*pbRet = TRUE;
			break;
		}

		if (nKey == UIKEY_NONE)
			continue;

		if (nKey == UIKEY_UP)
		{
			nRet++;
			bStartNew = TRUE;
		}

		if (nKey == UIKEY_DOWN)
		{
			nRet--;
			bStartNew = TRUE;
		}

		if (uiKeyIsDigit(nKey))
		{
			if (bStartNew)
			{
				nRet = (int)nKey - '0';
				bStartNew = FALSE;
			}
			else
			{
				nRet = nRet * 10 + (int)nKey - '0';
			}

			sprintf(pnBuf, "%d", nRet);
			if (strlen(pnBuf) > nMaxLen)
			{
				nRet = nMax;
				bStartNew = TRUE;
			}
			
			//printf("nRet==%d\r\n",nRet);
			//printf("nPerRet==%d\r\n",nPerRet);
		}

		dwPrevTime = uiTimeGetTickCount();
		bPrevBlack = FALSE;
	}

	return nRet;
}








/////////////////////////////////////////////////////////////////
//显示读取回来的状态值，如门限，载重等
u8 uiProcShowFlag(u16 flag)
{
//	u8 j;


	uiLcdLineErase8x16(3,5,14,0);//将先前状态擦除
	
	if(flag == Input_invalid)
	{
		//用于显示设备状态	
		uiLcd_1212_ch(UISTR_DEVICE_IS_OK,FLAG_ROW,FLAG_COL_FOUR,4);//设备正常
		return 0;
	}
	//
	if(flag&Input_invalid_1)//进料门未关
	{
		uiLcd_1212_ch(UISTR_FLAG_IN_DOOR,FLAG_ROW,FLAG_COL_FOUR,5);

		
		return 1;
	}
	if(flag&Input_invalid_2)//出料门未关
	{
		
		uiLcd_1212_ch(UISTR_FLAG_OUT_DOOR,FLAG_ROW,FLAG_COL_FOUR,1);
		uiLcd_1212_ch(UISTR_FLAG_IN_DOOR+1,FLAG_ROW,FLAG_COL_FOUR+12,4);
		return 1;
	}
	if(flag&Input_invalid_3)//天窗未关
	{
		uiLcd_1212_ch(UISTR_FLAG_SKY_DOOR,FLAG_ROW,FLAG_COL_FOUR,2);
		uiLcd_1212_ch(UISTR_FLAG_IN_DOOR+3,FLAG_ROW,FLAG_COL_FOUR+24,2);
		return 1;
	}

	
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////

static void uiProcMenuBuild(void)
{
	int i, j=0;

	for(i=0; i<sizeof(g_uiMenuItemsSrc) / sizeof(g_uiMenuItemsSrc[0]); i++)
	{
		g_uiMenuItems[j] = g_uiMenuItemsSrc[i];
		j++;
	}
}
void uiProcMainDraw(BOOL Initialise,Menu_Parameter *Parameter)	/*待机界面的显示*/
{
	int ywm=0;
	int zuidalouceng=0;//存放最大楼层
	if (Initialise)
	{
		uiLcdClear();
		//uiLcdSmallString("whatbook",0,0,0);
		
		//uiLcd_1212_ch(UISTR_HENGBANGKEJI,0,4+12*3,4);
		
		//uiLcd_1212_ch(UISTR_CURRENT_LOU_CHENG,0+2,4,4);//显示“当前楼层”
		//uiLcd_1212_ch(UISTR_CURRENT_LOU_CHENG+3,0+2,4+12*2,1);//显示“当前楼层”
		uiLcd_1414_ch(UISTR_XXX_DANGQIAN, 2, 4, 2);
		uiLcd_1414_ch(UISTR_XXX_HUJIAOLOUCENG+2, 2, 4+14*2, 2);

		
		uiLcd_1212_ch(UISTR_AIM_LOU_CHENG,0+2,76,4);//显示“目标楼层”
		//uiLcd_1212_ch(UISTR_AIM_LOU_CHENG+3,0+2,76+12*2,1);//显示“当前楼层�
		uiLcd_1414_ch(UISTR_XXX_MUBIAO, 2, 70, 2);
		uiLcd_1414_ch(UISTR_XXX_HUJIAOLOUCENG+2, 2, 70+14*2, 2);

		
		uiLcdMediumString("00",0+2,3,0);//显示“当前楼层”数字
		uiLcdMediumString("00",0+2,12,1);//显示“目标楼层”数字
		
		//uiLcd_1212_ch(UISTR_WEIGHT,4,4,3);//显示“吊重：”
		uiLcd_1616_ch(0,2+4,0,2);//显示“重量”
		//uiLcd_1414_ch(UISTR_XXX_ZAIZHONG_SET_ZHONGLIANGZHILING, 4+2, 4, 2);
		uiLcdMediumString(":",1+2,4,0);//冒号
		
		uiLcdMediumString("0.00",1+2,5,0);//这个数值填吊重实时数据
		//uiLcdMediumString("T",1+2,9,0);
		//显示  吨
		uiLcd_1616_ch(11,6,74,1);
		
		uiLcdMediumString("000",3,12,0);//这个数值填吊重百分比
		uiLcdMediumString("%",3,15,0);
		
		//用于显示呼叫楼层
		uiLcd_1414_ch(UISTR_XXX_HUJIAOLOUCENG, 0, 4, 2);
		uiLcd_1414_ch(UISTR_XXX_HUJIAOLOUCENG+3, 0, 4+14*2, 1);

		//uiLcd_1616_ch(8,0,2,3);//显示  呼叫楼层
		uiLcdMediumString(":",0,6,0);//冒号

		//uiLcd_1416_ch(0,0,2,4);
		return ;
	}


	 //显示呼叫的楼层数
	for(ywm=0;ywm<3;ywm++)
	{
		if(uiTimeGetTickCount() - HB_Floor_Call_Buff[ywm].Call_Time_Count < 3000)//3//3s内显示
		{
			if(HB_Floor_Call_Buff[ywm].Call_num != 0)
			{
				uiLcdDecimal(HB_Floor_Call_Buff[ywm].Call_num,0,8+(ywm*3),0,2);
			}
		}
		else
		{
			HB_Floor_Call_Buff[ywm].Call_flag = 0;
			HB_Floor_Call_Buff[ywm].Call_num = 0;
			uiLcdLineErase8x16(0,8+(ywm*3),8+(ywm*3)+3,0);//将先前状态擦除
		}
	}
	
	//显示目标楼层
	//uiLcdDecimal(Target_F,0+2,12,0,2);
	if((Cursor_Flag == 1)&&(Target_F!=0))
	{
		uiLcdDecimal(Target_F,0+2,12,1,2);
	}
	else
	{
		uiLcdDecimal(Target_F,0+2,12,0,2);
	}

	//显示当前楼层
	for(ywm=0;ywm<App.Max_floor;ywm++)
	{
		if(floor_tmp[ywm].floor_flag == 0)
		{
			continue;
		}
		if(Floor_CurrentCount <=  (floor_tmp[ywm].floor_count+(floor_tmp[1].floor_diff/2)))
		{
			uiLcdDecimal(ywm+1,0+2,3,0,2);
			break;
		}
	}
	//假如循环超过最大楼层，说明当前值已经在最大楼层上面了
	//那么就要找到最大楼层，然后显示
	if(ywm == App.Max_floor)
	{
		for(ywm=0;ywm<App.Max_floor;ywm++)
		{
			if(floor_tmp[ywm].floor_flag == 1)
			{
				zuidalouceng = ywm+1;//记录最后有效标记的楼层就是最大层
			}
		}
		//if(zuidalouceng==){}
		uiLcdDecimal(zuidalouceng,0+2,3,0,2);
	}
	//if(Floor_CurrentCount > (floor_tmp[App.Max_floor-2].floor_count+(floor_tmp[App.Max_floor-1].floor_diff/2)))
	//{
		//uiLcdDecimal(App.Max_floor,0+2,3,0,2);
	//}


	if(Parameter->Parameter_Change_Flag == 1)
	{

		if((Parameter->Parameter_Change_Position)&(WEITHT))
		{
			uiLcdDecimal(Parameter->Weight_Integer,1+2,5,0,1);

			uiLcdMediumString(".",1+2,6,0);
	
			uiLcdDecimal(Parameter->Weight_Decimal,1+2,7,0,1);
			uiLcdDecimal(Parameter->Weight_Percentile,1+2,8,0,1);

			//重量百分比
			uiLcdDecimal(Parameter->Weight_Percentage,1+2,12,0,3);
			
			//显示百分比
			//uiLcdDecimal(Parameter->Weight_Percentage,2,11,0,3);
			if(Parameter->Show_Flag== 1)
			{
				uiLcdMediumString("-",1+2,4,0);
			}
			else
			{
				uiLcdMediumString(":",1+2,4,0);//冒号
			}
		}
	/*	
		if((Parameter->Parameter_Change_Position)&(SHOWFLAG))
		{
			//用于显示设备状态	
			//uiProcShowFlag(App.Input_Data);
			//uiLcdLineErase8x16(3,0,10,0);//将先前状态擦除
			switch(Parameter->Show_Flag)
			{
				case CMD_FLOOR_DATA://楼层呼叫显示
					//pretime = uiTimeGetTickCount();
					
					
					break;
				
				default:
					break;
			}
			
		}*/
/*
		if((Parameter->Parameter_Change_Position)&(SHOWFLOOR))
		{
			switch(Parameter->Show_Floor)
			{
				case MASTER_STOP:
					uiLcd_1616_ch(5,2,44,1);//显示清空
					break;
				case MASTER_DOWN:
					uiLcd_1616_ch(4,2,44,1);//显示下降图标
					break;
				case MASTER_UP:
					uiLcd_1616_ch(3,2,44,1);//显示上升图标
					break;
				case CMD_START_ON://已启动
					uiLcd_1212_ch(UISTR_START,FLAG_ROW,FLAG_COL_FIVE+50,3);
					break;
				case CMD_START_OFF://未启动
					uiLcd_1212_ch(UISTR_FLAG_IN_DOOR+3,FLAG_ROW,FLAG_COL_FIVE+50,1);
					uiLcd_1212_ch(UISTR_START+1,FLAG_ROW,FLAG_COL_FIVE+50+12,2);
					break;
				default:
					break;
			}
		}*/

	}
	

}

/*
static u8  Handle_Voltage(void)
{
	u16 ad_temp;
	float temp;
	

	//获取电压的AD值
	ad_temp = ADC_Filter_V();

	uiLcdDecimal(ad_temp,0,0,0,5);

	temp=(float)ad_temp*(3.3/4096);

	uiLcdDecimal((u8)(temp*10),0,11,0,2);

	uiLcdDecimal(((u16)(temp*100))%10,0,11+3,0,1);

	uiLcdDecimal(((u16)(temp*1000))%10,0,11+4,0,1);


	
	
	return 0;
}
*/

static u8  Handle_Weight(Menu_Parameter *Parameter)
{
	u32 ad_temp,operate_temp;
	

	//获取重量的AD值
	Weight_Value = ADC_Filter_T();
	//printf("App.Weight_Tmp===%d\r\n",App.Weight_Tmp);
			
	if(App.Weight.weight_clear_ad_value_sign == 0)//为正
	{
		if(Weight_Value >= App.Weight.weight_clear_ad_value)
	         {
	                  ad_temp=Weight_Value-App.Weight.weight_clear_ad_value;
	         }
	         else
	         {
	               	ad_temp=App.Weight.weight_clear_ad_value-Weight_Value;//显示负
	                  //weight_display_sign=1;
			//Parameter->Show_Flag = 1;//显示负号提示
		}

	}
	if(App.Weight.weight_clear_ad_value_sign == 1)//为负
	{

		ad_temp=Weight_Value+App.Weight.weight_clear_ad_value;
	}

	operate_temp=ad_temp;
	ad_temp=(operate_temp * App.Weight.calibrate_sub_value)/App.Weight.calibrate_ad_sub_value;//得到最终的重量值	

	//临时显示读取的重量值
	//uiLcdDecimal(ad_temp,1+2, 0,0,4);
	//除去空笼载重
	if(ad_temp >App.Weight.Empty_weight)
	{
		ad_temp = ad_temp-App.Weight.Empty_weight;
	}
	else
	{
		//ad_temp=0;
		ad_temp = App.Weight.Empty_weight -ad_temp;
		Parameter->Show_Flag = 1;//显示负号提示
	}

	
	//在这里进行报警和预警判断
	if(ad_temp >(App.Weight.Rated_weight*App.Weight.Warning_weight/100))//超过预警值
	{
		uiLcdLineErase8x16(2,0,3,0);//将先前状态擦除
		//提示预警
		uiLcd_1212_ch(UISTR_ZAIZHONG_DAODAYUJINGZHI+2, 4,1, 2);
		
		
		if(ad_temp >(App.Weight.Rated_weight*App.Weight.Alarm_weight/100))//超过报警值
		{
			//提示报警
			uiLcdLineErase8x16(2,0,3,0);
			uiLcd_1212_ch(UISTR_ZAIZHONG_YICAOZAI+1, 4, 1, 2);
		
			fuck_count++;
			if(fuck_count >=2)//连续两次就断开
			{
				Buzzer_Beep(3,100);//报警三声
				System.Device.IO.HB_Gpio_Set_Value(RELAY_4,0);//超载打开
			}
		}else
		{
			fuck_count=0;
			System.Device.IO.HB_Gpio_Set_Value(RELAY_4,1);//超载闭合
			Buzzer_Beep(2,100);//预警三声
		}
	}
	else
	{
		uiLcdLineErase8x16(2,0,3,0);//将先前状态擦除
		fuck_count=0;
		System.Device.IO.HB_Gpio_Set_Value(RELAY_4,1);//超载闭合
	}

	//if(){}
	//显示百分比
	if(Parameter->Show_Flag == 1)
	{
		Parameter->Weight_Percentage=0;
	}else
	{
		Parameter->Weight_Percentage=(u8)((ad_temp*100/App.Weight.Rated_weight));
	}
	
	Parameter->Weight_Integer = (u8)(ad_temp/1000);
	ad_temp=ad_temp%1000;
	Parameter->Weight_Decimal =(u8)(ad_temp/100);
	ad_temp=ad_temp%100;
	Parameter->Weight_Percentile =(u8)(ad_temp/10);


	

	Parameter->Parameter_Change_Flag = 1;
	Parameter->Parameter_Change_Position |= WEITHT;

	return TRUE;
}

//显示相关参数
void Show_Param(void)
{
	u16 weignt_value_tmp;
	u8 nKey;
	u32 ad_temp;
	u32 operate_temp;
	u32 Time_tmp=0;
	uiLcdClear();


	uiLcd_1414_ch(UISTR_XXX_ESC, 6,76,2);
	uiLcdMediumString("ESC", 3, 13,0);

	//当前标定值
	uiLcd_1414_ch(UISTR_XXX_DANGQIAN, 0,4,2);
	uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UI+2, 0,4+14*2,2);
	uiLcd_1414_ch(UISTR_XXX_JISUANJIANSUZHI+4, 0,4+14*4,1);

	//电源电压
	uiLcdMediumString("V-AD:", 1, 0,0);

	//重量AD
	//uiLcd_1414_ch(UISTR_XXX_ZAIZHONG_SET_ZHONGLIANGZHILING, 2,4,2);
	uiLcdMediumString("T-AD:", 2, 0,0);

	//测试使用
	while(1)
	{

		 if(uiTimeGetTickCount() -Time_tmp >100)//1000ms更新一次
               	 {
			//得到重量的AD值
			weignt_value_tmp	= ADC_Filter_T();

			//得到电源电压的AD值
			ad_temp = ADC_Filter_V();
			
			Time_tmp = uiTimeGetTickCount();
			
			//第一行显示平层值
			uiLcdDecimal(Floor_CurrentCount,0, 2+7,0,6);

			//第二行显示电源电压
			uiLcdDecimal(ad_temp,1, 2+7,0,6);

			//第三行显示重量
			uiLcdDecimal(weignt_value_tmp,2, 2+7,0,6);
			
			
		 }
        		nKey = uiKeyGetKey();
		if(nKey == UIKEY_ESC)
			break;
	}
}

/*
static u8 Handle_ERR_Code(Menu_Parameter *Parameter)
{
	u16	Err_Code;
	Err_Code =  fram_data_buff[3];			
	Err_Code = (Err_Code << 8) |fram_data_buff[2];

	//printf("App.Input_Data==%x\r\n",App.Input_Data);
	//printf("Err_Code==%x\r\n",Err_Code);

	//临时加入
	uiLcdDecimal(Err_Code,3,9,0,5);
	//uiLcdDecimal(ad_temp,1+2, 0,0,4);
	
	if(App.Input_Data == Err_Code)
	{

		Parameter->Parameter_Change_Flag =1;
		Parameter->Parameter_Change_Position |= SHOWFLAG;
		return 0;
	}else
	{
		App.Input_Data = Err_Code;
		Parameter->Parameter_Change_Flag =1;
		Parameter->Parameter_Change_Position |= SHOWFLAG;
		
		return 1;
	}
	return 0;
}
*/
//处理主令扫描

/*
static u8  Handle_Master(Menu_Parameter *Parameter)
{

	u8 Scan_value ;
	App.Master_Flag = System.Device.IO.ScanPin();//扫描主令值
	DelayMs(10);//去抖
	Scan_value = System.Device.IO.ScanPin();//扫描主令值

	if(App.Master_Flag != Scan_value)
	{
		return 0;
	}
	else 
	{
		
	}

	if(master_pre_value ==  App.Master_Flag)
	{
		//return 0;//由于连续发送五次，导致语音播放杂音问题，暂时只发送一次
		
		tmpxxx++;
		if(tmpxxx == 2)//连续五次相等，就不发送
		{
			tmpxxx = 1;
			return 0;
		}
		//Send_Flag = 1;
	}
	else
	{
		//Send_Flag = 1;
		tmpxxx = 0;
		master_pre_value = App.Master_Flag;
	}
	Parameter->Parameter_Change_Flag = 1;
	Parameter->Parameter_Change_Position |= SHOWFLOOR;

	if(App.Master_Flag ==  0xfe)//第一位下降
	{
		
		HB_Send_Realy_CMD(CMD_RELAY_DOWN);
		Parameter->Show_Floor = MASTER_DOWN;
		uiLcdDecimal(1,3,15,0,1);
		WTV_Voice(MASTER_DOWM_FLAG);
		
	}
	else if(App.Master_Flag ==  0xfd)//第二位上升
	{
		HB_Send_Realy_CMD(CMD_RELAY_UP);
		Parameter->Show_Floor = MASTER_UP;
		uiLcdDecimal(2,3,15,0,1);


		WTV_Voice(MASTER_UP_FLAG);
		
	}
	else if(App.Master_Flag ==  0xfb)//第三位停止
	{
		HB_Send_Realy_CMD(CMD_RELAY_STOP);
		Parameter->Show_Floor = MASTER_STOP;
		uiLcdDecimal(3,3,15,0,1);
	}
	else
	{

	}

	return 0;

}*/
/////////////////////////////////////////////
//判断报文是否校验和正确,pRep保存的报文是掐头去尾
char   Cmd_Rep_valid(char*  pRep,char* plen)
{
	char i = 0,j = 0;
	//char sum = PROTOCOL_HEAD_1 + PROTOCOL_HEAD_2;
	char sum = 0;
	////分析报文，将转义符处理掉
    	for(i=0;i<(*plen-1);)//最后一个是校验和，不用判断
	{
		sum += pRep[i];
		if(pRep[i] == PROTOCOL_ESC_CHAR)
		{
			if(i == *plen-2)
			{
				return 0;
			}
			else
			{
				if((pRep[i+1]!= PROTOCOL_ESC_CHAR_COD)&&
					(pRep[i+1]!= PROTOCOL_TAIL_1_COD)&&
					(pRep[i+1]!= PROTOCOL_HEAD_1_COD))
					return 0;
				else
				{
					sum += pRep[i+1];
					fram_data_buff[j] = 0xff - pRep[i+1];
					j++;
					i+=2;
				}
			}
		}
		else
		{
			fram_data_buff[j] = pRep[i];
			j++;
			i++;
		}
	}
	fram_data_buff[j] = pRep[*plen-1];
	///判断校验和
	if(sum != fram_data_buff[j])
		return 0;
	*plen = fram_data_buff[1]+3;//头+数据长度+数据+校验和
    	return 1;
}



//////////////////////////////////////////////

void uiProcMenuHasValue(int nPopupMenuTitle, T_UI_MENUITEM *pUiMenuItem, int row,int nCurrentIndex)
{
	u32  nValue;
	u8 bRet = FALSE;
/*
	uiLcdLineErase6x16(row, 0, 20, FALSE);
	
	uiLcdMultiString(g_uiStrNumber[nCurrentIndex], row, 0, FALSE);
	if(g_uiLang != UILANG_EN)
	{	//双字节字体
		uiLcdMultiString(pUiMenuItem->nMenuStr, row, 1, FALSE);
	}
	else
	{
		//单字节字体
		uiLcdMultiString(pUiMenuItem->nMenuStr, row, 2, FALSE);
	}
*/
	switch(pUiMenuItem->nMenuStr)
	{
		case UISTR_MENU_ZAIZHONG_SET_EDINGZHONGLIANG:
			nValue = uiProcBoxNumber(&bRet, row, 10, App.Weight.Rated_weight, 1, 5000, 4, 4, TRUE);
			if (bRet) 
			{
				//gsSetting.bLockEn = nValue;
				App.Weight.Rated_weight = nValue;
			}
                		break;
		case UISTR_MENU_ZAIZHONG_SET_YUJINGZAIHE:
			nValue = uiProcBoxNumber(&bRet, row, 12, App.Weight.Warning_weight, 1, 100, 3, 3, TRUE);
			if (bRet) 
			{
				//gsSetting.bLockEn = nValue;
				App.Weight.Warning_weight = (u8)nValue;
			}
                		break;
		case UISTR_MENU_ZAIZHONG_SET_BAOJINGZAIHE://报警载荷
			nValue = uiProcBoxNumber(&bRet, row, 12, App.Weight.Alarm_weight, 1, 120, 3, 3, TRUE);
			if (bRet) 
			{
				//gsSetting.bLockEn = nValue;
				App.Weight.Alarm_weight= (u8)nValue;
			}
                		break;
		case UISTR_MENU_ZAIZHONG_SET_ZHONGLIANGZHILING://重量置零
			nValue = uiProcBoxNumber(&bRet, row, 11, App.Weight.weight_display_change_flg, 0, 1, 1, 1, TRUE);
			if (bRet) 
			{
				//gsSetting.bLockEn = nValue;
				App.Weight.weight_display_change_flg= (u8)nValue;
			}
                		break;
		case UISTR_MENU_ZAIZHONG_SET_KONGZAIZHONGLIANG://空载重量
			nValue = uiProcBoxNumber(&bRet, row, 10, App.Weight.Empty_weight, 1, 2000, 4, 4, TRUE);
			if (bRet) 
			{
				
				App.Weight.Empty_weight= nValue;
			}
                		break;
						

		case UISTR_MENU_PINGCHENG_SET_UP_BUCHANG://上升补偿
			nValue = uiProcBoxNumber(&bRet, row, 12, App.Up_buchang, 0, 9999, 4, 4, TRUE);
			if (bRet) 
			{
				
				App.Up_buchang= nValue;
			}
                		break;
		case UISTR_MENU_PINGCHENG_SET_DOWN_BUCHANG://下降补偿
			nValue = uiProcBoxNumber(&bRet, row, 12, App.Down_buchang, 0, 9999, 4, 4, TRUE);
			if (bRet) 
			{
				
				App.Down_buchang= nValue;
			}
                		break;
		case UISTR_MENU_PINGCHENG_SET_OTHER_BUCHANG://其他补偿
			nValue = uiProcBoxNumber(&bRet, row, 12, App.Other_buchang,0, 9999, 4, 4, TRUE);
			if (bRet) 
			{
				
				App.Other_buchang= nValue;
			}
                		break;
		//case UISTR_MENU_PINGCHENG_SET_MAX_FLOOR://最大楼层
			//nValue = uiProcBoxNumber(&bRet, row, 13, App.Max_floor,1, 99, 2, 2, TRUE);
			//if (bRet) 
			//{
				
				//App.Max_floor= (u8)nValue;
			//}
                		//break;
/*		case UISTR_MENU_PINGCHENG_SET_MAX_FLOOR_COUNT://由高速切换到低速的编码值
			nValue = uiProcBoxNumber(&bRet, row, 12, 3000,0, 9999, 4,4, TRUE);
			if (bRet) 
			{
				
				App.Floor_Count_Max= nValue;
			}
                		break;*/
		default:
			//uiLcdFatalError("HAS_VALUE!");
			break;
	}
}


void uiProcMenuCustom(int nPopupMenuTitle, T_UI_MENUITEM *pUiMenuItem, int row)
{
//	BOOL bDownloadRet, bDownloadEmpty;
//	u8 cRet;
//	u8 bRet;


	//printf("row==%d\r\n",row);
	
	switch(pUiMenuItem->nMenuStr)
	{

              	case UISTR_MENU_MIMA_SET:
			Pwd_Enroll_Process();
			break;
		case UISTR_MENU_ZAIZHONG_SET_BIAODINGDIAN_ONE://设置标定点一
			//Load_Set_Calibration(1);
			if(Load_Set_Calibration_tmp(1) == 1)
			{
				get_weight_clear_value();
			}
		
			break;
		case UISTR_MENU_ZAIZHONG_SET_BIAODINGDIAN_TWO://设置标定点二
			//if(Load_Set_Calibration(2) == 1)
			if(Load_Set_Calibration_tmp(2) == 1)
			{
				get_weight_clear_value();
			}
		
			
			break;
		case UISTR_MENU_PINGCHENG_SET_UI://平层标定界面
			Encoder_Demarcate();

			//Encoder_Demarcate_Init();
			break;
		case UISTR_MENU_PINGCHENG_QUANSHAN://平层标定删除
			Encoder_Demarcate_Del();

			//Encoder_Demarcate_Init();
			break;
		case UISTR_MENU_PINGCHENG_SET_UI_ZIDONG://平层修正界面
			Encoder_Demarcate_Init();
			break;
		//case UISTR_MENU_PINGCHENG_ZIDONGBUCANG://自动补偿
			//Auto_Encoder_Demarcate();
			//break;
		case UISTR_MENU_PINGCHENG_JISUAN_UP_BUCANG://计算上升补偿
			Auto_Fuck_Encoder(2);
			break;
		case UISTR_MENU_PINGCHENG_JISUAN_DOWN_BUCANG://计算下降补偿
			Auto_Fuck_Encoder(3);
			break;
		case UISTR_MENU_PINGCHENG_JISUAN_OTHER_BUCANG://计算减速补偿
			Auto_Fuck_Encoder(1);
			break;
			
		case UISTR_MENU_CALL_SET_UI://楼层学习界面
			Call_Learn();
			break;
		case UISTR_MENU_CALL_SET_DEL://楼层删除界面
			Call_Del();
			break;

		case UISTR_MENU_ZIJIAN_SET://状态自检就是显示当前重量值
			Machine_Info();
			
			break;
		default:
			break;
	}
}

//////////////////////////////////////////////////////////////////////////
//画有值要显示的部分
void uiProcMenuDrawValue(T_UI_MENUITEM *pUiMenuItem, int row, BOOL bIsReverse)
{
	BOOL bIsMulti = FALSE;
//	UINT32 dwValue = 0;
//	UINT8 	cTemp[8];

	switch(pUiMenuItem->nMenuStr)
	{

		case UISTR_MENU_ZAIZHONG_SET_EDINGZHONGLIANG:
			//uiLcdMediumString("Kg", row, 15, 0);
			//uiLcdSmallString("T", row*2+1, 18, 0);
			
			//uiLcdDecimal(2, row, 12, 0, 1);
			//uiLcdMediumString(".", row, 13, 0);
			uiLcdDecimal(App.Weight.Rated_weight, row, 10, 0, 4);

			uiLcdSmallString("Kg", row*2+1, 19, 0);
			//uiLcdMediumString("Kg", row, 14, 0);
			//wa 
			
			break;
		case UISTR_MENU_ZAIZHONG_SET_YUJINGZAIHE:
			uiLcdDecimal(App.Weight.Warning_weight, row, 12, 0, 3);
			uiLcdMediumString("%", row, 15, 0);
			break;
		case UISTR_MENU_ZAIZHONG_SET_BAOJINGZAIHE:
			uiLcdDecimal(App.Weight.Alarm_weight, row, 12, 0, 3);
			uiLcdMediumString("%", row, 15, 0);
			break;
		case UISTR_MENU_ZAIZHONG_SET_ZHONGLIANGZHILING:
			//uiLcdMediumString("YES", row*2, 12, 0);
			if(App.Weight.weight_display_change_flg == 1)
			{
				uiLcdSmallString("YES", row*2+1, 17, 0);
			}else
			{
				uiLcdSmallString("NO ", row*2+1, 17, 0);
			}
			break;
		case UISTR_MENU_ZAIZHONG_SET_KONGZAIZHONGLIANG://空载重量
			uiLcdDecimal(App.Weight.Empty_weight, row, 10, 0, 4);
			uiLcdSmallString("Kg", row*2+1, 19, 0);
			break;

		//平层部分
		case UISTR_MENU_PINGCHENG_SET_UP_BUCHANG:
			uiLcdDecimal(App.Up_buchang, row, 12, 0, 4);
		
			break;
		case UISTR_MENU_PINGCHENG_SET_DOWN_BUCHANG:
			uiLcdDecimal(App.Down_buchang, row, 12, 0, 4);
		
			break;
		case UISTR_MENU_PINGCHENG_SET_OTHER_BUCHANG:
			uiLcdDecimal(App.Other_buchang, row, 12, 0, 4);
			
			break;
		//case UISTR_MENU_PINGCHENG_SET_MAX_FLOOR:
			//uiLcdDecimal(App.Max_floor, row, 13, 0, 2);
			
			//break;
		
		default:
			//uiLcdFatalError("DRAW_VALUE!");
			break;
	}

	if (bIsMulti)
	{
		//uiLcdMultiStringRightAlign(dwValue, row, bIsReverse);
	}
	else
	{
		//uiLcdDecimalRightAlign((int)dwValue, row, bIsReverse, 0);
	}
}

void uiProcMenuDraw(T_UI_MENUITEM *pUiStartMenuItem, int nCount,int nTopIndex, int nCurrentIndex)
{
	int i, nIndex;
	BOOL bSelected;

	uiLcdClear();

	//printf("nCount==%d\r\n",nCount);
	//printf("nTopIndex==%d\r\n",nTopIndex);
	//printf("nCurrentIndex==%d\r\n",nCurrentIndex);

	//-----------------------------
	//显示菜单主题
	switch((pUiStartMenuItem-1)->nMenuStr)//找type对应的值
	{
		
		case UISTR_MENU:
			//i = UISTR_MENU_PINGCHENG_SET;
			uiLcd_1212_ch(UISTR_ZHUSHEZHICAIDAN, 0, 40, 5);
			
			break;
		case UISTR_MENU_ZAIZHONG_SET:
			uiLcd_1212_ch(UISTR_ZAIZHONG_SET_ZHONGLIANGZHILING, 0, 40, 2);
			uiLcd_1212_ch(UISTR_ZAIZHONG_SET+2, 0, 40+12*2, 2);
			//i = UISTR_MENU_PINGCHENG_SET;
			break;

		case UISTR_MENU_PINGCHENG_SET:
			uiLcd_1212_ch(UISTR_PINGCHENG_SET, 0, 40, 4);
			//i = UISTR_MENU_PINGCHENG_SET;
			break;
		case UISTR_MENU_CALL_SET:
			uiLcd_1212_ch(UISTR_ENCODER_FLOOR+2, 0, 40, 2);//楼层
			uiLcd_1212_ch(UISTR_CALL_SET, 0, 40+24, 2);//学习
			//i = UISTR_MENU_PINGCHENG_SET;
			break;
		default:

			break;
	}
	//这里显示主菜单
	//uiLcdMultiString(i, 0, 0, FALSE);

	
/*
	if (nCurrentIndex == 0)
		uiLcdMenuStatus(2);
	else
	if (nCurrentIndex == nCount - 1)
		uiLcdMenuStatus(1);
	else
		uiLcdMenuStatus(3);
*/

	for(i=0; i<3; i++)//循环显示一个主题下的三个项目
	{
		nIndex = nTopIndex + i;
		if (nIndex >= nCount)
			break;

		if (nIndex == nCurrentIndex)
		{
			//uiLcdLineErase6x16(1+i, 0, 20, TRUE);
			//uiLcdLineErase8x16(1+i, 0, 16, TRUE);
			bSelected = TRUE;
			uiLcdDecimal_Small(nCurrentIndex+1, (1+i)*2, 0, 0, 1);
			uiLcdSmallString(">", (1+i)*2, 1,0);
			//uiLcdDecimal_Small(nCurrentIndex, (1+i)*2, 0, 0, 1);
		}
		else
		{
			bSelected = FALSE;
		}
		//uiLcdMultiString(g_uiStrNumber[nIndex], 1+i, 0, bSelected);


		
		//uiLcdMultiString(pUiStartMenuItem[nIndex].nMenuStr, 1+i, 1, bSelected);

		//uiLcdSmallString
		//uiLcdSmallString("1:",0,0,0);
		//在这里添加菜单目录的绘画
		switch(pUiStartMenuItem[nIndex].nMenuStr)//找type对应的值
		{
			//主菜单下的显示
			case UISTR_MENU_PINGCHENG_SET:
				//uiLcdSmallString("1:",0,0,1);
				uiLcd_1212_ch(UISTR_PINGCHENG_SET, (1+i)*2, HEAD_LEN, 4);
				break;
			case UISTR_MENU_ZAIZHONG_SET:
				//uiLcdSmallString("2:",2,0,1);
				//uiLcd_1212_ch(UISTR_ZAIZHONG_SET, (1+i)*2, HEAD_LEN, 4);
				uiLcd_1212_ch(UISTR_ZAIZHONG_SET_ZHONGLIANGZHILING, (1+i)*2, HEAD_LEN, 2);
				uiLcd_1212_ch(UISTR_ZAIZHONG_SET+2, (1+i)*2, HEAD_LEN+12*2, 2);
				break;
			case UISTR_MENU_MIMA_SET:
				//uiLcdSmallString("3:",4,0,1);
				uiLcd_1212_ch(UISTR_MIMA_SET, (1+i)*2, HEAD_LEN, 4);
				break;
			case UISTR_MENU_CALL_SET:
				//uiLcdSmallString("3:",4,0,1);
				uiLcd_1212_ch(UISTR_ENCODER_FLOOR+2, (1+i)*2, HEAD_LEN, 2);//楼层
				uiLcd_1212_ch(UISTR_CALL_SET, (1+i)*2, HEAD_LEN+24, 2);//学习
				break;
			case UISTR_MENU_ZIJIAN_SET:
				//uiLcdSmallString("4:",6,0,1);
				uiLcd_1212_ch(UISTR_ZHAUNTAIZIJIAN_SET, (1+i)*2, HEAD_LEN, 4);
				break;

			//平层设置下的显示
			case UISTR_MENU_PINGCHENG_SET_UI://平层标定界面
				//uiLcd_1414_ch(UISTR_XXX_SHOUDONG, (1+i)*2, HEAD_LEN, 2);
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UI, (1+i)*2, HEAD_LEN, 6);
				break;
			//case UISTR_MENU_PINGCHENG_ZIDONGBUCANG://自动补偿
				//uiLcd_1414_ch(UISTR_XXX_ZIDONG, (1+i)*2, HEAD_LEN, 2);
				//uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UP_BUCHANG+2, (1+i)*2, HEAD_LEN+14*2, 2);
				//break;
			case UISTR_MENU_PINGCHENG_SET_UI_ZIDONG://平层修正界面
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UI, (1+i)*2, HEAD_LEN, 2);
				uiLcd_1414_ch(UISTR_XXX_XIUZHENG, (1+i)*2, HEAD_LEN+14*2, 2);
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UI+4, (1+i)*2, HEAD_LEN+14*4, 2);
				break;
			case UISTR_MENU_PINGCHENG_SET_UP_BUCHANG:
				
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UP_BUCHANG, (1+i)*2, HEAD_LEN, 4);
				break;
			case UISTR_MENU_PINGCHENG_SET_DOWN_BUCHANG:
			
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_DOWN_BUCHANG, (1+i)*2, HEAD_LEN, 4);
				break;
			case UISTR_MENU_PINGCHENG_SET_OTHER_BUCHANG:
				uiLcd_1414_ch(UISTR_XXX_JISUANJIANSUZHI+2, (1+i)*2, HEAD_LEN, 2);
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UP_BUCHANG+2, (1+i)*2, HEAD_LEN+14*2,2);
				break;
			//case UISTR_MENU_PINGCHENG_SET_MAX_FLOOR:
				//uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_MAX_FLOOR, (1+i)*2, HEAD_LEN, 4);
				//break;
			case UISTR_MENU_PINGCHENG_QUANSHAN://平层标定全删
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UI, (1+i)*2, HEAD_LEN, 4);
				uiLcd_1414_ch(UISTR_XXX_QUANBU, (1+i)*2, HEAD_LEN+14*4,1);
				uiLcd_1414_ch(UISTR_XXX_SHANCHU, (1+i)*2, HEAD_LEN+14*5,1);
				break;
				
			case UISTR_MENU_PINGCHENG_JISUAN_DOWN_BUCANG://计算下降补偿
				uiLcd_1414_ch(UISTR_XXX_JISUANJIANSUZHI, (1+i)*2, HEAD_LEN, 2);
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_DOWN_BUCHANG, (1+i)*2, HEAD_LEN+14*2,4);
				break;
			case UISTR_MENU_PINGCHENG_JISUAN_UP_BUCANG://计算上升补偿
				uiLcd_1414_ch(UISTR_XXX_JISUANJIANSUZHI, (1+i)*2, HEAD_LEN, 2);
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UP_BUCHANG, (1+i)*2, HEAD_LEN+14*2,4);
				break;
			case UISTR_MENU_PINGCHENG_JISUAN_OTHER_BUCANG://计算减速补偿
				uiLcd_1414_ch(UISTR_XXX_JISUANJIANSUZHI, (1+i)*2, HEAD_LEN, 4);
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UP_BUCHANG+2, (1+i)*2, HEAD_LEN+14*4,2);
				//uiLcd_1414_ch(UISTR_XXX_SHANCHU, (1+i)*2, HEAD_LEN+14*5,1);
				break;
				
			//载重设置下的显示
			case UISTR_MENU_ZAIZHONG_SET_EDINGZHONGLIANG:
				//uiLcd_1212_ch(UISTR_ZAIZHONG_SET_EDINGZHONGLIANG, (1+i)*2, HEAD_LEN, 4);
				//uiLcd_1616_ch(UISTR_XXX_ZAIZHONG_SET_EDINGZHONGLIANG, (1+i)*2, HEAD_LEN, 4);
				uiLcd_1414_ch(UISTR_XXX_ZAIZHONG_SET_EDINGZHONGLIANG, (1+i)*2, HEAD_LEN, 2);
				uiLcd_1414_ch(UISTR_XXX_ZAIZHONG_SET_YUJINGZAIHE+2, (1+i)*2, HEAD_LEN+14*2, 2);
				break;
			case UISTR_MENU_ZAIZHONG_SET_YUJINGZAIHE:
				//uiLcd_1212_ch(UISTR_ZAIZHONG_SET_YUJINGZAIHE, (1+i)*2, HEAD_LEN, 4);
				uiLcd_1414_ch(UISTR_XXX_ZAIZHONG_SET_YUJINGZAIHE, (1+i)*2, HEAD_LEN, 4);
				break;
			case UISTR_MENU_ZAIZHONG_SET_BAOJINGZAIHE:
				//uiLcd_1212_ch(UISTR_ZAIZHONG_SET_BAOJINGZAIHE, (1+i)*2, HEAD_LEN, 4);
				uiLcd_1414_ch(UISTR_XXX_ZAIZHONG_SET_BAOJINGZAIHE, (1+i)*2, HEAD_LEN, 4);
				break;
				
			case UISTR_MENU_ZAIZHONG_SET_BIAODINGDIAN_ONE://设置标定点一,实际是重量置零
				//uiLcd_1212_ch(UISTR_ZAIZHONG_SET_ZHONGLIANGZHILING, (1+i)*2, HEAD_LEN, 4);
				uiLcd_1414_ch(UISTR_XXX_ZAIZHONG_SET_ZHONGLIANGZHILING, (1+i)*2, HEAD_LEN, 4);
				break;
			case UISTR_MENU_ZAIZHONG_SET_BIAODINGDIAN_TWO://设置标定点二
				//uiLcd_1212_ch(UISTR_ZAIZHONG_SET_ZHONGLIANGZHILING, (1+i)*2, HEAD_LEN, 4);
				uiLcd_1414_ch(UISTR_XXX_ZAIZHONG_SET_BIAODINGDIAN_TWO, (1+i)*2, HEAD_LEN, 5);
				break;		
			case UISTR_MENU_ZAIZHONG_SET_ZHONGLIANGZHILING:
				//uiLcd_1212_ch(UISTR_ZAIZHONG_SET_ZHONGLIANGZHILING, (1+i)*2, HEAD_LEN, 4);
				uiLcd_1414_ch(UISTR_XXX_SHIFOU, (1+i)*2, HEAD_LEN, 2);
				uiLcd_1414_ch(UISTR_XXX_SHIYONG, (1+i)*2, HEAD_LEN+14*2, 2);
				break;
			case UISTR_MENU_ZAIZHONG_SET_KONGZAIZHONGLIANG://空载重量
				//uiLcd_1212_ch(UISTR_ZAIZHONG_SET_ZHONGLIANGZHILING, (1+i)*2, HEAD_LEN, 4);
				uiLcd_1414_ch(UISTR_XXX_KONGZAI, (1+i)*2, HEAD_LEN, 2);
				uiLcd_1414_ch(UISTR_XXX_ZAIZHONG_SET_EDINGZHONGLIANG+2, (1+i)*2, HEAD_LEN+14*2, 2);
				break;

			//楼层学习下的显示
			case UISTR_MENU_CALL_SET_UI://楼层学习界面
			
				uiLcd_1414_ch(UISTR_XXX_HUJIAOLOUCENG+2, (1+i)*2, HEAD_LEN, 2);
				uiLcd_1414_ch(UISTR_XXX_XUEXI, (1+i)*2, HEAD_LEN+14*2, 2);
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UI+4, (1+i)*2, HEAD_LEN+14*4, 2);
				break;
			case UISTR_MENU_CALL_SET_DEL://楼层删除界面
				uiLcd_1414_ch(UISTR_XXX_HUJIAOLOUCENG+2, (1+i)*2, HEAD_LEN, 2);
				uiLcd_1414_ch(UISTR_XXX_SHANCHU, (1+i)*2, HEAD_LEN+14*2, 2);
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UI+4, (1+i)*2, HEAD_LEN+14*4, 2);
				break;


				
			default:
				//i = (pUiStartMenuItem-1)->nMenuStr;
				break;
		}
		//uiLcd_1212_ch(UISTR_PINGCHENG_SET, 1+i, 1, 4);
		

		if (pUiStartMenuItem[nIndex].nType == UIMENU_HASVALUE ||
			pUiStartMenuItem[nIndex].nType == UIMENU_CUSTOM_HASVALUE)
		{
			uiProcMenuDrawValue(&pUiStartMenuItem[nIndex], 1+i, bSelected);
		}
	}
}



void uiProcMenuSettingStart(int nPopupMenuTitle)
{
	
/*
	gsSetting.wDeviceNum = gsSystemInfo.sFshSystemInfo.wDeviceNum;
	gsSetting.wRepeatTime = gsSystemInfo.sFshSystemInfo.wRepeatTime;
	gsSetting.wRecordAlarm = gsSystemInfo.sFshSystemInfo.wRecordAlarm;
	gsSetting.wSpeakerPromptEn = gsSystemInfo.sFshSystemInfo.wSpeakerPromptEn;
	gsSetting.wBaudrate = gsSystemInfo.sFshSystemInfo.wBaudrate;
	gsSetting.wLCDContrast = gsSystemInfo.sFshSystemInfo.wLCDContrast;
	gsSetting.bLanguage = gsSystemInfo.sFshSystemInfo.bLanguage;
	gsSetting.bMaxAdminNum = gsSystemInfo.sFshSystemInfo.bMaxAdminNum;
	gsSetting.bAtendFunctionKeyEn = gsSystemInfo.sFshSystemInfo.bAtendFunctionKeyEn;
	gsSetting.bLockEn = gsSystemInfo.sFshSystemInfo.wLockEn;
	gsSetting.bWiegandIn = gsSystemInfo.sFshSystemInfo.bWiegandType;
	gsSetting.bVolume = gsSystemInfo.sFshSystemInfo.bVolume;

	gsSetting.wPortNum = gsSystemInfo.sFshSystemInfo.wPortNum;
	gsSetting.dwCommPassword = gsSystemInfo.sFshSystemInfo.dwCommPassword;
	gsSetting.ipAddress = gsSystemInfo.sFshSystemInfo.ipAddress;
	gsSetting.ipSubnetMask = gsSystemInfo.sFshSystemInfo.ipSubnetMask;
	gsSetting.ipDefaultGateway = gsSystemInfo.sFshSystemInfo.ipDefaultGateway;

*/

}


//保存系统参数
UINT8 SaveSystemInfo(void)
{
	UINT8 *pData;
	UINT16 wTemp;
	UINT32 i;
	
	pData = (UINT8 *)&App;
	pData += 2;
	wTemp = 0x00;
	for(i = 0;i < sizeof(App) - 2;i++)
	{
		wTemp += *pData++;
	}
	App.wLicCheckSum = wTemp;


	memcpy(&App_Flash,&App,sizeof(App));
	
	Flash_WriteInfo((u8*)&App,APP_SETUP_ADD,sizeof(App));
	DelayMs(50);
	Flash_WriteInfo((u8*)&App,APP_SETUP_ADD2,sizeof(App));
	DelayMs(50);
	Flash_WriteInfo((u8*)&App,APP_SETUP_ADD3,sizeof(App));
	DelayMs(50);
	return TRUE;
}

void uiProcMenuSettingEnd(int nPopupMenuTitle)
{
	BOOL bSave0 = FALSE;

	//printf("nPopupMenuTitle===%d\r\n",nPopupMenuTitle);
	//printf("UISTR_MENU_ZAIZHONG_SET===%d\r\n",UISTR_MENU_ZAIZHONG_SET);
	if(nPopupMenuTitle == UISTR_MENU_ZAIZHONG_SET)
	{
		//printf("456\r\n");
		
		if(App.Weight.Rated_weight  != App_Flash.Weight.Rated_weight )
		{
			bSave0 = TRUE;
		}
		if(App.Weight.Warning_weight !=  App_Flash.Weight.Warning_weight)
		{
			bSave0 = TRUE;
		}
		if(App.Weight.Alarm_weight != App_Flash.Weight.Alarm_weight)
		{
			bSave0 = TRUE;
		}
		if(App.Weight.calibrate_two_suffer!= App_Flash.Weight.calibrate_two_suffer)
		{
			bSave0 = TRUE;
		}
		if(App.Weight.weight_display_change_flg!= App_Flash.Weight.weight_display_change_flg)
		{
			bSave0 = TRUE;
		}
		if(App.Weight.Empty_weight!= App_Flash.Weight.Empty_weight)
		{
			bSave0 = TRUE;
		}

	}
	
	if (nPopupMenuTitle == UISTR_MENU_PINGCHENG_SET)
	{
		if(App.Up_buchang!= App_Flash.Up_buchang)
		{
			bSave0 = TRUE;
			//HB_Send_CMD_DATA4(CMD_LEVELING_DATA_UP_BUCHANG,App.Up_buchang);
		}
		if(App.Down_buchang!=  App_Flash.Down_buchang)
		{
			bSave0 = TRUE;
			//HB_Send_CMD_DATA4(CMD_LEVELING_DATA_DOWN_BUCHANG,App.Down_buchang);
		}
		if(App.Other_buchang!= App_Flash.Other_buchang)
		{
			bSave0 = TRUE;
			//HB_Send_CMD_DATA4(CMD_LEVELING_DATA_OTHER_BUCHANG,App.Other_buchang);
		}
		if(App.Max_floor!= App_Flash.Max_floor)
		{
			bSave0 = TRUE;
			//HB_Send_CMD_DATA4(CMD_LEVELING_DATA_OTHER_BUCHANG,App.Other_buchang);
		}	
	
	}
	
	if(bSave0 == TRUE)
	{
		
		SaveSystemInfo();

	}
}


void uiProcMenu(int nPopupMenuTitle)
{
	int nCount, nTopIndex = 0, nCurrentIndex = 0;
	T_UI_MENUITEM *pUiStartMenuItem = &g_uiMenuItems[0], *pUiEndMenuItem;

	u8 nKey;
//	u8 bRet;

	
	uiProcMenuSettingStart(nPopupMenuTitle);

	//////////////////////////////////////////////////////////////////////////
	while (pUiStartMenuItem->nMenuStr != nPopupMenuTitle ||
		(int)pUiStartMenuItem->nType < 0)
	{
		pUiStartMenuItem++;
	}
	pUiStartMenuItem++;//加1

	pUiEndMenuItem = pUiStartMenuItem;
	while ((int)pUiEndMenuItem->nType < 0)//找第结束位置
	{
		pUiEndMenuItem++;
	}

	nCount = ((UINT32)pUiEndMenuItem - (UINT32)pUiStartMenuItem)/sizeof(T_UI_MENUITEM);

	//////////////////////////////////////////////////////////////////////////
	//uiProcMenuSysInfoViewInit(pUiStartMenuItem);
	
	//////////////////////////////////////////////////////////////////////////
	uiProcMenuDraw(pUiStartMenuItem, nCount, nTopIndex, nCurrentIndex);

	while(1)
	{
		
		if( (nKey = uiKeyGetKey()) == UIKEY_ESC)
		{
			uiProcMenuSettingEnd(nPopupMenuTitle);
			break;
		}

		switch(nKey) 
		{
			case UIKEY_OK:
			/*	
				if (pUiStartMenuItem[nCurrentIndex].nType == UIMENU_POPUP &&
					uiProcMenuCheckPrivilege(pUiStartMenuItem[nCurrentIndex].nMenuStr))
				{
					

				}
				else if (pUiStartMenuItem[nCurrentIndex].nType == UIMENU_HASVALUE)
				{
					uiProcMenuHasValue(nPopupMenuTitle, &pUiStartMenuItem[nCurrentIndex], 
						1 + (nCurrentIndex - nTopIndex),nCurrentIndex);
				}
				else if (pUiStartMenuItem[nCurrentIndex].nType == UIMENU_CUSTOM_HASVALUE)
				{
					uiProcMenuSysInfoViewProcess(&pUiStartMenuItem[nCurrentIndex]);
				}
				else if (pUiStartMenuItem[nCurrentIndex].nType == UIMENU_CUSTOM)
				{
					uiProcMenuCustom(nPopupMenuTitle, &pUiStartMenuItem[nCurrentIndex],1 + (nCurrentIndex - nTopIndex));
				}
			*/
				if (pUiStartMenuItem[nCurrentIndex].nType == UIMENU_POPUP)//处理有下拉菜单的选项
				{
					
					uiProcMenu(pUiStartMenuItem[nCurrentIndex].nMenuStr);
					
				}
				else if(pUiStartMenuItem[nCurrentIndex].nType == UIMENU_HASVALUE)//有值改变的情况处理
				{

					uiProcMenuHasValue(nPopupMenuTitle, &pUiStartMenuItem[nCurrentIndex], 
						1 + (nCurrentIndex - nTopIndex),nCurrentIndex);
				}
				else if (pUiStartMenuItem[nCurrentIndex].nType == UIMENU_CUSTOM)
				{
					uiProcMenuCustom(nPopupMenuTitle, &pUiStartMenuItem[nCurrentIndex],1 + (nCurrentIndex - nTopIndex));
				}
				
				uiProcMenuDraw(pUiStartMenuItem, nCount, nTopIndex, nCurrentIndex);

				break;
			case UIKEY_UP:
				if (nCurrentIndex > 0)
					nCurrentIndex --;

				if (nCurrentIndex < nTopIndex)
					nTopIndex = nCurrentIndex;

				uiProcMenuDraw(pUiStartMenuItem, nCount, nTopIndex, nCurrentIndex);
				break;
			case UIKEY_DOWN:
				if (nCurrentIndex < nCount - 1)
					nCurrentIndex ++;

				if (nCurrentIndex > nTopIndex + 2)
					nTopIndex = nCurrentIndex - 2;

				uiProcMenuDraw(pUiStartMenuItem, nCount, nTopIndex, nCurrentIndex);
				break;
			default:
				break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////
#define AFTER_CENG	350
#define AFTER_SHUZI	350

void Voice_Call(u8 call_floor)
{
	if(call_floor < 1)
	{
		return;
	}
	if((call_floor > 0) && (call_floor < 11))//ok
	{
		WTV_Voice(ONE_FLAG-1+call_floor);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
		DelayMs(AFTER_CENG);
		WTV_Voice(HUJIAO);
		
	}else if((call_floor > 10) && (call_floor < 20))//ok
	{
		WTV_Voice(TEN_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(ONE_FLAG-1+call_floor-10);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
		DelayMs(AFTER_CENG);
		WTV_Voice(HUJIAO);
		
	}else if(call_floor == 20)//ok
	{
		WTV_Voice(TWO_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(TEN_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
		DelayMs(AFTER_CENG);
		WTV_Voice(HUJIAO);
	}else if((call_floor > 20) && (call_floor < 30))
	{
		WTV_Voice(TWO_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(TEN_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(ONE_FLAG-1+call_floor-20);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
		DelayMs(AFTER_CENG);
		WTV_Voice(HUJIAO);
	}else if(call_floor == 30)
	{
		WTV_Voice(THREE_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(TEN_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
		DelayMs(AFTER_CENG);
		WTV_Voice(HUJIAO);
	}else if((call_floor > 30) && (call_floor < 40))
	{
		WTV_Voice(THREE_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(TEN_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(ONE_FLAG-1+call_floor-30);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
		DelayMs(AFTER_CENG);
		WTV_Voice(HUJIAO);
	}else if(call_floor == 40)
	{
		WTV_Voice(FOUR_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(TEN_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
		DelayMs(AFTER_CENG);
		WTV_Voice(HUJIAO);
	}else if((call_floor > 40) && (call_floor < 50))
	{
		WTV_Voice(FOUR_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(TEN_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(ONE_FLAG-1+call_floor-40);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
		DelayMs(AFTER_CENG);
		WTV_Voice(HUJIAO);
	}else if(call_floor == 50)
	{
		WTV_Voice(FIVE_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(TEN_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
		DelayMs(AFTER_CENG);
		WTV_Voice(HUJIAO);
	}
	else if((call_floor > 50) && (call_floor < 60))
	{
		WTV_Voice(FIVE_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(TEN_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(ONE_FLAG-1+call_floor-50);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
		DelayMs(AFTER_CENG);
		WTV_Voice(HUJIAO);
	}else
	{

	}

}

void Voice_Call_Num(u8 call_floor)
{

	if(call_floor < 1)
	{
		return;
	}
	if((call_floor > 0) && (call_floor < 11))//ok
	{
		WTV_Voice(ONE_FLAG-1+call_floor);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
		
	}else if((call_floor > 10) && (call_floor < 20))//ok
	{
		WTV_Voice(TEN_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(ONE_FLAG-1+call_floor-10);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
	
		
	}else if(call_floor == 20)//ok
	{
		WTV_Voice(TWO_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(TEN_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
		
	}else if((call_floor > 20) && (call_floor < 30))
	{
		WTV_Voice(TWO_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(TEN_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(ONE_FLAG-1+call_floor-20);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
		
	}else if(call_floor == 30)
	{
		WTV_Voice(THREE_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(TEN_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
	
	}else if((call_floor > 30) && (call_floor < 40))
	{
		WTV_Voice(THREE_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(TEN_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(ONE_FLAG-1+call_floor-30);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
		
	}else if(call_floor == 40)
	{
		WTV_Voice(FOUR_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(TEN_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
	
	}else if((call_floor > 40) && (call_floor < 50))
	{
		WTV_Voice(FOUR_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(TEN_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(ONE_FLAG-1+call_floor-40);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
	
	}else if(call_floor == 50)
	{
		WTV_Voice(FIVE_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(TEN_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
	
	}
	else if((call_floor > 50) && (call_floor < 60))
	{
		WTV_Voice(FIVE_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(TEN_FLAG);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(ONE_FLAG-1+call_floor-50);
		DelayMs(AFTER_SHUZI);
		WTV_Voice(CENG_FALG);
	
	}else
	{

	}


}

void HB_RELAY_Mode(void)
{

            if(Floor_CurrentCount > Floor_TargetCount)//向下运行
            {
                     HB_RELAY_Flag = 2;
		uiLcd_1616_ch(4,4,57,1);//显示下降图标
		DelayMs(1000);
		   

            }
            else if(Floor_CurrentCount < Floor_TargetCount)//向上运行
            {
                    HB_RELAY_Flag = 1;
		 uiLcd_1616_ch(3,4,57,1);//显示上升图标
		 DelayMs(1000);
            }
            else//停止
            {
                    HB_RELAY_Flag = 0;
            }
}



u8  uiProcKey(u8 nKey,Menu_Parameter *Parameter)
{
	if(uiKeyIsDigit(nKey) )	/*判断按键是否是数字*/
	{
		
		//Target_F += (nKey - '0');
		Target_F = Target_F * 10 + (int)nKey - '0';
		if(Target_F > App.Max_floor)
		{
			Target_F =  ((Target_F%100)/10)*10 +(nKey - '0');
		}
		if(Target_F < 1)
		{
			Target_F =  0;
		}
			
	}
	else if (nKey == UIKEY_UP)//目标楼层加1
	{
		Target_F = Target_F+1;
		if(Target_F > App.Max_floor)
		{
			Target_F =  App.Max_floor;
		}

		//System.Device.IO.HB_Gpio_Set_Value(RELAY_1,1);
		//System.Device.IO.HB_Gpio_Set_Value(RELAY_2,1);

		//HB_Send_Realy_CMD(CMD_RELAY_UP);
	}
	else if (nKey == UIKEY_DOWN)//目标楼层减1
	{
		if(Target_F == 0)
		{
			
		}else
		{
			Target_F = Target_F-1;
		}
		//System.Device.IO.HB_Gpio_Set_Value(RELAY_1,0);
		//System.Device.IO.HB_Gpio_Set_Value(RELAY_2,0);
		//HB_Send_Realy_CMD(CMD_RELAY_DOWN);
	}
	else if (nKey == UIKEY_ESC)//目标楼层退出
	{
		Target_F = 0;
		//if(Target_F < 1)
		//{
			//Target_F =  1;
		//}
		
		//HB_Send_Realy_CMD(CMD_RELAY_STOP);
	}
	else if (nKey == UIKEY_OK)//目标楼层启动
	{
		//处理目标楼层启动流程
		//get_weight_clear_value();
		
		if(Target_F < 1)
		{	
			//continue;	
		}
		else if(Target_F > App.Max_floor)
		{
			//界面提示错误值"目标层过大"
			
			//uiLcd_1212_ch(UISTR_AIM_LOU_CHENG,0,66,2);
			//uiLcd_1212_ch(UISTR_AIM_LOU_CHENG+3,0,66+12*2,1);
			//uiLcd_1212_ch(UISTR_UI_GUODA,0,66+12*3,2);
			uiLcd_1212_ch(UISTR_AIM_LOU_CHENG+2,0,66,2);
			uiLcd_1212_ch(UISTR_FLAG_IN_DOOR+3,0,66+12*2,1);
			uiLcd_1212_ch(UISTR_ENCODER_FLOOR,0,66+12*3,2);
			DelayMs(1200);
			Target_F =0;
			uiLcdLineErase8x16(0,8,17,0);//将先前状态擦除
			
		}
		else
		{
			if(floor_tmp[Target_F-1].floor_flag == 0)//排除没有标定的楼层
			{
				//uiLcd_1212_ch(UISTR_AIM_LOU_CHENG,0,66,2);
				//uiLcd_1212_ch(UISTR_AIM_LOU_CHENG+3,0,66+12*2,1);
				//uiLcd_1212_ch(UISTR_UI_GUODA,0,66+12*3,2);
				//楼层未标定
				uiLcd_1212_ch(UISTR_AIM_LOU_CHENG+2,0,66,2);
				uiLcd_1212_ch(UISTR_FLAG_IN_DOOR+3,0,66+12*2,1);
				uiLcd_1212_ch(UISTR_ENCODER_FLOOR,0,66+12*3,2);
				DelayMs(1200);
				Target_F =0;
				HB_Start_Flag = 0;
				HB_RELAY_Flag = 0;

				uiLcdLineErase8x16(0,8,17,0);//将先前状态擦除
				
				return 0;
			
			}
			
			//读取目标楼层的平层值
			Floor_TargetCount = floor_tmp[Target_F-1].floor_count;
			
			Floor_StartCount = Floor_CurrentCount;

			System.Device.IO.HB_Gpio_Set_Value(RELAY_3,1);//高速闭合
			System.Device.IO.HB_Gpio_Set_Value(RELAY_1,1);//上升
			System.Device.IO.HB_Gpio_Set_Value(RELAY_2,1);//下降

			//这里存在先后问题，要先判断运行方向，再开启运行
			HB_RELAY_Mode();
			HB_Start_Flag = 1;

			return 1;
			//uiLcdDecimal(Floor_TargetCount,0,11,0,5);
		}
		
		
	}
	else if (nKey == UIKEY_MENU)
	{			

		//验证密码
		if(Pwd_Identify_Process())
		{
			uiProcMenu(UISTR_MENU);
		}
		
		//显示
		
		uiProcMainDraw(TRUE,Parameter);	
		Target_F = 0;
	}
	else if (nKey == UIKEY_DEL)//临时调出重量的数据显示
	{
		//get_weight_clear_value();
		//Auto_Encoder_Demarcate();
		//weight_flag();
		Show_Param();
		System.Device.Lcd_12864.LCD_Init();
		uiProcMainDraw(TRUE,Parameter);

	}

	return 0;

}
void uiProcProtocl(void)
{
	int i;
	u8  xxxx;
	u32 wahh;
	wahh = System.Device.Call_Floor.Remote_Scan();
	

	//if(wahh != 0)
	//{
		//printf("wahh=========%x\r\n",wahh);
	//}
	//过滤多余信息方式
	if((wahh != 0))//&&(wahh < 0x01000000) &&((wahh&0xf)==0xf)&&((wahh&0xffffff)!= 0xffffff))
	{
		//uiLcdDecimal(wahh,3,0,0,7);
		//printf("wahh=====%x\r\n",wahh);
		for(xxxx=0;xxxx<MAX_FLOOR_NUM;xxxx++)
		{
			if(wahh == HB_Floor_Call[xxxx].Call_Time_Count)
			{	
				if(System.Device.IO.HB_Gpio_Get_Value(WTV_BUSY) ==1)
				{
					Voice_Call(HB_Floor_Call[xxxx].Call_num);
				}
				
				
				for(i=0;i<3;i++)
				{
					if(HB_Floor_Call_Buff[i].Call_num == HB_Floor_Call[xxxx].Call_num)//如果已经呼叫了，就不另行显示了
					{
						HB_Floor_Call_Buff[i].Call_Time_Count = uiTimeGetTickCount();
						
						//Voice_Call(HB_Floor_Call[xxxx].Call_num);
						
						return;
					}
				}

				for(i=0;i<3;i++)
				{
					if(HB_Floor_Call_Buff[i].Call_flag==0)
					{
						HB_Floor_Call_Buff[i].Call_flag = 1;
						HB_Floor_Call_Buff[i].Call_num = HB_Floor_Call[xxxx].Call_num; 
						HB_Floor_Call_Buff[i].Call_Time_Count = uiTimeGetTickCount();
						
						//Voice_Call(HB_Floor_Call[xxxx].Call_num);
						
						return;
					}

				}
				break;
			}
		}
	}	
}

void uiProcMain(void)
{
	u8 nKey;
	u8 call_flag=0;//楼层到站的呼叫标志
	u32 Time_tmp;
	u32 Time_tmp1;
	u32 Time_tmp2;
	u32 Time_tmp3;
	u32 Time_tmp4;//用于重量采集
	u32  Floor_CurrentCount_pre;
	u16 ad_temp;
	static  u32 fuck_count=0;
	static  u8  fuck_flag=0;//当前平层值已保存的标志==0是没有 ==1是有
	u32 ywm_tmp;//临时变量
	//static u32	xxxx=0;

	

	Menu_Parameter Parameter;
	Target_F = 0;//初始话楼层
	
	//重置密码使用
	nKey  = uiKeyGetKey();
	switch(nKey)
	{
		case UIKEY_2:		
			Pwd_Enroll_Process();
			break;
		default:
			break;
	}
	//FlushUart2();
	//动态显示"恒邦科技"
	uiLcd_1212_ch(UISTR_HENGBANGKEJI,1,4+12*3,1);
	DelayMs(500);
	uiLcd_1212_ch(UISTR_HENGBANGKEJI+1,1,4+12*4,1);
	DelayMs(500);
	uiLcd_1212_ch(UISTR_HENGBANGKEJI+2,1,4+12*5,1);
	DelayMs(500);
	uiLcd_1212_ch(UISTR_HENGBANGKEJI+3,1,4+12*6,1);
	DelayMs(1000);
	
	uiProcMainDraw(TRUE,&Parameter);
	uiProcMenuBuild();//建立菜单目录

	//消除第一次开机时就显示呼叫楼层问题
	HB_Floor_Call_Buff[0].Call_Time_Count = 3000;
	HB_Floor_Call_Buff[1].Call_Time_Count = 3000;
	HB_Floor_Call_Buff[2].Call_Time_Count = 3000;
	
	Time_tmp =uiTimeGetTickCount();
	Time_tmp1 =uiTimeGetTickCount();
	Time_tmp2 = Time_tmp1;
	Time_tmp3 = Time_tmp2;
	Time_tmp4 = Time_tmp2;
	Floor_CurrentCount_pre =Floor_CurrentCount;//消除开机时的光标闪烁
	while(1)
	{


		//Floor_CurrentCount_pre = Floor_CurrentCount;
		
		nKey  = uiKeyGetKey();//处理按键和楼层
		if(nKey != UIKEY_NONE)
		{	
			if(uiProcKey(nKey,&Parameter) == 1)
			{
				Time_tmp4 = uiTimeGetTickCount();//特殊返回值处理
			}
		
		}
		uiProcProtocl();//楼层呼叫接受处理
		
		 if(uiTimeGetTickCount() -Time_tmp >100)//1000ms更新一次重量值
               	 {
                       	 //以后每隔2s钟上传一次
                       	if(App.Weight.weight_display_change_flg ==1)
			{
				Handle_Weight(&Parameter);//重量值采集和处理
			}
                        	Time_tmp =uiTimeGetTickCount();
			//获取电压的AD值
			ad_temp = ADC_Filter_V();
			//连续1.0s*4=4s左右没有检测到电压时，就保存当前楼层数值
			if(ad_temp <100)//检测到的电压AD值小于100时认为
			{
				fuck_count++;
				if(fuck_count >= 4)
				{
					if(fuck_flag ==0)
					{
						Flash_WriteInfo((u8*)&Floor_CurrentCount,FLOOR_CUR_ADD,4);
						fuck_flag =1;
						//uiLcdDecimal(Floor_CurrentCount,0,7,0,5);
					}
				}
			}
			else
			{
				fuck_count =0;
				fuck_flag =0;
			}
                  }
		if(HB_RELAY_Close_Flag == 1)//存在手柄断开了
		{
			
			if(call_flag==0)
			{
				for(ywm_tmp=0;ywm_tmp<App.Max_floor;ywm_tmp++)
				{
					if(floor_tmp[ywm_tmp].floor_flag == 0)
					{
						continue;
					}
					if(Floor_CurrentCount <=  (floor_tmp[ywm_tmp].floor_count+(floor_tmp[1].floor_diff/2)))
					{
						break;
					}
				}
				WTV_Voice(DINGDONG);
				DelayMs(1000);
				Voice_Call_Num(ywm_tmp+1);
				call_flag=1;
			}
			
			

			if(uiTimeGetTickCount() -Time_tmp1 > 700)//7s后闭合
               	 	{

				System.Device.IO.HB_Gpio_Set_Value(RELAY_3,1);//高速闭合
				
				System.Device.IO.HB_Gpio_Set_Value(RELAY_1,1);//上升
				System.Device.IO.HB_Gpio_Set_Value(RELAY_2,1);//下降
				HB_RELAY_Close_Flag = 0;
				call_flag=0;
				Time_tmp1 =uiTimeGetTickCount();
           
                  	}
		}else
		{
			Time_tmp1 = uiTimeGetTickCount();
		}
			
		//Handle_Master(&Parameter);

		 if(uiTimeGetTickCount() -Time_tmp2 >30)//300ms获取一次标定编码值
               	 {
			
			if(Floor_CurrentCount_pre >Floor_CurrentCount+10)
			{
				//Floor_CurrentCount_diff = Floor_CurrentCount_pre-Floor_CurrentCount;
				uiLcd_1616_ch(4,4,57,1);//显示下降图标
				Parameter.Parameter_Change_Position = 0;
			}
			else if(Floor_CurrentCount_pre < Floor_CurrentCount-10)
			{
				//Floor_CurrentCount_diff = Floor_CurrentCount-Floor_CurrentCount_pre;
				uiLcd_1616_ch(3,4,57,1);//显示上升图标
				Parameter.Parameter_Change_Position = 0;
			}
			else
			{
				uiLcd_1616_ch(5,4,57,1);//显示清空
			}
			
			Time_tmp2 = uiTimeGetTickCount();
		}

		 
		if(uiTimeGetTickCount() -Time_tmp3 >80)//800ms更新一次目标层闪烁
               	 {
			Cursor_Flag= !Cursor_Flag;
			Time_tmp3 = uiTimeGetTickCount();

			Floor_CurrentCount_pre = Floor_CurrentCount;
			
		}

		//uiLcdDecimal(Floor_CurrentCount,0,11,0,5);
		
		if(HB_Start_Flag == 1)
		{
			//按键按下6秒后，如果范围没超过按下是的正负10时，就清零
			if(uiTimeGetTickCount() -Time_tmp4 > 900)//2////9000ms
	               	{

				//Time_tmp4 = uiTimeGetTickCount();
				if((Floor_CurrentCount >(Floor_StartCount-10)) &&(Floor_CurrentCount <(Floor_StartCount+10)) )
				{
					HB_Start_Flag=0;
					HB_RELAY_Flag =0;
					Target_F =0;
				}
			}

		}else
		{
			Time_tmp4 = uiTimeGetTickCount();
		}
		

		
		 
			
		

		uiProcMainDraw(FALSE,&Parameter);//更新显示
		Parameter.Parameter_Change_Flag =0;
		Parameter.Parameter_Change_Position=0;
		Parameter.Show_Flag=0;



	}

}



	




////////以下是被优化处理掉的代码
//20141203从主函数中去掉，优化到void uiProcKey(u8 nKey,Menu_Parameter *Parameter)
/*
			if(uiKeyIsDigit(nKey) )	//判断按键是否是数字
			{
				
				//Target_F += (nKey - '0');
				Target_F = Target_F * 10 + (int)nKey - '0';
				if(Target_F > 99)
				{
					Target_F =  (nKey - '0');
				}
				if(Target_F < 1)
				{
					Target_F =  1;
				}
					
			}
			else if (nKey == UIKEY_UP)//目标楼层加1
			{
				Target_F = Target_F+1;
				if(Target_F > 99)
				{
					Target_F =  99;
				}
			}
			else if (nKey == UIKEY_DOWN)//目标楼层减1
			{
				Target_F = Target_F-1;
				if(Target_F < 1)
				{
					Target_F =  1;
				}
			}
			else if (nKey == UIKEY_ESC)//目标楼层退出
			{
				Target_F = 0;
				//if(Target_F < 1)
				//{
					//Target_F =  1;
				//}
			}
			else if (nKey == UIKEY_OK)//目标楼层启动
			{
				//处理目标楼层启动流程
				get_weight_clear_value();
				
			}
			else if (nKey == UIKEY_MENU)
			{			

				//验证密码
				if(Pwd_Identify_Process())
				{
					uiProcMenu(UISTR_MENU);
				}
				
				//显示
				
				uiProcMainDraw(TRUE,&Parameter);	
			}
*/
/*


*/

