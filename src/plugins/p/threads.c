/*
 * Copyright (C) 2008-2013 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

/* This plugin is compatible with munin-mainline version 2.0.17. */

#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "common.h"
#include "plugins.h"

int threads(int argc, char **argv)
{
	FILE *f;
	char buff[270];
	const char *s;
	int i, sum;
	DIR *d;
	struct dirent *e;

	if (argc > 1) {
		if (!strcmp(argv[1], "autoconf")) {
			i = getpid();
			snprintf(buff, sizeof(buff), "/proc/%d/status", i);
			if (NULL == (f = fopen(buff, "r")))
				return
				    fail("failed to open /proc/$$/status");
			while (fgets(buff, 256, f))
				if (!strncmp(buff, "Threads:", 8)) {
					fclose(f);
					return writeyes();
				}
			fclose(f);
			puts("no");
			return 0;
		}
		if (!strcmp(argv[1], "config")) {
			puts("graph_title Number of threads\n"
			     "graph_vlabel number of threads\n"
			     "graph_category processes\n"
			     "graph_info This graph shows the number of threads.\n"
			     "threads.label threads\n"
			     "threads.info The current number of threads.");
			return 0;
		}
	}
	if (NULL == (d = opendir("/proc")))
		return fail("cannot open /proc");
	sum = 0;
	while ((e = readdir(d))) {
		for (s = e->d_name; *s; ++s)
			if (!xisdigit(*s))
				break;
		if (*s)		/* non-digit found */
			continue;
		snprintf(buff, 270, "/proc/%s/status", e->d_name);
		if (!(f = fopen(buff, "r")))
			continue;	/* process has vanished */
		while (fgets(buff, 256, f)) {
			if (strncmp(buff, "Threads:", 8))
				continue;
			if (1 != sscanf(buff + 8, "%d", &i)) {
				fclose(f);
				closedir(d);
				return fail("failed to parse "
					    "/proc/somepid/status");
			}
			sum += i;
		}
		fclose(f);
	}
	closedir(d);
	printf("threads.value %d\n", sum);
	return 0;
}
