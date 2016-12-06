#ifndef __EJ_LOG_H__
#define __EJ_LOG_H__

#include "ej_porting_layer.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define EJ_AlwaysPrintf(msg) \
    do { \
        EJ_Printf msg; \
    } while (0)

#ifndef NDEBUG
	

#define AJ_DEBUG_OFF   0  /**< Suppresses all debug output */
#define EJ_DEBUG_ERROR 1  /**< Indicates a log message conveying an error condition */
#define EJ_DEBUG_WARN  2  /**< Indicates a log message corresponding to a warning */
#define EJ_DEBUG_DEBUG 3
#define EJ_DEBUG_INFO  4  /**< Indicates a log message with general information */
#define EJ_DEBUG_DUMP  5  /**< Indicates a message with a detailed, possibly byte-by-byte dump */
#define EJ_DEBUG_ALL   6  /**< A placeholder level above other levels */
	
	/**
	 * Type definition for a value used to control the debug level (verbosity)
	 * threshold.
	 */
typedef uint32_t EJ_DebugLevel;
	

#ifndef EJ_DEBUG_RESTRICT
#define EJ_DEBUG_RESTRICT EJ_DEBUG_INFO
#endif
	
	/**
	 * Set this value to control the debug output threshold level. The default is EJ_DEBUG_ERROR
	 */
	extern EJ_DebugLevel EJ_DbgLevel;
	extern uint8_t dbgALL;
	
	extern int _EJ_DbgEnabled(const char* module);
	
	/**
	 * Internal debug printf function. Don't call this directly, use the AJ_*Printf() macros.
	 *
	 * @param level The level associated with this debug print
	 * @param file	File name for file calling this function
	 * @param line	Line number for line this function was called from
	 */
	int _EJ_DbgHeader(EJ_DebugLevel level, const char* file, int line);
	
#define QUOTE(x) # x
#define STR(x) QUOTE(x)
	
#define CONCAT(x, y) x ## y
#define MKVAR(x, y) CONCAT(x, y)
	
	
#if EJ_DEBUG_RESTRICT >= EJ_DEBUG_ERROR
	/**
	 * Print an error message.	Error messages may be suppressed by EJ_DEBUG_RESTRICT
	 *
	 * @param msg  A format string and arguments
	 */
#define EJ_ErrPrintf(msg) \
		do { \
			if (_EJ_DbgHeader(EJ_DEBUG_ERROR, __FILE__, __LINE__)) { EJ_Printf msg; } \
		} while (0)
#else
#define EJ_ErrPrintf(_msg)
#endif
	
#if EJ_DEBUG_RESTRICT >= EJ_DEBUG_WARN
	/**
	 * Print a warning message. Warnings may be suppressed by EJ_DEBUG_RESTRICT
	 *
	 * @param msg  A format string and arguments
	 */
#define EJ_WarnPrintf(msg) \
		do { \
			if (_EJ_DbgHeader(EJ_DEBUG_WARN, __FILE__, __LINE__)) { EJ_Printf msg; } \
		} while (0)
#else
#define EJ_WarnPrintf(_msg)
#endif
	
#if EJ_DEBUG_RESTRICT >= EJ_DEBUG_DEBUG
	/**
	 * Print a debug message. Warnings may be suppressed by EJ_DEBUG_RESTRICT
	 *
	 * @param msg  A format string and arguments
	 */
#define EJ_DebugPrintf(msg) \
		do { \
			if (_EJ_DbgHeader(EJ_DEBUG_DEBUG, __FILE__, __LINE__)) { EJ_Printf msg; } \
		} while (0)
#else
#define EJ_DebugPrintf(_msg)
#endif
	
#if EJ_DEBUG_RESTRICT >= EJ_DEBUG_INFO
	/**
	 * Print an informational message.	Informational messages may be suppressed by
	 * EJ_DEBUG_RESTRICT or by the module selection (global memory value or shell
	 * environment variable) mechanism.
	 *
	 * @param msg  A format string and arguments
	 */
#define EJ_InfoPrintf(msg) \
		do { \
				if (_EJ_DbgHeader(EJ_DEBUG_INFO, __FILE__, __LINE__)) { EJ_Printf msg; } \
		} while (0)
#else
#define EJ_InfoPrintf(_msg)
#endif
	
#if EJ_DEBUG_RESTRICT >= EJ_DEBUG_DUMP
	/**
	 * Dump the bytes in a buffer in a human readable way.	Byte dumps messages may
	 * be suppressed by EJ_DEBUG_RESTRICT or by the module selection (global memory
	 * value or shell environment variable) mechanism.
	 *
	 * @param msg A format string
	 * and arguments
	 */
