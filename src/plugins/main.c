/*
 * Copyright (C) 2008-2013 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 * Copyright (C) 2013 Steve Schnepp <steve.schnepp@pwkf.org> - All rights reserved.
 * Copyright (C) 2013 Diego Elio Petteno <flameeyes@flameeyes.eu> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */
#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include "common.h"
#include "plugins.h"

static int busybox(int argc, char **argv) {
	if(argc < 2)
		return fail("missing parameter");
	if(0 != strcmp(argv[1], "listplugins"))
		return fail("unknown parameter");

	/* The following is focused on readability over efficiency. */
	puts("cpu");
	puts("entropy");
	puts("forks");
	puts("fw_packets");
	puts("interrupts");
	puts("load");
	puts("open_files");
	puts("open_inodes");
	puts("swap");
	puts("threads");
	puts("uptime");
	return 0;
}

int main(int argc, char **argv) {
	char *progname;
	char *ext;
	progname = basename(argv[0]);
	ext = strrchr(progname, '.');
	if (ext != NULL) ext[0] = '\0';
	switch(*progname) {
		case 'c':
			if(!strcmp(progname, "cpu"))
				return cpu(argc, argv);
			break;
		case 'e':
			if(!strcmp(progname, "entropy"))
				return entropy(argc, argv);
			if(!strcmp(progname, "external_"))
				return external_(argc, argv);
			break;
		case 'f':
			if(!strcmp(progname, "forks"))
				return forks(argc, argv);
			if(!strcmp(progname, "fw_packets"))
				return fw_packets(argc, argv);
			break;
		case 'i':
			if(!strcmp(progname, "interrupts"))
				return interrupts(argc, argv);
			if(!strncmp(progname, "if_err_", strlen("if_err_")))
				return if_err_(argc, argv);
			break;
		case 'l':
			if(!strcmp(progname, "load"))
				return load(argc, argv);
			break;
		case 'm':
			if(!strcmp(progname, "memory"))
				return memory(argc, argv);
			if(!strcmp(progname, "munin-plugins-c"))
				return busybox(argc, argv);
			break;
		case 'o':
			if(!strcmp(progname, "open_files"))
				return open_files(argc, argv);
			if(!strcmp(progname, "open_inodes"))
				return open_inodes(argc, argv);
			break;
		case 'p':
			if(!strcmp(progname, "processes"))
				return processes(argc, argv);
			break;
		case 's':
			if(!strcmp(progname, "swap"))
				return swap(argc, argv);
			break;
		case 't':
			if(!strcmp(progname, "threads"))
				return threads(argc, argv);
			break;
		case 'u':
			if(!strcmp(progname, "uptime"))
				return uptime(argc, argv);
			break;
	}
	return fail("unknown basename");
}
