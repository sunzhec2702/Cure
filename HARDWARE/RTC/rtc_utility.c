#include "rtc_utility.h"

void value_backup_func(u8 reg_index, u8 offset, u16 len_mask, u16 value)
{
    volatile u32 *reg_pointer;
    u32 clear_value = ~(len_mask << offset);
    u32 set_value = ((value & len_mask) << offset);

    switch (reg_index) {
    case 0:
        reg_pointer = &RTC->BKP0R;
        break;
    case 1:
        reg_pointer = &RTC->BKP1R;
        break;
    case 2:
        reg_pointer = &RTC->BKP2R;
        break;
    case 3:
        reg_pointer = &RTC->BKP3R;
        break;
    case 4:
        reg_pointer = &RTC->BKP4R;
        break;
    default:
        break;
    }
    *reg_pointer &= clear_value;
    *reg_pointer |= set_value;
}

u16 value_restore_func(u8 reg_index, u8 offset, u16 len_mask)
{
    volatile u32 *reg_pointer;
    u16 read_value;
    switch (reg_index) {
    case 0:
        reg_pointer = &RTC->BKP0R;
        break;
    case 1:
        reg_pointer = &RTC->BKP1R;
        break;
    case 2:
        reg_pointer = &RTC->BKP2R;
        break;
    case 3:
        reg_pointer = &RTC->BKP3R;
        break;
    case 4:
        reg_pointer = &RTC->BKP4R;
        break;
    default:
        break;
    }
    read_value = ((*reg_pointer) & (len_mask<<offset)) >> offset;
    return read_value;
}