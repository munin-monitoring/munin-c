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
#include "common.h"
#include "plugins.h"

#define ENTROPY_AVAIL "/proc/sys/kernel/random/entropy_avail"

int entropy(int argc, char **argv)
{
	FILE *f;
	int entropy;
	if (argc > 1) {
		if (!strcmp(argv[1], "config")) {
			puts("graph_title Available entropy\n"
			     "graph_args --base 1000 -l 0\n"
			     "graph_vlabel entropy (bytes)\n"
			     "graph_scale no\n"
			     "graph_category system\n"
			     "graph_info This graph shows the amount of entropy available in the system.\n"
			     "entropy.label entropy\n"
			     "entropy.info The number of random bytes available. This is typically used by cryptographic applications.");
			print_warncrit("entropy");
			return 0;
		}
		if (!strcmp(argv[1], "autoconf"))
			return autoconf_check_readable(ENTROPY_AVAIL);
	}
	if (!(f = fopen(ENTROPY_AVAIL, "r")))
		return fail("cannot open " ENTROPY_AVAIL);
	if (1 != fscanf(f, "%d", &entropy)) {
		fclose(f);
		return fail("cannot read from " ENTROPY_AVAIL);
	}
	fclose(f);
	printf("entropy.value %d\n", entropy);
	return 0;
}
