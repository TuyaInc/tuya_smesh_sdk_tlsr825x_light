/*************************************************************************
	> File Name: hal_log.h
	> Author: 
	> Mail: 
	> Created Time: Wed 03 Jul 2019 22:05:09 CST
 ************************************************************************/

#ifndef _HAL_LOG_H
#define _HAL_LOG_H

#include "base_oem_config.h"

#define TELINK_LOG_EN    1    //enable telink log print

typedef int ty_log_level;
#define TY_LOG_LEVEL_ERR       0  // 错误信息，程序正常运行不应发生的信息 
#define TY_LOG_LEVEL_WARN      1  // 警告信息
#define TY_LOG_LEVEL_NOTICE    2  // 需要注意的信息
#define TY_LOG_LEVEL_INFO      3  // 通知信息
#define TY_LOG_LEVEL_DEBUG     4  // 程序运行调试信息，RELEASE版本中删除
#define TY_LOG_LEVEL_TRACE     5  // 程序运行路径信息，RELEASE版本中删除


#define TY_DEAULT_LOG_LEVEL    TY_LOG_LEVEL_DEBUG

#define TY_PRINT_LOG(level,fmt, ...) \
do \
{ \
    app_printf_rn(fmt, ##__VA_ARGS__); \
}while(0)

#if (CONFIG_LOG_ON && (TY_DEAULT_LOG_LEVEL >= TY_LOG_LEVEL_ERR))
#define TY_LOG_ERR(fmt, ...)    TY_PRINT_LOG(TY_LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)
#else
#define TY_LOG_ERR(fmt, ...)
#endif

#if (CONFIG_LOG_ON && (TY_DEAULT_LOG_LEVEL >= TY_LOG_LEVEL_WARN))
#define TY_LOG_WARN(fmt, ...)    TY_PRINT_LOG(TY_LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
#else
#define TY_LOG_WARN(fmt, ...)
#endif

#if (CONFIG_LOG_ON && (TY_DEAULT_LOG_LEVEL >= TY_LOG_LEVEL_NOTICE))
#define TY_LOG_NOTICE(fmt, ...)    TY_PRINT_LOG(TY_LOG_LEVEL_NOTICE, fmt, ##__VA_ARGS__)
#else
#define TY_LOG_NOTICE(fmt, ...)
#endif

#if (CONFIG_LOG_ON && (TY_DEAULT_LOG_LEVEL >= TY_LOG_LEVEL_INFO))
#define TY_LOG_INFO(fmt, ...)    TY_PRINT_LOG(TY_LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#else
#define TY_LOG_INFO(fmt, ...)
#endif

#if (CONFIG_LOG_ON && (TY_DEAULT_LOG_LEVEL >= TY_LOG_LEVEL_DEBUG))
#define TY_LOG_DEBUG(fmt, ...)    TY_PRINT_LOG(TY_LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#else
#define TY_LOG_DEBUG(fmt, ...)
#endif

#if (CONFIG_LOG_ON && (TY_DEAULT_LOG_LEVEL >= TY_LOG_LEVEL_TRACE))
#define TY_LOG_TRACE(fmt, ...)    TY_PRINT_LOG(TY_LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)
#else
#define TY_LOG_TRACE(fmt, ...)
#endif


//OLD DEBUG LOG----------------------------------------------------

#if CONFIG_LOG_ON
#define APP_LOG(format,...)  app_printf(format, ##__VA_ARGS__)
#define APP_LOG_DUMP(title, buffer, len)  app_log_dump(title, buffer, len)
#else
#define APP_LOG(format,...)
#define APP_LOG_DUMP(title, buffer, len)
#endif


#if 1 // old components use
#define PR_DEBUG(...)
#define PR_DEBUG_RAW(_fmt_, ...)
#define PR_DEBUG_HEX_ARRAY(data,len)
#endif

void app_log_dump(unsigned char *title, unsigned char *buffer, unsigned short int len);



#endif
