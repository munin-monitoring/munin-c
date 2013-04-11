AC_INIT([munin-c], [m4_esyscmd_s([./getversion])], [https://github.com/munin-monitoring/munin-c/issues], , [https://github.com/munin-monitoring/munin-c])
AC_CONFIG_AUX_DIR([build])

AM_INIT_AUTOMAKE([foreign subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_MAINTAINER_MODE([enable])

AC_PROG_CC
AC_PROG_CC_C_O

AC_PROG_LN_S

CC_CHECK_CFLAGS_APPEND([-Wall -Wextra])

AC_CONFIG_FILES([Makefile src/node/Makefile src/plugins/Makefile])
AC_OUTPUT
