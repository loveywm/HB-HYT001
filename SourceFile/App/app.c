/*
********************************************************************************
*                     
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    whatbook        2014/09/25       ��ʼ�汾
********************************************************************************
*/
/*
********************************************************************************
//���Ȼ�
//1:��ɽ���ܹ��Ĳ��ֺʹ
//2:��ɴ洢���ִ���
//3:������빦�ܵ���֤�ʹ洢
//4:ʵ��Ŀ��¥��ĵ���
//5:





*********************************************************************************
*/
#include "system.h"
#include "app.h"


//loveywm 20140923
extern u32 gTimeCount;//�ڰ��������ж����

//#define IdentifyNumberAddress  0x1FFFF7E8

//#define LogicTaskStackSum   400             //ҵ���߼�����ջ���
//#define MenuTaskStackSum    100             //�˵���������ջ���

//uint  LogicTaskStack[LogicTaskStackSum];    //ҵ���߼�����ջ����
//uint  MenuTaskStack[MenuTaskStackSum];      //�˵���������ջ����

//void * LogicTaskMessageQueue[40];           //ҵ���߼���Ϣ����

AppStruct App;
AppStruct App_Flash;

u32	Target_F;//Ŀ��¥���ȫ�ֱ���
unsigned char  fram_data_buff[MAX_FRAM_LEN];////������鱣��õ���һ����ͷȥβ�ı���֡
unsigned char  fram_len = 0;

//u32 tmpxxx;//���ڼ���
//u8 master_pre_value;//ɨ�谴��ֵ��ǰ��
//��ʼ��ʱ��ֱ�ӽ���Щ�仯ֵ����app���棬���ڱ����ʱ��������ֵ�Ǳ仯��ֱ�ӵ��±����ֵ
//һֱ�Ǵ���ģ�Ҳ����У���һֱ����

u16  Voltage_Tmp;//��ѹ������ADֵ
u16  Weight_Tmp;//�������Ĳ�׽������ֵ
u16  Weight_Value;////ʵ�ʲɼ������������ADֵ

//¥�����ݱ���
Floor_Data   floor_tmp[MAX_FLOOR_NUM];
signed int  Floor_CurrentCount = 0;//���ڼ����������ĵ�ֵ
u32	Floor_TargetCount =0;//���ڱ���Ŀ��¥��ı�����ֵ

//¥�����
u8  HB_Floor_Call_Num = 0;//������ʾ¥�����¥��
Call_Data    HB_Floor_Call_Buff[3];//������ʾ¥�����¥��buff
Call_Data    HB_Floor_Call[MAX_FLOOR_NUM];//���ڱ���¥����еı���ֵ

u8 Cursor_Flag=0;//�����˸��ȫ�ֱ���


//////////////////////////////////////////////////////////////////////////////////////

//==0ʱ��δ������==1��������
u8  HB_Start_Flag = 0;//��������������־��ֻ�����������º���ܽ������²���

u8  HB_RELAY_Flag =0;//�����������½�����ģʽ��=0��ֹͣ��=1������ ==2���½�

u8  HB_RELAY_Close_Flag =0;//����Ͽ�7s���ٴαպϵı�־ ==0��������==1����Ҫ�պ�
//////////////////////////////////////////////////////////////////////////////////////

void HB_Run_Check_Stop(void)//����Ƿ���Ҫֹͣ
{
	if(HB_Start_Flag == 1)
	{
		   if(HB_RELAY_Flag == 2)//�ж�����������
	            {
				if(Floor_CurrentCount <= (Floor_TargetCount+App.Other_buchang))//�ڶ�ֵǰ�жϸ��٣�תΪ����
				{
					System.Device.IO.HB_Gpio_Set_Value(RELAY_3,0);//���ٶϿ�	
				}
				
	                    	if(Floor_CurrentCount <= (Floor_TargetCount+App.Down_buchang))//�ж�����ֹͣ����
	                    	{
	                        		HB_RELAY_Flag = 0;
	                        		HB_Start_Flag = 0;
					//System.Device.IO.HB_Gpio_Set_Value(RELAY_3,0);//���ٶϿ�			
					System.Device.IO.HB_Gpio_Set_Value(RELAY_2,0);//�½��Ͽ�
					System.Device.IO.HB_Gpio_Set_Value(RELAY_3,1);//���ٱպ�
				
					//�Ͽ�7s���ٱպ�
					HB_RELAY_Close_Flag =1;
					
					//��վ��Ŀ��¥������
					Target_F = 0;
	                   
	                    	}
	            }
	            else if(HB_RELAY_Flag == 1)//�ж�����������
	            {

			if(Floor_CurrentCount >= (Floor_TargetCount-App.Other_buchang))//�ڶ�ֵǰ�жϸ��٣�תΪ����
			{
				System.Device.IO.HB_Gpio_Set_Value(RELAY_3,0);//���ٶϿ�	
			}
			
	                    if(Floor_CurrentCount >= (Floor_TargetCount-App.Up_buchang))//�ж�����ֹͣ����
	                    {
	                        	HB_RELAY_Flag = 0;
	                        	HB_Start_Flag = 0;
				//System.Device.IO.HB_Gpio_Set_Value(RELAY_3,0);//���ٶϿ�
				System.Device.IO.HB_Gpio_Set_Value(RELAY_1,0);//�����Ͽ�
				System.Device.IO.HB_Gpio_Set_Value(RELAY_3,1);//���ٱպ�
				

				//�Ͽ�7s���ٱպ�
				HB_RELAY_Close_Flag =1;
				
				//��վ��Ŀ��¥������
				Target_F = 0;
	    
	                    }
	            }
	            else
	            {
	                        HB_RELAY_Flag = 0;
	                        HB_Start_Flag = 0;
	            }
	}
}


