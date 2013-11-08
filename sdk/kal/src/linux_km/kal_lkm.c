#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include "kal.h"

static int kal_lkm_init()
{
    return 0;
}

static void kal_lkm_exit()
{
}

module_init(kal_lkm_init);
module_exit(kal_lkm_exit);
