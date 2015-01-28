/*
********************************************************************************
*                          
********************************************************************************
*�ļ���     : device_io.c
*����       : ��������豸
*ԭ��       : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/08/13       ��ʼ�汾
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
* ������	: MasterSystick100Routine
* ����	    : ϵͳ����100/S����10mSһ��ɨ���ȡ��ֵ,��ȡ����ֵ
* �������  : ��
* ���ز���  : ��
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

	//�̵���������ƽ�
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
        	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        	GPIO_Init(GPIOD, &GPIO_InitStructure);
			
	PdOut(8) = 1;
	PdOut(9) =1;
	PdOut(10)=1;
	PdOut(11)=1;


	//�������ĸ����ƶ˿�
	//RST����
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11;
         GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        	GPIO_Init(GPIOE, &GPIO_InitStructure);

	//�����������ƽ�
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
        	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        	GPIO_Init(GPIOE, &GPIO_InitStructure);

	//busy����
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//��������
 	GPIO_Init(GPIOE, &GPIO_InitStructure);

	
    
    	System.Device.IO.SetBeep = SetBeep;
	System.Device.IO.HB_Gpio_Set_Value = HB_Gpio_Set_Value;
	System.Device.IO.HB_Gpio_Get_Value =HB_Gpio_Get_Value;
}

