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
//[3]:获取头后下降沿捕获标志
//[2:0]:溢出计时器，暂时没用
u8 	RmtSta=0;	

u16 Dval;		//下降沿时计数器的值
u32 RmtRec=0;	//红外接收到的数据	   		
u32 RmtRec_tmp=0;	//红外接收到的数据	   	
u8  RmtCnt=0;	//按键按下的次数	 


//对头进行过滤变量
static u16 Dval1=0;
static u16 Dval2=0;
static u16 Dval3=0;
static u8 Dval_flag=0;
u16 Dval_tmp;

//u32 RmtRec_xxx=0;	//调试
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

/*
//定时器4中断服务程序	 ,延时版本
void TIM4_IRQHandler(void)
{ 	

if(RmtSta&(1<<6))//得到一个按键的所有信息了
	{
		TIM_TimeBaseStructure.TIM_Period = 50000; //设定计数器自动重装值 15ms
				TIM_TimeBaseStructure.TIM_Prescaler =72-1; 	//预分频器   
				TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
				TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
				TIM_TimeBaseInit(CALL_TIMER, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

		TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Rising); //CC4P=0	设置为上升沿捕获
		TIM_ClearFlag(CALL_TIMER,TIM_IT_Update|TIM_IT_CC3);
		return;
	}

if(RmtSta&(1<<5))//第五位用于模式切换接收到头后的定时问题
{	
    	if(TIM_GetITStatus(CALL_TIMER,TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0X80)//接收到了引导码后切换到定时检测信号电平
		{
			if(RDATA)//1/////1
			{
				RmtRec<<=1;	//左移一位.
				RmtRec|=1;	//接收到1
				RmtCnt++;
				
			}else
			{
				RmtRec<<=1;	//左移一位.
				RmtRec|=0;	//接收到0	 
				RmtCnt++;
			}
			
			if(RmtCnt  !=  0)
			{
				TIM_SetCounter(CALL_TIMER,0);	   	//定时器值为1400us	
				//初始化定时器4 TIM4
				TIM_TimeBaseStructure.TIM_Period = 1300; //设定计数器自动重装值 15ms
				TIM_TimeBaseStructure.TIM_Prescaler =72-1; 	//预分频器   
				TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
				TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
				TIM_TimeBaseInit(CALL_TIMER, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
			  
			}

			//TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Falling);//CC1P=1 设置为下降沿捕获
			//RmtSta|=1<<3;//开启下一次捕获
			//RmtSta&=~(1<<5);//切换到捕捉模式

			if(RmtCnt == 20)
			{
				RmtCnt =0;
				RmtRec_tmp = RmtRec;
				RmtRec=0;
				RmtSta|=1<<6;//标记已经完成一次按键的键值信息采集
				RmtSta&=~(1<<7);//清空引导标识
				RmtSta&=~(1<<5);//切换到捕捉模式
				
			}
		}
	}
		//printf("Head33==us\r\n");
	TIM_ClearITPendingBit(CALL_TIMER, TIM_IT_Update   ); //清除 TIM4更新中断标志
}


if((RmtSta&(1<<5)) == 0)//第五位用于模式切换
{
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
			if(RmtSta&0X10)//完成一次高电平捕获 
			{
				//if(RmtSta&0X80)//接收到了引导码
				//{
					//将定时器切换模式
					//TIM_TimeBaseStructure.TIM_Period = 699; //设定计数器自动重装值 699
					//TIM_TimeBaseStructure.TIM_Prescaler =72-1; 	//预分频器   
					//TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
					//TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
					//TIM_TimeBaseInit(CALL_TIMER, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

					//TIM_SetCounter(CALL_TIMER,699);	   	//定时器值为700us	
					//RmtSta|=1<<5;	//切换到定时模式
					
 				//}
				//else
				{
					if(Dval>10000&&Dval<11000)//&&(RmtCnt==0))		//10664为标准值10ms
					{
						RmtSta|=1<<7;	//标记成功接收到了引导码
						RmtCnt=0;		//重新开始对位的计数
						RmtRec =0;		//接收值清零
						
						TIM_SetCounter(CALL_TIMER,0);	   	//定时器值为700us	
						//初始化定时器4 TIM4
				TIM_TimeBaseStructure.TIM_Period = 600; //设定计数器自动重装值 15ms
				TIM_TimeBaseStructure.TIM_Prescaler =72-1; 	//预分频器   
				TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
				TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
				TIM_TimeBaseInit(CALL_TIMER, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
			  
						RmtSta|=1<<5;	//切换到定时模式

						printf("Head==%dus\r\n",Dval);
					}
				}
			}
			RmtSta&=~(1<<4);//取消上升沿已经被捕获标记
		}
		//}
	}
	//TIM_ClearFlag(CALL_TIMER,TIM_IT_Update|TIM_IT_CC3);
}

TIM_ClearFlag(CALL_TIMER,TIM_IT_Update|TIM_IT_CC3);

}

*/


