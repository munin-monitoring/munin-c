/*
 * Copyright (C) 2008-2013 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

#include <string.h>
#include <stdio.h>
#include <sys/sysctl.h>
#include <unistd.h>
#include "common.h"
#include "plugins.h"

int forks(int argc, char **argv)
{
	if (argc > 1) {
		if (!strcmp(argv[1], "config")) {
			puts("graph_title Fork rate\n"
			     "graph_args --base 1000 -l 0\n"
			     "graph_vlabel forks / ${graph_period}\n"
			     "graph_category processes\n"
			     "graph_info This graph shows the forking rate (new processes started).\n"
			     "forks.label forks\n"
			     "forks.type DERIVE\n"
			     "forks.min 0\n"
			     "forks.max 100000\n"
			     "forks.info The number of forks per second.");
			print_warncrit("forks");
			return 0;
		}
		if (!strcmp(argv[1], "autoconf"))
			return writeyes();
	}

	u_long forks, vforks, rforks;
	size_t len = sizeof(u_long);
	if (sysctlbyname("vm.stats.vm.v_forks", &forks, &len, NULL, 0) < 0)  {
		return fail("sysctlbyname");
	}
	if (sysctlbyname("vm.stats.vm.v_vforks", &vforks, &len, NULL, 0) < 0)  {
		return fail("sysctlbyname");
	}
	if (sysctlbyname("vm.stats.vm.v_rforks", &rforks, &len, NULL, 0) < 0)  {
		return fail("sysctlbyname");
	}

	printf("forks.value %d\n", forks + vforks + rforks);

	return 0;
}
