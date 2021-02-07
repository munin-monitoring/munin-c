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
#include <stdbool.h>
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

struct meminfo_pair {
	char *key;
	char *label;
	char *draw;
	char *info;
	int colour;
	int_fast64_t value;
	bool exists;
};

struct meminfo_pair meminfo[] = {
	{
	 .key = "Shmem",
	 .label = "shmem",
	 .draw = "STACK",
	 .info = "Shared Memory (SYSV SHM segments, tmpfs).",
	 .colour = 9,
	 },
	{
	 .key = "Slab",
	 .label = "slab_cache",
	 .draw = "STACK",
	 .info =
	 "Memory used by the kernel (major users are caches like inode, "
	 "dentry, etc).",
	 .colour = 3,
	 },
	{
	 .key = "SwapCached",
	 .label = "swap_cache",
	 .draw = "STACK",
	 .info =
	 "A piece of memory that keeps track of pages that have been "
	 "fetched from swap but not yet been modified.",
	 .colour = 2,
	 },
	{
	 .key = "PageTables",
	 .label = "page_tables",
	 .draw = "STACK",
	 .info =
	 "Memory used to map between virtual and physical memory addresses.",
	 .colour = 1,
	 },
	{
	 .key = "VmallocUsed",
	 .label = "vmalloc_used",
	 .draw = "LINE2",
	 .info = "'VMalloc' (kernel) memory used.",
	 .colour = 8,
	 },
	{
	 .key = "Committed_AS",
	 .label = "committed",
	 .draw = "LINE2",
	 .info =
	 "The amount of memory allocated to programs. Overcommitting is "
	 "normal, but may indicate memory leaks.",
	 .colour = 10,
	 },
	{
	 .key = "Mapped",
	 .label = "mapped",
	 .draw = "LINE2",
	 .info = "All mmap()ed pages.",
	 .colour = 11,
	 },
	{
	 .key = "Active",
	 .label = "active",
	 .draw = "LINE2",
	 .info =
	 "Memory recently used. Not reclaimed unless absolutely necessary.",
	 .colour = 12,
	 },
	{
	 .key = "ActiveAnon",
	 .label = "active_anon",
	 .draw = "LINE1",
	 .colour = 13,
	 },
	{
	 .key = "ActiveCache",
	 .label = "active_cache",
	 .draw = "LINE1",
	 .colour = 14,
	 },
	{
	 .key = "Inactive",
	 .label = "inactive",
	 .draw = "LINE2",
	 .info = "Memory not currently used.",
	 .colour = 15,
	 },
	{
	 .key = "Inact_dirty",
	 .label = "inactive_dirty",
	 .draw = "LINE1",
	 .info =
	 "Memory not currently used, but in need of being written to disk.",
	 .colour = 16,
	 },
	{
	 .key = "Inact_laundry",
	 .label = "inactive_laundry",
	 .draw = "LINE1",
	 .colour = 17,
	 },
	{
	 .key = "Inact_clean",
	 .label = "inactive_clean",
	 .draw = "LINE1",
	 .info = "Memory not currently used.",
	 .colour = 18,
	 },
	{
	 .key = "KSM",
	 .label = "ksm_sharing",
	 .draw = "LINE2",
	 .info = "Memory saved by KSM sharing.",
	 .colour = 19,
	 },
	// Fields that we do not report directly, but still care about parsing from
	// /proc/meminfo.
	{.key = "MemTotal"},
	{.key = "MemFree"},
	{.key = "SwapTotal"},
	{.key = "SwapFree"},
	{.key = "Buffers"},
	{.key = "Cached"},
	// Sentinel field.
	{.key = NULL}
};

struct meminfo_pair *get_meminfo_key(char *key)
{
	for (struct meminfo_pair * info = meminfo; info->key; info++) {
		if (!strcmp(info->key, key)) {
			return info;
		}
	}

	return NULL;
}

