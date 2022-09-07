/*
 * Copyright (C) 2015 Steve Schnepp <steve.schnepp@pwkf.org> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

/* This plugin is compatible with munin-mainline version 2.0.25. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include "common.h"
#include "plugins.h"

#include <sys/vmmeter.h>

int load(int argc, char **argv) {
	if(argc > 1) {
		if(!strcmp(argv[1], "config")) {
			puts("graph_title Load average\n"
				"graph_args --base 1000 -l 0\n"
				"graph_vlabel load\n"
				"graph_scale no\n"
				"graph_category system\n"
				"load.label load");
			print_warncrit("load");
			puts("graph_info The load average of the machine describes how many processes are in the run-queue (scheduled to run \"immediately\").\n"
				"load.info 5 minute load average");
			return 0;
		}
		if(!strcmp(argv[1], "autoconf"))
			return writeyes();
	}

	struct loadavg l;
	size_t len = sizeof(l);

	if (sysctlbyname("vm.loadavg", &l, &len, NULL, 0) < 0)  {
		return fail("sysctl");
	}

	double ldavg = l.ldavg[0];
	double fscale = l.fscale;

	printf("# vm.loadavg, ldavg:%f fscale:%f -", ldavg, fscale);
	printf("\n");

	double load_1 = ldavg / fscale;
	printf("load.value %.2f\n", load_1);
	return 0;
}
