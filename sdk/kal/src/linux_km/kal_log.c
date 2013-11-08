#include "kal.h"

extern kal_loglevel_t debug_level_flag;

static const char *ll2str[] =
{
    KERN_CRIT    "FATAL %s(%d): %s\n",
    KERN_ERR     "ERROR %s(%d): %s\n",
    KERN_WARNING "WARN  %s(%d): %s\n",
    KERN_INFO    "INFO  %s(%d): %s\n",
    KERN_DEBUG   "DEBUG %s(%d): %s\n",
};

int kal_log(int log_level,
            const char *file,
            int line,
            const char *fmt,
            ...)
{
    char fmtbuf[0x100];
    va_list ap;

    if (log_level < debug_level_flag)
     return 0;

    if (log_level < KAL_LL_FATAL)
        log_level = KAL_LL_FATAL;

    if (log_level < KAL_LL_INFO)
    {
        snprintf(fmtbuf, 0x100, ll2str[log_level], file, line, fmt);
        fmtbuf[0xFF] = 0;
    }
    else
    {
        strncpy(fmtbuf, fmt, 0x100);
        fmtbuf[0xFF] = 0;
    }

    va_start(ap, fmt);
    vprintk(fmtbuf, ap);
    va_end(ap);
    return 0;
}

int kal_print_func(const char *fmt,  ...)
{
    int i = 0;
    int j = 0;
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(kal_print_buf, 1023,fmt, ap);
    va_end(ap);

  i = 0;
    while(i<1024 &&  j<512 && kal_print_buf[i] != '\0')
    {
        switch(kal_print_buf[i])
        {
            case '\n':
                kal_print_line[j] = '\0';
               vprintk("%s\r\n", kal_print_line);
                j = 0;
                break;
            default:
                kal_print_line[j] = kal_print_buf[i];
                j++;
                break;
        }
        i++;
    }
    if(kal_print_buf[i] == '\0')
    {
        kal_print_line[j] = '\0';
       vprintk("%s", kal_print_line);
    }
    return 0;
}

EXPORT_SYMBOL(kal_log);

