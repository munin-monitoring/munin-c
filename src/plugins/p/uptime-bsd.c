/*
 * Copyright (C) 2008-2013 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

#include <stdio.h>
#include <string.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include "common.h"
#include "plugins.h"

int uptime(int argc, char **argv)
{
	if (argc > 1) {
		if (!strcmp(argv[1], "config")) {
			puts("graph_title Uptime\n"
			     "graph_args --base 1000 -l 0 \n"
			     "graph_vlabel uptime in days\n"
			     "graph_category system\n"
			     "uptime.label uptime\n" "uptime.draw AREA");
			print_warncrit("uptime");
			return 0;
		}
		if (!strcmp(argv[1], "autoconf"))
			return writeyes();
	}

	struct timeval now;
	if (gettimeofday(&now, NULL) < 0) {
		return fail("gettimeofday");
	}

	struct timeval bootTime;
	size_t len = sizeof(bootTime);
	if (sysctlbyname("kern.boottime", &bootTime, &len, NULL, 0) < 0)  {
		return fail("sysctlbyname");
	}

	printf("uptime.value %.2f\n", (float)(now.tv_sec - bootTime.tv_sec) / 86400);

	return 0;
}
