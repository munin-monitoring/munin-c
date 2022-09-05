/*
 * Copyright (C) 2017 Bastiaan van Kesteren <bas@edeation.nl> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

#define _DARWIN_FEATURE_64_BIT_INODE

#include <stdio.h>
#include <string.h>
#include <sys/mount.h>
#include "common.h"
#include "plugins.h"

static int should_show(struct statfs mount)
{
	return !(mount.f_flags & MNT_RDONLY)
		&& strcmp(mount.f_fstypename, "autofs") != 0
		&& strcmp(mount.f_fstypename, "devfs") != 0
		&& strcmp(mount.f_mntonname, "/System/Volumes/Hardware") != 0
		&& strcmp(mount.f_mntonname, "/System/Volumes/iSCPreboot") != 0
		&& strcmp(mount.f_mntonname, "/System/Volumes/Preboot") != 0
		&& strcmp(mount.f_mntonname, "/System/Volumes/Update") != 0
		&& strcmp(mount.f_mntonname, "/System/Volumes/VM") != 0
		&& strcmp(mount.f_mntonname, "/System/Volumes/xarts") != 0;
}

static char *replace_slash(char *c)
{
	char *p = c;

	while (*p) {
		if (*p == '/') {
			*p = '_';
		}
		p++;
	}
	return c;
}

int df(int argc, char **argv)
{
	struct statfs *mounts;
	int mountCount = getmntinfo(&mounts, MNT_NOWAIT);

	if (argc > 1) {
		if (!strcmp(argv[1], "config")) {
			puts("graph_title Disk usage in percent\n"
			     "graph_args --lower-limit 0 --upper-limit 100\n"
			     "graph_vlabel %\n"
			     "graph_scale no\n"
				 "graph_category disk");

			for (int i = 0; i < mountCount; ++i) {
				if (!should_show(mounts[i]))
					continue;

				char *safeName = replace_slash(strdup(mounts[i].f_mntonname));
				printf("%s.label %s\n"
					   "%s.info %s\n",
				    safeName, mounts[i].f_mntonname,
				    safeName, mounts[i].f_fstypename);
			}

			return 0;
		}
		if (!strcmp(argv[1], "autoconf"))
			return writeyes();
	}

	for (int i = 0; i < mountCount; ++i) {
		if (!should_show(mounts[i]))
			continue;

		printf("%s.value %lf\n", replace_slash(mounts[i].f_mntonname),
			(100.0 / mounts[i].f_blocks) * (mounts[i].f_blocks - mounts[i].f_bfree));
	}

	return 0;
}
