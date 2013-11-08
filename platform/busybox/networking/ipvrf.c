#include "libiproute/utils.h"
#include "libiproute/ip_common.h"

#include "busybox.h"

int ipvrf_main(int argc, char **argv)
{
	ip_parse_common_args(&argc, &argv);

	return do_ipvrf(argc-1, argv+1);
}

