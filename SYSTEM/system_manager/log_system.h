#ifndef __LOG_SYSTEM_H__
#define __LOG_SYSTEM_H__
#include "sys.h"
#include "rtc.h"

#define ERROR_FILE "error.log" // error.log. For end user analysis.
#define LOG_FOLDER "0:/logs" // 0 is sd card.

#define MAX_FILE_NAME_LEN 30
#define MAX_LOG_MESSAGE_LEN 30

typedef enum
{
    OUTPUT_ENABLE = 0,
    OUTPUT_DISABLE,
} output_status;

/* We should update the use time by adding some delta time.
Once we power on the system, the enable flag is false until the button is pressed.
Then update the start time using RTC.
Check today's log file existance. If existed, open and update. two file, one for log, one for summary.
TODO: log more often. Summary to variable and update rarely.
*/

typedef struct
{
    u32 start_time;
    u32 last_time;
    u32 end_time;
} single_use_time_t; 

typedef struct
{
    u32 total_time; //total time used today.
    calendar_obj date_info; // record today's info.
    u32 use_count; // How many time has been used. stop/start is counted as one time.
    //single_use_time_t single_use[0]; // to record the start/end time for a single use.
} daily_use_time_t;

typedef struct
{
    u8 hour;
    u8 min;
    u8 sec;
} hour_min_sec;

/* Compare current time and start time to update the daily_use_time and total_use_time */



// Create the log file according to the current date.
extern ErrorStatus log_system_init(void);
extern void close_all_files(void);
extern ErrorStatus record_update_output_log(void);
extern ErrorStatus record_update_output_status(output_status status);
extern ErrorStatus print_debug_log(char *message);
#endif