/*			else if(Dval < 10000)//对头干扰进行过滤
					{
						if(Dval_flag == 0)//第一次获取不符合的值
						{
							Dval1=Dval;
							Dval_tmp=Dval1;
							Dval_flag++;
						}else if(Dval_flag == 1)//第二次获取不符合的值
						{
							Dval2=Dval;
							Dval_tmp =Dval2+Dval1;
							Dval_flag++;
						}
						else if(Dval_flag == 2)//第三次次获取不符合的值
						{
							Dval3=Dval;
							Dval_tmp =Dval2+Dval1+Dval3;
							Dval_flag++;
						}
						else//假如三次还不满足要求，就舍去
						{
							Dval_flag = 0;//将计数清零
							Dval1=0;
							Dval2=0;
							Dval3=0;
							//printf("Dval_tmp111==%dus\r\n",Dval_tmp);
							Dval_tmp=0;
						}

						if(Dval_tmp>10000&&Dval_tmp<10500)
						{
							RmtSta|=1<<7;	//标记成功接收到了引导码
							RmtCnt=0;		//重新开始对位的计数
							RmtRec =0;		//接收值清零
							
							Dval_flag = 0;//将计数清零
							Dval1=0;
							Dval2=0;
							Dval3=0;
							printf("Dval_tmp222==%dus\r\n",Dval_tmp);
						}else if(Dval_tmp>10500)
						{
							Dval_flag = 0;//将计数清零
							Dval1=0;
							Dval2=0;
							Dval3=0;
							//printf("Dval_tmp333==%dus\r\n",Dval_tmp);
						}else{}
					}*/


//定时器4中断服务程序	 
void TIM4_IRQHandler(void)
{ 		    	 
    	if(TIM_GetITStatus(CALL_TIMER,TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0x80)//上次有数据被接收到了//处理溢出
		{	
			RmtSta&=~0X10; //取消上升沿已经被捕获标记
			RmtCnt =0;
			RmtRec=0;
			RmtSta&=~(1<<7);//清空引导标识
			//printf("==%dus\r\n",Dval);
		}else{}

		//printf("TIM_IT_Update==%dus\r\n",Dval);
		
	}else{}
		
 	if(TIM_GetITStatus(CALL_TIMER,TIM_IT_CC3)!=RESET)
	{	  
		if(RmtSta&(1<<6))//得到一个按键的所有信息了
		{
			TIM_SetCounter(CALL_TIMER,0);//清空定时器值
			TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Rising); //CC4P=0	设置为上升沿捕获
			TIM_ClearFlag(CALL_TIMER,TIM_IT_Update|TIM_IT_CC3);
			return;
		}else{}
		
		if(RDATA)//上升沿捕获
		{
			TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Falling);//CC1P=1 设置为下降沿捕获				
	    		TIM_SetCounter(CALL_TIMER,0);	   	//清空定时器值
			RmtSta|=0X10;					//标记上升沿已经被捕获
		}else //下降沿捕获
		{			
  			 Dval=TIM_GetCapture3(CALL_TIMER);//读取CCR1也可以清CC1IF标志位
			 TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Rising); //CC4P=0	设置为上升沿捕获
			if(RmtSta&0X10)//完成一次高电平捕获 
			{
				if(RmtSta&0X80)//接收到了引导码
				{
					if(Dval>150&&Dval<600)	//280为标准值,280us
					{
						RmtRec<<=1;	//左移一位.
						RmtRec|=1;	//接收到1
						RmtCnt++;
						//printf("data1-%d==%dus\r\n",RmtCnt-1,Dval);
					}
					else if(Dval>800&&Dval<2000)			//930为标准值,930us
					{
						RmtRec<<=1;	//左移一位.
						RmtRec|=0;	//接收到0	 
						RmtCnt++;
						//printf("data0-%d==%dus\r\n",RmtCnt-1,Dval);
					}
					else if(Dval>10000&&Dval<15000)//&&(RmtCnt==0))		//10664为标准值10ms
					{
						RmtSta|=1<<7;	//标记成功接收到了引导码
						RmtCnt=0;		//重新开始对位的计数
						RmtRec =0;		//接收值清零
						//printf("Head==%dus\r\n",Dval);
					}
					else
					{
						RmtCnt =0;
						RmtRec=0;
						RmtSta&=~(1<<7);//清空引导标识
						//printf("data2==%dus\r\n",Dval);
					}
					
					if(RmtCnt == 20)
					{
						RmtCnt =0;
						RmtRec_tmp = RmtRec;
						RmtRec=0;
						RmtSta&=~(1<<7);//清空引导标识
						RmtSta|=1<<6;//标记已经完成一次按键的键值信息采集
					}else{}
 				}
				else
				{
					if(Dval>10000&&Dval<15000)//&&(RmtCnt==0))		//10664为标准值10ms
					{
						RmtSta|=1<<7;	//标记成功接收到了引导码
						RmtCnt=0;		//重新开始对位的计数
						RmtRec =0;		//接收值清零
						//printf("Head==%dus\r\n",Dval);
					}
		
				}
			}
			else{}
			RmtSta&=~(1<<4);//取消上升沿已经被捕获标记
		}			 		     	    					   
	}
	else{}
	TIM_ClearFlag(CALL_TIMER,TIM_IT_Update|TIM_IT_CC3);
}


