/*
********************************************************************************
*                          
********************************************************************************
*文件名     : device_io.c
*作用       : 输入输出设备
*原理       : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/08/13       初始版本
********************************************************************************
*/

#include "drive.h"
#include "system.h"



//loveywm 20140906
#define BeepPin	PcOut(3)


static void SetBeep(bool status)
{
    BeepPin = status;
}


/*******************************************************************************
* 函数名	: MasterSystick100Routine
* 描述	    : 系统节拍100/S，即10mS一次扫描获取键值,获取主令值
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*******************************************************************************/
void MasterSystick100Routine(void) 
{
    
}


static void HB_Gpio_Set_Value(u8 HB_Gpio_type,u8 HB_Gpio_Value)
{
        switch(HB_Gpio_type)
        {
                case    WTV_RST:
                        PeOut(11) = HB_Gpio_Value; 
                        break;
                case    WTV_CS:
                        PeOut(8) = HB_Gpio_Value; 
                        break;
                case    WTV_DATA:
                        PeOut(10) = HB_Gpio_Value; 
                        break;
                case    WTV_CLK:
                        PeOut(9) = HB_Gpio_Value; 
                        break;

		 case    RELAY_1:
                        PdOut(8) = HB_Gpio_Value; 
                        break;
		 case    RELAY_2:
                        PdOut(9) = HB_Gpio_Value; 
                        break;
		 case    RELAY_3:
                        PdOut(10) = HB_Gpio_Value; 
                        break;
		 case    RELAY_4:
                        PdOut(11) = HB_Gpio_Value; 
                        break;
                    default:
                        break;
        }
}

static u8  HB_Gpio_Get_Value(u8 HB_Gpio_type)
{
        u8 value=0;
        switch(HB_Gpio_type)
        {
	        case    WTV_BUSY:
	                        value = PeIn(7);
	                        break;                             
	        default:
	                break;
        }
        return value;
}

void InitializeIO(void)
{
    	//loveywm 20140906
    	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
    	BeepPin = 0;    

	//继电器输出控制脚
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
        	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        	GPIO_Init(GPIOD, &GPIO_InitStructure);
			
	PdOut(8) = 1;
	PdOut(9) =1;
	PdOut(10)=1;
	PdOut(11)=1;


	//语音的四个控制端口
	//RST引脚
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11;
         GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        	GPIO_Init(GPIOE, &GPIO_InitStructure);

	//其他三个控制脚
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
        	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        	GPIO_Init(GPIOE, &GPIO_InitStructure);

	//busy引脚
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//下拉输入
 	GPIO_Init(GPIOE, &GPIO_InitStructure);

	
    
    	System.Device.IO.SetBeep = SetBeep;
	System.Device.IO.HB_Gpio_Set_Value = HB_Gpio_Set_Value;
	System.Device.IO.HB_Gpio_Get_Value =HB_Gpio_Get_Value;
}

