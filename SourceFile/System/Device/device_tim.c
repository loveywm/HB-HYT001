#include "system.h"
#include "drive.h"

#define CALL_TIMER   TIM4 // Encoder unit connected to TIM4

#define RDATA PbIn(8)	 //��ʱ�����������

//��ʱ��2ͨ��3���벶������
TIM_ICInitTypeDef  TIM2_ICInitStructure;


//ң��������״̬
//[7]:�յ����������־
//[6]:�õ���һ��������������Ϣ
//[5]:����	
//[4]:����������Ƿ��Ѿ�������
//[3]:��ȡͷ���½��ز����־
//[2:0]:�����ʱ������ʱû��
u8 	RmtSta=0;	

u16 Dval;		//�½���ʱ��������ֵ
u32 RmtRec=0;	//������յ�������	   		
u32 RmtRec_tmp=0;	//������յ�������	   	
u8  RmtCnt=0;	//�������µĴ���	 


//��ͷ���й��˱���
static u16 Dval1=0;
static u16 Dval2=0;
static u16 Dval3=0;
static u8 Dval_flag=0;
u16 Dval_tmp;

//u32 RmtRec_xxx=0;	//����
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

/*
//��ʱ��4�жϷ������	 ,��ʱ�汾
void TIM4_IRQHandler(void)
{ 	

if(RmtSta&(1<<6))//�õ�һ��������������Ϣ��
	{
		TIM_TimeBaseStructure.TIM_Period = 50000; //�趨�������Զ���װֵ 15ms
				TIM_TimeBaseStructure.TIM_Prescaler =72-1; 	//Ԥ��Ƶ��   
				TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
				TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
				TIM_TimeBaseInit(CALL_TIMER, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

		TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Rising); //CC4P=0	����Ϊ�����ز���
		TIM_ClearFlag(CALL_TIMER,TIM_IT_Update|TIM_IT_CC3);
		return;
	}

if(RmtSta&(1<<5))//����λ����ģʽ�л����յ�ͷ��Ķ�ʱ����
{	
    	if(TIM_GetITStatus(CALL_TIMER,TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0X80)//���յ�����������л�����ʱ����źŵ�ƽ
		{
			if(RDATA)//1/////1
			{
				RmtRec<<=1;	//����һλ.
				RmtRec|=1;	//���յ�1
				RmtCnt++;
				
			}else
			{
				RmtRec<<=1;	//����һλ.
				RmtRec|=0;	//���յ�0	 
				RmtCnt++;
			}
			
			if(RmtCnt  !=  0)
			{
				TIM_SetCounter(CALL_TIMER,0);	   	//��ʱ��ֵΪ1400us	
				//��ʼ����ʱ��4 TIM4
				TIM_TimeBaseStructure.TIM_Period = 1300; //�趨�������Զ���װֵ 15ms
				TIM_TimeBaseStructure.TIM_Prescaler =72-1; 	//Ԥ��Ƶ��   
				TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
				TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
				TIM_TimeBaseInit(CALL_TIMER, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
			  
			}

			//TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Falling);//CC1P=1 ����Ϊ�½��ز���
			//RmtSta|=1<<3;//������һ�β���
			//RmtSta&=~(1<<5);//�л�����׽ģʽ

			if(RmtCnt == 20)
			{
				RmtCnt =0;
				RmtRec_tmp = RmtRec;
				RmtRec=0;
				RmtSta|=1<<6;//����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ�
				RmtSta&=~(1<<7);//���������ʶ
				RmtSta&=~(1<<5);//�л�����׽ģʽ
				
			}
		}
	}
		//printf("Head33==us\r\n");
	TIM_ClearITPendingBit(CALL_TIMER, TIM_IT_Update   ); //��� TIM4�����жϱ�־
}


if((RmtSta&(1<<5)) == 0)//����λ����ģʽ�л�
{
 	if(TIM_GetITStatus(CALL_TIMER,TIM_IT_CC3)!=RESET)
	{	  
	
		if(RDATA)//�����ز���
		{
			TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Falling);//CC1P=1 ����Ϊ�½��ز���				
	    		TIM_SetCounter(CALL_TIMER,0);	   	//��ն�ʱ��ֵ
			RmtSta|=0X10;					//����������Ѿ�������
		}else //�½��ز���
		{			
  			Dval=TIM_GetCapture3(CALL_TIMER);//��ȡCCR1Ҳ������CC1IF��־λ
			TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Rising); //CC4P=0	����Ϊ�����ز���
			if(RmtSta&0X10)//���һ�θߵ�ƽ���� 
			{
				//if(RmtSta&0X80)//���յ���������
				//{
					//����ʱ���л�ģʽ
					//TIM_TimeBaseStructure.TIM_Period = 699; //�趨�������Զ���װֵ 699
					//TIM_TimeBaseStructure.TIM_Prescaler =72-1; 	//Ԥ��Ƶ��   
					//TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
					//TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
					//TIM_TimeBaseInit(CALL_TIMER, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

					//TIM_SetCounter(CALL_TIMER,699);	   	//��ʱ��ֵΪ700us	
					//RmtSta|=1<<5;	//�л�����ʱģʽ
					
 				//}
				//else
				{
					if(Dval>10000&&Dval<11000)//&&(RmtCnt==0))		//10664Ϊ��׼ֵ10ms
					{
						RmtSta|=1<<7;	//��ǳɹ����յ���������
						RmtCnt=0;		//���¿�ʼ��λ�ļ���
						RmtRec =0;		//����ֵ����
						
						TIM_SetCounter(CALL_TIMER,0);	   	//��ʱ��ֵΪ700us	
						//��ʼ����ʱ��4 TIM4
				TIM_TimeBaseStructure.TIM_Period = 600; //�趨�������Զ���װֵ 15ms
				TIM_TimeBaseStructure.TIM_Prescaler =72-1; 	//Ԥ��Ƶ��   
				TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
				TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
				TIM_TimeBaseInit(CALL_TIMER, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
			  
						RmtSta|=1<<5;	//�л�����ʱģʽ

						printf("Head==%dus\r\n",Dval);
					}
				}
			}
			RmtSta&=~(1<<4);//ȡ���������Ѿ���������
		}
		//}
	}
	//TIM_ClearFlag(CALL_TIMER,TIM_IT_Update|TIM_IT_CC3);
}

TIM_ClearFlag(CALL_TIMER,TIM_IT_Update|TIM_IT_CC3);

}

*/


