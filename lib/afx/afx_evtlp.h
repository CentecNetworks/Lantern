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

#ifndef __AFX_EVTLP_H__
#define __AFX_EVTLP_H__

/*
 * Event Loop
 */
int afx_event_loop_create(void);
void afx_event_loop_destroy(void);
int afx_event_loop_run(void);
void afx_event_loop_exit(void);

/*
 * IO Directions
 */
typedef enum
{
    AFX_IO_IN,
    AFX_IO_OUT,
    AFX_IO_ANY
} afx_io_dir_t;

/*
 * Multiplexed IO
 */
typedef struct afx_mio afx_mio_t;
typedef void (*afx_mio_cb_t)(int fd, afx_io_dir_t dir, void *arg);

int
afx_mio_create(afx_mio_t **pp_mio,
               int fd,
               afx_io_dir_t dir,
               afx_mio_cb_t cb,
               void *arg);

void afx_mio_destroy(afx_mio_t *p_mio);
int afx_mio_get_count(afx_mio_t *p_mio);
int afx_mio_setarg(afx_mio_t *p_mio, void *arg);

/*
 * Timers
 */
typedef struct afx_timer afx_timer_t;
typedef void (*afx_timer_cb_t)(void *arg);

int
afx_timer_create(afx_timer_t **pp_timer,
                 afx_timer_cb_t cb,
                 void *arg);

int afx_timer_destroy(afx_timer_t *p_timer);
int afx_timer_start(afx_timer_t *p_timer, uint32_t timeout);
int afx_timer_stop(afx_timer_t *p_timer);

/* Time API. */
int afx_get_cur_time(struct timeval *tv);

#endif /* !__AFX_EVTLP_H__ */
