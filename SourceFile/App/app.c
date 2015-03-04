/*
********************************************************************************
*                     
********************************************************************************
*版本     作者            日期            说明
*V0.1    whatbook        2014/09/25       初始版本
********************************************************************************
*/
/*
********************************************************************************
//进度汇
//1:完成界面架构的布局和搭建
//2:完成存储部分代码
//3:完成密码功能的验证和存储
//4:实现目标楼层的调整
//5:





*********************************************************************************
*/
#include "system.h"
#include "app.h"


//loveywm 20140923
extern u32 gTimeCount;//在按键驱动中定义的

//#define IdentifyNumberAddress  0x1FFFF7E8

//#define LogicTaskStackSum   400             //业务逻辑任务栈深度
//#define MenuTaskStackSum    100             //菜单界面任务栈深度

//uint  LogicTaskStack[LogicTaskStackSum];    //业务逻辑任务栈数组
//uint  MenuTaskStack[MenuTaskStackSum];      //菜单界面任务栈数组

//void * LogicTaskMessageQueue[40];           //业务逻辑消息队列

AppStruct App;
AppStruct App_Flash;

u32	Target_F;//目标楼层的全局变量
unsigned char  fram_data_buff[MAX_FRAM_LEN];////这个数组保存得到的一个掐头去尾的报文帧
unsigned char  fram_len = 0;

//u32 tmpxxx;//用于计数
//u8 master_pre_value;//扫描按键值的前面
//开始的时候直接将这些变化值放在app里面，当在保存的时候这三个值是变化，直接导致保存的值
//一直是错误的，也就是校验和一直不对

u16  Voltage_Tmp;//电压采样的AD值
u16  Weight_Tmp;//传感器的捕捉的重量值
u16  Weight_Value;////实际采集到的最后重量AD值

//楼层数据保存
Floor_Data   floor_tmp[MAX_FLOOR_NUM];
signed int  Floor_CurrentCount = 0;//用于计数编码器的的值
u32	Floor_TargetCount =0;//用于保存目标楼层的编码器值

//楼层呼叫
u8  HB_Floor_Call_Num = 0;//用于显示楼层呼叫楼层
Call_Data    HB_Floor_Call_Buff[3];//用于显示楼层呼叫楼层buff
Call_Data    HB_Floor_Call[MAX_FLOOR_NUM];//用于保存楼层呼叫的编码值

u8 Cursor_Flag=0;//光标闪烁的全局变量


//////////////////////////////////////////////////////////////////////////////////////

//==0时是未启动，==1是已启动
u8  HB_Start_Flag = 0;//按键按下启动标志，只有启动键按下后才能进行上下操作

u8  HB_RELAY_Flag =0;//升降机上升下降工作模式，=0是停止，=1是上升 ==2是下降

u8  HB_RELAY_Close_Flag =0;//手令断开7s后再次闭合的标志 ==0是正常。==1是需要闭合
//////////////////////////////////////////////////////////////////////////////////////

