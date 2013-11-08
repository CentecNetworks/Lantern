#ifndef __KAL_ERROR_H__
#define __KAL_ERROR_H__

/**
 * @file kal_error.h
 */

/**
 * @defgroup errors Errors
 * @{
 */

/**
 * The <em>kal_err_t</em> type indicates specific error reason
 */
typedef int kal_err_t;

/**
 * Check if the value of <em>err</em> indicates success
 */
#define KAL_OK(err)     ((err) == 0)

/**
 * Check if the value of <em>err</em> indicates failure
 */
#define KAL_FAILED(err) ((err) != 0)

/**
 * 1 - (KAL_ERR_BASE - 1):      OS defined errors<br>
 * KAL_ERR_BASE - 0xFFFFFFFF:   KAL defined errors
 */
#define KAL_ERR_BASE        0xFFFF0000

#ifdef __cplusplus
extern "C"
{
#endif

const char *kal_err2str(kal_err_t err);

#ifdef __cplusplus
}
#endif

/**@}*/ /* End of @defgroup errors */

#endif /* !__KAL_ERROR_H__ */

