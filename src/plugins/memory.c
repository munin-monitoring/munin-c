/*
 * Copyright (C) 2013 Steve Schnepp <steve.schnepp@pwkf.org> - All rights reserved.
 * Copyright (C) 2013 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include "common.h"
#include "plugins.h"

#define SYSWARNING 30
#define SYSCRITICAL 50
#define USRWARNING 80

#define PROC_MEMINFO "/proc/meminfo"

/* Samples "/proc/meminfo"

Linux 2.6.32:
MemTotal:         121484 kB
MemFree:           16036 kB
Buffers:           13260 kB
Cached:            35432 kB
SwapCached:        13600 kB
Active:            43160 kB
Inactive:          43920 kB
Active(anon):      12616 kB
Inactive(anon):    26096 kB
Active(file):      30544 kB
Inactive(file):    17824 kB
Unevictable:           0 kB
Mlocked:               0 kB
SwapTotal:        262136 kB
SwapFree:         215608 kB
Dirty:                92 kB
Writeback:             0 kB
AnonPages:         29468 kB
Mapped:             6728 kB
Shmem:               308 kB
Slab:              12024 kB
SReclaimable:       6708 kB
SUnreclaim:         5316 kB
KernelStack:         680 kB
PageTables:         2116 kB
NFS_Unstable:          0 kB
Bounce:                0 kB
WritebackTmp:          0 kB
CommitLimit:      322876 kB
Committed_AS:     108564 kB
VmallocTotal:   34359738367 kB
VmallocUsed:        1196 kB
VmallocChunk:   34359737024 kB
HardwareCorrupted:     0 kB
HugePages_Total:       0
HugePages_Free:        0
HugePages_Rsvd:        0
HugePages_Surp:        0
Hugepagesize:       2048 kB
DirectMap4k:      131072 kB
DirectMap2M:           0 kB


Cygwin:
MemTotal:     121484 kB
MemFree:       16036 kB
HighTotal:         0 kB
HighFree:          0 kB
LowFree:       16036 kB
SwapTotal:    262136 kB
SwapFree:     215608 kB
}

*/

/* TODO - For now we only support Cygwin fields. */
int memory(int argc, char **argv) {
	FILE *f;
	char buff[256];

	int_fast64_t mem_total = -1;
	int_fast64_t mem_free  = -1;
	int_fast64_t swap_total = -1;
	int_fast64_t swap_free  = -1;

	if(argc > 1) {
		if(!strcmp(argv[1], "config")) {
			printf(
				"graph_args --base 1024 -l 0\n"
				"graph_vlabel Bytes\n"
				"graph_title Memory usage\n"
				"graph_category system\n"
				"graph_info This graph shows what the machine uses memory for.\n"
			);
			printf("apps.label apps\n");
			printf("apps.draw AREA\n");
			printf("apps.info Memory used by user-space applications.\n");

			printf("free.label free\n");
			printf("free.draw STACK\n");
			printf("free.info Wasted memory. Memory that is not used for anything at all.\n");

			printf("swap.label swap\n");
			printf("swap.draw STACK\n");
			printf("swap.info Swap space used.\n");

			return 0;
		}
		if(!strcmp(argv[1], "autoconf"))
			return autoconf_check_readable(PROC_MEMINFO);
	}

	/* Asking for a fetch */
	if(!(f=fopen(PROC_MEMINFO, "r")))
		return fail("cannot open " PROC_MEMINFO);

	while(fgets(buff, 256, f)) {
		char key[256];
		int_fast64_t value;
		if (!sscanf(buff, "%s %" SCNdFAST64, key, &value)) {
			fclose(f);
			return fail("cannot parse " PROC_MEMINFO " line");
		}

		if(!strcmp(key, "MemTotal:"))
			mem_total = value * 1024;
		else if(!strcmp(key, "MemFree:"))
			mem_free = value * 1024;
		else if(!strcmp(key, "SwapTotal:"))
			swap_total = value * 1024;
		else if(!strcmp(key, "SwapFree:"))
			swap_free = value * 1024;
	}
	fclose(f);
	if(mem_total < 0 || mem_free < 0 || swap_total < 0 || swap_free < 0)
		return fail("missing fileds in " PROC_MEMINFO);

	printf("apps.value %" PRIdFAST64 "\n", mem_total - mem_free);
	printf("free.value %" PRIdFAST64 "\n", mem_free);
	printf("swap.value %" PRIdFAST64 "\n", swap_total - swap_free);
	return 0;
}
