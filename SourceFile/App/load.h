#ifndef		_LOAD_H_
#define		_LOAD_H_

#define N_ADC   32


u8 Load_Set_Calibration(u8 calibration);


void get_weight_clear_value(void);
void weight_flag(void);

u16  ADC_Filter(void);
u16  ADC_Filter_V(void);
#endif

