/*
 * Copyright (C) 2008-2013 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>
#include <unistd.h>
#include "common.h"
#include "plugins.h"

int open_files(int argc, char **argv)
{
	if (argc > 1) {
		if (!strcmp(argv[1], "config")) {
			puts("graph_title File table usage\n"
			     "graph_args --base 1000 -l 0\n"
			     "graph_vlabel number of open files\n"
			     "graph_category system\n"
			     "graph_info This graph monitors the Linux open files table.\n"
			     "used.label open files\n"
			     "used.info The number of currently open files.\n"
			     "max.label max open files\n"
			     "max.info The maximum supported number of open files.");
			return 0;
		}
		if (!strcmp(argv[1], "autoconf"))
			return writeyes();
	}

	uint32_t maxfiles;
	size_t len = sizeof(maxfiles);
	if (sysctlbyname("kern.maxfiles", &maxfiles, &len, NULL, 0) < 0) {
		return fail("sysctlbyname");
	}

	uint32_t openfiles;
	len = sizeof(openfiles);
	if (sysctlbyname("kern.openfiles", &openfiles, &len, NULL, 0) < 0) {
		return fail("sysctlbyname");
	}

	printf("used.value %lu\n"
		   "max.value %lu\n",
		   openfiles,
		   maxfiles);

	return 0;
}
