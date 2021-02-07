/*
 * Copyright (C) 2018 Michal Sojka <wsh@2x.cz> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "plugins.h"

#define PROC_DISKSTAT "/proc/diskstats"

#define XSTR(x) #x
#define STR(x) XSTR(x)
#define NAME_SIZE 16

struct dev {
	struct dev *next;
	uint8_t major;
	char key[10];
	char name[NAME_SIZE + 1];
	unsigned long rsect;
	unsigned long wsect;
};

static bool is_numbered(struct dev *dev)
{
	char *tail = dev->name + strlen(dev->name) - 1;
	if (strncmp(dev->name, "mmcblk", 6) == 0) {
		/* Skip mmcblkXpY, mmcblkXbootY, etc but not mmcblkX */
		while (tail >= dev->name && *tail >= '0' && *tail <= '9')
			tail--;
		if (tail - dev->name > 6)
			return true;
	} else {
		if (tail >= dev->name && isdigit(*tail))
			return true;
	}
	return false;
}

int iostat(int argc, char **argv)
{
	/* TODO: char *include_only = getenv("include_only"); */
	bool include_numbered = getenv("SHOW_NUMBERED") != NULL;	/* By default we want sda but not sda1 */

	FILE *f;
	struct dev *devs = NULL, *devs_end = NULL;
	unsigned dev_cnt = 0;
	struct dev *dev;

	if (!(f = fopen(PROC_DISKSTAT, "r")))
		return fail("cannot open " PROC_DISKSTAT);

	while (!feof(f)) {
		unsigned cnt = 0;
		struct dev *d;

		dev = alloca(sizeof(*dev));
		dev->next = NULL;

		if (4 != fscanf(f, "%hhu %*hhu %" STR(NAME_SIZE)
				"s %*lu %*lu %lu %*lu %*lu %*lu %lu%*[^\n]",
				&dev->major, dev->name, &dev->rsect,
				&dev->wsect))
			continue;

		if (!include_numbered && is_numbered(dev))
			continue;

		if (dev->rsect == 0 && dev->wsect == 0)
			continue;

		for (d = devs; d; d = d->next)
			if (dev->major == d->major)
				cnt++;
		snprintf(dev->key, sizeof(dev->key), "dev%d_%u",
			 dev->major, cnt);

		dev_cnt++;
		if (!devs) {
			devs = devs_end = dev;
		} else {
			devs_end->next = dev;
			devs_end = dev;
		}
	}
	fclose(f);

	if (argc > 1) {
		if (!strcmp(argv[1], "config")) {
			puts("graph_title IOstat\n"
			     "graph_args --base 1024\n"
			     "graph_vlabel blocks per ${graph_period} read (-) / written (+)\n"
			     "graph_category disk");
			if (dev_cnt > 1)
				puts("graph_total Total");
			puts("graph_info This graph shows the I/O to and from block devices.");
			printf("graph_order");
			for (dev = devs; dev; dev = dev->next)
				printf(" %s_read %s_write ", dev->key,
				       dev->key);
			printf("\n");
			for (dev = devs; dev; dev = dev->next) {
				char graph_name[128];

				printf("%s_read.label %s\n", dev->key,
				       dev->name);
				printf("%s_read.type DERIVE\n", dev->key);
				printf("%s_read.min 0\n", dev->key);
				printf("%s_read.graph no\n", dev->key);
				printf("%s_write.label %s\n", dev->key,
				       dev->name);
				printf("%s_write.info I/O on device %s\n",
				       dev->key, dev->name);
				printf("%s_write.type DERIVE\n", dev->key);
				printf("%s_write.min 0\n", dev->key);
				printf("%s_write.negative %s_read\n",
				       dev->key, dev->key);

				snprintf(graph_name, sizeof(graph_name),
					 "%s_read", dev->key);
				print_warncrit(graph_name);

				snprintf(graph_name, sizeof(graph_name),
					 "%s_write", dev->key);
				print_warncrit(graph_name);
			}

			return 0;
		}
		if (!strcmp(argv[1], "autoconf"))
			return writeyes();
	}
	for (dev = devs; dev; dev = dev->next) {
		printf("%s_read.value %lu\n", dev->key, dev->rsect);
		printf("%s_write.value %lu\n", dev->key, dev->wsect);
	}
	return 0;
}
