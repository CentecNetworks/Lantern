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

#ifndef __CTCLIB_DEBUG_H__
#define __CTCLIB_DEBUG_H__

#include "sal.h"

#include "glb_const.h"
#include "ctclib_list.h"

#define DEBUG_ALL_TYPE (~(0UL))
#define DEBUG_NO_TYPE (0UL)

/* ======= debug control frame part: ======= */

/* length of module and sub-module*/
#define CTCLIB_DEBUG_NAME_LEN 20

/* define mod/submode name "none"*/
#define CTCLIB_DEBUG_NAME_NONE "none"

/* debug handle structure */
struct ctclib_debug_s
{
    uint32 flags; /* debug flag */
    char module[CTCLIB_DEBUG_NAME_LEN];
    char submodule[CTCLIB_DEBUG_NAME_LEN];
    ctclib_list_node_t list_head;
};
typedef struct ctclib_debug_s ctclib_debug_t;

/* define a global variable of the debug handle*/
#define CTCLIB_DEBUG_IMPLEMENT(level, mod, sub)                    \
ctclib_debug_t g_st_##level##_##mod##_##sub##_debug_handle =       \
{                                                                  \
    .flags        = DEBUG_NO_TYPE,                                 \
    .module       = #mod,                                          \
    .submodule    = #sub,                                          \
}

#define CTCLIB_DEBUG_OPEN(level, mod, sub)                         \
g_st_##level##_##mod##_##sub##_debug_handle.flags = DEBUG_ALL_TYPE;

#define CTCLIB_DEBUG_CLOSE(level, mod, sub)                        \
g_st_##level##_##mod##_##sub##_debug_handle.flags = DEBUG_NO_TYPE;

/* define a global list*/
#define CTCLIB_DEBUG_LIST_IMPLEMENT(level)                         \
ctclib_list_t g_st_##level##_debug_list;

/* use this global list*/
#define CTCLIB_DEBUG_LIST(level)  g_st_##level##_debug_list

/* init the global list*/
#define CTCLIB_DEBUG_LIST_INIT(level)  ctclib_list_init(&(CTCLIB_DEBUG_LIST(level)))

