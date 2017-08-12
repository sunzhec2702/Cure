#include "log_system.h"
#include "rtc.h"
#include "ff.h"
#include "string.h"

static DIR log_dir;
static FIL log_file, daily_record_file, total_record_file, error_file;

static output_status current_status = OUTPUT_DISABLE;

static char log_file_name[MAX_FILE_NAME_LEN];
static char daily_record_file_name[MAX_FILE_NAME_LEN];
static char total_record_file_name[MAX_FILE_NAME_LEN];
static char error_file_name[MAX_FILE_NAME_LEN];

static single_use_time_t single_use_time;
static daily_use_time_t daily_use_time;
static u32 total_use_time;

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
    u32 ret = FR_OK, byte_read = 0;

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
    // Error file
    ret = f_open(&error_file, error_file_name, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
    if (ret != FR_OK)
        return ERROR;
    f_sync(&error_file);
    
    // Daily record file
    ret = f_open(&daily_record_file, daily_record_file_name, FA_READ | FA_WRITE);
    if (ret == FR_NO_FILE)
    {
        ret = f_open(&daily_record_file, daily_record_file_name, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
        memcpy(daily_use_time.date_info, cur_cal, sizeof(calendar_obj));
        daily_use_time.total_time = 0;
        daily_use_time.use_count = 0;
        update_daily_use_time();
    }
    else if (ret == FR_OK)
    {
        ret = f_read(&daily_record_file, &daily_use_time, sizeof(daily_use_time), &byte_read);
        if (ret != FR_OK)
            return ERROR;
        update_daily_use_time();
    } 
    else
        return ERROR;
    f_sync(&daily_record_file);

    //f_close(&daily_record_file);
    // Total record file
    ret = f_open(&total_record_file, total_record_file_name, FA_READ | FA_WRITE);
    if (ret == FR_NO_FILE)
    {
        ret = f_open(&total_record_file, total_record_file_name, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
        update_total_use_time();
    }
    else if (ret == FR_OK)
    {
        f_lseek(&total_record_file, 0);
        ret = f_read(&total_record_file, &total_use_time, sizeof(total_use_time_t), &byte_read);
        if (ret != FR_OK)
            return ERROR;
        update_total_use_time();
    }
    else
        rerturn ERROR;
    f_sync(&total_record_file);
    //f_close(&total_record_file);
    
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
    if (is_calendar_initialized() == FALSE)
        return ERROR;
    calendar_obj *cur_cal = get_current_calendar();
    // TODO: Check if this is okay for this.
    if (f_lseek(&log_file, log_file.fsize) != FR_OK)
        return ERROR;
    if (f_printf(&log_file, "%d-%d-%d-%d-%d-%d\n", cur_cal->year, cur_cal->month, cur_cal->date, cur_cal->hour, cur_cal->min, cur_cal->sec) != FR_OK)
        return ERROR;
    if (f_sync(&log_file) != FR_OK)
        return ERROR;
    // TODO: dump the system_status
    //f_close(&log_file);
    return SUCCESS;
}

ErrorStatus sync_daily_record()
{
    u32 byte_write;
    f_lseek(&daily_record_file, 0);
    f_write(&daily_record_file, &daily_use_time, sizeof(daily_use_time_t), &byte_write);
    if (daily_use_time.use_count > 0 && (current_status == OUTPUT_ENABLE))
    { 
        f_lseek(&daily_record_file, sizeof(daily_use_time_t) + (daily_use_time.use_count - 1) + sizeof(single_use_time_t));
        f_write(&daily_record_file, &single_use_time, sizeof(single_use_time_t), &byte_write);
    }
    f_sync(&daily_record_file);
}

ErrorStatus sync_total_record()
{
    u32 byte_write;
    f_lseek(&total_record_file, 0);
    f_write(&total_record_file, &total_use_time, sizeof(u32), &byte_write);
    f_sync(&total_record_file);
}

// This should be called less than output 
ErrorStatus update_record_log()
{
    calendar_obj *cur_cal = get_current_calendar();
    u32 cur_sec = time_to_seconds(cur_cal);
    daily_use_time.total_time += cur_sec - single_use_time.last_time;
    total_use_time += cur_sec - single_use_time.last_time;
    single_use_time.last_time = cur_sec;
    sync_daily_record();
    sync_total_record();
}

ErrorStatus update_record_status(output_status status)
{
    calendar_obj *cur_cal = get_current_calendar();
    switch(status)
    {
    case OUTPUT_ENABLE:
        if (current_status == OUTPUT_ENABLE)
            return ERROR;
        current_status = OUTPUT_ENABLE;
        daily_use_time.use_count++;
        single_use_time.start_tsime = time_to_seconds(cur_cal);
        single_use_time.last_time = single_use_time.start_time;
        single_use_time.end_time = 0;
        break;
    case OUTPUT_DISABLE:
        if (current_status == OUTPUT_DISABLE)
            return ERROR;
        current_status == OUTPUT_DISABLE;
        single_use_time.end_time = time_to_seconds(cal_cal);
        break;
    default:
        return ERROR;
    }
    update_record_log();
}

ErrorStatus print_debug_log(char *message)
{
    if (is_calendar_initialized() == FALSE)
        return ERROR;
    calendar_obj *cur_cal = get_current_calendar();
    if (f_lseek(&error_file, error_file.fsize) != FR_OK)
        return ERROR;
    if (f_printf(&error_file, "%d-%d-%d-%d-%d: %s\n", cur_cal->year, cur_cal->month, cur_cal->date, cur_cal->hour, cur_cal->min, message) != FR_OK)
        return ERROR;
    if (f_sync(&log_file) != FR_OK)
        return ERROR;
    return SUCCESS;
}

u32 time_to_seconds(calendar_obj *cal)
{
    return cur_cal->hour * 3600 + cur_cal->min * 60 + cur_cal->sec;
}