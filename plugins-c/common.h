/*
 * Copyright (C) 2008 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */
#ifndef COMMON_H
#define COMMON_H

#define PROC_STAT "/proc/stat"

int writeyes(void);
int autoconf_check_readable(const char *);
int getenvint(const char *, int);
/*@null@*/ /*@observer@*/ const char *getenv_composed(const char *,
		const char *);
void print_warning(const char *);
void print_critical(const char *);
void print_warncrit(const char *);
int fail(const char *);

#endif
