/*
 * Copyright (C) 2015 Steve Schnepp <steve.schnepp@pwkf.org> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

#include <mach/mach_host.h>
#include <string.h>
#include <stdio.h>
#include "common.h"
#include "plugins.h"

int cpu(int argc, char **argv) {
	if(argc > 1) {
		if(!strcmp(argv[1], "config")) {
			puts("graph_title CPU usage\n"
				 "graph_args --base 1000 -r --lower-limit 0 --upper-limit 100\n"
				 "graph_vlabel %\n"
				 "graph_scale no\n"
				 "graph_info This graph shows how CPU time is spent.\n"
				 "graph_category system\n"
				 "graph_period second\n"

				 "system.label system\n"
				 "system.type DERIVE\n"
				 "system.draw AREA\n"
				 "system.info CPU time spent by the kernel in system activities\n"

				 "user.label user\n"
				 "user.type DERIVE\n"
				 "user.draw STACK\n"
				 "user.info CPU time spent by normal programs and daemons\n"

				 "nice.label nice\n"
				 "nice.type DERIVE\n"
				 "nice.draw STACK\n"
				 "nice.info CPU time spent by nice(1)d programs\n"

				 "idle.label idle\n"
				 "idle.type DERIVE\n"
				 "idle.draw STACK\n"
				 "idle.info Idle CPU time");

			return 0;
		}
		if(!strcmp(argv[1], "autoconf"))
			return writeyes();
	}

	host_t host = mach_host_self();
	host_cpu_load_info_data_t li;
	mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
	if (host_statistics(host, HOST_CPU_LOAD_INFO, (host_info_t)&li, &count) != KERN_SUCCESS) {
		return fail("host_statistics");
	}

	printf("system.value %lu\n", li.cpu_ticks[CPU_STATE_SYSTEM]);
	printf("user.value %lu\n",   li.cpu_ticks[CPU_STATE_USER]);
	printf("nice.value %lu\n",   li.cpu_ticks[CPU_STATE_NICE]);
	printf("idle.value %lu\n",   li.cpu_ticks[CPU_STATE_IDLE]);

	return 0;
}
