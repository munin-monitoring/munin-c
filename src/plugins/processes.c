/*
 * Copyright (C) 2008-13 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include "common.h"
#include "plugins.h"

/* TODO: The upstream plugin does way more nowawdays. */

int processes(int argc, char **argv) {
	DIR *d;
	struct dirent *e;
	char *s;
	int n=0;
	struct stat statbuf;

	if(argc > 1) {
		if(!strcmp(argv[1], "config")) {
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
		if(!strcmp(argv[1], "autoconf")) {
			if(0 != stat("/proc/1", &statbuf)) {
				printf("no (cannot stat /proc/1, errno=%d)\n",
						errno);
				return 1;
			}
			if(!S_ISDIR(statbuf.st_mode)) {
				printf("no (/proc/1 is not a directory\n");
				return 1;
			}
			return writeyes();
		}
	}
	if(!(d = opendir("/proc")))
		return fail("cannot open /proc");
	while((e = readdir(d))) {
		for(s=e->d_name;*s;++s)
			if(!isdigit((int) *s))
				break;
		if(!*s)
			++n;
	}
	closedir(d);
	printf("processes.value %d\n", n);
	return 0;
}
