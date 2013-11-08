#ifndef __KAL_LOG_H__
#define __KAL_LOG_H__

/**
 * @file kal_log.h
 */

/**
 * @defgroup log Message Logging
 * @{
 */

/** Log levels */
typedef enum kal_loglevel
{
	KAL_LL_FATAL,    /**< Fatal log level */
	KAL_LL_ERROR,    /**< Error log level */
	KAL_LL_WARN,     /**< Warning log level */
	KAL_LL_INFO,      /**< Information log level */
	KAL_LL_DEBUG,   /**< Debug log level */
	KAL_LL_OFF        /**< Off log level */
}kal_loglevel_t;

extern kal_loglevel_t debug_level_flag;

/**
 * @brief Log fatal error message
 *
 * The macro prototype is the same as printf()'s.
 * The output has "FATAL <file>(<line>): " prefix and auto-newline.
 */
#define KAL_LOG_FATAL(fmt, args...) \
    kal_log(KAL_LL_FATAL, __FILE__, __LINE__, fmt, ##args)

/**
 * @brief Log error message
 *
 * The macro prototype is the same as printf()'s.
 * The output has "ERROR <file>(<line>): " prefix and auto-newline.
 */
#define KAL_LOG_ERROR(fmt, args...) \
    kal_log(KAL_LL_ERROR, __FILE__, __LINE__, fmt, ##args)

/**
 * @brief Log warning message
 *
 * The macro prototype is the same as printf()'s.
 * The output has "WARN  <file>(<line>): " prefix and auto-newline.
 */
#define KAL_LOG_WARN(fmt, args...) \
    kal_log(KAL_LL_WARN, __FILE__, __LINE__, fmt, ##args)

/**
 * @brief Log informational message
 *
 * The macro prototype is the same as printf()'s.
 * The output has no prefix and auto-newline.
 */
#define KAL_LOG_INFO(fmt, args...) \
    kal_log(KAL_LL_INFO, __FILE__, __LINE__, fmt, ##args)

/**
 * @brief Log debug message
 *
 * The macro prototype is the same as printf()'s.
 * The output has no prefix and auto-newline.
 */
#ifdef _KAL_DEBUG
#define KAL_LOG_DEBUG(fmt, args...) \
    kal_log(KAL_LL_DEBUG, __FILE__, __LINE__, fmt, ##args)
#else
#define KAL_LOG_DEBUG(fmt, args...)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define _KAL_LOG_ATTR __attribute__((format(printf, 4, 5)))

/**
 * @brief Internal logging function
 *
 * @param[in] log_level
 * @param[in] file
 * @param[in] line
 * @param[in] fmt
 * @param[in] ...
 *
 * @return
 */
int kal_log(int log_level,
            const char *file,
            int32 line,
            const char *fmt,
            ...);

int kal_print_func(const char *fmt,  ...);

#ifdef __cplusplus
}
#endif

/**@}*/ /* End of @defgroup log */

#endif /* !__KAL_LOG_H__ */

