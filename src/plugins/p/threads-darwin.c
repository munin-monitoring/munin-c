/*
 * Copyright (C) 2008-2013 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

#include <libproc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>
#include <unistd.h>
#include "common.h"
#include "plugins.h"

int threads(int argc, char **argv)
{
	if (argc > 1) {
		if (!strcmp(argv[1], "config")) {
			puts("graph_title Number of threads\n"
			     "graph_vlabel number of threads\n"
			     "graph_category processes\n"
			     "graph_info This graph shows the number of threads.\n"
			     "threads.label threads\n"
			     "threads.info The current number of threads.");
			return 0;
		}
		if(!strcmp(argv[1], "autoconf"))
			return writeyes();
	}

	uint32_t maxproc;
	size_t len = sizeof(maxproc);
	if (sysctlbyname("kern.maxproc", &maxproc, &len, NULL, 0) < 0) {
		return fail("sysctlbyname");
	}

	uint64_t sum = 0;
	size_t pidBufferSize = sizeof(pid_t) * maxproc;
	pid_t *pids = malloc(pidBufferSize);
	uint32_t pidCount = proc_listallpids(pids, pidBufferSize);

	for (uint32_t i = 0; i < pidCount; ++i) {
		struct proc_taskinfo pti;
		if (proc_pidinfo(pids[i], PROC_PIDTASKINFO, 0, &pti, sizeof(pti)) < 0) {
			return fail("proc_pidinfo");
		}
		sum += pti.pti_threadnum;
	}

	printf("threads.value %llu\n", sum);

	return 0;
}
