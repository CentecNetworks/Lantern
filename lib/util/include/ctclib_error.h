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
#ifndef __CTCLIB_ERROR_H__
#define __CTCLIB_ERROR_H__

#include "glb_const.h"

enum ctclib_err
{
    CTCLIB_E_NONE = 0,
    CTCLIB_E_NO_MEMORY = GLB_CTCLIB_ERROR_BASE,
    CTCLIB_E_INVALID_PARAM,
    CTCLIB_E_INVALID_PTR,
    CTCLIB_E_CREATE_MEM_CACHE_FAIL,
    CTCLIB_E_FAIL_CREATE_MUTEX,

    CTCLIB_E_MAX
};

#endif /*__CTCLIB_ERROR_H__*/
