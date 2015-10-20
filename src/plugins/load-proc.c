/*
 * Copyright (C) 2008-2013 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

/* This plugin is compatible with munin-mainline version 2.0.17. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "plugins.h"

#define PROC_LOADAVG "/proc/loadavg"

int load(int argc, char **argv) {
	FILE *f;
	float val;
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
	if(!(f=fopen(PROC_LOADAVG, "r")))
		return fail("cannot open " PROC_LOADAVG);
	if(1 != fscanf(f, "%*f %f", &val)) {
		fclose(f);
		return fail("cannot read from " PROC_LOADAVG);
	}
	fclose(f);
	printf("load.value %.2f\n", val);
	return 0;
}
