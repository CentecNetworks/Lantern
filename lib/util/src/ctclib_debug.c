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

#include <dirent.h>
#include "sal.h"
#include "ctclib_error.h"
#include "ctclib_debug.h"

ctclib_debug_control_t g_ctclib_debug_switches;

glb_err_mapper_t g_ast_err_mapper[] =
{
    {GLB_E_NONE, "No error"},
    {GLB_E_NO_MEMORY, "Error: no memory"},
    {GLB_E_RESOURCE_FULL, "Error: resource full"},

    {GLB_E_INVALID_PARAM, "Error: invalid param"},
    {GLB_E_INVALID_PTR, "Error: invalid pointer"},

    {GLB_E_SOCK_CREATE_ERR, "Error: socket create failed"},
    {GLB_E_SOCK_BIND_ERR,"Error: socket bind failed"},
    {GLB_E_SOCK_ACCEPT_ERR, "Error: socket accept failed"},
    {GLB_E_SOCK_READ_ERR, "Error: socket read failed"},
    {GLB_E_SOCK_WRITE_ERR, "Error: socket write failed"},

    {GLB_E_MSG_LEN_TOO_SHORT, "Error: message too short"},
    {GLB_E_MSG_LEN_TOO_LONG, "Error: message too long"},
    {GLB_E_MSG_TYPE_EER, "Error: wrong message type"},

    {GLB_E_NOT_INIT, "Error: module not initialized"},

    {GLB_E_ENTRY_NOT_EXIST, "Error: entry not exist"},
    {GLB_E_ENTRY_EXISTED, "Error: entry not exist"},
    {GLB_E_ENTRY_CONFLICT, "Error: entry conflict"},
    {GLB_E_ENTRY_EXCEED_MAX, "Error: entry exceed max"},

    {GLB_E_MAX, "Error Unknown"}
};

/*********************************************************************
 * Name    : ctclib_debug_init
 * Purpose : This function will init the debug function's global control variable
 * Input   : uint32 is_master - 1 for main board, 0 for linecard
 * Output  : N/A
 * Return  : CTCLIB_E_NONE
 * Note    : 2010.7 init version
*********************************************************************/
int32
ctclib_debug_init(uint32 is_master)
{
    sal_memset(&g_ctclib_debug_switches, 0, sizeof(ctclib_debug_control_t));

    #ifdef _GLB_DISTRIBUTE_SYSTEM_
    g_ctclib_debug_switches.lc_on = is_master;
    #else
    g_ctclib_debug_switches.lc_on = 1;
    #endif

    return CTCLIB_E_NONE;
}

/*********************************************************************
 * Name    : ctclib_debug_line_function_on
 * Purpose : This function will turn on or turn off the linenum & function print out
 * Input   : int8 on_off
 * Output  : N/A
 * Return  : CTCLIB_E_NONE
 * Note    : 2010.7 init version
*********************************************************************/
int32
ctclib_debug_line_function_on(int8 on_off)
{
    g_ctclib_debug_switches.line_fun_on = on_off;
    return CTCLIB_E_NONE;
}

/*********************************************************************
 * Name    : ctclib_debug_linecard_on
 * Purpose : This function will turn on or turn off the linecard debug
 * Input   : int8 on_off
 * Output  : N/A
 * Return  : CTCLIB_E_NONE
 * Note    : 2010.7 init version
*********************************************************************/
int32
ctclib_debug_linecard_on(int8 on_off)
{
    g_ctclib_debug_switches.lc_on = on_off;
    return CTCLIB_E_NONE;
}

/*********************************************************************
 * Name    : ctclib_debug_clisetting_common
 * Purpose : This function will set the debug handle in local process
 * Input   : ctclib_list_t* p_list - list for store debug handles,
             uint32 flag - debug flag, it is 32 bit map
             uint8* module - module name
             int8* submodule - sub-module name
 * Output  : N/A
 * Return  : CTCLIB_E_NONE
 * Note    : 2010.7 init version
*********************************************************************/
int32
ctclib_debug_clisetting_common(ctclib_list_t* p_list, uint32 flag, char* sz_module, char* sz_submodule)
{
    ctclib_list_node_t* p_node = NULL;
    ctclib_debug_t* p_debug_hdl;

    ctclib_list_for_each(p_node, p_list)
    {
        p_debug_hdl = ctclib_container_of(p_node, ctclib_debug_t, list_head);

        if (!sal_strcmp(p_debug_hdl->module, sz_module)
            && !sal_strcmp(p_debug_hdl->submodule, sz_submodule))
        {
            p_debug_hdl->flags = flag;
            break;
        }
    }

    return CTCLIB_E_NONE;
}

