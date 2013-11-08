/**
 *  Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __SAL_ERROR_H__
#define __SAL_ERROR_H__

/**
 * @file sal_error.h
 */

/**
 * @defgroup errors Errors
 * @{
 */

/**
 * The <em>sal_err_t</em> type indicates specific error reason
 */
typedef int sal_err_t;

/**
 * Check if the value of <em>err</em> indicates success
 */
#define SAL_OK(err)     ((err) == 0)

/**
 * Check if the value of <em>err</em> indicates failure
 */
#define SAL_FAILED(err) ((err) != 0)

/**
 * 1 - (SAL_ERR_BASE - 1):      OS defined errors<br>
 * SAL_ERR_BASE - 0xFFFFFFFF:   SAL defined errors
 */
#define SAL_ERR_BASE        0xFFFF0000

#ifdef __cplusplus
extern "C"
{
#endif

const char *sal_err2str(sal_err_t err);

#ifdef __cplusplus
}
#endif

/**@}*/ /* End of @defgroup errors */

#endif /* !__SAL_ERROR_H__ */
