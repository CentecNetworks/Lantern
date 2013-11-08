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

#ifndef _CTCKAL_USRCTRL_H_
#define _CTCKAL_USRCTRL_H_

/*
 * exported functions
 */
int32_t ctckal_usrctrl_init(void);

/*
 * Bay
 */
extern int32_t dal_usrctrl_write_bay(uint32_t chip_id, uint32_t reg_offset, uint32_t value);
extern int32_t dal_usrctrl_read_bay(uint32_t chip_id, uint32_t reg_offset, uint32_t p_value);

#endif  /* _CTCKAL_USRCTRL_H_ */
