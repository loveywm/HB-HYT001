#include "system.h"
#include "drive.h"

#define CALL_TIMER   TIM4 // Encoder unit connected to TIM4

#define RDATA PbIn(8)	 //定时器数据输入脚

//定时器2通道3输入捕获配置
TIM_ICInitTypeDef  TIM2_ICInitStructure;


//遥控器接收状态
//[7]:收到了引导码标志
//[6]:得到了一个按键的所有信息
//[5]:保留	
//[4]:标记上升沿是否已经被捕获								   
//[3:0]:溢出计时器
u8 	RmtSta=0;	

u16 Dval;		//下降沿时计数器的值
u32 RmtRec=0;	//红外接收到的数据	   		
u32 RmtRec_tmp=0;	//红外接收到的数据	   	
u8  RmtCnt=0;	//按键按下的次数	  
//定时器4中断服务程序	 
void TIM4_IRQHandler(void)
{ 		    	 
    	if(TIM_GetITStatus(CALL_TIMER,TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0x80)//上次有数据被接收到了
		{	
			
		}

	}
 	if(TIM_GetITStatus(CALL_TIMER,TIM_IT_CC3)!=RESET)
	{	  
		if(RDATA)//上升沿捕获
		{

			TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Falling);//CC1P=1 设置为下降沿捕获				
	    		TIM_SetCounter(CALL_TIMER,0);	   	//清空定时器值
			RmtSta|=0X10;					//标记上升沿已经被捕获
		}else //下降沿捕获
		{			
  			 Dval=TIM_GetCapture3(CALL_TIMER);//读取CCR1也可以清CC1IF标志位
			 TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Rising); //CC4P=0	设置为上升沿捕获
 			
			if(RmtSta&0X10)					//完成一次高电平捕获 
			{
 				if(RmtSta&0X80)//接收到了引导码
				{
					
					if(Dval>880&&Dval<990)			//930为标准值,930us
					{
						RmtRec<<=1;	//左移一位.
						RmtRec|=0;	//接收到0	 
						RmtCnt++;
						//printf("data0==%dus\r\n",Dval);
					}else if(Dval>240&&Dval<320)	//280为标准值,280us
					{
						RmtRec<<=1;	//左移一位.
						RmtRec|=1;	//接收到1
						RmtCnt++;
						//printf("data1==%dus\r\n",Dval);
					}
					else if(Dval>1500)	//得到标志头后超过2000us后就舍弃
					{
						RmtCnt =0;
						RmtSta&=~(1<<7);//清空引导标识
						//RmtSta&=~(1<<6);//清除接收到有效按键标识
						RmtRec=0;
						//printf("datax==%dus\r\n",Dval);
						//RmtSta&=0XF0;	//清空计时器		
					}
					if(RmtCnt == 24)
					{
						RmtCnt =0;
						RmtRec_tmp = RmtRec;
						RmtSta|=1<<6;//标记已经完成一次按键的键值信息采集
					}
					//else if(Dval>2200&&Dval<2600)	//得到按键键值增加的信息 2500为标准值2.5ms
					//{
						//RmtCnt++; 		//按键次数增加1次
						//RmtSta&=0XF0;	//清空计时器		
					//}
 				}
				if((Dval>10000&&Dval<11000) &&(RmtCnt==0))		//10664为标准值4.5ms
				{
					RmtSta|=1<<7;	//标记成功接收到了引导码
					//RmtCnt=0;		//清除按键次数计数器
					//printf("Head==%dus\r\n",Dval);
				}						 
			}
			RmtSta&=~(1<<4);//取消上升沿已经被捕获标记
		}				 		     	    					   
	}
 	TIM_ClearFlag(CALL_TIMER,TIM_IT_Update|TIM_IT_CC3);	    
}
/*

void TIM2_IRQHandler(void)
{ 		    	 
    	if(TIM_GetITStatus(CALL_TIMER,TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0x80)//上次有数据被接收到了
		{	
			RmtSta&=~0X10;						//取消上升沿已经被捕获标记
			if((RmtSta&0X0F)==0X00)
				RmtSta|=1<<6;//标记已经完成一次按键的键值信息采集
			if((RmtSta&0X0F)<14)
			{
				RmtSta++;
			}
			else
			{
				RmtSta&=~(1<<7);//清空引导标识
				RmtSta&=0XF0;	//清空计数器	
			}						 	   	
		}							    
	}
 	if(TIM_GetITStatus(CALL_TIMER,TIM_IT_CC3)!=RESET)
	{	  
		if(RDATA)//上升沿捕获
		{

			TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Falling);//CC1P=1 设置为下降沿捕获				
	    		TIM_SetCounter(CALL_TIMER,0);	   	//清空定时器值
			RmtSta|=0X10;					//标记上升沿已经被捕获
		}else //下降沿捕获
		{			
  			 Dval=TIM_GetCapture3(CALL_TIMER);//读取CCR1也可以清CC1IF标志位
			 TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Rising); //CC4P=0	设置为上升沿捕获
 			
			if(RmtSta&0X10)					//完成一次高电平捕获 
			{
 				if(RmtSta&0X80)//接收到了引导码
				{
					
					if(Dval>900&&Dval<1200)			//1032为标准值,560us
					{
						RmtRec<<=1;	//左移一位.
						RmtRec|=0;	//接收到0	   
					}else if(Dval>280&&Dval<400)	//344为标准值,1680us
					{
						RmtRec<<=1;	//左移一位.
						RmtRec|=1;	//接收到1
					}
					//else if(Dval>2200&&Dval<2600)	//得到按键键值增加的信息 2500为标准值2.5ms
					//{
						//RmtCnt++; 		//按键次数增加1次
						//RmtSta&=0XF0;	//清空计时器		
					//}
 				}else if(Dval>10000&&Dval<14000)		//10664为标准值4.5ms
				{
					RmtSta|=1<<7;	//标记成功接收到了引导码
					RmtCnt=0;		//清除按键次数计数器
				}						 
			}
			RmtSta&=~(1<<4);
		}				 		     	    					   
	}
 	TIM_ClearFlag(CALL_TIMER,TIM_IT_Update|TIM_IT_CC3);	    
}

*/

//处理红外键盘
//返回值:
//	 0,没有任何按键按下
//其他,按下的按键键值.
u32 Remote_Scan(void)
{        
	u32 tmp=0;
	if(RmtSta&(1<<6))//得到一个按键的所有信息了
	{
		tmp = RmtRec_tmp;
		RmtRec_tmp =0;
		RmtSta&=~(1<<6);//清除接收到有效按键标识
	}
    	return tmp;
}


void InitializeTim(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
   	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //使能TIM2时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIOB时钟
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;  //PA2 清除之前设置  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA2 输入  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_2);						 //PA2下拉
	
	//初始化定时器2 TIM2	 
	TIM_TimeBaseStructure.TIM_Period = 20000; //设定计数器自动重装值 15ms
	TIM_TimeBaseStructure.TIM_Prescaler =72-1; 	//预分频器   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(CALL_TIMER, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
  
	//初始化TIM4输入捕获参数
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=01 	选择输入端 IC3映射到TI3上
  	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  	TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
  	TIM_ICInit(CALL_TIMER, &TIM2_ICInitStructure);
	
	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
	
	TIM_ITConfig(CALL_TIMER,TIM_IT_Update|TIM_IT_CC3,ENABLE);//允许更新中断 ,允许CC1IE捕获中断	
	
   	TIM_Cmd(CALL_TIMER,ENABLE ); 	//使能定时器2

	System.Device.Call_Floor.Remote_Scan =Remote_Scan;
}

