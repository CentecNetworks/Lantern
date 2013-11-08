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

#ifndef __SAL_ASSERT_H__
#define __SAL_ASSERT_H__

/**
 * @file sal_assert.h
 */

/**
 * @defgroup assert Assertion Macros
 * @{
 */

/**
 * Abort the program if assertion failed
 *
 * @param[in] e
 */
#ifdef _SAL_DEBUG
#define SAL_ASSERT(e) if (!(e)) sal_assert_failed(#e, __FILE__, __LINE__)
#else
#define SAL_ASSERT(e) (void)0
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _SAL_DEBUG
void sal_assert_failed(const char *expr, const char *file, int line);
#endif

#ifdef __cplusplus
}
#endif

/**@}*/ /* End of @defgroup assert */

#endif /* !__SAL_ASSERT_H__ */
