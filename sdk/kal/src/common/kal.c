#include"kal.h"

kal_err_t kal_init(void)
{
    kal_err_t ret;
    ret = kal_timer_init();
    return ret;
}

void kal_deinit(void)
{
    kal_timer_fini();
}


