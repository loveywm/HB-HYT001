#ifndef		_UART_H_
#define		_UART_H_


/*
通讯协议规定

命令头(2字节)+命令字(1字节)+数据长度(1字节)+数据(若干字节)+校验和(1字节)+命令尾(2字节)

1:载重有关的
命令字:CMD_RT_WEIGHT
//实时将重量的电压采样值传到服务端
数据长度== 4
数据 == 4字节的采样值

2:载重有关的
命令字:CMD_RT_WEIGHT
//实时将重量的电压采样值传到服务端
数据长度== 4
数据 == 4字节的采样值


*/






typedef unsigned char byte;

#define         RELAY_UP_LEN         0
#define         RELAY_DOWN_LEN         0
#define         RELAY_STOP_LEN         0
#define		RELAY_LEN  		0

#define         CMD_CURRENT_COUNT_DATA_LEN         4

enum{

       CMD_RT_WEIGHT = 0x01,//实时上传重量的电压采样值

	CMD_RT_START_ON,//同步启动按键是否按下
	CMD_RT_START_OFF,//同步启动按键是否按下


        //继电器控制的上，下，停止三个转太
        CMD_RELAY_UP,
        CMD_RELAY_DOWN,
        CMD_RELAY_STOP,

        //各种状态检测值的反馈
        CMD_ERROR_OK,
        CMD_ERROR_IN_DOOR_NOT_CLOSE,//进料门未关
        CMD_ERROR_OUT_DOOR_NOT_CLOSE,//进料门未关
        CMD_ERROR_DORMANT_NOT_CLOSE,//天窗未关
        CMD_ERROR_UP_LIMIT,//上限
        CMD_ERROR_DOWN_LIMIT,//下限

        //平层值相关的命令字
        CMD_RT_LEVELING,//实时上传平层传感器的值
        CMD_LEVEL_FLOOR_NUMBER,//下发要平层的楼层
        CMD_LEVELING_DATA_UP_BUCHANG,//平层上升补偿
	 CMD_LEVELING_DATA_DOWN_BUCHANG,//平层下降补偿
	CMD_LEVELING_DATA_OTHER_BUCHANG,//平层其他补偿






        
        CMD_RT_DATA,//和16路接口错误代码
        CMD_LEVELING_DATA,//平层传平层的值

       
        //平层相关的控制代码
        CMD_LEVEL_UPDATA_CURRENT_COUNT,//更新当前编码器计数值
        CMD_LEVEL_UPDATA_FLOOR,//更新楼层数据结构指令
        CMD_LEVEL_UPDATA_LAST_COUNT,//跟新上一次保留的计数器值
        CMD_LEVEL_MODE_AUTO,//设置自动平层模式，
        CMD_LEVEL_RETURN_FLAG,//返回平层结果值
        

	

};

///////////////////////////////////帧描述
//帧头
#define   PROTOCOL_HEAD_1       0x5A
#define   PROTOCOL_HEAD_1_COD   0xA5
#define   PROTOCOL_HEAD_2       0x55

//帧尾
#define   PROTOCOL_TAIL_1       0x6A
#define   PROTOCOL_TAIL_1_COD   0x95
#define   PROTOCOL_TAIL_2       0x69

//转义符
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
	CMD_FLOOR_DATA,//楼层
	CMD_SET_DATA,//设置楼层
	CMD_DEL_DATA,//删除楼层
};

void HB_Send_CMD_DATA1(u8 cmd,u8 data1);
	
#endif

