#include "log_system.h"
#include "rtc.h"
#include "ff.h"

#define ERROR_FILE "error.log" // error.log. For end user analysis.

#define LOG_FOLDER "0:/logs/" // 0 is sd card.

#define MAX_FILE_NAME_LEN 20
#define MAX_LOG_MESSAGE_LEN 30

static DIR log_dir;
static FIL log_file, record_file;
static char log_file_name[MAX_FILE_NAME_LEN];
static char record_file_name[MAX_FILE_NAME_LEN];

static calendar_obj output_start;

/* Maybe we have to make sure the sd card is formatted or not */
/* 
   When we press the button, we set the start_output and enable a task
   to update the accumulate time. And at the same time, print a log in
   the log file.
*/

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
    snprintf(log_file_name, MAX_FILE_NAME_LEN, "%d_%d_%d.log", cur_cal->year, cur_cal->month, cur_cal->date);
    snprintf(record_file_name, MAX_FILE_NAME_LEN, "%d_%d_%d.dat", cur_cal->year, cur_cal->month, cur_cal->date);
    // Try to create a new file
    ret = f_open(&log_file, log_file_name, FA_READ | FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS | FA_CREATE_NEW);
    if (ret != FR_OK)
        return ERROR;
    f_close(&log_file);
    ret = f_open(&record_file, log_file_name, FA_READ | FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS | FA_CREATE_NEW);
    if (ret != FR_OK)
        return ERROR;
    f_close(&record_file);
    return SUCCESS;
}

ErrorStatus update_output_log()
{
    if (is_calendar_initialized() == FALSE)
        return ERROR;
    char log_message[MAX_LOG_MESSAGE_LEN];
    int byte_write;
    calendar_obj *cur_cal = get_current_calendar();
    // print a log.
    snprintf(log_message, MAX_LOG_MESSAGE_LEN, "%d-%d-%d-%d-%d\n", cur_cal->year, cur_cal->month, cur_cal->date, cur_cal->hour, cur_cal->min);
    ret = f_open(&log_file, log_file_name, FA_READ | FA_WRITE);
    f_lseek(&log_file, log_file.fsize);
    f_write(&log_file, log_message, MAX_LOG_MESSAGE_LEN, &byte_write);
    f_sync(&log_file);
    f_close(&log_file);
    // TODO: update the accumulate time.
    // read current time, compare with the start time.
    // snprintf()
    return SUCCESS;
}

void print_debug_log(char *message)
{
    return;
}