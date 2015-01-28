#ifndef		_ENCODER_H_
#define		_ENCODER_H_

u8	Encoder_Demarcate(void);
void	Encode_Save(Floor_Data  *floor_data);
void	Encode_Read(u8 floor_num,Floor_Data  *floor_data);

u8	Encoder_Demarcate_Init(void);

u8	Auto_Encoder_Demarcate(void);
								
#endif		//_ENCODER_H_