void HB_Run_Check_Stop(void)//检测是否需要停止
{
	if(HB_Start_Flag == 1)
	{
		   if(HB_RELAY_Flag == 2)//判断是向下运行
	            {
				if(Floor_CurrentCount <= (Floor_TargetCount+App.Other_buchang))//在定值前切断高速，转为低速
				{
					System.Device.IO.HB_Gpio_Set_Value(RELAY_3,0);//高速断开	
				}
				
	                    	if(Floor_CurrentCount <= (Floor_TargetCount+App.Down_buchang))//判断向下停止条件
	                    	{
	                        		HB_RELAY_Flag = 0;
	                        		HB_Start_Flag = 0;
					//System.Device.IO.HB_Gpio_Set_Value(RELAY_3,0);//高速断开			
					System.Device.IO.HB_Gpio_Set_Value(RELAY_2,0);//下降断开
					System.Device.IO.HB_Gpio_Set_Value(RELAY_3,1);//高速闭合
				
					//断开7s后再闭合
					HB_RELAY_Close_Flag =1;
					
					//到站后目标楼层清零
					Target_F = 0;
	                   
	                    	}
	            }
	            else if(HB_RELAY_Flag == 1)//判断是向上运行
	            {

			if(Floor_CurrentCount >= (Floor_TargetCount-App.Other_buchang))//在定值前切断高速，转为低速
			{
				System.Device.IO.HB_Gpio_Set_Value(RELAY_3,0);//高速断开	
			}
			
	                    if(Floor_CurrentCount >= (Floor_TargetCount-App.Up_buchang))//判断向上停止条件
	                    {
	                        	HB_RELAY_Flag = 0;
	                        	HB_Start_Flag = 0;
				//System.Device.IO.HB_Gpio_Set_Value(RELAY_3,0);//高速断开
				System.Device.IO.HB_Gpio_Set_Value(RELAY_1,0);//上升断开
				System.Device.IO.HB_Gpio_Set_Value(RELAY_3,1);//高速闭合
				

				//断开7s后再闭合
				HB_RELAY_Close_Flag =1;
				
				//到站后目标楼层清零
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
* 函数名	: SystemTickRoutine
* 描述	    : 应用层系统节拍，用于执行固定间隔功能，间隔时间取决于注册函数
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/03       初始版本
*******************************************************************************/
static void SystickRoutine(void) 
{
    //回调函数，10ms一次被系统时钟调用

	gTimeCount++;
	
	//Floor_CurrentCount  +=System.Device.Encoder.Enc_GetCount();

}
static void SystickRoutine1(void) 
{
    	//回调函数，1ms一次被系统时钟调用
	Floor_CurrentCount  +=System.Device.Encoder.Enc_GetCount();

	
	//在此预留编码值处理流程
	HB_Run_Check_Stop();
}


/*******************************************************************************
* 函数名	: Usart1RxdFunction
* 描述	    : Usart1接收处理回调函数，需要通过注册才能使用
* 输入参数  : data Usart接收值
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/13       初始版本
*******************************************************************************/
/*
static void Usart1RxdFunction(byte data)
{
   	//PostMessageToLogicTask(KeyMessageType, data);  
   	//printf("data==%d\r\n",data);
	//printf("data==%x\r\n",data);

	//将data中的值传给应用层
	//Usart1RxDataTOApp(data);
}
*/
static void Usart2RxdFunction(byte data)
{
	//将data中的值传给应用层
	Usart2RxDataTOApp(data);
}

/////////////////////////////////////////////////////////////////

/*******************************************************************************
* 函数名	: InitializeData
* 描述	    : 初始化默认参数
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/08       初始版本
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
			pPoint += 2;//去掉前两个，从第2位开始校验
			for(i = 0x00;i < sizeof(App) -2;i++)
			{
				wCheckSum += *pPoint++;
			}
		}else if(ReadCheckSumCounts == 1)
		{
			Flash_ReadInfo((UINT8 *)&App, APP_SETUP_ADD2, sizeof(App));
			pPoint = (UINT8 *)&App;
			pPoint += 2;//去掉前两个，从第2位开始校验
			for(i = 0x00;i < sizeof(App) -2;i++)
			{
				wCheckSum += *pPoint++;
			}

		}else if(ReadCheckSumCounts == 2)
		{
			Flash_ReadInfo((UINT8 *)&App, APP_SETUP_ADD3, sizeof(App));
			pPoint = (UINT8 *)&App;
			pPoint += 2;//去掉前两个，从第2位开始校验
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
		App.Weight.weight_display_change_flg = 1;//默认是显示控制

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

	 //printf("\n芯片ID号 = %d", System.Device.W25q64.SPI_Flash_ReadID());
	 //printf("\n芯片ID号 = 0x%x", System.Device.W25q64.SPI_Flash_ReadID());
	 
	//读取保存的当前编码值
	Flash_ReadInfo((UINT8 *)&xxx_tmp, FLOOR_CUR_ADD, 4);
	DelayMs(50);
	Floor_CurrentCount = xxx_tmp-10001;
	
	//读取楼层数据的值到应用缓存里
	Flash_ReadInfo((UINT8 *)floor_tmp, FLOOR_ADD, sizeof(Floor_Data)*MAX_FLOOR_NUM);
	DelayMs(50);

	Flash_ReadInfo((UINT8 *)HB_Floor_Call, CALL_ADD, sizeof(Call_Data)*MAX_FLOOR_NUM);
	//Flash_WriteInfo((u8*)&App,APP_SETUP_ADD,sizeof(App));


}

/*******************************************************************************
* 函数名	: InitializeApp
* 描述	    : 初始化App
*           : InitializeMmi:初始化界面
*           : System.Device.SystemTick.Register:注册应用层节拍
*           : System.Device.Adc.Register:注册ADC参数设备变量
*           : System.Device.Usart1.RxdRegister Usart1接收函数注册
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/03       初始版本
*******************************************************************************/
static void InitializeApp(void)
{

	InitializeData();
    



		

	//10ms
    	System.Device.Systick.Register(Systick100, SystickRoutine);//备用

	//用于编码器数值读取0.1ms
        System.Device.Systick.Register(Systick10000, SystickRoutine1);

	

   	 System.Device.Adc.Register(AdcChannel0, (ushort *)(&Weight_Tmp));
	System.Device.Adc.Register2(AdcChannel1, (ushort *)(&Voltage_Tmp));


		
	 System.Device.Usart3.RxdRegister(Usart2RxdFunction);
}


/*******************************************************************************
* 函数名	: main
* 描述	    : msOS整个程序入口
*           : InitializeData:初始化存储数据区
*           : System.Initialize:System初始化，实现底层系统初始化 
*           : 任务初始化优先的，优先级也优先，TaskSum要配套设置
*           : LogicTask: 业务逻辑任务，初始化为最高优先级 
*           : MenuTask: 界面任务，初始化为最低优先级,注意，它必须为最低优先级
*           : LogicTaskMessageQueuePoint:初始化一个逻辑任务消息队列
* 输入参数  : 无
* 返回参数  : int
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/03       初始版本
*******************************************************************************/
int main(void) 
{            

	System.Initialize();                                                //初始化系统层
        
    	InitializeApp();                                                    //初始化应用层


	uiProcMain();//主界面循环处理函数


}



