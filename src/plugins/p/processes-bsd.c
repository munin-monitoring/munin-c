/*
 * Copyright (C) 2008-13 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

#include <kvm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>
#include <unistd.h>
#include "common.h"
#include "plugins.h"

/* TODO: The upstream plugin does way more nowawdays. */

int processes(int argc, char **argv)
{
	if (argc > 1) {
		if (!strcmp(argv[1], "config")) {
			puts("graph_title Number of Processes\n"
			     "graph_args --base 1000 -l 0 \n"
			     "graph_vlabel number of processes\n"
			     "graph_category processes\n"
			     "graph_info This graph shows the number of processes in the system.\n"
			     "processes.label processes\n"
			     "processes.draw LINE2\n"
			     "processes.info The current number of processes.");
			return 0;
		}
		if (!strcmp(argv[1], "autoconf"))
			return writeyes();
	}

	kvm_t *kd = kvm_open(NULL, NULL, NULL, 0, NULL);
	if (!kd)
		return 1;

	int processCount = 0;
	kvm_getprocs(kd, KERN_PROC_PROC, 0, &processCount);
	printf("processes.value %u\n", processCount);

	return 0;
}