/* put the debug handles into the global list*/
#define CTCLIB_DEBUG_MODULE_REG(level, mod, sub)                   \
ctclib_list_insert_tail(&(g_st_##level##_debug_list),              \
    &(g_st_##level##_##mod##_##sub##_debug_handle.list_head)       \
)

/* define debug enum for each mode */
#define CTCLIB_DEBUG_ENUM(level, mod, sub, typeenum, ...)          \
extern ctclib_debug_t g_st_##level##_##mod##_##sub##_debug_handle; \
enum ctclib_##level##_##mod##_##sub##_debug_enum                   \
{                                                                  \
    typeenum = 0                                                   \
    ,##__VA_ARGS__                                                 \
    ,typeenum_##level##_##mod##_##sub##_max                        \
}

/* debug switch judgment*/
#define CTCLIB_IS_DEBUG_ON(level, mod, sub, typeenum)              \
((typeenum < typeenum_##level##_##mod##_##sub##_max)              \
    && (g_st_##level##_##mod##_##sub##_debug_handle.flags & (1 << (typeenum))) == (1 << (typeenum)))

/* auto build debug struct and initial for each mode */
struct ctclib_debug_control_s
{
    int8 lc_on;       /*whether enable debug on this linecard. on the mainboard it should always be 1.*/
    int8 line_fun_on; /*whether print line number and function name */
    int8 reserved[2];
};
typedef struct ctclib_debug_control_s ctclib_debug_control_t;
extern ctclib_debug_control_t g_ctclib_debug_switches;

/* if line card debug is on*/
#define CTCLIB_IS_LC_DEBUG_ON()        (g_ctclib_debug_switches.lc_on)

/* if print line num & function name is on*/
#define CTCLIB_IS_LINE_FUNC_DEBUG_ON() (g_ctclib_debug_switches.line_fun_on)


/* ======= debug print part: =======*/
/* print encapsulation*/
#if _GLB_UML_SYSTEM_
#define CTCLIB_PRINT(level, fmt, args...)                      \
{                                                              \
    FILE * fp_console = NULL;                                  \
    fp_console = fopen("/dev/console", "w+");                  \
    sal_fprintf(fp_console, "[%s] "fmt"\n", #level, ##args);   \
    fclose(fp_console);                                        \
                                                               \
    log_sys (CTCLIB_E_MOUDUL(level), E_DEBUG, fmt, ##args);    \
}
#else
#define CTCLIB_PRINT(level, fmt, args...)                      \
log_sys (CTCLIB_E_MOUDUL(level), E_DEBUG, fmt, ##args);
#endif

#if _GLB_UML_SYSTEM_
#define CTCLIB_PRINT_DIAG(level, fmt, args...)                      \
{                                                              \
    FILE * fp_console = NULL;                                  \
    fp_console = fopen("/dev/console", "w+");                  \
    sal_fprintf(fp_console, "[%s] "fmt"\n", #level, ##args);   \
    fclose(fp_console);                                        \
                                                               \
    log_diag (CTCLIB_E_MOUDUL(level), E_DEBUG, fmt, ##args);    \
}
#else
#define CTCLIB_PRINT_DIAG(level, fmt, args...)                      \
log_diag (CTCLIB_E_MOUDUL(level), E_DEBUG, fmt, ##args);
#endif

/* M_MOD_XXX*/
#define CTCLIB_E_MOUDUL(level)  M_MOD_##level

/* debug print frame*/
#define CTCLIB_DEBUG_OUT_INFO(level, mod, sub, typeenum, fmt, args...) \
do                                                                     \
{                                                                      \
    if (CTCLIB_IS_LC_DEBUG_ON()                                        \
        && CTCLIB_IS_DEBUG_ON (level, mod, sub, typeenum))             \
    {                                                                  \
        if(CTCLIB_IS_LINE_FUNC_DEBUG_ON())                             \
        {                                                              \
            CTCLIB_PRINT (level, "[%d][%s]"fmt,                        \
                __LINE__, __FUNCTION__, ##args);                       \
        }                                                              \
        else                                                           \
        {                                                              \
            CTCLIB_PRINT (level, fmt, ##args);                         \
        }                                                              \
    }                                                                  \
}                                                                      \
while (0)

#define CTCLIB_DEBUG_LINEBUF_SIZE  256
#define CTCLIB_DEBUG_OUT_HEX(level, mod, sub, typeenum, msg, buf, len, offset) \
do                                                                                   \
{                                                                                    \
    if (CTCLIB_IS_LC_DEBUG_ON()                                        \
        && CTCLIB_IS_DEBUG_ON (level, mod, sub, typeenum))             \
    {                                                                             \
        CTCLIB_PRINT (level, msg);                         \
        unsigned i = 0;                                                         \
        for (i = 0; i < len; i += 0x10)                      \
        {                                                                  \
            char ln[CTCLIB_DEBUG_LINEBUF_SIZE];                         \
            memset(ln, 0x0, sizeof(ln));                                            \
            ctclib_debug_print_hex_emacs(&g_st_##level##mod##sub##_debughandle, (i), (buf), (len), (offset), ln); \
            CTCLIB_PRINT(level, "%s", ln);                                              \
        }                                                                                          \
    }                                                                                \
}                                                                                    \
while (0)

/* debug print frame*/
#define CTCLIB_DEBUG_OUT_INFO_DIAG(level, mod, sub, typeenum, fmt, args...) \
do                                                                     \
{                                                                      \
    if (CTCLIB_IS_LC_DEBUG_ON()                                        \
        && CTCLIB_IS_DEBUG_ON (level, mod, sub, typeenum))             \
    {                                                                  \
        if(CTCLIB_IS_LINE_FUNC_DEBUG_ON())                             \
        {                                                              \
            CTCLIB_PRINT_DIAG (level, "[%d][%s]"fmt,                        \
                __LINE__, __FUNCTION__, ##args);                       \
        }                                                              \
        else                                                           \
        {                                                              \
            CTCLIB_PRINT_DIAG (level, fmt, ##args);                         \
        }                                                              \
    }                                                                  \
}                                                                      \
while (0)

#define CTCLIB_DEBUG_OUT_HEX_DIAG(level, mod, sub, typeenum, msg, buf, len, offset) \
do                                                                                   \
{                                                                                    \
    if (CTCLIB_IS_LC_DEBUG_ON()                                        \
        && CTCLIB_IS_DEBUG_ON (level, mod, sub, typeenum))             \
    {                                                                             \
        CTCLIB_PRINT (level, msg);                         \
        unsigned i = 0;                                                         \
        for (i = 0; i < len; i += 0x10)                      \
        {                                                                  \
            char ln[CTCLIB_DEBUG_LINEBUF_SIZE];                         \
            memset(ln, 0x0, sizeof(ln));                                            \
            ctclib_debug_print_hex_emacs(&g_st_##level##mod##sub##_debughandle, (i), (buf), (len), (offset), ln); \
            CTCLIB_PRINT_DIAG(level, "%s", ln);                          \
        }                                                                                          \
    }                                                                                \
}                                                                                    \
while (0)

/* ======= debug call back functions part: =======*/

/* callback functions for set/unset submodule*/
typedef int32(*CTCLIB_CALLBACK_CLI_SETTING) (char*, uint32, char*, char*);
/* callback functions for set module*/
typedef int32(*CTCLIB_CALLBACK_CLI_MOD_ON) (char*, char*);
/* callback functions for unset module*/
typedef int32(*CTCLIB_CALLBACK_CLI_MOD_OFF) (char*, char*);
/* callback functions for show module*/
typedef int32(*CTCLIB_CALLBACK_CLI_MOD_SHOW) (char*, char*);

/* callback functions structure*/
struct ctclib_debug_func_s
{
    CTCLIB_CALLBACK_CLI_SETTING     p_func_cli_setting;
    CTCLIB_CALLBACK_CLI_MOD_ON      p_func_cli_mod_on;
    CTCLIB_CALLBACK_CLI_MOD_OFF     p_func_cli_mod_off;
    CTCLIB_CALLBACK_CLI_MOD_SHOW     p_func_cli_mod_show;
};
typedef struct ctclib_debug_func_s ctclib_debug_func_t;

/* define a callback functions record handle*/
#define   CTCLIB_FUNCTION_IMPLEMENT(level)   \
ctclib_debug_func_t  g_##level##_debug_functions;

/* use the callback functions record handle*/
#define CTCLIB_FUNCTION(level)    g_##level##_debug_functions

/* callback encapsulation*/
#define CTCLIB_CLI_SETTING(level)  (g_##level##_debug_functions.p_func_cli_setting)
#define CTCLIB_CLI_MOD_ON(level)   (g_##level##_debug_functions.p_func_cli_mod_on)
#define CTCLIB_CLI_MOD_OFF(level)  (g_##level##_debug_functions.p_func_cli_mod_off)
#define CTCLIB_CLI_MOD_SHOW(level)  (g_##level##_debug_functions.p_func_cli_mod_show)

/* ======= debug CLI part: =======*/

/* debug handle for CLI*/
struct ctclib_debug_cli_s
{
    char*  level;         /* level name: */
    char*  module;        /* module name */
    char*  submodule;     /* sub module name */
    uint32  flags;         /* debug switch */
    char** keywords;     /* type keywords */
    void*   p_arg;            /* callback argument: pointer to module Id */

    /* callback argument:pointer to command mode, when in internal debug mode, we needn't do something*/
    void*   p_cmdmode;
};
typedef struct ctclib_debug_cli_s ctclib_debug_cli_t;

/* define debug CLI keyword*/
#define CTCLIB_DEBUG_TYPESTR(level, mod, sub, typestr, ...)           \
char *g_st_##level##mod##sub##_debugtypestrings[] =          \
{                                                                     \
    typestr                                                           \
    ,##__VA_ARGS__                                                    \
    ,NULL                                                             \
};                                                                    \
ctclib_debug_cli_t g_st_##level##mod##sub##_debughandle =             \
{                                                                     \
    .module     = #mod,                                               \
    .submodule  = #sub,                                               \
    .flags      = 0x00,                                               \
    .keywords  = g_st_##level##mod##sub##_debugtypestrings,           \
};

#define CTCLIB_DEBUG_DEBUG_HANDLE_EXTERN(level, mod, sub)                              \
extern ctclib_debug_cli_t g_st_##level##mod##sub##_debughandle;                                      \

/* cli implement*/
#define CTCLIB_DEBUG_CLI_IMPLEMENT(level, mod, sub, cmdstr, ...)                                    \
extern ctclib_debug_func_t  g_##level##_debug_functions;                                            \
extern ctclib_list_t g_st_##level##_debug_list;                                                     \
CLI (_debug_##level##mod##sub##_on,                                                                 \
     _debug_##level##mod##sub##_on_cmd,                                                             \
     "debug "cmdstr,                                                                                \
     CLI_DEBUG_STR,                                                                                 \
     __VA_ARGS__                                                                                    \
    )                                                                                               \
{                                                                                                   \
    ctclib_debug_cli_t *pst_hdl = &g_st_##level##mod##sub##_debughandle;                            \
    uint32 type = 0;                                                                                \
    char* keywords = pst_hdl->keywords[type];                                                      \
    int32 retval = 0;                                                                               \
                                                                                                    \
    if (!sal_strcasecmp(argv[0], "all"))                                                            \
    {                                                                                               \
        pst_hdl->flags = DEBUG_ALL_TYPE;                                                            \
        if(CTCLIB_CLI_SETTING(level))                                                               \
        {                                                                                           \
            retval = (CTCLIB_CLI_SETTING(level))(#level,                                            \
                pst_hdl->flags, pst_hdl->module, pst_hdl->submodule);                               \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            retval = ctclib_debug_clisetting_common(&g_st_##level##_debug_list,                     \
                pst_hdl->flags, pst_hdl->module, pst_hdl->submodule);                               \
        }                                                                                           \
                                                                                                    \
        if(retval < 0 )                                                                             \
        {                                                                                           \
            cli_out(cli, "%% Fail to set config debug to %s %s %s!\n", #level, #mod, #sub);         \
            return CLI_ERROR;                                                                       \
        }                                                                                           \
                                                                                                    \
        return CLI_SUCCESS;                                                                         \
    }                                                                                               \
                                                                                                    \
    while (NULL != keywords)                                                                        \
    {                                                                                               \
        if (!sal_strcasecmp(argv[0], keywords))                                                     \
        {                                                                                           \
            pst_hdl->flags |= (1 << type);                                                          \
            if(CTCLIB_CLI_SETTING(level))                                                           \
            {                                                                                       \
                retval = (CTCLIB_CLI_SETTING(level))(#level,                                        \
                    pst_hdl->flags, pst_hdl->module, pst_hdl->submodule);                           \
            }                                                                                       \
            else                                                                                    \
            {                                                                                       \
                retval = ctclib_debug_clisetting_common(&g_st_##level##_debug_list,                 \
                    pst_hdl->flags, pst_hdl->module, pst_hdl->submodule);                           \
            }                                                                                       \
                                                                                                    \
            if(retval < 0 )                                                                         \
            {                                                                                       \
            cli_out(cli, "%% Fail to set config debug to %s %s %s!\n", #level, #mod, #sub);         \
                return CLI_ERROR;                                                                   \
            }                                                                                       \
            return CLI_SUCCESS;                                                                     \
        }                                                                                           \
        type++;                                                                                     \
        keywords = pst_hdl->keywords[type];                                                         \
    }                                                                                               \
                                                                                                    \
    cli_out(cli, "%% Invalid debug switch for %s %s\n",                                             \
            pst_hdl->module, pst_hdl->submodule);                                                   \
    SAL_ASSERT(0);                                                                                  \
    return CLI_ERROR;                                                                               \
}                                                                                                   \
                                                                                                    \
CLI (_debug_##level##mod##sub##_off,                                                                \
     _debug_##level##mod##sub##_off_cmd,                                                            \
     "no debug "cmdstr,                                                                             \
     CLI_NO_STR,                                                                                    \
     CLI_DEBUG_STR,                                                                                 \
     __VA_ARGS__                                                                                    \
    )                                                                                               \
{                                                                                                   \
    ctclib_debug_cli_t *pst_hdl = &g_st_##level##mod##sub##_debughandle;                            \
    uint32 type = 0;                                                                                \
    char* keywords = pst_hdl->keywords[type];                                                      \
    int32 retval = 0;                                                                               \
                                                                                                    \
    if (!sal_strcasecmp(argv[0], "all"))                                                            \
    {                                                                                               \
        pst_hdl->flags = DEBUG_NO_TYPE;                                                             \
        if(CTCLIB_CLI_SETTING(level))                                                               \
        {                                                                                           \
            retval = (CTCLIB_CLI_SETTING(level))(#level,                                            \
                pst_hdl->flags, pst_hdl->module, pst_hdl->submodule);                               \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            retval = ctclib_debug_clisetting_common(&g_st_##level##_debug_list,                     \
                pst_hdl->flags, pst_hdl->module, pst_hdl->submodule);                               \
        }                                                                                           \
        if(retval < 0 )                                                                             \
        {                                                                                           \
            cli_out(cli, "%% Fail to set config debug to %s %s %s!\n", #level, #mod, #sub);         \
            return CLI_ERROR;                                                                       \
        }                                                                                           \
        return CLI_SUCCESS;                                                                         \
    }                                                                                               \
                                                                                                    \
    while (NULL != keywords)                                                                        \
    {                                                                                               \
        if (!sal_strcasecmp(argv[0], keywords))                                                     \
        {                                                                                           \
            pst_hdl->flags &= (~(1 << type));                                                       \
            if(CTCLIB_CLI_SETTING(level))                                                           \
            {                                                                                       \
                retval = (CTCLIB_CLI_SETTING(level))(#level,                                        \
                    pst_hdl->flags, pst_hdl->module, pst_hdl->submodule);                           \
            }                                                                                       \
            else                                                                                    \
            {                                                                                       \
                retval = ctclib_debug_clisetting_common(&g_st_##level##_debug_list,                 \
                    pst_hdl->flags, pst_hdl->module, pst_hdl->submodule);                           \
            }                                                                                       \
            if(retval < 0 )                                                                         \
            {                                                                                       \
                cli_out(cli, "%% Fail to set config debug to %s %s %s!\n", #level, #mod, #sub);     \
                return CLI_ERROR;                                                                   \
            }                                                                                       \
            return CLI_SUCCESS;                                                                     \
        }                                                                                           \
        type++;                                                                                     \
        keywords = pst_hdl->keywords[type];                                                         \
    }                                                                                               \
                                                                                                    \
    cli_out(cli, "%% Invalid debug switch for %s %s\n",                                             \
            pst_hdl->module, pst_hdl->submodule);                                                   \
    SAL_ASSERT(0);                                                                                  \
    return CLI_ERROR;                                                                               \
}                                                                                                   \
                                                                                                    \
CLI (_debug_##level##mod##sub##_show,                                                               \
     _debug_##level##mod##sub##_show_cmd,                                                           \
     "show debugging "cmdstr,                                                                       \
     CLI_SHOW_STR,                                                                                  \
     CLI_DEBUG_STR,                                                                                 \
     __VA_ARGS__                                                                                    \
    )                                                                                               \
{                                                                                                   \
    ctclib_debug_cli_t *pst_hdl = &g_st_##level##mod##sub##_debughandle;                            \
    uint32 type = 0;                                                                                \
    char *keywords= pst_hdl->keywords[type];                                                       \
                                                                                                    \
    cli_out(cli, #level" "#mod" "#sub" debugging status:\n");                                       \
                                                                                                    \
    if (!sal_strcasecmp(argv[0], "all"))                                                            \
    {                                                                                               \
        if (DEBUG_ALL_TYPE == pst_hdl->flags)                                                       \
        {                                                                                           \
            cli_out(cli, "  all debugging are on\n");                                               \
            return CLI_SUCCESS;                                                                     \
        }                                                                                           \
                                                                                                    \
        while (NULL != keywords)                                                                    \
        {                                                                                           \
            if ((pst_hdl->flags & (1 << type)))                                                     \
            {                                                                                       \
                cli_out(cli, "  %s debugging is on\n",                                              \
                        pst_hdl->keywords[type]);                                                   \
            }                                                                                       \
            type++;                                                                                 \
            keywords = pst_hdl->keywords[type];                                                     \
        }                                                                                           \
    }                                                                                               \
                                                                                                    \
    while (NULL != keywords)                                                                        \
    {                                                                                               \
        if (!sal_strcasecmp(argv[0], keywords))                                                     \
        {                                                                                           \
            if (pst_hdl->flags & (1 << type))                                                       \
            {                                                                                       \
                cli_out(cli, "  %s debugging is on\n",                                              \
                        pst_hdl->keywords[type]);                                                   \
            }                                                                                       \
            else                                                                                    \
            {                                                                                       \
                cli_out(cli, "  %s debugging is off\n",                                             \
                        pst_hdl->keywords[type]);                                                   \
            }                                                                                       \
        }                                                                                           \
                                                                                                    \
        type++;                                                                                     \
        keywords = pst_hdl->keywords[type];                                                         \
    }                                                                                               \
                                                                                                    \
    return CLI_SUCCESS;                                                                             \
}

#define CTCLIB_DEBUG_ON(level, mod, sub)                                      \
do                                                                                                  \
{                                                                                                   \
    ctclib_debug_cli_t *pst_hdl = &g_st_##level##mod##sub##_debughandle;                            \
    int32 retval = 0;                                                                               \
                                                                                                    \
    pst_hdl->flags = DEBUG_ALL_TYPE;                                                            \
    if(CTCLIB_CLI_SETTING(level))                                                               \
    {                                                                                           \
       retval = (CTCLIB_CLI_SETTING(level))(#level,                                            \
       pst_hdl->flags, pst_hdl->module, pst_hdl->submodule);                               \
    }                                                                                           \
    else                                                                                        \
    {                                                                                           \
       retval = ctclib_debug_clisetting_common(&g_st_##level##_debug_list,                     \
       pst_hdl->flags, pst_hdl->module, pst_hdl->submodule);                               \
    }                                                                                           \
                                                                                                   \
    if(retval < 0 )                                                                             \
    {                                                                                           \
       cli_out(cli, "%% Fail to set config debug to %s %s %s!\n", #level, #mod, #sub);         \
       return;                                                                       \
    }                                                                                           \
                                                                                                 \
    return;                                                                         \
}                                                                                                   \
while (0)

#define CTCLIB_DEBUG_OFF(level, mod, sub)                                      \
do                                                                                                  \
{                                                                                                   \
    ctclib_debug_cli_t *pst_hdl = &g_st_##level##mod##sub##_debughandle;                            \
    int32 retval = 0;                                                                               \
    pst_hdl->flags = DEBUG_NO_TYPE;                                                             \
    if(CTCLIB_CLI_SETTING(level))                                                               \
    {                                                                                           \
        retval = (CTCLIB_CLI_SETTING(level))(#level,                                            \
        pst_hdl->flags, pst_hdl->module, pst_hdl->submodule);                               \
    }                                                                                           \
    else                                                                                        \
    {                                                                                           \
        retval = ctclib_debug_clisetting_common(&g_st_##level##_debug_list,                     \
        pst_hdl->flags, pst_hdl->module, pst_hdl->submodule);                               \
    }                                                                                           \
    if(retval < 0 )                                                                             \
    {                                                                                           \
        cli_out(cli, "%% Fail to set config debug to %s %s %s!\n", #level, #mod, #sub);         \
        return;                                                                       \
    }                                                                                           \
    return;                                                                        \
}                                                                                                   \
while (0)

#define CTCLIB_DEBUG_CLI_INSTALL(level, mod, sub, ctree, mode)                                      \
do                                                                                                  \
{                                                                                                   \
    cli_install_gen (ctree, mode, PRIVILEGE_NORMAL, 0, &_debug_##level##mod##sub##_on_cmd);         \
    cli_install_gen (ctree, mode, PRIVILEGE_NORMAL, 0, &_debug_##level##mod##sub##_off_cmd);        \
    if (mode == INTERNAL_DEBUG_MODE)                                                                \
    {                                                                                               \
        cli_install_gen (ctree, EXEC_MODE, PRIVILEGE_MAX,                                           \
            CLI_FLAG_HIDDEN, &_debug_##level##mod##sub##_show_cmd);                                 \
    }                                                                                               \
    cli_install_gen (ctree, mode, PRIVILEGE_NORMAL, 0, &_debug_##level##mod##sub##_show_cmd);       \
}                                                                                                   \
while (0)

typedef int32 ctclib_debug_ret_t;

/*define a cli to open/close all the debug switch for a module*/
#define CTCLIB_DEBUG_CLI_IMPLEMENT_MODALL(level, mod, cmdstr, ...)                                 \
extern ctclib_debug_func_t  g_##level##_debug_functions;                                           \
extern ctclib_list_t g_st_##level##_debug_list;                                                    \
CLI (_debug_##level##mod##_on,                                                                     \
     _debug_##level##mod##_on_cmd,                                                                 \
     "debug "cmdstr,                                                                               \
     CLI_DEBUG_STR,                                                                                \
     __VA_ARGS__                                                                                   \
    )                                                                                              \
{                                                                                                  \
    ctclib_debug_ret_t retval = 0;                                                                 \
                                                                                                   \
    if(CTCLIB_CLI_MOD_ON(level))                                                                   \
    {                                                                                              \
        retval = (CTCLIB_CLI_MOD_ON(level))(#level, #mod);                                         \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
        retval = ctclib_debug_climodon_common(&g_st_##level##_debug_list,#mod);                    \
    }                                                                                              \
                                                                                                   \
    if(retval)                                                                                     \
    {                                                                                              \
        cli_out(cli, "%% Fail to set config debug to %s %s!\n", #level, #mod);                     \
        return CLI_ERROR;                                                                          \
    }                                                                                              \
                                                                                                   \
    return CLI_SUCCESS;                                                                            \
}                                                                                                  \
                                                                                                   \
CLI (_debug_##level##mod##_off,                                                                    \
     _debug_##level##mod##_off_cmd,                                                                \
     "no debug "cmdstr,                                                                            \
     CLI_NO_STR,                                                                                   \
     CLI_DEBUG_STR,                                                                                \
     __VA_ARGS__                                                                                   \
    )                                                                                              \
{                                                                                                  \
    ctclib_debug_ret_t retval = 0;                                                                 \
                                                                                                   \
    if(CTCLIB_CLI_MOD_OFF(level))                                                                  \
    {                                                                                              \
        retval = (CTCLIB_CLI_MOD_OFF(level))(#level, #mod);                                        \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
        retval = ctclib_debug_climodoff_common(&g_st_##level##_debug_list,#mod);                   \
    }                                                                                              \
    if(retval)                                                                                     \
    {                                                                                              \
        cli_out(cli, "%% Fail to set config debug to %s %s!\n", #level, #mod);                     \
        return CLI_ERROR;                                                                          \
    }                                                                                              \
                                                                                                   \
    return CLI_SUCCESS;                                                                            \
}                                                                                                  \
                                                                                                   \
CLI (_debug_##level##mod##_show,                                                                   \
     _debug_##level##mod##_show_cmd,                                                               \
     "show debugging "cmdstr,                                                                      \
     CLI_SHOW_STR,                                                                                 \
     CLI_DEBUG_STR,                                                                                \
     __VA_ARGS__                                                                                   \
    )                                                                                              \
{                                                                                                  \
    ctclib_debug_ret_t retval = CLI_SUCCESS;                                                       \
                                                                                                   \
    ctclib_list_node_t* p_node = NULL;                                                             \
    ctclib_debug_t* p_debug_hdl = NULL;                                                            \
                                                                                                   \
    ctclib_list_for_each(p_node, &(CTCLIB_DEBUG_LIST(level)))                                      \
    {                                                                                              \
        p_debug_hdl = ctclib_container_of(p_node, ctclib_debug_t, list_head);                      \
        if (!sal_strcmp(p_debug_hdl->module, #mod))                                                \
        {                                                                                          \
            cli_out(cli, " debug flag is  %x \n",  p_debug_hdl->flags);                            \
        }                                                                                          \
    }                                                                                              \
                                                                                                   \
    return retval;                                                                                 \
}

#define CTCLIB_DEBUG_CLI_INSTALL_MODALL(level, mod, ctree, mode)                                    \
do                                                                                                  \
{                                                                                                   \
    cli_install_gen (ctree, mode, PRIVILEGE_NORMAL, 0, &_debug_##level##mod##_on_cmd);              \
    cli_install_gen (ctree, mode, PRIVILEGE_NORMAL, 0, &_debug_##level##mod##_off_cmd);             \
    if (mode == INTERNAL_DEBUG_MODE)                                                                \
    {                                                                                               \
        cli_install_gen (ctree, EXEC_MODE, PRIVILEGE_MAX,                                           \
            CLI_FLAG_HIDDEN, &_debug_##level##mod##_show_cmd);                                      \
    }                                                                                               \
    cli_install_gen (ctree, mode, PRIVILEGE_NORMAL, 0, &_debug_##level##mod##_show_cmd);            \
}                                                                                                   \
while (0)

#define CTCLIB_DEBUG_REGISTER(level, mod, sub, typeenum) \
ctclib_debug_register(&g_st_##level##_##mod##_##sub##_debug_handle)

#define CTCLIB_DEBUG_UNREGISTER(level, mod, sub) \
ctclib_debug_unregister(&g_st_##level##_##mod##_##sub##_debug_handle, (void *)level)

/* ======= for errer return : =======*/
struct glb_err_mapper_s
{
    glb_errno_e_t err_no;
    char* sz_err_str;
};
typedef struct glb_err_mapper_s glb_err_mapper_t;

#define CTCLIB_ERR_LOG_RETURN(level, errno)                                \
extern glb_err_mapper_t g_ast_err_mapper[];                                \
do                                                                         \
{                                                                          \
    uint32 i = 0;                                                          \
    int32 ret = errno;                                                     \
    for (i = 0; i != GLB_E_MAX; i++)                                       \
    {                                                                      \
        if(NULL == g_ast_err_mapper[i].sz_err_str)                         \
        {                                                                  \
            break;                                                         \
        }                                                                  \
        if(ret == g_ast_err_mapper[i].err_no)                              \
        {                                                                  \
            CTCLIB_PRINT(level, "%s return %d: %s",                        \
                __FUNCTION__,                                              \
                ret,                                                       \
                g_ast_err_mapper[i].sz_err_str);                           \
        }                                                                  \
    }                                                                      \
    return ret;                                                            \
}while(0)

/* ======= function declare: =======*/
int32
ctclib_debug_init(uint32 is_master);
int32
ctclib_debug_line_function_on(int8 on_off);
int32
ctclib_debug_linecard_on(int8 on_off);
int32
ctclib_debug_clisetting_common(ctclib_list_t* p_list, uint32 flag, char* sz_module, char* sz_submodule);
int32
ctclib_debug_climodon_common(ctclib_list_t* p_list, char* sz_module);
int32
ctclib_debug_climodoff_common(ctclib_list_t* p_list, char* sz_module);
void
ctclib_print_msg_on_tty(char *printout);
void
ctclib_debug_print_hex_emacs(ctclib_debug_cli_t *pHdl, unsigned i, const u_char *cp, unsigned int length, unsigned int offset, char *ln);

#endif /*__CTCLIB_DEBUG_H__*/
