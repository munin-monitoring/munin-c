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
		if (!strcmp(argv[1], "autoconf"))
			return writeyes();
	}

	kvm_t *kd = kvm_open(NULL, NULL, NULL, 0, NULL);
	if (!kd)
		return 1;

	int processAndThreadCount = 0;
	kvm_getprocs(kd, KERN_PROC_ALL, 0, &processAndThreadCount);

	int processCount = 0;
	kvm_getprocs(kd, KERN_PROC_PROC, 0, &processCount);

	printf("threads.value %u\n", processAndThreadCount - processCount);

	return 0;
}
