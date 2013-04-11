/*
 * Copyright (C) 2008-2013 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "plugins.h"

#define PROC_UPTIME "/proc/uptime"

int uptime(int argc, char **argv) {
	FILE *f;
	float uptime;
	if(argc > 1) {
		if(!strcmp(argv[1], "config")) {
			puts("graph_title Uptime\n"
				"graph_args --base 1000 -l 0 \n"
				"graph_vlabel uptime in days\n"
				"uptime.label uptime\n"
				"uptime.draw AREA");
			print_warncrit("uptime");
			return 0;
		}
		if(!strcmp(argv[1], "autoconf"))
			return writeyes();
	}
	if(!(f=fopen(PROC_UPTIME, "r")))
		return fail("cannot open " PROC_UPTIME);
	if(1 != fscanf(f, "%f", &uptime)) {
		fclose(f);
		return fail("cannot read from " PROC_UPTIME);
	}
	fclose(f);
	printf("uptime.value %.2f\n", uptime/86400);
	return 0;
}
