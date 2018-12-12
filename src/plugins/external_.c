/*
 * Copyright (C) 2013 Steve Schnepp <steve.schnepp@pwkg.org> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <libgen.h>


#include "common.h"
#include "plugins.h"

#ifndef LINE_MAX
  #define LINE_MAX 2048
#endif

static int read_file_to_stdout(const char *filename) {
	FILE *f;
	int c;

	if(!(f=fopen(filename, "r"))) {
		fputs("cannot open ", stderr); /* filename is not a constant */
		return fail(filename);
	}

	while ((c = fgetc(f)) != EOF) {
		fputc(c, stdout);
	}

	fclose(f);

	return 0;
}

static int set_filename(char *filename, const char* plugin_basename, const char *action) {

	if (getenv(action) == NULL) {
		/* Default */
		return snprintf(filename, LINE_MAX, "%s/%s.%s", getenv("MUNIN_PLUGSTATE"), plugin_basename, action);
	}

	return snprintf(filename, LINE_MAX, "%s", getenv(action));
}

int external_(int argc, char **argv) {
	char filename[LINE_MAX];

	/* Default is "fetch" */
	set_filename(filename, basename(argv[0]), "fetch");

	if(argc > 1) {
		if(!strcmp(argv[1], "autoconf"))
			return puts("no (not yet implemented)");

		if(!strcmp(argv[1], "config")) {
			set_filename(filename, basename(argv[0]), "config");
		}
	}

	read_file_to_stdout(filename);

	return 0;
}