/*			else if(Dval < 10000)//��ͷ���Ž��й���
					{
						if(Dval_flag == 0)//��һ�λ�ȡ�����ϵ�ֵ
						{
							Dval1=Dval;
							Dval_tmp=Dval1;
							Dval_flag++;
						}else if(Dval_flag == 1)//�ڶ��λ�ȡ�����ϵ�ֵ
						{
							Dval2=Dval;
							Dval_tmp =Dval2+Dval1;
							Dval_flag++;
						}
						else if(Dval_flag == 2)//�����δλ�ȡ�����ϵ�ֵ
						{
							Dval3=Dval;
							Dval_tmp =Dval2+Dval1+Dval3;
							Dval_flag++;
						}
						else//�������λ�������Ҫ�󣬾���ȥ
						{
							Dval_flag = 0;//����������
							Dval1=0;
							Dval2=0;
							Dval3=0;
							//printf("Dval_tmp111==%dus\r\n",Dval_tmp);
							Dval_tmp=0;
						}

						if(Dval_tmp>10000&&Dval_tmp<10500)
						{
							RmtSta|=1<<7;	//��ǳɹ����յ���������
							RmtCnt=0;		//���¿�ʼ��λ�ļ���
							RmtRec =0;		//����ֵ����
							
							Dval_flag = 0;//����������
							Dval1=0;
							Dval2=0;
							Dval3=0;
							printf("Dval_tmp222==%dus\r\n",Dval_tmp);
						}else if(Dval_tmp>10500)
						{
							Dval_flag = 0;//����������
							Dval1=0;
							Dval2=0;
							Dval3=0;
							//printf("Dval_tmp333==%dus\r\n",Dval_tmp);
						}else{}
					}*/


