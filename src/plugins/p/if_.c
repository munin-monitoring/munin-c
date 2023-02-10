/*
 * Copyright (C) 2008-2013 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 * Copyright (C) 2023 Kirill Ovchinnikov <kirill.ovchinn@gmail.com> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */
#include <ctype.h>
#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#define SYS_CLASS_NET "/sys/class/net/"
#define TMPBUFSIZE 256

int if_(int argc, char **argv)
{
	char *interface;
	FILE *f;
	char tmpbuf[TMPBUFSIZE];

	interface = basename(argv[0]);
	if (strncmp(interface, "if_", 3) != 0)
		return fail("if_ invoked with invalid basename");
	interface += 3;

	if (argc > 1) {
		if (!strcmp(argv[1], "autoconf")) {
			puts("yes");
			return 0;
		}
		if (!strcmp(argv[1], "suggest")) {
			struct dirent *dent;
			DIR *ifdir;
			if (NULL == (ifdir = opendir(SYS_CLASS_NET)))
				return 1;
			while ((dent = readdir(ifdir)) != NULL) {
				if (strcmp(dent->d_name, ".") == 0 ||
				    strcmp(dent->d_name, "..") == 0)
					continue;
				if (strcmp(dent->d_name, "lo") == 0)
					continue;
				// skip docker and bridge interfaces by default
				// user can add them manually if needed
				if (strncmp(dent->d_name, "docker", 6) == 0
				    || strncmp(dent->d_name, "br", 2) == 0)
					continue;
				puts(dent->d_name);
			}
			closedir(ifdir);
			return 0;
		}
		if (!strcmp(argv[1], "config")) {
			puts("graph_order down up");
			printf("graph_title %s traffic\n", interface);
			puts("graph_args --base 1000\n"
			     "graph_vlabel bits in (-) / out (+) per "
			     "${graph_period}\n" "graph_category network");
			printf("graph_info This graph shows the amount of "
			       "traffic on the %s network interface.\n",
			       interface);
			puts("down.label received\n"
			     "down.type DERIVE\n"
			     "down.graph no\n"
			     "down.cdef down,8,*\n"
			     "down.min 0\n"
			     "up.label bps\n"
			     "up.type DERIVE\n"
			     "up.cdef up,8,*\n"
			     "up.min 0\n" "up.negative down\n");
			print_warncrit("up");
			print_warncrit("down");
			return 0;
		}
	}
	snprintf(tmpbuf, TMPBUFSIZE, "%s/%s/statistics/tx_bytes",
		 SYS_CLASS_NET, interface);
	if (NULL == (f = fopen(tmpbuf, "r")))
		return 1;
	if (NULL == fgets(tmpbuf, TMPBUFSIZE, f))
		return 1;
	printf("up.value %s", tmpbuf);
	fclose(f);
	snprintf(tmpbuf, 255, "%s/%s/statistics/rx_bytes", SYS_CLASS_NET,
		 interface);
	if (NULL == (f = fopen(tmpbuf, "r")))
		return 1;
	if (NULL == fgets(tmpbuf, TMPBUFSIZE, f))
		return 1;
	printf("down.value %s", tmpbuf);
	fclose(f);
	return 0;
}
