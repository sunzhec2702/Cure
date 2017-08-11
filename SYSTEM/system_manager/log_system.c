#include "log_system.h"
#include "rtc.h"
#include "ff.h"
#include "string.h"

static DIR log_dir;
static FIL log_file, daily_record_file, total_record_file, error_file;

static char log_file_name[MAX_FILE_NAME_LEN];
static char daily_record_file_name[MAX_FILE_NAME_LEN];
static char total_record_file_name[MAX_FILE_NAME_LEN];
static char error_file_name[MAX_FILE_NAME_LEN];

static char log_message[MAX_LOG_MESSAGE_LEN];
static char error_message[MAX_LOG_MESSAGE_LEN];


/* Maybe we have to make sure the sd card is formatted or not */
/* 
   When we press the button, we set the start_output and enable a task
   to update the accumulate time. And at the same time, print a log in
   the log file.
*/

// http://www.tuicool.com/articles/3qIn6v
// f_open document.
ErrorStatus log_system_init()
{
    if (is_calendar_initialized() == FALSE)
        return ERROR;
    u32 ret = FR_OK;

    // Create the log folder if not existed.
    ret = f_opendir(&log_dir, LOG_FOLDER);
    if (ret == FR_NO_PATH || ret == FR_NO_FILE)
    {
        ret = f_mkdir(LOG_FOLDER);
        if (ret != FR_OK)
        {
            return ERROR;
        }
        ret = f_opendir(&log_dir, LOG_FOLDER);
        if (ret != FR_OK)
            return ERROR;
    }
    f_closedir(&log_dir);

    // Initiate the three file names.
    // Get the current calendar.
    calendar_obj *cur_cal = get_current_calendar();
    snprintf(log_file_name, MAX_FILE_NAME_LEN, "%s/%d_%d_%d.log", LOG_FOLDER, cur_cal->year, cur_cal->month, cur_cal->date);
    snprintf(daily_record_file_name, MAX_FILE_NAME_LEN, "%s/%d_%d_%d.dat", LOG_FOLDER, cur_cal->year, cur_cal->month, cur_cal->date);
    snprintf(total_record_file_name£¬ MAX_FILE_NAME_LEN, "%s/record_summary.dat", LOG_FOLDER);
    snprintf(error_file_name, MAX_FILE_NAME_LEN, "%s/error_log.txt", LOG_FOLDER);

    // Log File
    ret = f_open(&log_file, log_file_name, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
    if (ret != FR_OK)
        return ERROR;
    f_sync(&log_file);
    //f_close(&log_file);
    // Daily record file
    ret = f_open(&daily_record_file, daily_record_file_name, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
    if (ret != FR_OK)
        return ERROR;
    f_sync(&daily_record_file);
    //f_close(&daily_record_file);
    // Total record file
    ret = f_open(&total_record_file, total_record_file_name, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
    if (ret != FR_OK)
        return ERROR;
    f_sync(&total_record_file);
    //f_close(&total_record_file);
    // Error file
    ret = f_open(&error_file, error_file_name, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
    if (ret != FR_OK)
        return ERROR;
    f_sync(&error_file);

    update_output_log();
    return SUCCESS;
}

void close_all_files()
{
    f_close(&log_file);
    f_close(&daily_record_file);
    f_close(&total_record_file);
    f_close(&error_file);
}

// TODO: We should print the machine status as well.
ErrorStatus update_output_log()
{
    u8 ret = 0;
     if (is_calendar_initialized() == FALSE)
        return ERROR;
    calendar_obj *cur_cal = get_current_calendar();
    if (f_lseek(&log_file, log_file.fsize) != FR_OK)
        return ERROR;
    if (f_printf(&log_file, "%d-%d-%d-%d-%d-%d\n", cur_cal->year, cur_cal->month, cur_cal->date, cur_cal->hour, cur_cal->min, cur_cal->sec) != FR_OK)
        return ERROR;
    if (f_sync(&log_file) != FR_OK)
        return ERROR;
    //f_close(&log_file);
    return SUCCESS;
}

ErrorStatus update_record_log()
{
    
}

void print_debug_log(char *message)
{
    return;
}
