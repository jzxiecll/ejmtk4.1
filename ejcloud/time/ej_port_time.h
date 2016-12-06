#ifndef __EJ_PORT_TIME_H__
#define __EJ_PORT_TIME_H__


#include "ej_porting_layer.h"

//typedef unsigned int time_t;
typedef long suseconds_t;
struct ej_tm {
	/** seconds [0-59] */
	int tm_sec;
	/** minutes [0-59] */
	int tm_min;
	/** hours [0-23] */
	int tm_hour;
	/** day of the month [1-31] */
	int tm_mday;
	/** month [0-11] */
	int tm_mon;
	/** year. The number of years since 1900 */
	int tm_year;
	/** day of the week [0-6] 0-Sunday...6-Saturday */
	int tm_wday;

	int	tm_yday;
	int tm_isdst;
};





/** Convert HTTP date format to POSIX time format
 *
 * \param[in] date HTTP date format
 *
 * \return success or failure as:
 *     -WM_FAIL: Conversion failed. Invalid format/data
 *     else valid time_t value
 */
//time_t http_date_to_time(const unsigned char *date);

/** Set the date and time
 *
 * \param[in] tm The rtc value is updated with the values in tm structure
 * \return success or failure as:
 *     0: Success
 *     -1: Failed validation of tm structure
 */
//extern int EJ_time_set(const struct tm *tm);

/**
 * Get date and time
 *
 * \param[out] tm tm structure is updated to get the current value in rtc
 * \return success or failure as:
 *     0: Success
 *     non-zero: Internal error
 */
//extern int EJ_time_get(struct tm *tm);

/** Set the date and time using posix time
*
*  \param[in] time The rtc value is updated with the value present in time
*  \return success or failure as:
*      0: Success
*      non-zero: Internal error
*/
extern int EJ_time_set_posix(time_t time);

/**
 * Get date and time in posix format
 *
*  \return time_t value from RTC
 */
extern time_t EJ_time_get_posix(void);

/**
 * Convert to tm structure from POSIX/Unix time (Seconds since epoch)
 *
 * \param[in] time This is POSIX time that is to be converted into \ref tm
 * \param[out] result This should point to pre-allocated \ref tm instance
 * \return pointer to struct tm; NULL in case of error
 */
//struct tm *gmtime_r(const time_t *time, struct ej_tm *result);

/**
 * Converts to POSIX/Unix time from tm structure
 *
 * \param[in] tm This is \ref tm instance that is to be converted into
 * time_t format
 * \return time_t POSIX/Unix time equivalent
 */
//extern time_t mktime(struct ej_tm *tm);

/**
 * Converts the broken-down time value tm into a null-terminated string.
 *
 * \param[in] tm This is \ref tm instance that is to be converted string.
 *
 * @return Pointer to a statically allocated string which contains the date
 * and time format as follows "Tue Mar 24 09:20:14 2015". The statically
 * allocated string might be overwritten by subsequent calls to any of the
 * date and time functions.
 */
//char *asctime(const struct ej_tm *tm);

/**
 * Initialize time subsystem including RTC. Sets system time to 1/1/1970 00:00
 * (i.e. epoch 0)
 *
 * \return WM_SUCCESS on success, zero otherwise
 */
//extern int EJ_time_init(void);

/**
 * Register wmtime cli commands for the application use
 *
 * \note This function can be called by the application
 * after time subsystem is initialized.
 *
 * \return success or failure as:
 *     0: Success
 *     non-zero: Failure
 */
int EJ_time_init_posix(void);

#endif