#define EJ_DumpBytes(tag, data, len) \
		do { \
			if (MKVAR(dbg, AJ_MODULE) || _EJ_DbgEnabled(STR(AJ_MODULE))) { _AJ_DumpBytes(tag, data, len); } \
		} while (0)
#else
#define EJ_DumpBytes(tag, data, len)
#endif
	
#if EJ_DEBUG_RESTRICT >= EJ_DEBUG_DUMP
	/**
	 * Print a human readable summary of a message.  Message dumps messages may be
	 * suppressed by EJ_DEBUG_RESTRICT or by the module selection (global memory
	 * value or shell environment variable) mechanism.
	 *
	 * @param msg  A format string and arguments
	 */
#define EJ_DumpMsg(tag, msg, body) \
		do { \
			if (MKVAR(dbg, AJ_MODULE) || _EJ_DbgEnabled(STR(AJ_MODULE))) { _AJ_DumpMsg(tag, msg, body); } \
		} while (0)
#else
#define EJ_DumpMsg(tag, msg, body)
#endif
	
#else
	
#define EJ_DumpMsg(tag, msg, body)
#define EJ_DumpBytes(tag, data, len)
#define EJ_ErrPrintf(_msg)
#define EJ_WarnPrintf(_msg)
#define EJ_InfoPrintf(_msg)
#define AJ_AlwaysHdrPrintf EJ_AlwaysPrintf
	
#endif


#if 0
#define EJ_LOG_LEVEL_FATAL      (0)
#define EJ_LOG_LEVEL_NOTICE     (1)
#define EJ_LOG_LEVEL_INFO       (2)
#define EJ_LOG_LEVEL_ERROR      (3)
#define EJ_LOG_LEVEL_WARN       (4)
#define EJ_LOG_LEVEL_DEBUG      (5)

#define Black   0;30
#define Red     0;31
#define Green   0;32
#define Brown   0;33
#define Blue    0;34
#define Purple  0;35
#define Cyan    0;36




#define EJ_LOG_LEVEL  EJ_LOG_LEVEL_DEBUG

#define EJ_FatalPrintf(format, ...) \
    do{\
        if(EJ_LOG_LEVEL >= EJ_LOG_LEVEL_FATAL){\
            wmprintf("\033[0;31m[FATAL][%s][%s][%d]\r\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
	        wmstdio_flush(); \
	        wmprintf("\033[0m"); \
        }\
    }while(0)

#define EJ_NoticePrintf(format, ...) \
    do{\
        if(EJ_LOG_LEVEL >= EJ_LOG_LEVEL_NOTICE){\
            wmprintf("\033[0;36m[NOTICE][%s][%s][%d]\r\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
			wmstdio_flush(); \
			wmprintf("\033[0m"); \
        }\
    }while(0)

#define EJ_InfoPrintf(format, ...) \
    do{\
        if(EJ_LOG_LEVEL >= EJ_LOG_LEVEL_INFO){\
            wmprintf("\033[1;36m[INFO][%s][%s][%d]\r\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
			wmstdio_flush(); \
			wmprintf("\033[0m"); \
        }\
    }while(0)

#define EJ_ErrPrintf(format, ...) \
    do{\
        if(EJ_LOG_LEVEL >= EJ_LOG_LEVEL_ERROR){\
            wmprintf("\033[0;31m[ERROR][%s][%s][%d]\r\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
			wmstdio_flush(); \
			wmprintf("\033[0m"); \
        }\
    }while(0)

#define EJ_WarnPrintf(format, ...) \
    do{\
        if(EJ_LOG_LEVEL >= EJ_LOG_LEVEL_WARN){\
            wmprintf("\033[1;33m[WARN][%s][%s][%d]\r\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
			wmstdio_flush(); \
		    wmprintf("\033[0m"); \
        }\
    }while(0)
#ifdef LOG_DEBUG
#define EJ_DebugPrintf(format, ...) \
    do{\
        if(EJ_LOG_LEVEL >= EJ_LOG_LEVEL_DEBUG){\
            wmprintf("\033[1;32m[DEBUG][%s][%s][%d]\r\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
			wmstdio_flush(); \
		    wmprintf("\033[0m"); \
        }\
    }while(0)
#else
#define EJ_DebugPrintf(format, ...) \
		{\
            ;\
		}

#endif
#endif 
#ifdef __cplusplus
}
#endif

#endif /* __LOGGING_H__ */







