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
 * @file
 * @brief This file is the header file of hal.c
 */

#ifndef __HAL_H__
#define __HAL_H__ 

/******************************************************************************
* Header Files 
******************************************************************************/

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/

#define HAL_ERROR_RETURN(op)                                            \
do {                                                                    \
    int rv;                                                             \
    if ((rv = (op)) < 0)                                                \
    {                                                                   \
        return(rv);                                                     \
    }                                                                   \
}while(0)

#define HAL_PTR_CHECK(ptr)                                              \
do {                                                                    \
    if (NULL == (ptr))                                                  \
    {                                                                   \
        return (OFP_ERR_INVALID_PTR);                                   \
    }                                                                   \
}while(0)

/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/

/**
 * init some feature in sdk
 * @return OFP_ERR_XXX
 */
int32_ofp
hal_misc_init(void);

#endif
