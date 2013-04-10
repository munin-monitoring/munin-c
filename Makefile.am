pkglibexecdir = $(libexecdir)/$(PACKAGE)

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src/node src/plugins

dist_doc_DATA = gpl-2.0.txt gpl-3.0.txt

all-local: $(pkglibexec_PROGRAMS)
	$(MKDIR_P) plugins
	for l in `./src/plugins/munin-plugins-c listplugins`; do \
		test -L plugins/$$l || $(LN_S) ../src/plugins/munin-plugins-c plugins/$$l; \
	done

clean-local:
	rm -rf plugins

install-exec-hook:
	for l in $(LINKS); do \
		test -f $(DESTDIR)$(pkglibexecdir)$$l || $(LN_S) munin-plugins-c $(DESTDIR)$(pkglibexecdir)/$$l; \
	done
