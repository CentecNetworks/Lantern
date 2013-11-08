#include <config.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "dpif.h"
#include "dummy.h"

/*dpif.c stubs*/
void dp_blacklist_provider(const char *type OVS_UNUSED)
{
}

/*dpif_normalize_type and dp_parse_name is used by ovs-ofctl*/
const char *
dpif_normalize_type(const char *type)
{
    return type && type[0] ? type : "system";
}

void
dp_parse_name(const char *datapath_name_, char **name, char **type)
{
    char *datapath_name = xstrdup(datapath_name_);
    char *separator;

    separator = strchr(datapath_name, '@');
    if (separator) {
        *separator = '\0';
        *type = datapath_name;
        *name = xstrdup(dpif_normalize_type(separator + 1));
    } else {
        *name = datapath_name;
        *type = xstrdup(dpif_normalize_type(NULL));
    }
}
/*End dpif.c stubs*/

/*dummy.c stubs*/
void 
dummy_enable(bool override OVS_UNUSED)
{  
}
/*End dummy.c stubs*/
