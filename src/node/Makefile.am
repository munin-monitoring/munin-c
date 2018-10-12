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

sbin_PROGRAMS = munin-node-c munin-inetd-c
AM_CPPFLAGS = -DPLUGINDIR=\"$(sysconfdir)/munin/plugins\" \
              -DPLUGINCONFDIR=\"$(sysconfdir)/munin/plugin-conf.d\"
munin_node_c_SOURCES = node.c
munin_inetd_c_SOURCES = inetd.c
man_MANS = munin-node-c.1
CLEANFILES = $(man_MANS)
EXTRA_DIST = munin-node-c.pod