/*********************************************************************
 * Name    : ctclib_debug_climodon_common
 * Purpose : This function will set the debug handle in local process
             It will turn on the whole module
 * Input   : ctclib_list_t* p_list - list for store debug handles,
             uint8* module - module name
 * Output  : N/A
 * Return  : CTCLIB_E_NONE
 * Note    : 2010.7 init version
*********************************************************************/
int32
ctclib_debug_climodon_common(ctclib_list_t* p_list, char* sz_module)
{
    ctclib_list_node_t* p_node = NULL;
    ctclib_debug_t* p_debug_hdl;

    ctclib_list_for_each(p_node, p_list)
    {
        p_debug_hdl = ctclib_container_of(p_node, ctclib_debug_t, list_head);

        if (!sal_strcmp(p_debug_hdl->module, sz_module))
        {
            p_debug_hdl->flags = DEBUG_ALL_TYPE;
            continue;
        }
    }

    return CTCLIB_E_NONE;
}

/*********************************************************************
 * Name    : ctclib_debug_climodoff_common
 * Purpose : This function will set the debug handle in local process
             It will turn off the whole module
 * Input   : ctclib_list_t* p_list - list for store debug handles,
             uint8* module - module name
 * Output  : N/A
 * Return  : CTCLIB_E_NONE
 * Note    : 2010.7 init version
*********************************************************************/
int32
ctclib_debug_climodoff_common(ctclib_list_t* p_list, char* sz_module)
{
    ctclib_list_node_t* p_node = NULL;
    ctclib_debug_t* p_debug_hdl;

    ctclib_list_for_each(p_node, p_list)
    {
        p_debug_hdl = ctclib_container_of(p_node, ctclib_debug_t, list_head);

        if (!sal_strcmp(p_debug_hdl->module, sz_module))
        {
            p_debug_hdl->flags = DEBUG_NO_TYPE;
            continue;
        }
    }

    return CTCLIB_E_NONE;
}

#define CTCLIB_MAX_NAME_LEN 256
void
ctclib_print_msg_on_tty(char *printout)
{
    static DIR *dir;
    struct dirent *entry;
    char fullname[CTCLIB_MAX_NAME_LEN];
    int fd;

    if (printout == NULL)
        return;

    /*print on console*/
    sal_sprintf(fullname, "/dev/console");
    fd = sal_open(fullname, O_WRONLY);
    if (fd != -1)
    {
        sal_write(fd, printout, sal_strlen(printout));
        sal_close(fd);
    }

    dir = opendir("/dev/pts");
    do
    {
        if((entry = readdir(dir)) == NULL)
        {
            closedir(dir);
            return;
        }

        if (!sal_strcmp(entry->d_name, ".")
                || !sal_strcmp(entry->d_name, ".."))
        {
            continue;
        }

        sal_sprintf(fullname, "/dev/pts/%s", entry->d_name);
        fd = sal_open(fullname, O_WRONLY);
        if (fd == -1)
        {
            continue;
        }
        sal_write(fd, printout, sal_strlen(printout));
        sal_close(fd);
    }
    while(1);
}

/*******************************************************************************
 * Name:    ctclib_debug_print_hex_emacs
 * Purpose: dump the buffer in `emacs-hex1` style
 * Input:
 *   pHdl: debugging handle
 *   cp:   buffer
 *   length: buffer length
 *   offset: show offset
 * Output:
 * Return: N/A
 * Note:
 ******************************************************************************/
 void
 ctclib_debug_print_hex_emacs(ctclib_debug_cli_t *pHdl, unsigned i, const u_char *cp, unsigned int length, unsigned int offset, char *ln)
 {
     unsigned j, jm;
     char *p = NULL;
     int nLeft = 0;
     int nRet = 0;
     char buf[2];
     int c;

     p = ln;
     nLeft = CTCLIB_DEBUG_LINEBUF_SIZE;
     nRet = sal_snprintf(p, nLeft, "  %04x: ", (unsigned int)(i + offset));
     p += nRet;
     nLeft -= nRet;
     jm = length - i;
     jm = jm > 16 ? 16 : jm;

     for (j = 0; j < jm; j++)
     {
         if ((j % 2) == 1)
         {
             nRet = sal_snprintf(p, nLeft, "%02x ", (unsigned int)cp[i + j]);
         }
         else
         {
             nRet = sal_snprintf(p, nLeft, "%02x", (unsigned int)cp[i + j]);
         }
         p += nRet;
         nLeft -= nRet;
     }

     for (; j < 16; j++)
     {
         if ((j % 2) == 1)
         {
             nRet = sal_snprintf(p, nLeft, "   ");
         }
         else
         {
             nRet = sal_snprintf(p, nLeft, "  ");
         }
         p += nRet;
         nLeft -= nRet;
     }

     nRet = sal_snprintf(p, nLeft, " ");
     p += nRet;
     nLeft -= nRet;
     for (j = 0; j < jm; j++)
     {
         c = cp[i + j];
         c = sal_char_isprint(c) ? c : '.';
         buf[0] = c;
         buf[1] = '\0';
         nRet = sal_snprintf(p, nLeft, buf);
         p += nRet;
         nLeft -= nRet;
     }
     return;
 }
