/*
 * Copyright (C) 2008-2013 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include "common.h"
#include "plugins.h"

#define PROC_NET_SNMP "/proc/net/snmp"

int fw_packets(int argc, char **argv)
{
	FILE *f;
	char buff[1024], *s;
	int ret;
	if (argc > 1) {
		if (!strcmp(argv[1], "config")) {
			puts("graph_title Firewall Throughput\n"
			     "graph_args --base 1000 -l 0\n"
			     "graph_vlabel Packets/${graph_period}\n"
			     "graph_category network\n"
			     "received.label Received\n"
			     "received.draw AREA\n"
			     "received.type DERIVE\n"
			     "received.min 0\n"
			     "forwarded.label Forwarded\n"
			     "forwarded.draw LINE2\n"
			     "forwarded.type DERIVE\n" "forwarded.min 0");
			return 0;
		}
		if (!strcmp(argv[1], "autoconf"))
			return autoconf_check_readable(PROC_NET_SNMP);
	}
	if (!(f = fopen(PROC_NET_SNMP, "r")))
		return fail("cannot open " PROC_NET_SNMP);
	while (fgets(buff, 1024, f)) {
		if (!strncmp(buff, "Ip: ", 4) && xisdigit(buff[4])) {
			if (!(s = strtok(buff + 4, " \t")))
				break;
			if (!(s = strtok(NULL, " \t")))
				break;
			if (!(s = strtok(NULL, " \t")))
				break;
			printf("received.value %s\n", s);
			if (!(s = strtok(NULL, " \t")))
				break;
			if (!(s = strtok(NULL, " \t")))
				break;
			if (!(s = strtok(NULL, " \t")))
				break;
			printf("forwarded.value %s\n", s);
			ret = 0;
			goto OK;
		}
	}
	ret = fail("no ip line found in " PROC_NET_SNMP);
      OK:
	fclose(f);
	return ret;
}