//��ʱ��4�жϷ������	 
void TIM4_IRQHandler(void)
{ 		    	 
    	if(TIM_GetITStatus(CALL_TIMER,TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0x80)//�ϴ������ݱ����յ���//�������
		{	
			RmtSta&=~0X10; //ȡ���������Ѿ���������
			RmtCnt =0;
			RmtRec=0;
			RmtSta&=~(1<<7);//���������ʶ
			//printf("==%dus\r\n",Dval);
		}else{}

		//printf("TIM_IT_Update==%dus\r\n",Dval);
		
	}else{}
		
 	if(TIM_GetITStatus(CALL_TIMER,TIM_IT_CC3)!=RESET)
	{	  
		if(RmtSta&(1<<6))//�õ�һ��������������Ϣ��
		{
			TIM_SetCounter(CALL_TIMER,0);//��ն�ʱ��ֵ
			TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Rising); //CC4P=0	����Ϊ�����ز���
			TIM_ClearFlag(CALL_TIMER,TIM_IT_Update|TIM_IT_CC3);
			return;
		}else{}
		
		if(RDATA)//�����ز���
		{
			TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Falling);//CC1P=1 ����Ϊ�½��ز���				
	    		TIM_SetCounter(CALL_TIMER,0);	   	//��ն�ʱ��ֵ
			RmtSta|=0X10;					//����������Ѿ�������
		}else //�½��ز���
		{			
  			 Dval=TIM_GetCapture3(CALL_TIMER);//��ȡCCR1Ҳ������CC1IF��־λ
			 TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Rising); //CC4P=0	����Ϊ�����ز���
			if(RmtSta&0X10)//���һ�θߵ�ƽ���� 
			{
				if(RmtSta&0X80)//���յ���������
				{
					if(Dval>150&&Dval<600)	//280Ϊ��׼ֵ,280us
					{
						RmtRec<<=1;	//����һλ.
						RmtRec|=1;	//���յ�1
						RmtCnt++;
						//printf("data1-%d==%dus\r\n",RmtCnt-1,Dval);
					}
					else if(Dval>800&&Dval<2000)			//930Ϊ��׼ֵ,930us
					{
						RmtRec<<=1;	//����һλ.
						RmtRec|=0;	//���յ�0	 
						RmtCnt++;
						//printf("data0-%d==%dus\r\n",RmtCnt-1,Dval);
					}
					else if(Dval>10000&&Dval<15000)//&&(RmtCnt==0))		//10664Ϊ��׼ֵ10ms
					{
						RmtSta|=1<<7;	//��ǳɹ����յ���������
						RmtCnt=0;		//���¿�ʼ��λ�ļ���
						RmtRec =0;		//����ֵ����
						//printf("Head==%dus\r\n",Dval);
					}
					else
					{
						RmtCnt =0;
						RmtRec=0;
						RmtSta&=~(1<<7);//���������ʶ
						//printf("data2==%dus\r\n",Dval);
					}
					
					if(RmtCnt == 20)
					{
						RmtCnt =0;
						RmtRec_tmp = RmtRec;
						RmtRec=0;
						RmtSta&=~(1<<7);//���������ʶ
						RmtSta|=1<<6;//����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ�
					}else{}
 				}
				else
				{
					if(Dval>10000&&Dval<15000)//&&(RmtCnt==0))		//10664Ϊ��׼ֵ10ms
					{
						RmtSta|=1<<7;	//��ǳɹ����յ���������
						RmtCnt=0;		//���¿�ʼ��λ�ļ���
						RmtRec =0;		//����ֵ����
						//printf("Head==%dus\r\n",Dval);
					}
		
				}
			}
			else{}
			RmtSta&=~(1<<4);//ȡ���������Ѿ���������
		}			 		     	    					   
	}
	else{}
	TIM_ClearFlag(CALL_TIMER,TIM_IT_Update|TIM_IT_CC3);
}


