
#include "kal.h"
#include "drv_lib.h"
#include "ctc_cli.h"
#include "dbg_tool_cli.h"
#if SDK_WORK_PLATFORM
#include "cmodel_cli.h"
#endif

void
ctc_cli_mode_exit(void)
{
  switch(vty->node)
  {
    case EXEC_MODE:
        restore_terminal_mode();
        exit(0);
        break;
    case CTC_CMODEL_MODE:
    case CTC_DBG_TOOL_MODE:
        //vty->node = EXEC_MODE;
        restore_terminal_mode();
        exit(0);
        break;
    default:
        vty->node = EXEC_MODE;
        break;
  }
}


CTC_CLI(exit_config,
    exit_cmd,
    "exit",
    "End current mode and down to previous mode")
{

   ctc_cli_mode_exit ();
    return CLI_SUCCESS;
}

CTC_CLI(quit_config,
    quit_cmd,
    "quit",
    "Exit current mode and down to previous mode")
{
    ctc_cli_mode_exit();
    return CLI_SUCCESS;
}


CTC_CLI(cli_enter_mode,
        enter_mode_cmd,
        "enter (debug-tool | cmodel) mode",
        "Enter",
        "Ctc debug tool mode",
        "Ctc cmodel mode",
        "Mode")
{
    char *mode = NULL;

    mode = argv[0];

    if(0 == kal_strcmp(mode, "debug-tool"))
    {
        vty->node  = CTC_DBG_TOOL_MODE;
    }
    else
    {
        vty->node  = CTC_CMODEL_MODE;
    }

    return CLI_SUCCESS;
}


struct cmd_node exec_node =
{
  EXEC_MODE,
  "\rCTC_CLI# ",
};


struct cmd_node debug_tool_node =
{
  CTC_DBG_TOOL_MODE,
  "\rCTC_CLI(ctc-dt)# ",
};

struct cmd_node cmodel_node =
{
  CTC_CMODEL_MODE,
  "\rCTC_CLI(ctc-cmodel)# ",
};

int dbg_tool_cli_master(void)
{
    cmd_init (0);

    install_node (&exec_node, NULL);
    install_node (&debug_tool_node, NULL);
#if SDK_WORK_PLATFORM
    install_node (&cmodel_node, NULL);
#endif
    vty_init(CTC_DBG_TOOL_MODE);

    install_element(EXEC_MODE, &exit_cmd);
    install_element(EXEC_MODE, &quit_cmd);

    install_element(EXEC_MODE, &enter_mode_cmd);

    install_element(CTC_DBG_TOOL_MODE, &exit_cmd);
    install_element(CTC_DBG_TOOL_MODE, &quit_cmd);

#if SDK_WORK_PLATFORM
    install_element(CTC_CMODEL_MODE, &exit_cmd);
    install_element(CTC_CMODEL_MODE, &quit_cmd);
#endif

    ctc_dbg_tool_cli_init(CTC_DBG_TOOL_MODE);

#if SDK_WORK_PLATFORM
    ctc_cmodel_cli_init(CTC_CMODEL_MODE);
#endif

    sort_node();

    set_terminal_raw_mode();

    while(1)
    {
        vty_read();
    }

    restore_terminal_mode();

    return 0;
}

int main()
{
    dbg_tool_cli_master();

    return 0;
}

