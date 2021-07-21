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
	p/cpu.c \
	p/df.c \
	p/entropy.c \
	p/external_.c \
	p/forks.c \
	p/fw_packets.c \
	p/if_err_.c \
	p/interrupts.c \
	p/iostat.c \
	p/load.c \
	p/open_files.c \
	p/open_inodes.c \
	p/processes.c \
	p/swap.c \
	p/threads.c \
	p/memory.c \
	p/uptime.c \
	main.c
man_MANS = munin-plugins-c.1
CLEANFILES = $(man_MANS)
EXTRA_DIST = munin-plugins-c.pod
