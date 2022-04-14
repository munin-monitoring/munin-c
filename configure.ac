#
# Copyright (C) 2013 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
# Copyright (C) 2013 Steve Schnepp <steve.schnepp@pwkf.org> - All rights reserved.
# Copyright (C) 2013 Diego Elio Petteno <flameeyes@flameeyes.eu> - All rights reserved.
#
# This copyrighted material is made available to anyone wishing to use,
# modify, copy, or redistribute it subject to the terms and conditions
# of the GNU General Public License v.2 or v.3.
#

AC_INIT([munin-c], [m4_esyscmd_s([./getversion])], [https://github.com/munin-monitoring/munin-c/issues], , [https://github.com/munin-monitoring/munin-c])
AC_CONFIG_AUX_DIR([build])

AM_INIT_AUTOMAKE([foreign subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_MAINTAINER_MODE([enable])

AC_PROG_CC
AC_PROG_CC_C_O

AC_PROG_LN_S

AC_CHECK_DECLS([environ])

AC_FUNC_FORK
CC_CHECK_CFLAGS_APPEND([-Wall -Wextra -Werror -pedantic -Wno-format])

AC_MSG_CHECKING([whether to enable LTO])
AC_ARG_WITH(lto,
	    [  --with-lto     enable LTO) @<:@yes@:>@],
    with_lto=$withval,
    with_lto=yes)
AC_MSG_RESULT($with_lto)
if test "$with_lto" = "yes"; then
  CC_CHECK_CFLAGS_APPEND([-flto])
  CC_CHECK_LDFLAGS_APPEND([-flto])
fi

AC_MSG_CHECKING([whether to optimize for size])
AC_ARG_WITH(optimize_size,
	    [  --with-Os     enable -Os) @<:@yes@:>@],
    with_optimize_size=$withval,
    with_optimize_size=yes)
AC_MSG_RESULT($with_optimize_size)
if test "$with_optimize_size" = "yes"; then
  CC_CHECK_CFLAGS_APPEND([-Os])
fi

AC_CHECK_HEADERS([mntent.h sys/vfs.h])

AC_MSG_CHECKING([whether to enable legacy "fetch"])
AC_ARG_WITH(legacy_fetch,
	    [  --with-legacy-fetch     enable legacy fetch which does not add "fetch") @<:@yes@:>@],
    with_legacy_fetch=$withval,
    with_legacy_fetch=yes)
AC_MSG_RESULT($with_legacy_fetch)
if test "$with_legacy_fetch" = "yes"; then
  AC_DEFINE(LEGACY_FETCH)
fi

AC_MSG_CHECKING([whether to exit on vfork errors])
AC_ARG_WITH(exit_vfork_error,
	    [  --with-exit-vfork-error  exit on vfork errors @<:@no@:>@],
    with_exit_vfork_error=$withval,
    with_exit_vfork_error=no)
AC_MSG_RESULT($with_exit_vfork_error)
if test "$with_exit_vfork_error" = "yes"; then
  AC_DEFINE(INETD_EXIT_VFORK_ERROR)
fi

AC_CONFIG_FILES([Makefile src/node/Makefile src/plugins/Makefile t/Makefile])
AC_OUTPUT