/*******************************************************************************
* ������	: SystemTickRoutine
* ����	    : Ӧ�ò�ϵͳ���ģ�����ִ�й̶�������ܣ����ʱ��ȡ����ע�ắ��
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/03       ��ʼ�汾
*******************************************************************************/
static void SystickRoutine(void) 
{
    //�ص�������10msһ�α�ϵͳʱ�ӵ���

	gTimeCount++;
	
	//Floor_CurrentCount  +=System.Device.Encoder.Enc_GetCount();

}
static void SystickRoutine1(void) 
{
    	//�ص�������1msһ�α�ϵͳʱ�ӵ���
	Floor_CurrentCount  +=System.Device.Encoder.Enc_GetCount();

	
	//�ڴ�Ԥ������ֵ��������
	HB_Run_Check_Stop();
}


/*******************************************************************************
* ������	: Usart1RxdFunction
* ����	    : Usart1���մ���ص���������Ҫͨ��ע�����ʹ��
* �������  : data Usart����ֵ
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/13       ��ʼ�汾
*******************************************************************************/
/*
static void Usart1RxdFunction(byte data)
{
   	//PostMessageToLogicTask(KeyMessageType, data);  
   	//printf("data==%d\r\n",data);
	//printf("data==%x\r\n",data);

	//��data�е�ֵ����Ӧ�ò�
	//Usart1RxDataTOApp(data);
}
*/
static void Usart2RxdFunction(byte data)
{
	//��data�е�ֵ����Ӧ�ò�
	Usart2RxDataTOApp(data);
}

/////////////////////////////////////////////////////////////////

/*******************************************************************************
* ������	: InitializeData
* ����	    : ��ʼ��Ĭ�ϲ���
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/08       ��ʼ�汾
*******************************************************************************/


