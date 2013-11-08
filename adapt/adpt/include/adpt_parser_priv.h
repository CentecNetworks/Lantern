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
 * @brief This file is the private file for adpt_parser.c
 */

#ifndef __ADPT_PARSER_PRIV_H__
#define __ADPT_PARSER_PRIV_H__

/******************************************************************************
* Header Files 
******************************************************************************/

/*******************************************************************
 *
 * Structures and macros, enums
 *
 ********************************************************************/

struct adpt_parser_master_s
{
    uint16_ofp svlan_tpid;
    uint16_ofp cvlan_tpid;

    uint32_ofp linkagg_hash_use;    /* Bitmap of adpt_parser adpt_parser_hash_use_t */
    uint32_ofp linkagg_hash_keys;   /* Bitmap of adpt_parser adpt_parser_hash_key_t */
    
};
typedef struct adpt_parser_master_s adpt_parser_master_t;

#endif
