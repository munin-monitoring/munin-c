#
# Copyright (C) 2013 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
# Copyright (C) 2013 Steve Schnepp <steve.schnepp@pwkf.org> - All rights reserved.
# Copyright (C) 2013 Diego Elio Petteno <flameeyes@flameeyes.eu> - All rights reserved.
#
# This copyrighted material is made available to anyone wishing to use,
# modify, copy, or redistribute it subject to the terms and conditions
# of the GNU General Public License v.2 or v.3.
#

include $(top_srcdir)/common.am

pkglibexec_PROGRAMS = munin-plugins-c
munin_plugins_c_SOURCES = \
	common.c \
	common.h \
	plugins.h \
	cpu.c \
	df.c \
	entropy.c \
	external_.c \
	forks.c \
	fw_packets.c \
	if_err_.c \
	interrupts.c \
	iostat.c \
	load.c \
	open_files.c \
	open_inodes.c \
	processes.c \
	swap.c \
	threads.c \
	memory.c \
	uptime.c \
	main.c
man_MANS = munin-plugins-c.1
CLEANFILES = $(man_MANS)
EXTRA_DIST = munin-plugins-c.pod
