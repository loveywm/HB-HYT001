#ifndef		_UART_H_
#define		_UART_H_


/*
ͨѶЭ��涨

����ͷ(2�ֽ�)+������(1�ֽ�)+���ݳ���(1�ֽ�)+����(�����ֽ�)+У���(1�ֽ�)+����β(2�ֽ�)

1:�����йص�
������:CMD_RT_WEIGHT
//ʵʱ�������ĵ�ѹ����ֵ���������
���ݳ���== 4
���� == 4�ֽڵĲ���ֵ

2:�����йص�
������:CMD_RT_WEIGHT
//ʵʱ�������ĵ�ѹ����ֵ���������
���ݳ���== 4
���� == 4�ֽڵĲ���ֵ


*/






typedef unsigned char byte;

#define         RELAY_UP_LEN         0
#define         RELAY_DOWN_LEN         0
#define         RELAY_STOP_LEN         0
#define		RELAY_LEN  		0

#define         CMD_CURRENT_COUNT_DATA_LEN         4

enum{

       CMD_RT_WEIGHT = 0x01,//ʵʱ�ϴ������ĵ�ѹ����ֵ

	CMD_RT_START_ON,//ͬ�����������Ƿ���
	CMD_RT_START_OFF,//ͬ�����������Ƿ���


        //�̵������Ƶ��ϣ��£�ֹͣ����ת̫
        CMD_RELAY_UP,
        CMD_RELAY_DOWN,
        CMD_RELAY_STOP,

        //����״̬���ֵ�ķ���
        CMD_ERROR_OK,
        CMD_ERROR_IN_DOOR_NOT_CLOSE,//������δ��
        CMD_ERROR_OUT_DOOR_NOT_CLOSE,//������δ��
        CMD_ERROR_DORMANT_NOT_CLOSE,//�촰δ��
        CMD_ERROR_UP_LIMIT,//����
        CMD_ERROR_DOWN_LIMIT,//����

        //ƽ��ֵ��ص�������
        CMD_RT_LEVELING,//ʵʱ�ϴ�ƽ�㴫������ֵ
        CMD_LEVEL_FLOOR_NUMBER,//�·�Ҫƽ���¥��
        CMD_LEVELING_DATA_UP_BUCHANG,//ƽ����������
	 CMD_LEVELING_DATA_DOWN_BUCHANG,//ƽ���½�����
	CMD_LEVELING_DATA_OTHER_BUCHANG,//ƽ����������






        
        CMD_RT_DATA,//��16·�ӿڴ������
        CMD_LEVELING_DATA,//ƽ�㴫ƽ���ֵ

       
        //ƽ����صĿ��ƴ���
        CMD_LEVEL_UPDATA_CURRENT_COUNT,//���µ�ǰ����������ֵ
        CMD_LEVEL_UPDATA_FLOOR,//����¥�����ݽṹָ��
        CMD_LEVEL_UPDATA_LAST_COUNT,//������һ�α����ļ�����ֵ
        CMD_LEVEL_MODE_AUTO,//�����Զ�ƽ��ģʽ��
        CMD_LEVEL_RETURN_FLAG,//����ƽ����ֵ
        

	

};

///////////////////////////////////֡����
//֡ͷ
#define   PROTOCOL_HEAD_1       0x5A
#define   PROTOCOL_HEAD_1_COD   0xA5
#define   PROTOCOL_HEAD_2       0x55

//֡β
#define   PROTOCOL_TAIL_1       0x6A
#define   PROTOCOL_TAIL_1_COD   0x95
#define   PROTOCOL_TAIL_2       0x69

//ת���
#define   PROTOCOL_ESC_CHAR     0x99
#define   PROTOCOL_ESC_CHAR_COD 0x66


void Usart2RxDataTOApp(byte data);
unsigned char   Rcv_Cmd(void);
void  FlushUart2(void);

void HB_Send_Realy_CMD(u8 cmd);


void HB_Send_Realy_UP(void);
void HB_Send_Realy_DOWN(void);
void HB_Send_Realy_STOP(void);


void HB_Send_Current_Count(u32 count);
void HB_Send_Floor(Floor_Data   *floor);
void HB_Send_Last_Count(u32 count);

void HB_Send_Mode_Auto(u8 floor_count);


void HB_Send_CMD_DATA0(u8 cmd);
void HB_Send_CMD_DATA4(u8 cmd,u32 data4);


enum
{
	CMD_FLOOR_DATA,//¥��
	CMD_SET_DATA,//����¥��
	CMD_DEL_DATA,//ɾ��¥��
};

void HB_Send_CMD_DATA1(u8 cmd,u8 data1);
	
#endif

