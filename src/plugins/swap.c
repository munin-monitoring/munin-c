/*
 * Copyright (C) 2008-13 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "common.h"
#include "plugins.h"

#define PROC_VMSTAT "/proc/vmstat"

int swap(int argc, char **argv) {
	FILE *f;
	char buff[256];
	bool in, out;
	if(argc > 1) {
		if(!strcmp(argv[1], "config")) {
			puts("graph_title Swap in/out\n"
				"graph_args -l 0 --base 1000\n"
				"graph_vlabel pages per ${graph_period} in (-) / out (+)\n"
				"graph_category system\n"
				"swap_in.label swap\n"
				"swap_in.type DERIVE\n"
				"swap_in.max 100000\n"
				"swap_in.min 0\n"
				"swap_in.graph no\n"
				"swap_out.label swap\n"
				"swap_out.type DERIVE\n"
				"swap_out.max 100000\n"
				"swap_out.min 0\n"
				"swap_out.negative swap_in");
			print_warncrit("swap_in");
			print_warncrit("swap_out");
			return 0;
		}
		if(!strcmp(argv[1], "autoconf"))
			return autoconf_check_readable(PROC_STAT);
	}
	if(!access(PROC_VMSTAT, F_OK)) {
		in = out = false;
		if(!(f=fopen(PROC_VMSTAT, "r")))
			return fail("cannot open " PROC_VMSTAT);
		while(fgets(buff, 256, f)) {
			if(!in && !strncmp(buff, "pswpin ", 7)) {
				in = true;
				printf("swap_in.value %s", buff+7);
			}
			else if(!out && !strncmp(buff, "pswpout ", 8)) {
				out = true;
				printf("swap_out.value %s", buff+8);
			}
		}
		fclose(f);
		if(!(in && out))
			return fail("no usable data on " PROC_VMSTAT);
		return 0;
	} else {
		if(!(f=fopen(PROC_STAT, "r")))
			return fail("cannot open " PROC_STAT);
		while(fgets(buff, 256, f)) {
			if(!strncmp(buff, "swap ", 5)) {
				fclose(f);
				if(2 != sscanf(buff+5, "%d %d", &in, &out))
					return fail("bad data on " PROC_STAT);
				printf("swap_in.value %d\nswap_out.value %d\n", in, out);
				return 0;
			}
		}
		fclose(f);
		return fail("no swap line found in " PROC_STAT);
	}
}
