#
# Copyright (C) 2013 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
# Copyright (C) 2013 Steve Schnepp <steve.schnepp@pwkf.org> - All rights reserved.
# Copyright (C) 2013 Diego Elio Petteno <flameeyes@flameeyes.eu> - All rights reserved.
#
# This copyrighted material is made available to anyone wishing to use,
# modify, copy, or redistribute it subject to the terms and conditions
# of the GNU General Public License v.2 or v.3.
#

pkglibexecdir = $(libexecdir)/$(PACKAGE)

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src/node src/plugins t

dist_doc_DATA = gpl-2.0.txt gpl-3.0.txt
EXTRA_DIST = README.md

all-local: $(pkglibexec_PROGRAMS)
	$(MKDIR_P) plugins
	for l in `./src/plugins/munin-plugins-c$(EXEEXT) listplugins`; do \
		test -L plugins/$$l$(EXEEXT) || $(LN_S) ../src/plugins/munin-plugins-c$(EXEEXT) plugins/$$l$(EXEEXT); \
	done

clean-local:
	rm -rf plugins
