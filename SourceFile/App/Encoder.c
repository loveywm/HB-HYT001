
//��Ҫ��ź�������������صĴ������
#include "system.h"
#include "app.h"


#define MAX_BYTE_LENS		3	



//����¥�����ݽṹ����Ӧflashλ����
void	Encode_Save(Floor_Data  *floor_data)
{

	Flash_WriteInfo((u8*)floor_data,FLOOR_ADD+(floor_data->floor_num-1)*sizeof(Floor_Data),sizeof(Floor_Data));

}
//��ȡ��Ӧ¥�����flashֵ���������ݽṹ
void	Encode_Read(u8 floor_num,Floor_Data  *floor_data)
{
	Flash_ReadInfo((u8*)floor_data,FLOOR_ADD+(floor_num-1)*sizeof(Floor_Data),sizeof(Floor_Data));
}



//¥��궨������
u8	Encoder_Demarcate(void)
{
//	char pcRet[MAX_BYTE_LENS];
	u8 nKey;
	Floor_Data  floor_data;
	
	u8 floor_num_tmp = 1;
	u32 encoder_set_2 = 0;
	
	

//	pcRet[0] = 0;
		
	uiLcdClear();
	uiLcd_1212_ch(UISTR_ENCODER_FLOOR,0,4,4);//��ʾ¥��궨

	//uiLcd_1616_ch(6,2,4,1);//��ʾ�Ϸ�ͼ��
	uiLcd_1616_ch(7,2,6,1);//��ʾ�·�ͼ��
	
	uiLcd_1414_ch(UISTR_XXX_SAVE, 6, 4,2);
	uiLcdMediumString("OK", 3 ,4,0);


	uiLcd_1414_ch(UISTR_XXX_ESC, 6,76,2);
	uiLcdMediumString("ESC", 3, 13,0);
	
	while(1)
	{
		//���Խӿ�
		uiLcdDecimal(floor_tmp[floor_num_tmp-1].floor_count ,0,9,0,7);//��ʾ��Ӧ¥�㱣��ļ�����ֵ
		uiLcdDecimal(floor_tmp[floor_num_tmp-1].floor_diff,2,9,0,7);//��ʾ��Ӧ¥�㱣��ļ�����ֵ


		//��ʾ��ǰ����ֵ
		encoder_set_2 = Floor_CurrentCount;
		uiLcdDecimal(encoder_set_2,1,9,0,7);
 
		uiLcdDecimal(floor_num_tmp,1,3,0,3);//��ʾ¥����

        		nKey = uiKeyGetKey();
		if(nKey == UIKEY_ESC)
			break;
		
		if(nKey == UIKEY_OK)//������OK��ʱ������Ӧ¥�㱣�沢���͸����ƶ˸���
		{
			if(floor_num_tmp ==1)
			{
				floor_data.floor_flag = 1;//��־λʹ����
				floor_data.floor_num = floor_num_tmp;//¥��������ȷ
				floor_data.floor_diff = 0;
				floor_data.floor_count = encoder_set_2;//¥���Ӧ�ı���ֵ

			}
			else
			{
				floor_data.floor_flag = 1;//��־λʹ����
				floor_data.floor_num = floor_num_tmp;//¥��������ȷ
				floor_data.floor_diff = encoder_set_2-floor_tmp[floor_num_tmp-2].floor_count;
				floor_data.floor_count = encoder_set_2;//¥���Ӧ�ı���ֵ

			}

			//�����ǰֵС��ǰһ¥�����ں�һ¥�㣬�궨��Ч

	
			Encode_Save(&floor_data);
			
			Encode_Read(floor_num_tmp,&floor_tmp[floor_num_tmp-1]);
	
			//��ʾ������ȷ
			uiLcd_1414_ch(UISTR_XXX_SAVE, 4,14*2+4,2);
			uiLcd_1414_ch(UISTR_XXX_SUCESS, 4,14*4+4,2);
			DelayMs(1000);
			uiLcdLineErase8x16(2,0,15,0);
			
		}

		if (nKey == UIKEY_NONE)
			continue;
		
		if(nKey == UIKEY_MENU)//ɾ��
		{
			
		}
		if(nKey == UIKEY_DOWN)//�·�����
		{
			floor_num_tmp--;
		}
		if(nKey == UIKEY_UP)//�Ϸ�����
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


//¥��궨��ʼ��������
u8	Encoder_Demarcate_Init(void)
{

	u8 nKey;
	u8 floor_num_tmp;
	Floor_Data  floor_data;
//	u32 encoder_set_2 = 0;
	uiLcdClear();


	//�Ƿ�����Զ�ƽ��궨?
	uiLcd_1414_ch(UISTR_XXX_SHIFOU, 0, 35,4);
	uiLcd_1414_ch(UISTR_XXX_ZIDONG, 2, 20,2);
	uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UI, 2, 20+14*2,4);
	uiLcdMediumString("?", 1 ,13,0);

	//�����ͣ��һ¥
	uiLcdMediumString("*", 2 ,0,0);
	uiLcd_1414_ch(UISTR_XXX_TINGZHIYILOU, 4, 14,8);

	
	uiLcd_1414_ch(UISTR_XXX_SHIFOU, 6, 4+6,1);
	uiLcdMediumString("OK", 3 ,3,0);

	uiLcd_1414_ch(UISTR_XXX_SHIFOU+1, 6,76+6,1);
	uiLcdMediumString("ESC", 3, 12,0);

	//�Ƿ���г�ʼ��
	
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

	uiLcdClear();//����
	//¥��
	uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_MAX_FLOOR+2, 0,15,2);

	//�궨
	uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UI+2, 0,76+14,2);

	//¥�궨�ɹ�
	//uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_MAX_FLOOR+2,4,60,1);
	uiLcd_1414_ch(UISTR_XXX_PINGCHENG_SET_UI+2,4,4,2);
	uiLcd_1414_ch(UISTR_XXX_CHENGGONG,4,4+14*2,2);
	
	//��ʼ����ǰ¥��
	Floor_CurrentCount = 20000;
	//����
	Flash_WriteInfo((u8*)&Floor_CurrentCount,FLOOR_CUR_ADD,4);

	for(floor_num_tmp=0;floor_num_tmp<App.Max_floor;floor_num_tmp++)
	{
		if(floor_num_tmp==0)
		{
			floor_data.floor_flag = 1;//��־λʹ����
			floor_data.floor_num = floor_num_tmp+1;//¥��������ȷ
			floor_data.floor_diff = 0;//¥��֮��Ĳ�ֵ
			floor_data.floor_count = Floor_CurrentCount;//¥���Ӧ�ı���ֵ

		}
		else
		{
			floor_data.floor_flag = 1;//��־λʹ����
			floor_data.floor_num = floor_num_tmp+1;//¥��������ȷ
			floor_data.floor_diff = floor_tmp[floor_num_tmp].floor_diff;//¥��֮��Ĳ�ֵ
			//¥���Ӧ�ı���ֵ=ǰһ��ı���ֵ����¥���ֵ
			floor_data.floor_count = floor_tmp[floor_num_tmp-1].floor_count + floor_tmp[floor_num_tmp].floor_diff;

		}

		Encode_Save(&floor_data);
		DelayMs(100);
		Encode_Read(floor_data.floor_num,&floor_tmp[floor_data.floor_num-1]);


		uiLcdDecimal(floor_data.floor_num,1,2,0,3);//��ʾ¥����
		uiLcdDecimal(floor_tmp[floor_data.floor_num -1].floor_count ,1,9,0,7);//��ʾ��Ӧ¥�㱣��ļ�����ֵ

		//��ʾ��ǰ¥��������
		//uiLcdDecimal(floor_data.floor_num,2,5,0,2);//��ʾ¥����
		
		DelayMs(1000);
		
	}

	//��ʾȫ���������
	uiLcdClear();
	//�Զ�ƽ��ɹ�  ���˳�
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