/*
//��ʱ��4�жϷ������	 
void TIM4_IRQHandler(void)
{ 		    	 
    	if(TIM_GetITStatus(CALL_TIMER,TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0x80)//�ϴ������ݱ����յ���
		{	
			
		}
		if(RmtSta&(1<<6))//�õ�һ��������������Ϣ��
		{
			//tmp = RmtRec_tmp;
			//RmtRec_tmp =0;
			//RmtSta&=~(1<<6);//������յ���Ч������ʶ
			TIM_ClearFlag(CALL_TIMER,TIM_IT_Update|TIM_IT_CC3);	 
			return;
		}
	}
 	if(TIM_GetITStatus(CALL_TIMER,TIM_IT_CC3)!=RESET)
	{	  
		if(RDATA)//�����ز���
		{
			TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Falling);//CC1P=1 ����Ϊ�½��ز���				
	    		TIM_SetCounter(CALL_TIMER,0);	   	//��ն�ʱ��ֵ
			RmtSta|=0X10;					//����������Ѿ�������
		}else //�½��ز���
		{			
  			 Dval=TIM_GetCapture3(CALL_TIMER);//��ȡCCR1Ҳ������CC1IF��־λ
			 TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Rising); //CC4P=0	����Ϊ�����ز���
			if(RmtSta&0X10)					//���һ�θߵ�ƽ���� 
			{
 				if(RmtSta&0X80)//���յ���������
				{
					if(Dval>850&&Dval<1000)			//930Ϊ��׼ֵ,930us
					{
						RmtRec<<=1;	//����һλ.
						RmtRec|=0;	//���յ�0	 
						RmtCnt++;
						//printf("data0==%dus\r\n",Dval);
					}else if(Dval>200&&Dval<400)	//280Ϊ��׼ֵ,280us
					{
						RmtRec<<=1;	//����һλ.
						RmtRec|=1;	//���յ�1
						RmtCnt++;
						//printf("data1==%dus\r\n",Dval);
					}
					else if(Dval>1500)	//�õ���־ͷ�󳬹�2000us�������
					{
						RmtCnt =0;
						RmtSta&=~(1<<7);//���������ʶ
						//RmtSta&=~(1<<6);//������յ���Ч������ʶ
						RmtRec=0;
						//printf("datax==%dus\r\n",Dval);
						//RmtSta&=0XF0;	//��ռ�ʱ��		
					}else
					{

					}
					if(RmtCnt == 24)
					{
						RmtCnt =0;
						RmtRec_tmp = RmtRec;
						RmtSta|=1<<6;//����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ�
					}
					//else if(Dval>2200&&Dval<2600)	//�õ�������ֵ���ӵ���Ϣ 2500Ϊ��׼ֵ2.5ms
					//{
						//RmtCnt++; 		//������������1��
						//RmtSta&=0XF0;	//��ռ�ʱ��		
					//}
 				}
				else if((Dval>9000&&Dval<12000) &&(RmtCnt==0))		//10664Ϊ��׼ֵ10ms
				{
					RmtSta|=1<<7;	//��ǳɹ����յ���������
					//RmtCnt=0;		//�����������������
					//printf("Head==%dus\r\n",Dval);
				}						 
			}
			RmtSta&=~(1<<4);//ȡ���������Ѿ���������
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
		if(RmtSta&0x80)//�ϴ������ݱ����յ���
		{	
			RmtSta&=~0X10;						//ȡ���������Ѿ���������
			if((RmtSta&0X0F)==0X00)
				RmtSta|=1<<6;//����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ�
			if((RmtSta&0X0F)<14)
			{
				RmtSta++;
			}
			else
			{
				RmtSta&=~(1<<7);//���������ʶ
				RmtSta&=0XF0;	//��ռ�����	
			}						 	   	
		}							    
	}
 	if(TIM_GetITStatus(CALL_TIMER,TIM_IT_CC3)!=RESET)
	{	  
		if(RDATA)//�����ز���
		{

			TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Falling);//CC1P=1 ����Ϊ�½��ز���				
	    		TIM_SetCounter(CALL_TIMER,0);	   	//��ն�ʱ��ֵ
			RmtSta|=0X10;					//����������Ѿ�������
		}else //�½��ز���
		{			
  			 Dval=TIM_GetCapture3(CALL_TIMER);//��ȡCCR1Ҳ������CC1IF��־λ
			 TIM_OC3PolarityConfig(CALL_TIMER,TIM_ICPolarity_Rising); //CC4P=0	����Ϊ�����ز���
 			
			if(RmtSta&0X10)					//���һ�θߵ�ƽ���� 
			{
 				if(RmtSta&0X80)//���յ���������
				{
					
					if(Dval>900&&Dval<1200)			//1032Ϊ��׼ֵ,560us
					{
						RmtRec<<=1;	//����һλ.
						RmtRec|=0;	//���յ�0	   
					}else if(Dval>280&&Dval<400)	//344Ϊ��׼ֵ,1680us
					{
						RmtRec<<=1;	//����һλ.
						RmtRec|=1;	//���յ�1
					}
					//else if(Dval>2200&&Dval<2600)	//�õ�������ֵ���ӵ���Ϣ 2500Ϊ��׼ֵ2.5ms
					//{
						//RmtCnt++; 		//������������1��
						//RmtSta&=0XF0;	//��ռ�ʱ��		
					//}
 				}else if(Dval>10000&&Dval<14000)		//10664Ϊ��׼ֵ4.5ms
				{
					RmtSta|=1<<7;	//��ǳɹ����յ���������
					RmtCnt=0;		//�����������������
				}						 
			}
			RmtSta&=~(1<<4);
		}				 		     	    					   
	}
 	TIM_ClearFlag(CALL_TIMER,TIM_IT_Update|TIM_IT_CC3);	    
}

*/

