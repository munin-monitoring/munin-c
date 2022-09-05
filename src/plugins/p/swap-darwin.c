/*
 * Copyright (C) 2008-13 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

#include <mach/mach_host.h>
#include <mach/vm_statistics.h>
#include <stdio.h>
#include "common.h"
#include "plugins.h"

int swap(int argc, char **argv)
{
	if (argc > 1) {
		if (!strcmp(argv[1], "config")) {
			puts("graph_title Swap in/out\n"
			     "graph_args -l 0 --base 1000\n"
			     "graph_vlabel pages per ${graph_period} in (-) / out (+)\n"
			     "graph_category system\n"
			     "swap_in.label swap\n"
			     "swap_in.type DERIVE\n"
			     "swap_in.max 100000\n"
			     "swap_in.min 0\n"
			     "swap_in.graph no\n"
			     "swap_out.label swap\n"
			     "swap_out.type DERIVE\n"
			     "swap_out.max 100000\n"
			     "swap_out.min 0\n"
			     "swap_out.negative swap_in");
			print_warncrit("swap_in");
			print_warncrit("swap_out");
			return 0;
		}
		if (!strcmp(argv[1], "autoconf"))
			return writeyes();
	}

	struct vm_statistics64 stats;
	host_t host = mach_host_self();
	mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
	if (host_statistics64(host, HOST_VM_INFO64, (host_info64_t)&stats, &count) != KERN_SUCCESS)  {
		return fail("host_statistics64");
	}

	printf("swap_in.value %lu\n", stats.swapins);
	printf("swap_out.value %lu\n", stats.swapouts);

	return 0;
}
