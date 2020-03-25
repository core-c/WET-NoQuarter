#include "g_time.h"
#include "g_local.h"

time_t G_ConvertQtime(qtime_t* qt)
{
	// Use the stdc mktime and struct tm to convert qtime_t
	struct tm							tm;
	// Initialise our tm structure
	tm.tm_sec		= qt->tm_sec;
	tm.tm_min		= qt->tm_min;
	tm.tm_hour		= qt->tm_hour;
	tm.tm_mday		= qt->tm_mday;
	tm.tm_mon		= qt->tm_mon;
	tm.tm_year		= qt->tm_year;
	tm.tm_wday		= qt->tm_wday;
	tm.tm_yday		= qt->tm_yday;
	tm.tm_isdst		= qt->tm_isdst;
	// Perform the conversion and return
	return mktime(&tm);
}

time_t G_GetCurrentTime()
{
	qtime_t 							ct;
	// Get the current time
	trap_RealTime(&ct);
	// Convert it to time_t and return
	return G_ConvertQtime(&ct);
}