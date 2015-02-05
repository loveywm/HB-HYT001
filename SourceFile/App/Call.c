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
//Â¥²ãºô½ÐÆ÷µÄÉ¾³ý´¦Àíº¯Êý
u8 Call_Del(void)
{
	u8 nKey;
	u8 floor_num_tmp = 1;
	u32 Call_num_set_tmp= 0;
	//u32 Call_num_set= 0;

	u8 pcRet[2];
	u8 i,nPos = 0;
	pcRet[0] = 0;
	
	uiLcdClear();
	uiLcd_1212_ch(UISTR_INPUT_PASSWORD,0,4,3);//ÇëÊäÈë
	uiLcd_1212_ch(UISTR_ENCODER_FLOOR+2,0,4+36,2);//Â¥²ã

	uiLcd_1212_ch(UISTR_CALL_SET_FENJI,0,4+12*6,2);//·Ö»ú
	uiLcd_1212_ch(UISTR_ZAIZHONG_DAODAYUJINGZHI+4,0,4+12*8,1);//Öµ

	//uiLcd_1616_ch(6,2,4,1);//ÏÔÊ¾ÉÏ·­Í¼±ê
	uiLcd_1616_ch(7,2,6,1);//ÏÔÊ¾ÏÂ·­Í¼±ê
	
	uiLcd_1414_ch(UISTR_XXX_SHANCHU, 6, 4,2);
	uiLcdMediumString("OK", 3 ,4,0);


	uiLcd_1414_ch(UISTR_XXX_ESC, 6,76,2);
	uiLcdMediumString("ESC", 3, 13,0);

	
	uiLcdDecimal(1,1,3,0,3);//ÏÔÊ¾Â¥²ãÊý
	
	while(1)
	{
		//Call_num_set_tmp = System.Device.Call_Floor.Remote_Scan();
		//if(Call_num_set_tmp != 0)
		//{
			//uiLcdDecimal(Call_num_set_tmp,1, 2+6,0,8);
			//Call_num_set=Call_num_set_tmp;
		//}

		Call_num_set_tmp = HB_Floor_Call[floor_num_tmp-1].Call_Time_Count;
		uiLcdDecimal(Call_num_set_tmp,1, 2+6,0,8);

		
		//uiLcdDecimal(floor_num_tmp,1,3,0,3);//ÏÔÊ¾Â¥²ãÊý
		
        		nKey = uiKeyGetKey();
		if(nKey == UIKEY_ESC)
		{
			break;
		}	
		
		if(nKey == UIKEY_OK)//µ±°´ÏÂOK¼üÊ±£¬½«¶ÔÓ¦Â¥²ã±£´æ²¢·¢ËÍ¸ø¿ØÖÆ¶Ë¸üÐÂ
		{

			Call_Data  call_data;
			call_data.Call_flag =0;//±êÖ¾Î»Ê¹ÓÃÁË
			call_data.Call_num = floor_num_tmp;//Â¥²ãÉèÖÃÕýÈ·
			call_data.Call_Time_Count = 0;//Â¥²ã¶ÔÓ¦ÎÞÏß±àÂëµÄ±àÂëÖµ

	
			Call_Save(&call_data);
			
			Flash_ReadInfo((UINT8 *)HB_Floor_Call, CALL_ADD, sizeof(Call_Data)*MAX_FLOOR_NUM);
			
			
			//ÌáÊ¾É¾³ý³É¹¦
			uiLcd_1414_ch(UISTR_XXX_SHANCHU, 4,14*2+4,2);
			uiLcd_1414_ch(UISTR_XXX_SUCESS, 4,14*4+4,2);
			DelayMs(1000);
			uiLcdLineErase8x16(2,0,15,0);
			
		}

		if (nKey == UIKEY_NONE)
			continue;
		if (uiKeyIsDigit(nKey))
		{
			pcRet[nPos] = nKey;
			
			if(nPos == 0)
			{
				floor_num_tmp = (pcRet[0]-'0');
			}else if(nPos == 1)
			{
				floor_num_tmp = (pcRet[0]-'0')*10+(pcRet[1]-'0');
			}
			//printf("nPos== %d\r\n",nPos);
			//printf("floor_num_tmp== %d\r\n",floor_num_tmp);
			nPos++;

			if (nPos == 2)//µ±¶ÁÈ¡Î»ÖÃ´óÓÚ2Ê±,
			{	
				nPos = 0;
				for (i=0; i<2; i++)
				{	
					pcRet[i] = '0';
				}
				//continue;
			}
		}
		if(nKey == UIKEY_MENU)//É¾³ý
		{
			
		}
		if(nKey == UIKEY_DOWN)//ÏÂ·­Êý×Ö
		{
			floor_num_tmp--;
		}
		if(nKey == UIKEY_UP)//ÉÏ·­Êý×Ö
		{
			floor_num_tmp++;
		}

		if(floor_num_tmp < 1)
		{
			floor_num_tmp =1;
		}
	 	if(floor_num_tmp > App.Max_floor)
		{
			floor_num_tmp = App.Max_floor;
		}
	
		uiLcdDecimal(floor_num_tmp,1,3,0,3);//ÏÔÊ¾Â¥²ãÊý
	}
	return FALSE;


}

