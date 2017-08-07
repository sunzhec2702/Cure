#ifndef __RTC_UTILITY_H__
#define __RTC_UTILITY_H__
#include "sys.h"

// TODO: GD related code, may need to update to STM32.
void value_backup_func(u8 reg_index, u8 offset, u16 len_mask, u16 value);
u16 value_restore_func(u8 reg_index, u8 offset, u16 len_mask);


#endif