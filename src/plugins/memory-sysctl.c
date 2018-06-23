/*
 * Copyright (C) 2015 Steve Schnepp <steve.schnepp@pwkf.org> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

/* This plugin is compatible with munin-mainline version 2.0.25. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include "common.h"
#include "plugins.h"

#include <sys/vmmeter.h>

int memory(int argc, char **argv) {

	if(argc > 1) {
		if(!strcmp(argv[1], "config")) {
			printf(
				"graph_vlabel Bytes\n"
				"graph_args --base 1024 -l 0\n"
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


if [ "$1" = "config" ]; then
    echo 'graph_args --base 1024 -l 0 --vertical-label Bytes --upper-limit' $MEMMAX
    echo 'graph_title Memory usage'
    echo 'graph_category system'
    echo 'graph_info This graph shows what the machine uses its memory for.'
    echo 'graph_order active inactive wired cached free swap buffers'
    echo 'active.label active'
    echo 'active.info pages recently statistically used'
    echo 'active.draw AREA'
    echo 'inactive.label inactive'
    echo 'inactive.info pages recently statistically unused'
    echo 'inactive.draw STACK'
    echo 'wired.label wired'
    echo 'wired.info pages that are fixed into memory, usually for kernel purposes, but also sometimes for special use in processes'
    echo 'wired.draw STACK'
    echo 'cached.label cache'
    echo 'cached.info pages that have percolated from inactive to a status where they maintain their data, but can often be immediately reused'
    echo 'cached.draw STACK'
    echo 'free.label free'
    echo 'free.info pages without data content'
    echo 'free.draw STACK'
    echo 'swap.label swap'
    echo 'swap.info Swap space used'
    echo 'swap.draw STACK'
    echo 'buffers.label buffers'
    echo 'buffers.info pages used for filesystem buffers'
    echo 'buffers.draw LINE'
    exit 0
fi










			return 0;
		}
		if(!strcmp(argv[1], "autoconf"))
			return writeyes();
	}

	struct vmtotal s;
	size_t len = sizeof(s);

	if (sysctlbyname("vm.total", &s, &len, NULL, 0) < 0)  {
		return fail("sysctl");
	}

	double ldavg = l.ldavg[0];
	double fscale = l.fscale;

	printf("# vm.loadavg, ldavg:%f fscale:%f -", ldavg, fscale);
	printf("\n");

	double load_1 = ldavg / fscale;
	printf("load.value %.2f\n", load_1);
	return 0;
}
