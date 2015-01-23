#ifndef		_ENCODER_H_
#define		_ENCODER_H_

u8	Encoder_Demarcate(void);
void	Encode_Save(Floor_Data  *floor_data);
void	Encode_Read(u8 floor_num,Floor_Data  *floor_data);

u8	Encoder_Demarcate_Init(void);
								
#endif		//_ENCODER_H_
