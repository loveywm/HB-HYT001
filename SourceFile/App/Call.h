#ifndef		_CALL_H_
#define		_CALL_H_


u8 Call_Learn(void);
u8 Call_Del(void);


void	Call_Save(Call_Data  *call_data);
void	Call_Read(u8 call_num,Call_Data  *call_data);

#endif

