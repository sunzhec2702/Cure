#include "log_system.h"
#include "rtc.h"
#include "ff.h"

#define ERROR_FILE "error.log"
#define LOG_FOLDER "0:/logs/" // 0 is sd card.

#define MAX_FILE_NAME_LEN 20
static DIR log_dir;
static FIL log_fil;
static char log_file[MAX_FILE_NAME_LEN];

/* Maybe we have to make sure the sd card is formatted or not */

ErrorStatus log_system_init()
{
    if (is_calendar_initialized() == FALSE)
        return ERROR;
    u32 ret = FR_OK;
    ret = f_opendir(&log_dir, LOG_FOLDER);
    if (ret == FR_NO_PATH || ret == FR_NO_FILE)
    {
        ret = f_mkdir(LOG_FOLDER);
        if (ret != FR_OK)
        {
            return ERROR;
        }
    }
    ret = f_opendir(&log_dir, LOG_FOLDER);
    if (ret != FR_OK)
        return ERROR;

    // Get the current calendar.
    calendar_obj *cur_cal = get_current_calendar();
    snprintf(log_file, MAX_FILE_NAME_LEN, "%d_%d_%d.log", cur_cal->year, cur_cal->month, cur_cal->date);

    ret = f_open()
    return SUCCESS;
}