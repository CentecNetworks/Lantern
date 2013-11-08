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
 * @brief This file is the database implementation of ctc_api
 */

#ifndef __ADPT_OPF_H__
#define __ADPT_OPF_H__

/******************************************************************************
* Header Files 
******************************************************************************/
#include "sys_humber_opf.h"

/****************************************************************************
 *  
 * Defines and Macros
 *
 ****************************************************************************/
#define ADPT_OPF_ERROR_RETURN(op)\
do { \
    int rv; \
    if ((rv = (op)) < 0) \
    {\
        return(rv); \
    }\
}while(0)

/****************************************************************************
 *
 * Global and Declaration
 *
 ****************************************************************************/

/****************************************************************************
 *  
 * Function
 *
 ****************************************************************************/
/**
 * Allocate opf offset
 * @param[in]  type                     Type
 * @param[in]  num                      opf number
 * @param[out] p_offset                 Pointer to offset base
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_opf_alloc_offset(uint8_ofp type, uint8_ofp num, uint32_ofp* p_offset);

/**
 * Free opf offset
 * @param[in]  type                     Type
 * @param[in]  num                      opf number
 * @param[in]  offset                   offset base
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_opf_free_offset(uint8_ofp type, uint8_ofp num, uint32_ofp offset);

/**
 * Init opf
 * @return OFP_ERR_XXX
 */
int32_ofp
adpt_opf_init(void);

#endif
