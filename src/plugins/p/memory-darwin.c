/*
 * Copyright (C) 2015 Steve Schnepp <steve.schnepp@pwkf.org> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

#include <mach/mach_host.h>
#include <mach/vm_statistics.h>
#include <stdio.h>
#include <sys/sysctl.h>
#include "common.h"
#include "plugins.h"

int memory(int argc, char **argv) {

	if(argc > 1) {
		if(!strcmp(argv[1], "config")) {
			puts("graph_vlabel Bytes\n"
				 "graph_args --base 1024 -l 0\n"
				 "graph_title Memory usage\n"
				 "graph_category system\n"
				 "graph_info This graph shows what the machine uses memory for.\n"

				 "wired.label Wired\n"
				 "wired.draw AREA\n"
				 "wired.info Nonswappable memory used by the kernel.\n"

				 "active.label Active\n"
				 "active.draw STACK\n"
				 "active.info Recently-accessed memory.\n"

				 "inactive.label Inactive\n"
				 "inactive.draw STACK\n"
				 "inactive.info Memory likely to be purged or swapped out.\n"

				 "speculative.label Speculative\n"
				 "speculative.draw STACK\n"
				 "speculative.info File-backed/mmapped memory, speculatively read and cached.\n"

				 "other.label Other\n"
				 "other.draw STACK\n"

				 "free.label Free\n"
				 "free.draw STACK\n"
				 "free.info Available for immediate use.");

			return 0;
		}
		if(!strcmp(argv[1], "autoconf"))
			return writeyes();
	}

	uint32_t pagesize;
	size_t len = sizeof(pagesize);
	if (sysctlbyname("vm.pagesize", &pagesize, &len, NULL, 0) < 0) {
		return fail("sysctlbyname");
	}

	uint32_t totalPages;
	len = sizeof(totalPages);
	if (sysctlbyname("vm.pages", &totalPages, &len, NULL, 0) < 0) {
		return fail("sysctlbyname");
	}

	struct vm_statistics64 stats;
	host_t host = mach_host_self();
	mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
	if (host_statistics64(host, HOST_VM_INFO64, (host_info64_t)&stats, &count) != KERN_SUCCESS)  {
		return fail("host_statistics64");
	}

	printf("wired.value %lu\n", stats.wire_count * pagesize);
	printf("active.value %lu\n", stats.active_count * pagesize);
	printf("inactive.value %lu\n", stats.inactive_count * pagesize);
	printf("speculative.value %lu\n", stats.speculative_count * pagesize);

	int64_t freePages = stats.free_count - stats.speculative_count;
	printf("other.value %lu\n", MAX(0, (int64_t)totalPages - stats.wire_count - stats.active_count - stats.inactive_count - freePages) * pagesize);
	printf("free.value %lu\n", freePages * pagesize);

	return 0;
}
