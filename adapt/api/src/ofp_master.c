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
 * @brief This file contains the main function that will initialize the sdk, configure sdk and start ovs program
 */

#include "stdio.h"

#include "sal.h"
#include "sal_mutex.h"

#include "ofp_api.h"
#include "ofp_master.h"
#include "netdev-ctc.h"

#ifndef _OPEN_SOURCE_
extern sal_mutex_t *pg_mutex;
pthread_t ctc_master_cli_thread;

extern int
ctc_master_cli(void);
pthread_t ctc_master_cli_thread;
#else
pthread_t ctc_master_sdk_thread;
sal_mutex_t *pg_mutex;
#endif

extern int
ovs_main(int argc, char *argv[]);
extern int
pthread_create(pthread_t *,int *,void * t3,void * t4);

VLOG_DEFINE_THIS_MODULE(ofp_master);

#ifdef _OPEN_SOURCE_
void*
ctc_master_sdk_loop(void* p_unused)
{
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGALRM);
    pthread_sigmask(SIG_SETMASK, &sigset, NULL);

    afx_event_loop_create();
    afx_event_loop_run();
    afx_event_loop_destroy();
}
#endif

static int
create_init_done_file(char * file_path)
{
    FILE * fp = fopen(file_path, "a");
    if (fp != NULL)
    {
        fclose(fp);
    }

    return 0;
}

int
main(int argc, char *argv[])
{
#ifndef _OPEN_SOURCE_
    /* init lcm_main, drivers, and install CLIs */
    OFP_ERROR_RETURN(lcm_master());

    sal_mutex_create(&pg_mutex);

    /* start cli thread */
    pthread_create(&ctc_master_cli_thread, NULL, ctc_master_cli, NULL);
#else
    /* init lcm_main, drivers, and install CLIs */
    OFP_ERROR_RETURN(lcm_master());

    /* start sdk thread */
    pthread_create(&ctc_master_sdk_thread, NULL, ctc_master_sdk_loop, NULL);

    sal_mutex_create(&pg_mutex);
#endif

    /* register netdev/ofproto centec implementation */
    {
        extern void register_ofproto_live_class(void);
        extern void register_netdev_live_class(void);
        extern void register_netdev_live_vport_class(void);
        register_ofproto_live_class();
        register_netdev_live_class();
        register_netdev_live_vport_class();
    }

    create_init_done_file(OFP_INIT_DONE_FILE);
    
    /* start ovs */
    ovs_main(argc, argv);
    
    /* OFP_ERROR_RETURN(ofp_adapter_uninstall()); */

    return 1;
}
