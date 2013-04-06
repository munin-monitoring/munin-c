pkglibexecdir = $(libexecdir)/$(PACKAGE)

SUBDIRS = src/node src/plugins

LINKS = cpu entropy forks fw_packets interrupts if_err_eth0 load	\
      open_files open_inodes processes swap threads uptime memory
CFLAGS += -W -Wall -Wextra

dist_doc_DATA = gpl-2.0.txt gpl-3.0.txt

all-local: $(pkglibexec_PROGRAMS)
	$(MKDIR_P) plugins
	for l in $(LINKS); do \
		test -L plugins/$$l || $(LN_S) ../src/plugins/munin-plugins-c plugins/$$l; \
	done

clean-local:
	rm -rf plugins

install-exec-hook:
	for l in $(LINKS); do \
		test -f $(DESTDIR)$(pkglibexecdir)$$l || $(LN_S) munin-plugins-c $(DESTDIR)$(pkglibexecdir)/$$l; \
	done
