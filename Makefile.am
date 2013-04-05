sbin_PROGRAMS = munin-node-c
pkglibexecdir = $(libexecdir)/$(PACKAGE)
pkglibexec_PROGRAMS = munin-plugins-c

LINKS = cpu entropy forks fw_packets interrupts if_err_eth0 load	\
      open_files open_inodes processes swap threads uptime memory
CFLAGS += -W -Wall -Wextra

AM_CPPFLAGS = -DPLUGINDIR=\"$(pkglibexecdir)\"

dist_doc_DATA = gpl-2.0.txt gpl-3.0.txt

munin_node_c_SOURCES = src/node/main.c
munin_plugins_c_SOURCES = \
	src/plugins/common.c \
	src/plugins/common.h \
	src/plugins/cpu.c \
	src/plugins/entropy.c \
	src/plugins/forks.c \
	src/plugins/fw_packets.c \
	src/plugins/if_err_.c \
	src/plugins/interrupts.c \
	src/plugins/load.c \
	src/plugins/main.c \
	src/plugins/open_files.c \
	src/plugins/open_inodes.c \
	src/plugins/processes.c \
	src/plugins/swap.c \
	src/plugins/threads.c \
	src/plugins/memory.c \
	src/plugins/uptime.c

all-local: $(pkglibexec_PROGRAMS)
	$(MKDIR_P) plugins
	for l in $(LINKS); do \
		test -f plugins/$$l || $(LN_S) ../munin-plugins-c plugins/$$l; \
	done

clean-local:
	rm -rf plugins

install-exec-hook:
	for l in $(LINKS); do \
		test -f $(DESTDIR)$(pkglibexecdir)$$l || $(LN_S) munin-plugins-c $(DESTDIR)$(pkglibexecdir)/$$l; \
	done
