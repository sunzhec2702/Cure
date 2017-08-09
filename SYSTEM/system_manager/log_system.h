#ifndef __LOG_SYSTEM_H__
#define __LOG_SYSTEM_H__
#include "sys.h"
#include "rtc.h"


/* We should update the use time by adding some delta time.
Once we power on the system, the enable flag is false until the button is pressed.
Then update the start time using RTC.
Check today's log file existance. If existed, open and update. two file, one for log, one for summary.

TODO: log more often. Summary to variable and update rarely.
*/

/* Compare current time and start time to update the daily_use_time and total_use_time */

extern void update_daily_use_time(void);
extern void update_total_use_time(void);

// Create the log file according to the current date.
extern void log_system_init(void);
extern ErrorStatus update_output_log(void);
extern void print_debug_log(char *message);

#endif