void InitializeData(void)
{

	UINT16 wCheckSum = 0x00;
	UINT8 *pPoint;
	UINT16	i;
	BOOL 	bCheckSumErr = FALSE;
	UINT8	ReadCheckSumCounts = 0;
	u32  xxx_tmp;

	//while(!bCheckSumErr && (ReadCheckSumCounts < 5))
	while(ReadCheckSumCounts < 3)
	{
		if(ReadCheckSumCounts == 0)
		{
			Flash_ReadInfo((UINT8 *)&App, APP_SETUP_ADD, sizeof(App));
			pPoint = (UINT8 *)&App;
			pPoint += 2;//ȥ��ǰ�������ӵ�2λ��ʼУ��
			for(i = 0x00;i < sizeof(App) -2;i++)
			{
				wCheckSum += *pPoint++;
			}
		}else if(ReadCheckSumCounts == 1)
		{
			Flash_ReadInfo((UINT8 *)&App, APP_SETUP_ADD2, sizeof(App));
			pPoint = (UINT8 *)&App;
			pPoint += 2;//ȥ��ǰ�������ӵ�2λ��ʼУ��
			for(i = 0x00;i < sizeof(App) -2;i++)
			{
				wCheckSum += *pPoint++;
			}

		}else if(ReadCheckSumCounts == 2)
		{
			Flash_ReadInfo((UINT8 *)&App, APP_SETUP_ADD3, sizeof(App));
			pPoint = (UINT8 *)&App;
			pPoint += 2;//ȥ��ǰ�������ӵ�2λ��ʼУ��
			for(i = 0x00;i < sizeof(App) -2;i++)
			{
				wCheckSum += *pPoint++;
			}
		}else
		{

		}

		if(App.wLicCheckSum != wCheckSum)
		{
			bCheckSumErr = TRUE;
		}
		else
		{
			bCheckSumErr = FALSE;
			break;
		}
		ReadCheckSumCounts++;
	}
	if(bCheckSumErr)
	{

		App.Weight.Rated_weight = 2000;
		App.Weight.Warning_weight= 90;
		App.Weight.Alarm_weight = 110;
		App.Weight.Empty_weight= 1200;
		App.Weight.weight_display_change_flg = 1;//Ĭ������ʾ����

		//App.Input_Data = 0xff;

		App.Up_buchang = 0;
		App.Down_buchang = 0;
		App.Other_buchang = 3000;
		App.Max_floor = 25;
		//App.Floor_Count_Max = 3000;

		//App.Floor_CurrentCount_Init = 20000;

		wCheckSum = 0x00;
		pPoint = (UINT8 *)&App;
		pPoint += 2;
		for(i = 0x00;i < sizeof(App) -2;i++)
		{
			wCheckSum += *pPoint++;
		}
		App.wLicCheckSum = wCheckSum;

		Flash_WriteInfo((u8*)&App,APP_SETUP_ADD,sizeof(App));
		DelayMs(50);
		Flash_WriteInfo((u8*)&App,APP_SETUP_ADD2,sizeof(App));
		DelayMs(50);
		Flash_WriteInfo((u8*)&App,APP_SETUP_ADD3,sizeof(App));
		DelayMs(50);

	}

	if(ReadCheckSumCounts==1)
	{
		Flash_WriteInfo((u8*)&App,APP_SETUP_ADD,sizeof(App));
		DelayMs(50);
	}else if(ReadCheckSumCounts==2)
	{
		Flash_WriteInfo((u8*)&App,APP_SETUP_ADD,sizeof(App));
		DelayMs(50);
		Flash_WriteInfo((u8*)&App,APP_SETUP_ADD2,sizeof(App));
		DelayMs(50);
	}

	memcpy(&App_Flash,&App,sizeof(App));

	 //printf("\nоƬID�� = %d", System.Device.W25q64.SPI_Flash_ReadID());
	 //printf("\nоƬID�� = 0x%x", System.Device.W25q64.SPI_Flash_ReadID());
	 
	//��ȡ����ĵ�ǰ����ֵ
	Flash_ReadInfo((UINT8 *)&xxx_tmp, FLOOR_CUR_ADD, 4);
	DelayMs(50);
	Floor_CurrentCount = xxx_tmp-10001;
	
	//��ȡ¥�����ݵ�ֵ��Ӧ�û�����
	Flash_ReadInfo((UINT8 *)floor_tmp, FLOOR_ADD, sizeof(Floor_Data)*MAX_FLOOR_NUM);
	DelayMs(50);

	Flash_ReadInfo((UINT8 *)HB_Floor_Call, CALL_ADD, sizeof(Call_Data)*MAX_FLOOR_NUM);
	//Flash_WriteInfo((u8*)&App,APP_SETUP_ADD,sizeof(App));


}

/*******************************************************************************
* ������	: InitializeApp
* ����	    : ��ʼ��App
*           : InitializeMmi:��ʼ������
*           : System.Device.SystemTick.Register:ע��Ӧ�ò����
*           : System.Device.Adc.Register:ע��ADC�����豸����
*           : System.Device.Usart1.RxdRegister Usart1���պ���ע��
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/03       ��ʼ�汾
*******************************************************************************/
static void InitializeApp(void)
{

	InitializeData();
    



		

	//10ms
    	System.Device.Systick.Register(Systick100, SystickRoutine);//����

	//���ڱ�������ֵ��ȡ0.1ms
        System.Device.Systick.Register(Systick10000, SystickRoutine1);

	

   	 System.Device.Adc.Register(AdcChannel0, (ushort *)(&Weight_Tmp));
	System.Device.Adc.Register2(AdcChannel1, (ushort *)(&Voltage_Tmp));


		
	 System.Device.Usart3.RxdRegister(Usart2RxdFunction);
}


/*******************************************************************************
* ������	: main
* ����	    : msOS�����������
*           : InitializeData:��ʼ���洢������
*           : System.Initialize:System��ʼ����ʵ�ֵײ�ϵͳ��ʼ�� 
*           : �����ʼ�����ȵģ����ȼ�Ҳ���ȣ�TaskSumҪ��������
*           : LogicTask: ҵ���߼����񣬳�ʼ��Ϊ������ȼ� 
*           : MenuTask: �������񣬳�ʼ��Ϊ������ȼ�,ע�⣬������Ϊ������ȼ�
*           : LogicTaskMessageQueuePoint:��ʼ��һ���߼�������Ϣ����
* �������  : ��
* ���ز���  : int
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/03       ��ʼ�汾
*******************************************************************************/
int main(void) 
{            

	System.Initialize();                                                //��ʼ��ϵͳ��
        
    	InitializeApp();                                                    //��ʼ��Ӧ�ò�


	uiProcMain();//������ѭ��������


}



