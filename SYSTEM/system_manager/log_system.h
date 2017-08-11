#ifndef __LOG_SYSTEM_H__
#define __LOG_SYSTEM_H__
#include "sys.h"
#include "rtc.h"

#define ERROR_FILE "error.log" // error.log. For end user analysis.
#define LOG_FOLDER "0:/logs" // 0 is sd card.

#define MAX_FILE_NAME_LEN 30
#define MAX_LOG_MESSAGE_LEN 30

/* We should update the use time by adding some delta time.
Once we power on the system, the enable flag is false until the button is pressed.
Then update the start time using RTC.
Check today's log file existance. If existed, open and update. two file, one for log, one for summary.
TODO: log more often. Summary to variable and update rarely.
*/

typedef struct
{
    u32 start_time;
    u32 end_time;
} single_use_time_t; 

typedef struct
{
    u32 total_time;
    calendar_obj date_info;
    u32 use_count;
    single_use_time_t single_use[0];
} daily_use_time_t;

typedef struct
{
    u32 hour;
    u32 min;
    u32 sec;
} total_use_time_t;

/* Compare current time and start time to update the daily_use_time and total_use_time */

extern void update_daily_use_time(void);
extern void update_total_use_time(void);

// Create the log file according to the current date.
extern ErrorStatus log_system_init(void);
extern void close_all_files(void);
extern ErrorStatus update_output_log(void);
extern void print_debug_log(char *message);

#endif