int_fast64_t get_meminfo_value(char *key)
{
	struct meminfo_pair *info = get_meminfo_key(key);
	return info && info->exists ? info->value : -1;
}

void parse_meminfo(void)
{
	FILE *f;
	char buff[256];

	/* Asking for a fetch */
	if (!(f = fopen(PROC_MEMINFO, "r")))
		exit(fail("cannot open " PROC_MEMINFO));

	while (fgets(buff, 256, f)) {
		char key[256];
		char *colon;
		int_fast64_t value;
		if (!sscanf(buff, "%s %" SCNdFAST64, key, &value) ||
		    !(colon = strstr(key, ":"))) {
			fclose(f);
			exit(fail("cannot parse " PROC_MEMINFO " line"));
		}

		*colon = '\0';

		struct meminfo_pair *info = get_meminfo_key(key);
		if (info) {
			info->exists = true;
			info->value = value * 1024;
		}
	}

	fclose(f);
}

int memory(int argc, char **argv)
{
	parse_meminfo();

	if (argc > 1) {
		if (!strcmp(argv[1], "config")) {
			printf("graph_args --base 1024 -l 0\n"
			       "graph_vlabel Bytes\n"
			       "graph_title Memory usage\n"
			       "graph_category system\n"
			       "graph_info This graph shows what the machine uses memory for.\n");
			printf("apps.label apps\n");
			printf("apps.draw AREA\n");
			printf
			    ("apps.info Memory used by user-space applications.\n");

			printf("free.label free\n");
			printf("free.draw STACK\n");
			printf
			    ("free.info Wasted memory. Memory that is not used for anything at all.\n");

			printf("swap.label swap\n");
			printf("swap.draw STACK\n");
			printf("swap.info Swap space used.\n");

			printf("buffers.label buffers\n");
			printf("buffers.draw STACK\n");
			printf
			    ("buffers.info Block device (e.g. harddisk) cache. "
			     "Also where \"dirty\" blocks are stored until written.\n");
			printf("buffers.colour COLOUR5\n");

			printf("cached.label cache\n");
			printf("cached.draw STACK\n");
			printf
			    ("cached.info Parked file data (file content) cache.\n");
			printf("cached.colour COLOUR4\n");

			for (struct meminfo_pair * info = meminfo;
			     info->key; info++) {
				if (!info->exists || !info->label)
					continue;

				printf("%s.label %s\n", info->label,
				       info->label);
				printf("%s.draw %s\n", info->label,
				       info->draw);
				if (info->info)
					printf("%s.info %s\n", info->label,
					       info->info);
				printf("%s.colour COLOUR%d\n", info->label,
				       info->colour);
			}

			return 0;
		}

		if (!strcmp(argv[1], "autoconf"))
			return autoconf_check_readable(PROC_MEMINFO);
	}

	printf("apps.value %" PRIdFAST64 "\n",
	       get_meminfo_value("MemTotal") -
	       get_meminfo_value("MemFree") -
	       get_meminfo_value("Buffers") -
	       get_meminfo_value("Cached") -
	       get_meminfo_value("Slab") -
	       get_meminfo_value("PageTables") -
	       get_meminfo_value("SwapCached"));
	printf("free.value %" PRIdFAST64 "\n",
	       get_meminfo_value("MemFree"));
	printf("buffers.value %" PRIdFAST64 "\n",
	       get_meminfo_value("Buffers"));
	printf("cached.value %" PRIdFAST64 "\n",
	       get_meminfo_value("Cached"));
	printf("swap.value %" PRIdFAST64 "\n",
	       get_meminfo_value("SwapTotal") -
	       get_meminfo_value("SwapFree"));

	for (struct meminfo_pair * info = meminfo; info->key; info++) {
		if (info->exists && info->label)
			printf("%s.value %" PRIdFAST64 "\n", info->label,
			       info->value);
	}

	return 0;
}
