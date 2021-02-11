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
#include <unistd.h>
#include <stdbool.h>


#include "common.h"
#include "plugins.h"

#ifndef LINE_MAX
#define LINE_MAX 2048
#endif

static const char MAGIC_BOM_UTF8[] = "\xEF\xBB\xBF";

static int read_file_to_stdout(const char *filename)
{
	FILE *f;
	int c;
	bool should_remove_bom = false;
	bool should_convert_crlf = false;
	bool is_cr_retained = false;

	if (!(f = fopen(filename, "r"))) {
		fputs("cannot open ", stderr);	/* filename is not a constant */
		return fail(filename);
	}

	/* Test if we should remove the UTF-8 BOM */
	{
		char *remove_bom = getenv("remove_bom");
		if (remove_bom != NULL && !strcmp(remove_bom, "on")) {
			should_remove_bom = true;
		}
	}

	if (should_remove_bom) {
#ifdef DUMMY_BOM_HANDLING
		fseek(f, 3, SEEK_SET);
#else
		char bom_buf[3];

		/* Reading an eventual BOM */
		fread(bom_buf, 3, 1, f);

		/* Checking BOM */
		if (strncmp(bom_buf, MAGIC_BOM_UTF8, 3)) {
			/* Not a BOM, reverting to the beginning */
			fseek(f, 0, SEEK_SET);
		}
#endif
	}

	/* Test if we should convert the CRLF to LF */
	{
		char *convert_crlf = getenv("convert_crlf");
		if (convert_crlf != NULL && !strcmp(convert_crlf, "on")) {
			should_convert_crlf = true;
		}
	}

	while ((c = fgetc(f)) != EOF) {
		if (should_convert_crlf && c == '\r') {
			is_cr_retained = true;
			/* Directly get next char */
			continue;
		}

		/* Not a "\r\n", emit the missing \r */
		if (is_cr_retained && c != '\n')
			fputc('\r', stdout);
		/* is_cr_retained has been handled */
		is_cr_retained = false;

		fputc(c, stdout);
	}

	fclose(f);

	/* If the last char was \r, we should still emit it */
	if (is_cr_retained)
		fputc('\r', stdout);

	return 0;
}

static int set_filename(char *filename, const char *plugin_basename,
			const char *action)
{

	if (getenv(action) == NULL) {
		/* Default */
		return snprintf(filename, LINE_MAX, "%s/%s.%s",
				getenv("MUNIN_PLUGSTATE"), plugin_basename,
				action);
	}

	return snprintf(filename, LINE_MAX, "%s", getenv(action));
}

int external_(int argc, char **argv)
{
	char filename[LINE_MAX];
	char *action = "fetch";	/* Default is "fetch" */

	if (argc > 1) {
		if (!strcmp(argv[1], "autoconf"))
			return puts("no (not yet implemented)");

		if (!strcmp(argv[1], "config")) {
			action = "config";
		}
	}

	set_filename(filename, basename(argv[0]), action);
	read_file_to_stdout(filename);

	/* trigger on_read hook */
	{
		char hookname[LINE_MAX];
		char *on_read;

		snprintf(hookname, LINE_MAX, "on_%s", action);
		on_read = getenv(hookname);
		if (on_read == NULL || !strcmp(on_read, "nothing")) {
			/* nothing */
		} else if (!strcmp(on_read, "unlink")) {
			return unlink(filename);
		} else if (!strcmp(on_read, "truncate")) {
			return truncate(filename, 0);
		} else {
			/* Do nothing if unknown */
		}
	}

	return 0;
}
