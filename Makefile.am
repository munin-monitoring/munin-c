sbin_PROGRAMS = munin-node-c
pkglibexecdir = $(libexecdir)/$(PACKAGE)
pkglibexec_PROGRAMS = munin-plugins-c

LINKS = cpu entropy forks fw_packets interrupts if_err_eth0 load	\
      open_files open_inodes processes swap threads uptime memory

AM_CPPFLAGS = -DPLUGINDIR=\"$(pkglibexecdir)\"

munin_node_c_SOURCES = node/main.c
munin_plugins_c_SOURCES = \
	plugins-c/common.c \
	plugins-c/common.h \
	plugins-c/cpu.c \
	plugins-c/entropy.c \
	plugins-c/forks.c \
	plugins-c/fw_packets.c \
	plugins-c/if_err_.c \
	plugins-c/interrupts.c \
	plugins-c/load.c \
	plugins-c/main.c \
	plugins-c/open_files.c \
	plugins-c/open_inodes.c \
	plugins-c/processes.c \
	plugins-c/swap.c \
	plugins-c/threads.c \
	plugins-c/memory.c \
	plugins-c/uptime.c

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
