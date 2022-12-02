/*
 * Copyright (C) 2015 Steve Schnepp <steve.schnepp@pwkf.org> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include "common.h"
#include "plugins.h"

int memory(int argc, char **argv) {
	if(argc > 1) {
		if(!strcmp(argv[1], "config")) {
			puts(
				"graph_vlabel Bytes\n"
				"graph_args --base 1024 -l 0\n"
				"graph_title Memory usage\n"
				"graph_category system\n"
				"graph_info This graph shows what the machine uses memory for.\n"

				"wired.label wired\n"
				"wired.info pages that are fixed into memory for the kernel\n"
				"wired.draw AREA\n"

				// "user_wired.label user_wired\n"
				// "user_wired.info pages that are fixed into memory for user processes\n"
				// "user_wired.draw STACK\n"

				"active.label active\n"
				"active.info pages recently statistically used\n"
				"active.draw STACK\n"

				"inactive.label inactive\n"
				"inactive.info pages recently statistically unused\n"
				"inactive.draw STACK\n"

				"cached.label cache\n"
				"cached.info pages that have percolated from inactive to a status where they maintain their data, but can often be immediately reused\n"
				"cached.draw STACK\n"

				"laundry.label laundry\n"
				"laundry.info pages to be swapped out to disk\n"
				"laundry.draw STACK\n"

				"free.label free\n"
				"free.info pages without data content\n"
				"free.draw STACK\n"
			);
			return 0;
		}
		if(!strcmp(argv[1], "autoconf"))
			return writeyes();
	}

	size_t len = sizeof(u_long);
	u_long v_page_size;       if (sysctlbyname("vm.stats.vm.v_page_size",       &v_page_size,       &len, NULL, 0) < 0) return fail("sysctlbyname v_page_size");
	u_long v_wire_count;      if (sysctlbyname("vm.stats.vm.v_wire_count",      &v_wire_count,      &len, NULL, 0) < 0) return fail("sysctlbyname v_wire_count");
	// u_long v_user_wire_count; if (sysctlbyname("vm.stats.vm.v_user_wire_count", &v_user_wire_count, &len, NULL, 0) < 0) return fail("sysctlbyname v_user_wire_count");
	u_long v_active_count;    if (sysctlbyname("vm.stats.vm.v_active_count",    &v_active_count,    &len, NULL, 0) < 0) return fail("sysctlbyname v_active_count");
	u_long v_inactive_count;  if (sysctlbyname("vm.stats.vm.v_inactive_count",  &v_inactive_count,  &len, NULL, 0) < 0) return fail("sysctlbyname v_inactive_count");
	u_long v_cache_count;     if (sysctlbyname("vm.stats.vm.v_cache_count",     &v_cache_count,     &len, NULL, 0) < 0) return fail("sysctlbyname v_cache_count");
	u_long v_laundry_count;   if (sysctlbyname("vm.stats.vm.v_laundry_count",   &v_laundry_count,   &len, NULL, 0) < 0) return fail("sysctlbyname v_laundry_count");
	u_long v_free_count;      if (sysctlbyname("vm.stats.vm.v_free_count",      &v_free_count,      &len, NULL, 0) < 0) return fail("sysctlbyname v_free_count");

	printf("wired.value %u\n",      v_wire_count      * v_page_size);
	// printf("user_wired.value %u\n", v_user_wire_count * v_page_size);
	printf("active.value %u\n",     v_active_count    * v_page_size);
	printf("inactive.value %u\n",   v_inactive_count  * v_page_size);
	printf("cached.value %u\n",     v_cache_count     * v_page_size);
	printf("laundry.value %u\n",    v_laundry_count   * v_page_size);
	printf("free.value %u\n",       v_free_count      * v_page_size);

	return 0;
}
