
#include "system.h"
#include "app.h"

#define HEAD_LEN  16	//²Ëµ¥À¸Ä¿Ã¿ÐÐÏÔÊ¾Ô¤Áô
#define FLAG_ROW	6
#define FLAG_COL_FOUR	0//ËÄ¸ö×ÖÊ±ÖÐ¼äÏÔÊ¾
#define FLAG_COL_FIVE	44//Îå¸ö×ÖÊ±ÏÔÊ¾
#define FLAG_COL_SIX	44//Áù¸ö×ÖÊ±ÏÔÊ¾


static u8 fuck_count =0;//Á¬ÐøÈý´Î³¬ÖØ¾Í»á¹Ø±Õ

extern const T_UI_MENUITEM g_uiMenuItemsSrc[MAX_UI];

T_UI_MENUITEM	g_uiMenuItems[sizeof(g_uiMenuItemsSrc) / sizeof(g_uiMenuItemsSrc[0])];


//ÓÃÓÚ×´Ì¬×Ô¼ìÏÔÊ¾ÐÅÏ¢
void Machine_Info(void)
{
	u8 nKey;
	uiLcdClear();
	//ÏÔÊ¾Éè±¸ÐÅÏ¢
	//uiLcdSmallString("Kg", row*2+1, 19, 0);
	uiLcdMediumString("V1.0.150204", 0, 0, 0);//20150204Åú´Î

	uiLcdSmallString("TEL:0579-85232071", 7, 0, 0);//20150204Åú´Î

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
//ÏÔÊ¾¶ÁÈ¡»ØÀ´µÄ×´Ì¬Öµ£¬ÈçÃÅÏÞ£¬ÔØÖØµÈ
u8 uiProcShowFlag(u16 flag)
{
//	u8 j;


	uiLcdLineErase8x16(3,5,14,0);//½«ÏÈÇ°×´Ì¬²Á³ý
	
	if(flag == Input_invalid)
	{
		//ÓÃÓÚÏÔÊ¾Éè±¸×´Ì¬	
		uiLcd_1212_ch(UISTR_DEVICE_IS_OK,FLAG_ROW,FLAG_COL_FOUR,4);//Éè±¸Õý³£
		return 0;
	}
	//
	if(flag&Input_invalid_1)//½øÁÏÃÅÎ´¹Ø
	{
		uiLcd_1212_ch(UISTR_FLAG_IN_DOOR,FLAG_ROW,FLAG_COL_FOUR,5);

		
		return 1;
	}
	if(flag&Input_invalid_2)//³öÁÏÃÅÎ´¹Ø
	{
		
		uiLcd_1212_ch(UISTR_FLAG_OUT_DOOR,FLAG_ROW,FLAG_COL_FOUR,1);
		uiLcd_1212_ch(UISTR_FLAG_IN_DOOR+1,FLAG_ROW,FLAG_COL_FOUR+12,4);
		return 1;
	}
	if(flag&Input_invalid_3)//Ìì´°Î´¹Ø
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
void uiProcMainDraw(BOOL Initialise,Menu_Parameter *Parameter)	/*´ý»ú½çÃæµÄÏÔÊ¾*/
{
	int ywm;
	if (Initialise)
	{
		uiLcdClear();
		//uiLcdSmallString("whatbook",0,0,0);
		
		//uiLcd_1212_ch(UISTR_HENGBANGKEJI,0,4+12*3,4);
		
		//uiLcd_1212_ch(UISTR_CURRENT_LOU_CHENG,0+2,4,4);//ÏÔÊ¾¡°µ±Ç°Â¥²ã¡±
		//uiLcd_1212_ch(UISTR_CURRENT_LOU_CHENG+3,0+2,4+12*2,1);//ÏÔÊ¾¡°µ±Ç°Â¥²ã¡±
		uiLcd_1414_ch(UISTR_XXX_DANGQIAN, 2, 4, 2);
		uiLcd_1414_ch(UISTR_XXX_HUJIAOLOUCENG+2, 2, 4+14*2, 2);

		
		uiLcd_1212_ch(UISTR_AIM_LOU_CHENG,0+2,76,4);//ÏÔÊ¾¡°Ä¿±êÂ¥²ã¡±
		//uiLcd_1212_ch(UISTR_AIM_LOU_CHENG+3,0+2,76+12*2,1);//ÏÔÊ¾¡°µ±Ç°Â¥²ã¡
		uiLcd_1414_ch(UISTR_XXX_MUBIAO, 2, 70, 2);
		uiLcd_1414_ch(UISTR_XXX_HUJIAOLOUCENG+2, 2, 70+14*2, 2);

		
		uiLcdMediumString("00",0+2,3,0);//ÏÔÊ¾¡°µ±Ç°Â¥²ã¡±Êý×Ö
		uiLcdMediumString("11",0+2,12,1);//ÏÔÊ¾¡°Ä¿±êÂ¥²ã¡±Êý×Ö
		
		//uiLcd_1212_ch(UISTR_WEIGHT,4,4,3);//ÏÔÊ¾¡°µõÖØ£º¡±
		uiLcd_1616_ch(0,2+4,0,2);//ÏÔÊ¾¡°ÖØÁ¿¡±
		//uiLcd_1414_ch(UISTR_XXX_ZAIZHONG_SET_ZHONGLIANGZHILING, 4+2, 4, 2);
		uiLcdMediumString(":",1+2,4,0);//Ã°ºÅ
		
		uiLcdMediumString("0.00",1+2,5,0);//Õâ¸öÊýÖµÌîµõÖØÊµÊ±Êý¾Ý
		//uiLcdMediumString("T",1+2,9,0);
		//ÏÔÊ¾  ¶Ö
		uiLcd_1616_ch(11,6,74,1);
		
		uiLcdMediumString("100",3,12,0);//Õâ¸öÊýÖµÌîµõÖØ°Ù·Ö±È
		uiLcdMediumString("%",3,15,0);
		
		//ÓÃÓÚÏÔÊ¾ºô½ÐÂ¥²ã
		//uiLcd_1414_ch(UISTR_XXX_HUJIAOLOUCENG, 0, 4, 4);

		uiLcd_1616_ch(8,0,2,3);//ÏÔÊ¾  ºô½ÐÂ¥²ã
		uiLcdMediumString(":",0,6,0);//Ã°ºÅ

		//uiLcd_1416_ch(0,0,2,4);
		return ;
	}


	 //ÏÔÊ¾ºô½ÐµÄÂ¥²ãÊý
	for(ywm=0;ywm<3;ywm++)
	{
		if(uiTimeGetTickCount() - HB_Floor_Call_Buff[ywm].Call_Time_Count < 3000)//3//3sÄÚÏÔÊ¾
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
			uiLcdLineErase8x16(0,8+(ywm*3),8+(ywm*3)+3,0);//½«ÏÈÇ°×´Ì¬²Á³ý
		}
	}
	
	//ÏÔÊ¾Ä¿±êÂ¥²ã
	//uiLcdDecimal(Target_F,0+2,12,0,2);
	if((Cursor_Flag == 1)&&(Target_F!=0))
	{
		uiLcdDecimal(Target_F,0+2,12,1,2);
	}
	else
	{
		uiLcdDecimal(Target_F,0+2,12,0,2);
	}

	//ÏÔÊ¾µ±Ç°Â¥²ã
	for(ywm=0;ywm<App.Max_floor-1;ywm++)
	{
		if(Floor_CurrentCount < (floor_tmp[ywm].floor_count+(floor_tmp[1].floor_diff/2)))
		{
			uiLcdDecimal(ywm+1,0+2,3,0,2);
			break;
		}
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

			//ÖØÁ¿°Ù·Ö±È
			uiLcdDecimal(Parameter->Weight_Percentage,1+2,12,0,3);
			
			//ÏÔÊ¾°Ù·Ö±È
			//uiLcdDecimal(Parameter->Weight_Percentage,2,11,0,3);
			if(Parameter->Show_Flag== 1)
			{
				uiLcdMediumString("-",1+2,4,0);
			}
			else
			{
				uiLcdMediumString(":",1+2,4,0);//Ã°ºÅ
			}
		}
	/*	
		if((Parameter->Parameter_Change_Position)&(SHOWFLAG))
		{
			//ÓÃÓÚÏÔÊ¾Éè±¸×´Ì¬	
			//uiProcShowFlag(App.Input_Data);
			//uiLcdLineErase8x16(3,0,10,0);//½«ÏÈÇ°×´Ì¬²Á³ý
			switch(Parameter->Show_Flag)
			{
				case CMD_FLOOR_DATA://Â¥²ãºô½ÐÏÔÊ¾
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
					uiLcd_1616_ch(5,2,44,1);//ÏÔÊ¾Çå¿Õ
					break;
				case MASTER_DOWN:
					uiLcd_1616_ch(4,2,44,1);//ÏÔÊ¾ÏÂ½µÍ¼±ê
					break;
				case MASTER_UP:
					uiLcd_1616_ch(3,2,44,1);//ÏÔÊ¾ÉÏÉýÍ¼±ê
					break;
				case CMD_START_ON://ÒÑÆô¶¯
					uiLcd_1212_ch(UISTR_START,FLAG_ROW,FLAG_COL_FIVE+50,3);
					break;
				case CMD_START_OFF://Î´Æô¶¯
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
	

	//»ñÈ¡µçÑ¹µÄADÖµ
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
	

	//»ñÈ¡ÖØÁ¿µÄADÖµ
	Weight_Value = ADC_Filter_T();
	//printf("App.Weight_Tmp===%d\r\n",App.Weight_Tmp);
			
	if(App.Weight.weight_clear_ad_value_sign == 0)//ÎªÕý
	{
		if(Weight_Value >= App.Weight.weight_clear_ad_value)
	         {
	                  ad_temp=Weight_Value-App.Weight.weight_clear_ad_value;
	         }
	         else
	         {
	               	ad_temp=App.Weight.weight_clear_ad_value-Weight_Value;//ÏÔÊ¾¸º
	                  //weight_display_sign=1;
			//Parameter->Show_Flag = 1;//ÏÔÊ¾¸ººÅÌáÊ¾
		}

	}
	if(App.Weight.weight_clear_ad_value_sign == 1)//Îª¸º
	{

		ad_temp=Weight_Value+App.Weight.weight_clear_ad_value;
	}

	operate_temp=ad_temp;
	ad_temp=(operate_temp * App.Weight.calibrate_sub_value)/App.Weight.calibrate_ad_sub_value;//µÃµ½×îÖÕµÄÖØÁ¿Öµ	

	//ÁÙÊ±ÏÔÊ¾¶ÁÈ¡µÄÖØÁ¿Öµ
	//uiLcdDecimal(ad_temp,1+2, 0,0,4);
	//³ýÈ¥¿ÕÁýÔØÖØ
	if(ad_temp >App.Weight.Empty_weight)
	{
		ad_temp = ad_temp-App.Weight.Empty_weight;
	}
	else
	{
		//ad_temp=0;
		ad_temp = App.Weight.Empty_weight -ad_temp;
		Parameter->Show_Flag = 1;//ÏÔÊ¾¸ººÅÌáÊ¾
	}

	
	//ÔÚÕâÀï½øÐÐ±¨¾¯ºÍÔ¤¾¯ÅÐ¶Ï
	if(ad_temp >(App.Weight.Rated_weight*App.Weight.Warning_weight/100))//³¬¹ýÔ¤¾¯Öµ
	{
		uiLcdLineErase8x16(2,0,3,0);//½«ÏÈÇ°×´Ì¬²Á³ý
		//ÌáÊ¾Ô¤¾¯
		uiLcd_1212_ch(UISTR_ZAIZHONG_DAODAYUJINGZHI+2, 4,1, 2);
		
		if(ad_temp >(App.Weight.Rated_weight*App.Weight.Alarm_weight/100))//³¬¹ý±¨¾¯Öµ
		{
			//ÌáÊ¾±¨¾¯
			uiLcdLineErase8x16(2,0,3,0);
			uiLcd_1212_ch(UISTR_ZAIZHONG_YICAOZAI+1, 4, 1, 2);
		
			fuck_count++;
			if(fuck_count >=2)//Á¬ÐøÁ½´Î¾Í¶Ï¿ª
			{
				System.Device.IO.HB_Gpio_Set_Value(RELAY_4,0);//³¬ÔØ´ò¿ª
			}
		}else
		{
			fuck_count=0;
			System.Device.IO.HB_Gpio_Set_Value(RELAY_4,1);//³¬ÔØ±ÕºÏ
		}
	}
	else
	{
		uiLcdLineErase8x16(2,0,3,0);//½«ÏÈÇ°×´Ì¬²Á³ý
		fuck_count=0;
		System.Device.IO.HB_Gpio_Set_Value(RELAY_4,1);//³¬ÔØ±ÕºÏ
	}

	//if(){}
	//ÏÔÊ¾°Ù·Ö±È
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
/*
static u8 Handle_ERR_Code(Menu_Parameter *Parameter)
{
	u16	Err_Code;
	Err_Code =  fram_data_buff[3];			
	Err_Code = (Err_Code << 8) |fram_data_buff[2];

	//printf("App.Input_Data==%x\r\n",App.Input_Data);
	//printf("Err_Code==%x\r\n",Err_Code);

	//ÁÙÊ±¼ÓÈë
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
//´¦ÀíÖ÷ÁîÉ¨Ãè

/*
static u8  Handle_Master(Menu_Parameter *Parameter)
{

	u8 Scan_value ;
	App.Master_Flag = System.Device.IO.ScanPin();//É¨ÃèÖ÷ÁîÖµ
	DelayMs(10);//È¥¶¶
	Scan_value = System.Device.IO.ScanPin();//É¨ÃèÖ÷ÁîÖµ

	if(App.Master_Flag != Scan_value)
	{
		return 0;
	}
	else 
	{
		
	}

	if(master_pre_value ==  App.Master_Flag)
	{
		//return 0;//ÓÉÓÚÁ¬Ðø·¢ËÍÎå´Î£¬µ¼ÖÂÓïÒô²¥·ÅÔÓÒôÎÊÌâ£¬ÔÝÊ±Ö»·¢ËÍÒ»´Î
		
		tmpxxx++;
		if(tmpxxx == 2)//Á¬ÐøÎå´ÎÏàµÈ£¬¾Í²»·¢ËÍ
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

	if(App.Master_Flag ==  0xfe)//µÚÒ»Î»ÏÂ½µ
	{
		
		HB_Send_Realy_CMD(CMD_RELAY_DOWN);
		Parameter->Show_Floor = MASTER_DOWN;
		uiLcdDecimal(1,3,15,0,1);
		WTV_Voice(MASTER_DOWM_FLAG);
		
	}
	else if(App.Master_Flag ==  0xfd)//µÚ¶þÎ»ÉÏÉý
	{
		HB_Send_Realy_CMD(CMD_RELAY_UP);
		Parameter->Show_Floor = MASTER_UP;
		uiLcdDecimal(2,3,15,0,1);


		WTV_Voice(MASTER_UP_FLAG);
		
	}
	else if(App.Master_Flag ==  0xfb)//µÚÈýÎ»Í£Ö¹
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
//ÅÐ¶Ï±¨ÎÄÊÇ·ñÐ£ÑéºÍÕýÈ·,pRep±£´æµÄ±¨ÎÄÊÇÆþÍ·È¥Î²
char   Cmd_Rep_valid(char*  pRep,char* plen)
{
	char i = 0,j = 0;
	//char sum = PROTOCOL_HEAD_1 + PROTOCOL_HEAD_2;
	char sum = 0;
	////·ÖÎö±¨ÎÄ£¬½«×ªÒå·û´¦Àíµô
    	for(i=0;i<(*plen-1);)//×îºóÒ»¸öÊÇÐ£ÑéºÍ£¬²»ÓÃÅÐ¶Ï
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
	///ÅÐ¶ÏÐ£ÑéºÍ
	if(sum != fram_data_buff[j])
		return 0;
	*plen = fram_data_buff[1]+3;//Í·+Êý¾Ý³¤¶È+Êý¾Ý+Ð£ÑéºÍ
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
	{	//Ë«×Ö½Ú×ÖÌå
		uiLcdMultiString(pUiMenuItem->nMenuStr, row, 1, FALSE);
	}
	else
	{
		//µ¥×Ö½Ú×ÖÌå
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
		case UISTR_MENU_ZAIZHONG_SET_BAOJINGZAIHE://±¨¾¯ÔØºÉ
			nValue = uiProcBoxNumber(&bRet, row, 12, App.Weight.Alarm_weight, 1, 120, 3, 3, TRUE);
			if (bRet) 
			{
				//gsSetting.bLockEn = nValue;
				App.Weight.Alarm_weight= (u8)nValue;
			}
                		break;
		case UISTR_MENU_ZAIZHONG_SET_ZHONGLIANGZHILING://ÖØÁ¿ÖÃÁã
			nValue = uiProcBoxNumber(&bRet, row, 11, App.Weight.weight_display_change_flg, 0, 1, 1, 1, TRUE);
			if (bRet) 
			{
				//gsSetting.bLockEn = nValue;
				App.Weight.weight_display_change_flg= (u8)nValue;
			}
                		break;
		case UISTR_MENU_ZAIZHONG_SET_KONGZAIZHONGLIANG://¿ÕÔØÖØÁ¿
			nValue = uiProcBoxNumber(&bRet, row, 10, App.Weight.Empty_weight, 1, 2000, 4, 4, TRUE);
			if (bRet) 
			{
				
				App.Weight.Empty_weight= nValue;
			}
                		break;
						

		case UISTR_MENU_PINGCHENG_SET_UP_BUCHANG://ÉÏÉý²¹³¥
			nValue = uiProcBoxNumber(&bRet, row, 12, App.Up_buchang, 0, 9999, 4, 4, TRUE);
			if (bRet) 
			{
				
				App.Up_buchang= nValue;
			}
                		break;
		case UISTR_MENU_PINGCHENG_SET_DOWN_BUCHANG://ÏÂ½µ²¹³¥
			nValue = uiProcBoxNumber(&bRet, row, 12, App.Down_buchang, 0, 9999, 4, 4, TRUE);
			if (bRet) 
			{
				
				App.Down_buchang= nValue;
			}
                		break;
		case UISTR_MENU_PINGCHENG_SET_OTHER_BUCHANG://ÆäËû²¹³¥
			nValue = uiProcBoxNumber(&bRet, row, 12, App.Other_buchang,0, 9999, 4, 4, TRUE);
			if (bRet) 
			{
				
				App.Other_buchang= nValue;
			}
                		break;
		case UISTR_MENU_PINGCHENG_SET_MAX_FLOOR://×î´óÂ¥²ã
			nValue = uiProcBoxNumber(&bRet, row, 13, App.Max_floor,1, 99, 2, 2, TRUE);
			if (bRet) 
			{
				
				App.Max_floor= (u8)nValue;
			}
                		break;
/*		case UISTR_MENU_PINGCHENG_SET_MAX_FLOOR_COUNT://ÓÉ¸ßËÙÇÐ»»µ½µÍËÙµÄ±àÂëÖµ
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
		case UISTR_MENU_ZAIZHONG_SET_BIAODINGDIAN_ONE://ÉèÖÃ±ê¶¨µãÒ»
			//Load_Set_Calibration(1);
			if(Load_Set_Calibration_tmp(1) == 1)
			{
				get_weight_clear_value();
			}
		
			break;
		case UISTR_MENU_ZAIZHONG_SET_BIAODINGDIAN_TWO://ÉèÖÃ±ê¶¨µã¶þ
			//if(Load_Set_Calibration(2) == 1)
			if(Load_Set_Calibration_tmp(2) == 1)
			{
				get_weight_clear_value();
			}
		
			
			break;
		case UISTR_MENU_PINGCHENG_SET_UI://Æ½²ã±ê¶¨½çÃæ
			Encoder_Demarcate();

			//Encoder_Demarcate_Init();
			break;
		case UISTR_MENU_PINGCHENG_QUANSHAN://Æ½²ã±ê¶¨É¾³ý
			Encoder_Demarcate_Del();

			//Encoder_Demarcate_Init();
			break;
		case UISTR_MENU_PINGCHENG_SET_UI_ZIDONG://×Ô¶¯Æ½²ã±ê¶¨½çÃæ
			Encoder_Demarcate_Init();
			break;
		case UISTR_MENU_PINGCHENG_ZIDONGBUCANG://×Ô¶¯²¹³¥
			Auto_Encoder_Demarcate();
			break;
			
		case UISTR_MENU_CALL_SET_UI://Â¥²ãÑ§Ï°½çÃæ
			Call_Learn();
			break;
		case UISTR_MENU_CALL_SET_DEL://Â¥²ãÉ¾³ý½çÃæ
			Call_Del();
			break;

		case UISTR_MENU_ZIJIAN_SET://×´Ì¬×Ô¼ì¾ÍÊÇÏÔÊ¾µ±Ç°ÖØÁ¿Öµ
			Machine_Info();
			
			break;
		default:
			break;
	}
}

//////////////////////////////////////////////////////////////////////////
//»­ÓÐÖµÒªÏÔÊ¾µÄ²¿·Ö
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
		case UISTR_MENU_ZAIZHONG_SET_KONGZAIZHONGLIANG://¿ÕÔØÖØÁ¿
			uiLcdDecimal(App.Weight.Empty_weight, row, 10, 0, 4);
			uiLcdSmallString("Kg", row*2+1, 19, 0);
			break;

		//Æ½²ã²¿·Ö
		case UISTR_MENU_PINGCHENG_SET_UP_BUCHANG:
			uiLcdDecimal(App.Up_buchang, row, 12, 0, 4);
		
			break;
		case UISTR_MENU_PINGCHENG_SET_DOWN_BUCHANG:
			uiLcdDecimal(App.Down_buchang, row, 12, 0, 4);
		
			break;
		case UISTR_MENU_PINGCHENG_SET_OTHER_BUCHANG:
			uiLcdDecimal(App.Other_buchang, row, 12, 0, 4);
			
			break;
		case UISTR_MENU_PINGCHENG_SET_MAX_FLOOR:
			uiLcdDecimal(App.Max_floor, row, 13, 0, 2);
			
			break;
		
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
	//ÏÔÊ¾²Ëµ¥Ö÷Ìâ
	switch((pUiStartMenuItem-1)->nMenuStr)//ÕÒtype¶ÔÓ¦µÄÖµ
	{
		
		case UISTR_MENU:
			//i = UISTR_MENU_PINGCHENG_SET;
			uiLcd_1212_ch(UISTR_ZHUSHEZHICAIDAN, 0, 40, 5);
			
			break;
		case UISTR_MENU_ZAIZHONG_SET:
			uiLcd_1212_ch(UISTR_ZAIZHONG_SET, 0, 40, 4);
			//i = UISTR_MENU_PINGCHENG_SET;
			break;

		case UISTR_MENU_PINGCHENG_SET:
			uiLcd_1212_ch(UISTR_PINGCHENG_SET, 0, 40, 4);
			//i = UISTR_MENU_PINGCHENG_SET;
			break;
		case UISTR_MENU_CALL_SET:
			uiLcd_1212_ch(UISTR_ENCODER_FLOOR+2, 0, 40, 2);//Â¥²ã
			uiLcd_1212_ch(UISTR_CALL_SET, 0, 40+24, 2);//Ñ§Ï°
			//i = UISTR_MENU_PINGCHENG_SET;
			break;
		default:

			break;
	}
	//ÕâÀïÏÔÊ¾Ö÷²Ëµ¥
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

	for(i=0; i<3; i++)//Ñ­»·ÏÔÊ¾Ò»¸öÖ÷ÌâÏÂµÄÈý¸öÏîÄ¿
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
		//ÔÚÕâÀïÌí¼Ó²Ëµ¥Ä¿Â¼µÄ»æ»­
		switch(pUiStartMenuItem[nIndex].nMenuStr)//ÕÒtype¶ÔÓ¦µÄÖµ
		{
			//Ö÷²Ëµ¥ÏÂµÄÏÔÊ¾
			case UISTR_MENU_PINGCHENG_SET:
				//uiLcdSmallString("1:",0,0,1);
				uiLcd_1212_ch(UISTR_PINGCHENG_SET, (1+i)*2, HEAD_LEN, 4);
				break;
			case UISTR_MENU_ZAIZHONG_SET:
				//uiLcdSmallString("2:",2,0,1);
				uiLcd_1212_ch(UISTR_ZAIZHONG_SET, (1+i)*2, HEAD_LEN, 4);
				break;
			case UISTR_MENU_MIMA_SET:
				//uiLcdSmallString("3:",4,0,1);
				uiLcd_1212_ch(UISTR_MIMA_SET, (1+i)*2, HEAD_LEN, 4);
				break;
			case UISTR_MENU_CALL_SET:
				//uiLcdSmallString("3:",4,0,1);
				uiLcd_1212_ch(UISTR_ENCODER_FLOOR+2, (1+i)*2, HEAD_LEN, 2);//Â¥²ã
				uiLcd_1212_ch(UISTR_CALL_SET, (1+i)*2, HEAD_LEN+24, 2);//Ñ§Ï°
				break;
			case UISTR_MENU_ZIJIAN_SET:
				//uiLcdSmallString("4:",6,0,1);
				uiLcd_1212_ch(UISTR_ZHAUNTAIZIJIAN_SET, (1+i)*2, HEAD_LEN, 4);
				break;

			//Æ½²ãÉèÖÃÏÂµÄÏÔÊ¾
			case UISTR_MENU_PINGCHENG_SET_UI://ÊÖ¶¯Æ½²ã±ê¶¨½çÃæ
				//uiLcd_1414_ch(UISTR_XXX_SHOUDONG, (1+i)*2, HEAD_LEN, 2);
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UI, (1+i)*2, HEAD_LEN, 6);
				break;
			case UISTR_MENU_PINGCHENG_ZIDONGBUCANG://×Ô¶¯²¹³¥
				uiLcd_1414_ch(UISTR_XXX_ZIDONG, (1+i)*2, HEAD_LEN, 2);
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UP_BUCHANG+2, (1+i)*2, HEAD_LEN+14*2, 2);
				break;
			case UISTR_MENU_PINGCHENG_SET_UI_ZIDONG://×Ô¶¯Æ½²ã±ê¶¨½çÃæ
				uiLcd_1414_ch(UISTR_XXX_ZIDONG, (1+i)*2, HEAD_LEN, 2);
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UI, (1+i)*2, HEAD_LEN+14*2, 6);
				break;
			case UISTR_MENU_PINGCHENG_SET_UP_BUCHANG:
				
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UP_BUCHANG, (1+i)*2, HEAD_LEN, 4);
				break;
			case UISTR_MENU_PINGCHENG_SET_DOWN_BUCHANG:
			
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_DOWN_BUCHANG, (1+i)*2, HEAD_LEN, 4);
				break;
			case UISTR_MENU_PINGCHENG_SET_OTHER_BUCHANG:
			
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_OTHER_BUCHANG, (1+i)*2, HEAD_LEN, 4);
				break;
			case UISTR_MENU_PINGCHENG_SET_MAX_FLOOR:
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_MAX_FLOOR, (1+i)*2, HEAD_LEN, 4);
				break;
			case UISTR_MENU_PINGCHENG_QUANSHAN://Æ½²ã±ê¶¨È«É¾
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UI, (1+i)*2, HEAD_LEN, 4);
				uiLcd_1414_ch(UISTR_XXX_QUANBU, (1+i)*2, HEAD_LEN+14*4,1);
				uiLcd_1414_ch(UISTR_XXX_SHANCHU, (1+i)*2, HEAD_LEN+14*5,1);
				break;
				
			//ÔØÖØÉèÖÃÏÂµÄÏÔÊ¾
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
				
			case UISTR_MENU_ZAIZHONG_SET_BIAODINGDIAN_ONE://ÉèÖÃ±ê¶¨µãÒ»,Êµ¼ÊÊÇÖØÁ¿ÖÃÁã
				//uiLcd_1212_ch(UISTR_ZAIZHONG_SET_ZHONGLIANGZHILING, (1+i)*2, HEAD_LEN, 4);
				uiLcd_1414_ch(UISTR_XXX_ZAIZHONG_SET_ZHONGLIANGZHILING, (1+i)*2, HEAD_LEN, 4);
				break;
			case UISTR_MENU_ZAIZHONG_SET_BIAODINGDIAN_TWO://ÉèÖÃ±ê¶¨µã¶þ
				//uiLcd_1212_ch(UISTR_ZAIZHONG_SET_ZHONGLIANGZHILING, (1+i)*2, HEAD_LEN, 4);
				uiLcd_1414_ch(UISTR_XXX_ZAIZHONG_SET_BIAODINGDIAN_TWO, (1+i)*2, HEAD_LEN, 5);
				break;		
			case UISTR_MENU_ZAIZHONG_SET_ZHONGLIANGZHILING:
				//uiLcd_1212_ch(UISTR_ZAIZHONG_SET_ZHONGLIANGZHILING, (1+i)*2, HEAD_LEN, 4);
				uiLcd_1414_ch(UISTR_XXX_SHIFOU, (1+i)*2, HEAD_LEN, 2);
				uiLcd_1414_ch(UISTR_XXX_SHIYONG, (1+i)*2, HEAD_LEN+14*2, 2);
				break;
			case UISTR_MENU_ZAIZHONG_SET_KONGZAIZHONGLIANG://¿ÕÔØÖØÁ¿
				//uiLcd_1212_ch(UISTR_ZAIZHONG_SET_ZHONGLIANGZHILING, (1+i)*2, HEAD_LEN, 4);
				uiLcd_1414_ch(UISTR_XXX_KONGZAI, (1+i)*2, HEAD_LEN, 2);
				uiLcd_1414_ch(UISTR_XXX_ZAIZHONG_SET_EDINGZHONGLIANG+2, (1+i)*2, HEAD_LEN+14*2, 2);
				break;

			//Â¥²ãÑ§Ï°ÏÂµÄÏÔÊ¾
			case UISTR_MENU_CALL_SET_UI://Â¥²ãÑ§Ï°½çÃæ
			
				uiLcd_1414_ch(UISTR_XXX_HUJIAOLOUCENG+2, (1+i)*2, HEAD_LEN, 2);
				uiLcd_1414_ch(UISTR_XXX_XUEXI, (1+i)*2, HEAD_LEN+14*2, 2);
				uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UI+4, (1+i)*2, HEAD_LEN+14*4, 2);
				break;
			case UISTR_MENU_CALL_SET_DEL://Â¥²ãÉ¾³ý½çÃæ
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


//±£´æÏµÍ³²ÎÊý
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
	pUiStartMenuItem++;//¼Ó1

	pUiEndMenuItem = pUiStartMenuItem;
	while ((int)pUiEndMenuItem->nType < 0)//ÕÒµÚ½áÊøÎ»ÖÃ
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
				if (pUiStartMenuItem[nCurrentIndex].nType == UIMENU_POPUP)//´¦ÀíÓÐÏÂÀ­²Ëµ¥µÄÑ¡Ïî
				{
					
					uiProcMenu(pUiStartMenuItem[nCurrentIndex].nMenuStr);
					
				}
				else if(pUiStartMenuItem[nCurrentIndex].nType == UIMENU_HASVALUE)//ÓÐÖµ¸Ä±äµÄÇé¿ö´¦Àí
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

void Voice_Call(u8 call_floor)
{

	u8 shiwei;
	u8 gewei;
	if(call_floor < 1)
	{
		return;
	}
	if((call_floor > 0) && (call_floor < 11))//ok
	{
		WTV_Voice(ONE_FLAG-1+call_floor);
		DelayMs(400);
		WTV_Voice(CENG_FALG);
		//DelayMs(300);
	}else if((call_floor > 10) && (call_floor < 20))//ok
	{
		WTV_Voice(TEN_FLAG);
		DelayMs(500);
		WTV_Voice(ONE_FLAG-1+call_floor-10);
		DelayMs(400);
		WTV_Voice(CENG_FALG);
		//DelayMs(300);
		
	}else if(call_floor == 20)//ok
	{
		WTV_Voice(TWO_FLAG);
		DelayMs(500);
		WTV_Voice(TEN_FLAG);
		DelayMs(400);
		WTV_Voice(CENG_FALG);
		//DelayMs(300);
	}else if((call_floor > 20) && (call_floor < 30))
	{
		WTV_Voice(TWO_FLAG);
		DelayMs(500);
		WTV_Voice(TEN_FLAG);
		DelayMs(500);
		WTV_Voice(ONE_FLAG-1+call_floor-20);
		DelayMs(400);
		WTV_Voice(CENG_FALG);
		//DelayMs(300);
	}else if(call_floor == 30)
	{
		WTV_Voice(THREE_FLAG);
		DelayMs(500);
		WTV_Voice(TEN_FLAG);
		DelayMs(400);
		WTV_Voice(CENG_FALG);
		//DelayMs(300);
	}else if((call_floor > 30) && (call_floor < 40))
	{
		WTV_Voice(THREE_FLAG);
		DelayMs(500);
		WTV_Voice(TEN_FLAG);
		DelayMs(500);
		WTV_Voice(ONE_FLAG-1+call_floor-30);
		DelayMs(400);
		WTV_Voice(CENG_FALG);
		//DelayMs(300);
	}else if(call_floor == 40)
	{
		WTV_Voice(FOUR_FLAG);
		DelayMs(500);
		WTV_Voice(TEN_FLAG);
		DelayMs(400);
		WTV_Voice(CENG_FALG);
		//DelayMs(300);
	}else if((call_floor > 40) && (call_floor < 50))
	{
		WTV_Voice(FOUR_FLAG);
		DelayMs(500);
		WTV_Voice(TEN_FLAG);
		DelayMs(500);
		WTV_Voice(ONE_FLAG-1+call_floor-40);
		DelayMs(400);
		WTV_Voice(CENG_FALG);
		//DelayMs(300);
	}else if(call_floor == 50)
	{
		WTV_Voice(FIVE_FLAG);
		DelayMs(500);
		WTV_Voice(TEN_FLAG);
		DelayMs(400);
		WTV_Voice(CENG_FALG);
		//DelayMs(300);
	}
	else if((call_floor > 50) && (call_floor < 60))
	{
		WTV_Voice(FIVE_FLAG);
		DelayMs(500);
		WTV_Voice(TEN_FLAG);
		DelayMs(500);
		WTV_Voice(ONE_FLAG-1+call_floor-50);
		DelayMs(400);
		WTV_Voice(CENG_FALG);
		//DelayMs(300);
	}else
	{

	}

/*

	switch(call_floor)
	{
		case 1:
			WTV_Voice(ONE_FLAG);
			break;
		case 2:
			WTV_Voice(TWO_FLAG);
			break;
		case 3:
			WTV_Voice(THREE_FLAG);
			break;
		case 4:
			WTV_Voice(FOUR_FLAG);
			break;
		case 5:
			WTV_Voice(FIVE_FLAG);
			break;
		case 6:
			WTV_Voice(SIX_FLAG);
			break;
		case 7:
			WTV_Voice(SEVEN_FLAG);
			break;
		case 8:
			WTV_Voice(EGHIT_FLAG);
			break;
		case 9:
			WTV_Voice(NINE_FLAG);
			break;
		case 10:
			WTV_Voice(TEN_FLAG);
			break;
		case 11:
			WTV_Voice(TEN_FLAG);
			DelayMs(500);
			WTV_Voice(ONE_FLAG);
			break;
		case 12:
			WTV_Voice(TEN_FLAG);
			DelayMs(500);
			WTV_Voice(TWO_FLAG);
			break;
		case 13:
			WTV_Voice(TEN_FLAG);
			DelayMs(500);
			WTV_Voice(THREE_FLAG);
			break;
		case 14:
			WTV_Voice(TEN_FLAG);
			DelayMs(500);
			WTV_Voice(FOUR_FLAG);
			break;
		case 15:
			WTV_Voice(TEN_FLAG);
			DelayMs(500);
			WTV_Voice(FIVE_FLAG);
			break;
		case 16:
			WTV_Voice(TEN_FLAG);
			DelayMs(500);
			WTV_Voice(SIX_FLAG);
			break;
		case 17:
			WTV_Voice(TEN_FLAG);
			DelayMs(500);
			WTV_Voice(SEVEN_FLAG);
			break;
		case 18:
			WTV_Voice(TEN_FLAG);
			DelayMs(500);
			WTV_Voice(EGHIT_FLAG);
			break;
		case 19:
			WTV_Voice(TEN_FLAG);
			DelayMs(500);
			WTV_Voice(NINE_FLAG);
			break;
		case 20:
			WTV_Voice(TWO_FLAG);
			DelayMs(500);
			WTV_Voice(TEN_FLAG);
			break;
		case 21:
			WTV_Voice(TWO_FLAG);
			DelayMs(500);
			WTV_Voice(TEN_FLAG);
			DelayMs(500);
			WTV_Voice(ONE_FLAG);
			break;
		case 22:
			WTV_Voice(TWO_FLAG);
			DelayMs(500);
			WTV_Voice(TEN_FLAG);
			DelayMs(500);
			WTV_Voice(TWO_FLAG);
			break;
		case 23:
			WTV_Voice(TWO_FLAG);
			DelayMs(500);
			WTV_Voice(TEN_FLAG);
			DelayMs(500);
			WTV_Voice(THREE_FLAG);
			break;
		case 24:
			WTV_Voice(TWO_FLAG);
			DelayMs(500);
			WTV_Voice(TEN_FLAG);
			DelayMs(500);
			WTV_Voice(FOUR_FLAG);
			break;
		case 25:
			WTV_Voice(TWO_FLAG);
			DelayMs(500);
			WTV_Voice(TEN_FLAG);
			DelayMs(500);
			WTV_Voice(FIVE_FLAG);
			break;
		case 26:
			WTV_Voice(TWO_FLAG);
			DelayMs(500);
			WTV_Voice(TEN_FLAG);
			DelayMs(500);
			WTV_Voice(SIX_FLAG);
			break;
		case 27:
			WTV_Voice(TWO_FLAG);
			DelayMs(500);
			WTV_Voice(TEN_FLAG);
			DelayMs(500);
			WTV_Voice(SEVEN_FLAG);
			break;
		case 28:
			WTV_Voice(TWO_FLAG);
			DelayMs(500);
			WTV_Voice(TEN_FLAG);
			DelayMs(500);
			WTV_Voice(SEVEN_FLAG);
			break;
		default:
			break;
	}
	DelayMs(400);
	WTV_Voice(CENG_FALG);
	DelayMs(300);
*/

}

void HB_RELAY_Mode(void)
{

            if(Floor_CurrentCount > Floor_TargetCount)//ÏòÏÂÔËÐÐ
            {
                     HB_RELAY_Flag = 2;
		uiLcd_1616_ch(4,4,57,1);//ÏÔÊ¾ÏÂ½µÍ¼±ê
		DelayMs(1000);
		   

            }
            else if(Floor_CurrentCount < Floor_TargetCount)//ÏòÉÏÔËÐÐ
            {
                    HB_RELAY_Flag = 1;
		 uiLcd_1616_ch(3,4,57,1);//ÏÔÊ¾ÉÏÉýÍ¼±ê
		 DelayMs(1000);
            }
            else//Í£Ö¹
            {
                    HB_RELAY_Flag = 0;
            }
}



void uiProcKey(u8 nKey,Menu_Parameter *Parameter)
{
	if(uiKeyIsDigit(nKey) )	/*ÅÐ¶Ï°´¼üÊÇ·ñÊÇÊý×Ö*/
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
	else if (nKey == UIKEY_UP)//Ä¿±êÂ¥²ã¼Ó1
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
	else if (nKey == UIKEY_DOWN)//Ä¿±êÂ¥²ã¼õ1
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
	else if (nKey == UIKEY_ESC)//Ä¿±êÂ¥²ãÍË³ö
	{
		Target_F = 0;
		//if(Target_F < 1)
		//{
			//Target_F =  1;
		//}
		
		//HB_Send_Realy_CMD(CMD_RELAY_STOP);
	}
	else if (nKey == UIKEY_OK)//Ä¿±êÂ¥²ãÆô¶¯
	{
		//´¦ÀíÄ¿±êÂ¥²ãÆô¶¯Á÷³Ì
		//get_weight_clear_value();
		
		if(Target_F < 1)
		{	
			//continue;	
		}
		else if(Target_F > App.Max_floor)
		{
			//½çÃæÌáÊ¾´íÎóÖµ"Ä¿±ê²ã¹ý´ó"
			
			//uiLcd_1212_ch(UISTR_AIM_LOU_CHENG,0,66,2);
			//uiLcd_1212_ch(UISTR_AIM_LOU_CHENG+3,0,66+12*2,1);
			//uiLcd_1212_ch(UISTR_UI_GUODA,0,66+12*3,2);
			uiLcd_1212_ch(UISTR_AIM_LOU_CHENG+2,0,66,2);
			uiLcd_1212_ch(UISTR_FLAG_IN_DOOR+3,0,66+12*2,1);
			uiLcd_1212_ch(UISTR_ENCODER_FLOOR,0,66+12*3,2);
			DelayMs(1200);
			Target_F =0;
			uiLcdLineErase8x16(0,8,17,0);//½«ÏÈÇ°×´Ì¬²Á³ý
			
		}
		else
		{
			if(floor_tmp[Target_F-1].floor_flag == 0)//ÅÅ³ýÃ»ÓÐ±ê¶¨µÄÂ¥²ã
			{
				//uiLcd_1212_ch(UISTR_AIM_LOU_CHENG,0,66,2);
				//uiLcd_1212_ch(UISTR_AIM_LOU_CHENG+3,0,66+12*2,1);
				//uiLcd_1212_ch(UISTR_UI_GUODA,0,66+12*3,2);
				//Â¥²ãÎ´±ê¶¨
				uiLcd_1212_ch(UISTR_AIM_LOU_CHENG+2,0,66,2);
				uiLcd_1212_ch(UISTR_FLAG_IN_DOOR+3,0,66+12*2,1);
				uiLcd_1212_ch(UISTR_ENCODER_FLOOR,0,66+12*3,2);
				DelayMs(1200);
				Target_F =0;
				HB_Start_Flag = 0;
				HB_RELAY_Flag = 0;

				uiLcdLineErase8x16(0,8,17,0);//½«ÏÈÇ°×´Ì¬²Á³ý
				
				return ;
			
			}
			
			//¶ÁÈ¡Ä¿±êÂ¥²ãµÄÆ½²ãÖµ
			Floor_TargetCount = floor_tmp[Target_F-1].floor_count;

			System.Device.IO.HB_Gpio_Set_Value(RELAY_3,1);//¸ßËÙ±ÕºÏ
			System.Device.IO.HB_Gpio_Set_Value(RELAY_1,1);//ÉÏÉý
			System.Device.IO.HB_Gpio_Set_Value(RELAY_2,1);//ÏÂ½µ

			//ÕâÀï´æÔÚÏÈºóÎÊÌâ£¬ÒªÏÈÅÐ¶ÏÔËÐÐ·½Ïò£¬ÔÙ¿ªÆôÔËÐÐ
			HB_RELAY_Mode();
			HB_Start_Flag = 1;
			//uiLcdDecimal(Floor_TargetCount,0,11,0,5);
		}
		
		
	}
	else if (nKey == UIKEY_MENU)
	{			

		//ÑéÖ¤ÃÜÂë
		if(Pwd_Identify_Process())
		{
			uiProcMenu(UISTR_MENU);
		}
		
		//ÏÔÊ¾
		
		uiProcMainDraw(TRUE,Parameter);	
	}
	else if (nKey == UIKEY_DEL)//ÁÙÊ±µ÷³öÖØÁ¿µÄÊý¾ÝÏÔÊ¾
	{
		//get_weight_clear_value();
		//Auto_Encoder_Demarcate();
		weight_flag();
		System.Device.Lcd_12864.LCD_Init();
		uiProcMainDraw(TRUE,Parameter);

	}



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
	//¹ýÂË¶àÓàÐÅÏ¢·½Ê½
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
					if(HB_Floor_Call_Buff[i].Call_num == HB_Floor_Call[xxxx].Call_num)//Èç¹ûÒÑ¾­ºô½ÐÁË£¬¾Í²»ÁíÐÐÏÔÊ¾ÁË
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
	u8 call_flag=0;//Â¥²ãµ½Õ¾µÄºô½Ð±êÖ¾
	u32 Time_tmp;
	u32 Time_tmp1;
	u32 Time_tmp2;
	u32 Time_tmp3;
	u32 Time_tmp4;//ÓÃÓÚÖØÁ¿²É¼¯
	u32  Floor_CurrentCount_pre;
	u16 ad_temp;
	static  u32 fuck_count=0;
	static  u8  fuck_flag=0;//µ±Ç°Æ½²ãÖµÒÑ±£´æµÄ±êÖ¾==0ÊÇÃ»ÓÐ ==1ÊÇÓÐ

	//static u32	xxxx=0;

	

	Menu_Parameter Parameter;
	Target_F = 0;//³õÊ¼»°Â¥²ã
	
	//ÖØÖÃÃÜÂëÊ¹ÓÃ
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
	//¶¯Ì¬ÏÔÊ¾"ºã°î¿Æ¼¼"
	uiLcd_1212_ch(UISTR_HENGBANGKEJI,1,4+12*3,1);
	DelayMs(500);
	uiLcd_1212_ch(UISTR_HENGBANGKEJI+1,1,4+12*4,1);
	DelayMs(500);
	uiLcd_1212_ch(UISTR_HENGBANGKEJI+2,1,4+12*5,1);
	DelayMs(500);
	uiLcd_1212_ch(UISTR_HENGBANGKEJI+3,1,4+12*6,1);
	DelayMs(1000);
	
	uiProcMainDraw(TRUE,&Parameter);
	uiProcMenuBuild();//½¨Á¢²Ëµ¥Ä¿Â¼

	//Ïû³ýµÚÒ»´Î¿ª»úÊ±¾ÍÏÔÊ¾ºô½ÐÂ¥²ãÎÊÌâ
	HB_Floor_Call_Buff[0].Call_Time_Count = 3000;
	HB_Floor_Call_Buff[1].Call_Time_Count = 3000;
	HB_Floor_Call_Buff[2].Call_Time_Count = 3000;
	
	Time_tmp =uiTimeGetTickCount();
	Time_tmp1 =uiTimeGetTickCount();
	Time_tmp2 = Time_tmp1;
	Time_tmp3 = Time_tmp2;
	Time_tmp4 = Time_tmp2;
	while(1)
	{


		//Floor_CurrentCount_pre = Floor_CurrentCount;
		
		nKey  = uiKeyGetKey();//´¦Àí°´¼üºÍÂ¥²ã
		if(nKey != UIKEY_NONE)
		{	
			uiProcKey(nKey,&Parameter);
		
		}
		uiProcProtocl();//Â¥²ãºô½Ð½ÓÊÜ´¦Àí
		
		 if(uiTimeGetTickCount() -Time_tmp >100)//1000ms¸üÐÂÒ»´ÎÖØÁ¿Öµ
               	 {
                       	 //ÒÔºóÃ¿¸ô2sÖÓÉÏ´«Ò»´Î
                       	if(App.Weight.weight_display_change_flg ==1)
			{
				Handle_Weight(&Parameter);//ÖØÁ¿Öµ²É¼¯ºÍ´¦Àí
			}
                        	Time_tmp =uiTimeGetTickCount();
			//»ñÈ¡µçÑ¹µÄADÖµ
			ad_temp = ADC_Filter_V();
			//Á¬Ðø1.0s*4=4s×óÓÒÃ»ÓÐ¼ì²âµ½µçÑ¹Ê±£¬¾Í±£´æµ±Ç°Â¥²ãÊýÖµ
			if(ad_temp <100)//¼ì²âµ½µÄµçÑ¹ADÖµÐ¡ÓÚ100Ê±ÈÏÎª
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
		if(HB_RELAY_Close_Flag == 1)//´æÔÚÊÖ±ú¶Ï¿ªÁË
		{
			
			if(call_flag==0)
			{
				WTV_Voice(DINGDONG);
				call_flag=1;
			}
			
			

			if(uiTimeGetTickCount() -Time_tmp1 > 700)//7sºó±ÕºÏ
               	 	{

				System.Device.IO.HB_Gpio_Set_Value(RELAY_3,1);//¸ßËÙ±ÕºÏ
				
				System.Device.IO.HB_Gpio_Set_Value(RELAY_1,1);//ÉÏÉý
				System.Device.IO.HB_Gpio_Set_Value(RELAY_2,1);//ÏÂ½µ
				HB_RELAY_Close_Flag = 0;
				call_flag=0;
				Time_tmp1 =uiTimeGetTickCount();
           
                  	}
		}else
		{
			Time_tmp1 = uiTimeGetTickCount();
		}
			
		//Handle_Master(&Parameter);

		 if(uiTimeGetTickCount() -Time_tmp2 >30)//300ms»ñÈ¡Ò»´Î±ê¶¨±àÂëÖµ
               	 {
			
			if(Floor_CurrentCount_pre >Floor_CurrentCount)
			{
				//Floor_CurrentCount_diff = Floor_CurrentCount_pre-Floor_CurrentCount;
				uiLcd_1616_ch(4,4,57,1);//ÏÔÊ¾ÏÂ½µÍ¼±ê
				Parameter.Parameter_Change_Position = 0;
			}
			else if(Floor_CurrentCount_pre < Floor_CurrentCount)
			{
				//Floor_CurrentCount_diff = Floor_CurrentCount-Floor_CurrentCount_pre;
				uiLcd_1616_ch(3,4,57,1);//ÏÔÊ¾ÉÏÉýÍ¼±ê
				Parameter.Parameter_Change_Position = 0;
			}
			else
			{
				uiLcd_1616_ch(5,4,57,1);//ÏÔÊ¾Çå¿Õ
			}
			
			Time_tmp2 = uiTimeGetTickCount();
		}

		 
		if(uiTimeGetTickCount() -Time_tmp3 >80)//800ms¸üÐÂÒ»´ÎÄ¿±ê²ãÉÁË¸
               	 {
			Cursor_Flag= !Cursor_Flag;
			Time_tmp3 = uiTimeGetTickCount();

			Floor_CurrentCount_pre = Floor_CurrentCount;
			
		}

		//uiLcdDecimal(Floor_CurrentCount,0,11,0,5);
		
		if(uiTimeGetTickCount() -Time_tmp4 >2)//2////20ms
               	 {

			Time_tmp4 = uiTimeGetTickCount();

		}

		
		 
			
		

		uiProcMainDraw(FALSE,&Parameter);//¸üÐÂÏÔÊ¾
		Parameter.Parameter_Change_Flag =0;
		Parameter.Parameter_Change_Position=0;
		Parameter.Show_Flag=0;



	}

}



	




////////ÒÔÏÂÊÇ±»ÓÅ»¯´¦ÀíµôµÄ´úÂë
//20141203´ÓÖ÷º¯ÊýÖÐÈ¥µô£¬ÓÅ»¯µ½void uiProcKey(u8 nKey,Menu_Parameter *Parameter)
/*
			if(uiKeyIsDigit(nKey) )	//ÅÐ¶Ï°´¼üÊÇ·ñÊÇÊý×Ö
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
			else if (nKey == UIKEY_UP)//Ä¿±êÂ¥²ã¼Ó1
			{
				Target_F = Target_F+1;
				if(Target_F > 99)
				{
					Target_F =  99;
				}
			}
			else if (nKey == UIKEY_DOWN)//Ä¿±êÂ¥²ã¼õ1
			{
				Target_F = Target_F-1;
				if(Target_F < 1)
				{
					Target_F =  1;
				}
			}
			else if (nKey == UIKEY_ESC)//Ä¿±êÂ¥²ãÍË³ö
			{
				Target_F = 0;
				//if(Target_F < 1)
				//{
					//Target_F =  1;
				//}
			}
			else if (nKey == UIKEY_OK)//Ä¿±êÂ¥²ãÆô¶¯
			{
				//´¦ÀíÄ¿±êÂ¥²ãÆô¶¯Á÷³Ì
				get_weight_clear_value();
				
			}
			else if (nKey == UIKEY_MENU)
			{			

				//ÑéÖ¤ÃÜÂë
				if(Pwd_Identify_Process())
				{
					uiProcMenu(UISTR_MENU);
				}
				
				//ÏÔÊ¾
				
				uiProcMainDraw(TRUE,&Parameter);	
			}
*/
/*


*/