/*
//定时器4中断服务程序	 
void TIM4_IRQHandler(void)
{ 		    	 
    	if(TIM_GetITStatus(CALL_TIMER,TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0x80)//上次有数据被接收到了
		{	
			
		}
		if(RmtSta&(1<<6))//得到一个按键的所有信息了
		{
			//tmp = RmtRec_tmp;
			//RmtRec_tmp =0;
			//RmtSta&=~(1<<6);//清除接收到有效按键标识
			TIM_ClearFlag(CALL_TIMER,TIM_IT_Update|TIM_IT_CC3);	 
			return;
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
					if(Dval>850&&Dval<1000)			//930为标准值,930us
					{
						RmtRec<<=1;	//左移一位.
						RmtRec|=0;	//接收到0	 
						RmtCnt++;
						//printf("data0==%dus\r\n",Dval);
					}else if(Dval>200&&Dval<400)	//280为标准值,280us
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
					}else
					{

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
				else if((Dval>9000&&Dval<12000) &&(RmtCnt==0))		//10664为标准值10ms
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
*/
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
		//RmtRec =0;
		RmtSta&=~(1<<6);//清除接收到有效按键标识
		//printf("RmtRec_tmp==%x\r\n",tmp);
	}
    	return tmp;
}

//TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

void InitializeTim(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	//TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
   	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //使能TIM4时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIOB时钟
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;  //PB8清除之前设置  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IPD; //PB8 输入  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_8);						
	
	//初始化定时器4 TIM4
	TIM_TimeBaseStructure.TIM_Period = 60000; //设定计数器自动重装值 15ms
	TIM_TimeBaseStructure.TIM_Prescaler =72-1; 	//预分频器   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(CALL_TIMER, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
  
	//初始化TIM4输入捕获参数
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=01 	选择输入端 IC3映射到TI3上
  	TIM2_ICInitStructure.TIM_ICPolarity =TIM_ICPolarity_Rising;	//上升沿捕获
  	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  	TIM2_ICInitStructure.TIM_ICFilter = 0x0c;//IC1F=0000 配置输入滤波器 不滤波//滤除16*8=108us的杂波
  	TIM_ICInit(CALL_TIMER, &TIM2_ICInitStructure);
	
	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
	
	TIM_ITConfig(CALL_TIMER,TIM_IT_Update|TIM_IT_CC3,ENABLE);//允许更新中断 ,允许CC1IE捕获中断	
	
   	TIM_Cmd(CALL_TIMER,ENABLE ); 	//使能定时器4

	System.Device.Call_Floor.Remote_Scan =Remote_Scan;
	//System.Device.Call_Floor.Remote_Scan1 =Remote_Scan1;
}

