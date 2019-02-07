/*
 * Copyright (C) 2017 Bastiaan van Kesteren <bas@edeation.nl> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#ifdef HAVE_MNTENT_H
#include <mntent.h>		/* for getmntent(), et al. */
#endif

#include <unistd.h>		/* for getopt() */
#include <sys/types.h>

#ifdef HAVE_SYS_VFS_H
#include <sys/vfs.h>
#endif

#include "common.h"

/* Defines taken from statfs(2) man page: */
#define ISOFS_SUPER_MAGIC     0x9660
#define SQUASHFS_MAGIC        0x73717368
#define UDF_SUPER_MAGIC       0x15013346
#define ROMFS_MAGIC           0x7275
#define RAMFS_MAGIC           0x858458f6
#define DEBUGFS_MAGIC         0x64626720
#define CGROUP_SUPER_MAGIC    0x27e0eb
#define DEVPTS_SUPER_MAGIC    0x1cd1


#ifndef HAVE_MNTENT_H
int df(int argc, char **argv)
{
       if (argc && argv) {
               // Do nothing, but silence the warnings
       }
       return fail("getmntent() is not supported on your system");
}
#else

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
	FILE *fp;
	struct mntent *fs;
	struct statfs vfs;

	fp = setmntent("/etc/mtab", "r");
	if (fp == NULL) {
		return fail("cannot open /etc/mtab");
	}

	if (argc > 1) {
		if (strcmp(argv[1], "config") == 0) {
			printf("graph_title Disk usage in percent\n"
			       "graph_args --upper-limit 100 -l 0\n"
			       "graph_vlabel %\n"
			       "graph_scale no\n" "graph_category disk\n");

			while ((fs = getmntent(fp)) != NULL) {
				if (fs->mnt_fsname[0] != '/') {
					continue;
				}

				if (statfs(fs->mnt_dir, &vfs) != 0) {
					continue;
				}

				if ((unsigned int) vfs.f_type ==
				    ISOFS_SUPER_MAGIC
				    || (unsigned int) vfs.f_type ==
				    SQUASHFS_MAGIC
				    || (unsigned int) vfs.f_type ==
				    UDF_SUPER_MAGIC
				    || (unsigned int) vfs.f_type ==
				    ROMFS_MAGIC
				    || (unsigned int) vfs.f_type ==
				    RAMFS_MAGIC
				    || (unsigned int) vfs.f_type ==
				    DEBUGFS_MAGIC
				    || (unsigned int) vfs.f_type ==
				    CGROUP_SUPER_MAGIC
				    || (unsigned int) vfs.f_type ==
				    DEVPTS_SUPER_MAGIC) {
					continue;
				}

				printf("%s.label %s\n",
				       replace_slash(fs->mnt_fsname),
				       fs->mnt_dir);
			}
			endmntent(fp);

			return 0;
		}
	}

	/* Asking for a fetch */
	while ((fs = getmntent(fp)) != NULL) {
		if (fs->mnt_fsname[0] != '/') {
			continue;
		}

		if (statfs(fs->mnt_dir, &vfs) != 0) {
			continue;
		}

		if ((unsigned int) vfs.f_type == ISOFS_SUPER_MAGIC ||
		    (unsigned int) vfs.f_type == SQUASHFS_MAGIC ||
		    (unsigned int) vfs.f_type == UDF_SUPER_MAGIC ||
		    (unsigned int) vfs.f_type == ROMFS_MAGIC ||
		    (unsigned int) vfs.f_type == RAMFS_MAGIC ||
		    (unsigned int) vfs.f_type == DEBUGFS_MAGIC ||
		    (unsigned int) vfs.f_type == CGROUP_SUPER_MAGIC ||
		    (unsigned int) vfs.f_type == DEVPTS_SUPER_MAGIC) {
			continue;
		}

		printf("%s.value %lf\n", replace_slash(fs->mnt_fsname),
		       (100.0 / vfs.f_blocks) * (vfs.f_blocks -
						 vfs.f_bfree));
	}
	endmntent(fp);

	return 0;
}
#endif
