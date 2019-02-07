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

AC_FUNC_FORK
CC_CHECK_CFLAGS_APPEND([-Wall -Wextra -Werror -pedantic -Wno-format])

AC_CHECK_HEADERS([mntent.h sys/vfs.h])

AC_CONFIG_FILES([Makefile src/node/Makefile src/plugins/Makefile t/Makefile])
AC_OUTPUT