/*			for(ywm_tmp=0;ywm_tmp<MAX_FLOOR_NUM;ywm_tmp++)
			{
				if(Call_num_set == HB_Floor_Call[ywm_tmp].Call_Time_Count)
				{
					//·Ö»úÒÑÑ§Ï°
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


//Â¥²ãºô½ÐÆ÷µÄÑ§Ï°´¦Àíº¯Êý
u8 Call_Learn(void)
{
	u8 nKey;
	u8 ywm_tmp;
	u8 floor_num_tmp = 0;
	u32 Call_num_set_tmp= 0;
	u32 Call_num_set= 0;
	u32 Call_num_set2= 0;
	u32 Call_num_set3= 0;
	u32 Call_Time;

	u8 pcRet[2];
	u8 i,nPos = 0;
	pcRet[0] = 0;
	
	uiLcdClear();
	uiLcd_1212_ch(UISTR_CALL_SET,0,4,2);//Ñ§Ï°
	uiLcd_1212_ch(UISTR_ENCODER_FLOOR+2,0,4+24,2);//Â¥²ã

	uiLcd_1212_ch(UISTR_CALL_SET,0,4+12*6,2);//Ñ§Ï°
	uiLcd_1212_ch(UISTR_ZAIZHONG_DAODAYUJINGZHI+4,0,4+12*8,1);//Öµ

	//uiLcd_1616_ch(6,2,4,1);//ÏÔÊ¾ÉÏ·­Í¼±ê
	uiLcd_1616_ch(7,2,6,1);//ÏÔÊ¾ÏÂ·­Í¼±ê
	
	uiLcd_1414_ch(UISTR_XXX_SAVE, 6, 4,2);
	uiLcdMediumString("OK", 3 ,4,0);

	uiLcd_1414_ch(UISTR_XXX_ESC, 6,76,2);
	uiLcdMediumString("ESC", 3, 13,0);

	uiLcdDecimal(0,1,3,0,2);//ÏÔÊ¾Â¥²ãÊý
	Call_Time =uiTimeGetTickCount();
	while(1)
	{
		if(floor_num_tmp != 0)
		{
			Call_num_set_tmp = System.Device.Call_Floor.Remote_Scan();
			if((Call_num_set_tmp != 0)&&(Call_num_set_tmp < 0x01000000) &&((Call_num_set_tmp&0xf)==0xf)&&((Call_num_set_tmp&0xffffff)!= 0xffffff))
			{
				uiLcdDecimal(Call_num_set_tmp,1, 2+6,0,8);
				Call_num_set=Call_num_set_tmp;
				
				DelayMs(800);
					
				Call_num_set_tmp = System.Device.Call_Floor.Remote_Scan();
				if((Call_num_set_tmp != 0)&&(Call_num_set_tmp < 0x01000000) &&((Call_num_set_tmp&0xf)==0xf)&&((Call_num_set_tmp&0xffffff)!= 0xffffff))
				{
					uiLcdDecimal(Call_num_set_tmp,2, 2+6,0,8);
					Call_num_set2=Call_num_set_tmp;

					if(Call_num_set != Call_num_set2)
					{
						continue;
					}

					DelayMs(800);
					
					Call_num_set_tmp = System.Device.Call_Floor.Remote_Scan();
					if((Call_num_set_tmp != 0)&&(Call_num_set_tmp < 0x01000000) &&((Call_num_set_tmp&0xf)==0xf)&&((Call_num_set_tmp&0xffffff)!= 0xffffff))
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
	/*	Call_num_set_tmp = System.Device.Call_Floor.Remote_Scan();
		if((Call_num_set_tmp != 0)&&(Call_num_set_tmp < 0x01000000) &&((Call_num_set_tmp&0xf)==0xf)&&((Call_num_set_tmp&0xffffff)!= 0xffffff))
		{
			uiLcdDecimal(Call_num_set_tmp,1, 2+6,0,8);
			Call_num_set=Call_num_set_tmp;

		}
*/
        		nKey = uiKeyGetKey();
		if (nKey == UIKEY_NONE)
			continue;
		if(nKey == UIKEY_ESC)
		{
			break;
		}	
		
		if(nKey == UIKEY_OK)//µ±°´ÏÂOK¼üÊ±£¬½«¶ÔÓ¦Â¥²ã±£´æ²¢·¢ËÍ¸ø¿ØÖÆ¶Ë¸üÐÂ
		{
			Call_Data  call_data;
			if(floor_num_tmp == 0)
			{
				continue;
			}
			//É¾³ýÖ®Ç°Ñ§Ï°¹ýµÄ
			for(ywm_tmp=0;ywm_tmp<MAX_FLOOR_NUM;ywm_tmp++)
			{
				if(Call_num_set3 == HB_Floor_Call[ywm_tmp].Call_Time_Count)
				{
					call_data.Call_flag = 0;//±êÖ¾Î»ÖÃ0
					call_data.Call_num = ywm_tmp+1;//Â¥²ãÉèÖÃÕýÈ·
					call_data.Call_Time_Count = 0;//Â¥²ã¶ÔÓ¦ÎÞÏß±àÂëµÄ±àÂëÖ
					Call_Save(&call_data);
					HB_Floor_Call[ywm_tmp].Call_flag=0;
					HB_Floor_Call[ywm_tmp].Call_Time_Count =0;
					//Flash_ReadInfo((UINT8 *)HB_Floor_Call, CALL_ADD, sizeof(Call_Data)*MAX_FLOOR_NUM);
					break;
				}

			}		
			
			call_data.Call_flag = 1;//±êÖ¾Î»Ê¹ÓÃÁË
			call_data.Call_num = floor_num_tmp;//Â¥²ãÉèÖÃÕýÈ·
			call_data.Call_Time_Count = Call_num_set3;//Â¥²ã¶ÔÓ¦ÎÞÏß±àÂëµÄ±àÂëÖµ

			Call_Save(&call_data);
			Flash_ReadInfo((UINT8 *)HB_Floor_Call, CALL_ADD, sizeof(Call_Data)*MAX_FLOOR_NUM);
			
			//ÌáÊ¾±£´æÕýÈ·
			uiLcd_1414_ch(UISTR_XXX_SAVE, 4,4,2);
			uiLcd_1414_ch(UISTR_XXX_SUCESS, 4,4+14*2,2);
			DelayMs(1000);
			uiLcdLineErase8x16(2,0,16,0);
			
		}
	
		if (uiKeyIsDigit(nKey))
		{
			pcRet[nPos] = nKey;
			
			if(nPos == 0)
			{
				floor_num_tmp = (pcRet[0]-'0');
			}else if(nPos == 1)
			{
				floor_num_tmp = (pcRet[0]-'0')*10+(pcRet[1]-'0');
			}
			//printf("nPos== %d\r\n",nPos);
			//printf("floor_num_tmp== %d\r\n",floor_num_tmp);
			nPos++;

			if (nPos == 2)//µ±¶ÁÈ¡Î»ÖÃ´óÓÚ2Ê±,
			{	
				nPos = 0;
				for (i=0; i<2; i++)
				{	
					pcRet[i] = '0';
				}
				//continue;
			}
		}
		if(nKey == UIKEY_MENU)//É¾³ý
		{
			
		}
		if(nKey == UIKEY_DOWN)//ÏÂ·­Êý×Ö
		{
			floor_num_tmp--;
		}
		if(nKey == UIKEY_UP)//ÉÏ·­Êý×Ö
		{
			floor_num_tmp++;
		}

		if(floor_num_tmp < 1)
		{
			floor_num_tmp =0;
		}
	 	if(floor_num_tmp > App.Max_floor)
		{
			floor_num_tmp = App.Max_floor;
		}
		uiLcdDecimal(floor_num_tmp,1,3,0,2);//ÏÔÊ¾Â¥²ãÊý
	
	}
	return FALSE;

}






