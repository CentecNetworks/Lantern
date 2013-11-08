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

#ifndef __AFX_H__
#define __AFX_H__

/*
 * Application Framework Library
 */
#include <sys/types.h>
#include <stdio.h>
#ifndef _SSHD_
#include <sys/poll.h>
#endif
#include <sys/time.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <linux/stddef.h>
#include <stdbool.h>

/* XXX: for CLOCK_MONOTONIC. */
#include <string.h> /* memset. */
#include <time.h>

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define afx_container_of(p, type, member) \
({const typeof(((type *)0)->member) *_p = p; (type *)((char *)_p - offsetof(type, member));})

#include "afx_list.h"
#include "afx_evtlp.h"

#endif /* !__AFX_H__ */