//����������
//����ֵ:
//	 0,û���κΰ�������
//����,���µİ�����ֵ.
u32 Remote_Scan(void)
{        
	u32 tmp=0;
	if(RmtSta&(1<<6))//�õ�һ��������������Ϣ��
	{

		tmp = RmtRec_tmp;
		RmtRec_tmp =0;
		//RmtRec =0;
		RmtSta&=~(1<<6);//������յ���Ч������ʶ
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
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʹ��TIM4ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //ʹ��GPIOBʱ��
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;  //PB8���֮ǰ����  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IPD; //PB8 ����  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_8);						
	
	//��ʼ����ʱ��4 TIM4
	TIM_TimeBaseStructure.TIM_Period = 60000; //�趨�������Զ���װֵ 15ms
	TIM_TimeBaseStructure.TIM_Prescaler =72-1; 	//Ԥ��Ƶ��   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(CALL_TIMER, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  
	//��ʼ��TIM4���벶�����
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=01 	ѡ������� IC3ӳ�䵽TI3��
  	TIM2_ICInitStructure.TIM_ICPolarity =TIM_ICPolarity_Rising;	//�����ز���
  	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  	TIM2_ICInitStructure.TIM_ICFilter = 0x0c;//IC1F=0000 ���������˲��� ���˲�//�˳�16*8=108us���Ӳ�
  	TIM_ICInit(CALL_TIMER, &TIM2_ICInitStructure);
	
	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;  //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 
	
	TIM_ITConfig(CALL_TIMER,TIM_IT_Update|TIM_IT_CC3,ENABLE);//��������ж� ,����CC1IE�����ж�	
	
   	TIM_Cmd(CALL_TIMER,ENABLE ); 	//ʹ�ܶ�ʱ��4

	System.Device.Call_Floor.Remote_Scan =Remote_Scan;
	//System.Device.Call_Floor.Remote_Scan1 =Remote_Scan1;
}

