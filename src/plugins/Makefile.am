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
	p/cpu-$(SYS_API).c \
	p/df-$(DF_API).c \
	p/entropy-$(SYS_API).c \
	p/external_.c \
	p/forks-$(SYS_API).c \
	p/fw_packets-$(SYS_API).c \
	p/if_err_-$(SYS_API).c \
	p/interrupts-$(SYS_API).c \
	p/iostat-$(SYS_API).c \
	p/load-$(LOAD_API).c \
	p/memory-$(SYS_API).c \
	p/open_files-$(SYS_API).c \
	p/open_inodes-$(SYS_API).c \
	p/processes-$(SYS_API).c \
	p/swap-$(SYS_API).c \
	p/threads-$(SYS_API).c \
	p/uptime-$(SYS_API).c \
	main.c
man_MANS = munin-plugins-c.1
CLEANFILES = $(man_MANS)
EXTRA_DIST = munin-plugins-c.pod
