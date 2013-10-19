/*
 * Copyright (C) 2013 Steve Schnepp <steve.schnepp@pwkf.org> - All rights reserved.
 * Copyright (C) 2013 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 * Copyright (C) 2013 Diego Elio Petteno <flameeyes@flameeyes.eu> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */
#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>

#if !(defined(HAVE_WORKING_VFORK) || defined(S_SPLINT_S))
  #define vfork fork
#endif

#ifndef HOST_NAME_MAX
  #define HOST_NAME_MAX 256
#endif

static const int yes = 1;
static const int no = 0;

static int verbose = 0;
static bool extension_stripping = false;

static char* host = "";
static char* plugin_dir = PLUGINDIR;
static char* spoolfetch_dir = "";
static char* client_ip = NULL;

static int handle_connection();

static /*@noreturn@*/ void oom_handler() {
	static const char* OOM_MSG = "Out of memory\n";

	if ( write(STDOUT_FILENO, OOM_MSG, sizeof(OOM_MSG)-1) < 0) {
		/* Do nothing on write failure, we are torched anyway */
	}

	/* OOM triggers abort() since it's better to fail fast */
	abort();
}

/* an allocation bigger than MAX_ALLOC_SIZE is bogus */
#define MAX_ALLOC_SIZE (16 * 1024 * 1024)
static /*@only@*/ void *xmalloc(size_t size) {
	void* ptr;

	assert(size < MAX_ALLOC_SIZE);

	ptr = malloc(size);
	if (ptr == NULL) oom_handler();
	return ptr;
}

static /*@only@*/ char *xstrdup(const char* s) {
	char* new_str;

	assert(s != NULL);
	assert(strlen(s) < MAX_ALLOC_SIZE);
	new_str = strdup(s);
	if (new_str == NULL) oom_handler();
	return new_str;
}

static int find_plugin_with_basename(/*@out@*/ char *cmdline,
		const char *plugin_dir, const char *plugin_basename) {
	DIR* dirp = opendir(plugin_dir);
	struct dirent* dp;
	int found = 0;
	size_t plugin_basename_len = strlen(plugin_basename);

	if (dirp == NULL) {
		perror("Cannot open plugin dir in " __FILE__);
		return(found);
	}

	/* Empty cmdline */
	cmdline[0] = '\0';

	while ((dp = readdir(dirp)) != NULL) {
		char* plugin_filename = dp->d_name;

		if (plugin_filename[0] == '.') {
			/* No dotted plugin */
			continue;
		}

		if (strncmp(plugin_filename, plugin_basename, plugin_basename_len) != 0) {
			/* Does not start with base */
			continue;
		}

		if (plugin_filename[plugin_basename_len] != '\0' && plugin_filename[plugin_basename_len] != '.') {
			/* Does not end the string or start an extension */
			continue;
		}

		snprintf(cmdline, LINE_MAX, "%s/%s", plugin_dir, plugin_filename);
		if (access(cmdline, X_OK) == 0) {
			/* Found it */
			found ++;
			break;
		}
	}
	closedir(dirp);

	return found;
}

static void setenvvars_system(void);

int main(int argc, char *argv[]) {

	int optch;
	extern int opterr;

	char format[] = "evd:H:s:l:";

	struct sockaddr_in client;

	socklen_t client_len = sizeof(client);

	opterr = 1;

	while ((optch = getopt(argc, argv, format)) != -1)
	switch (optch) {
		case 'e':
			extension_stripping = true;
			break;
		case 'v':
			verbose ++;
			break;
		case 'd':
			plugin_dir = xstrdup(optarg);
			break;
		case 'H':
			host = xstrdup(optarg);
			break;
		case 's':
			spoolfetch_dir = xstrdup(optarg);
			break;
	}

	/* get default hostname if not precised */
	if ('\0' == *host) {
		int idx;

		host = xmalloc(HOST_NAME_MAX + 1);
		gethostname(host, HOST_NAME_MAX);

		/* going to lowercase */
		for (idx = 0; host[idx] != 0; idx++) {
			host[idx] = tolower((int) host[idx]);
		}
	}

	/* Prepare static plugin env vars once for all */
	setenvvars_system();

	/* use a 1-shot stdin/stdout */
	client_ip = "-";
	client_len = sizeof(client);
	if(0 == getpeername(STDIN_FILENO, (struct sockaddr*)&client,
				&client_len))
		if(client.sin_family == AF_INET)
			client_ip = inet_ntoa(client.sin_addr);
	return handle_connection();
}

/* Setting munin specific vars */
static void setenvvars_system() {
	/* Some locales use "," as decimal separator.
	 * This can mess up a lot of plugins. */
	setenv("LC_ALL", "C", yes);

	/* LC_ALL should be enough, but some plugins don't
	 * follow specs (#1014) */
	setenv("LANG", "C", yes);

	/* PATH should be *very* sane by default. Can be
	 * overrided via config file if needed
	 * (Closes #863 and #1128).  */
	setenv("PATH", "/usr/sbin:/usr/bin:/sbin:/bin", yes);
}

/* Setting munin specific vars */
static void setenvvars_munin() {
	/* munin-node will override this with the IP of the
	 * connecting master */
	if (client_ip && client_ip[0] != '\0') {
		setenv("MUNIN_MASTER_IP", client_ip, no);
	}

	/* Tell plugins about supported capabilities */
	setenv("MUNIN_CAP_MULTIGRAPH", "1", no);

	/* We only have one user, so using a fixed path */
	setenv("MUNIN_PLUGSTATE", "/var/tmp", no);
	setenv("MUNIN_STATEFILE", "/dev/null", no);

	/* That's where plugins should live */
	setenv("MUNIN_LIBDIR", "/usr/share/munin", no);
}

/* Setting user configured vars */
static void setenvvars_conf() {
	/* TODO - add plugin conf parsing */
}

static int handle_connection() {
	char line[LINE_MAX];

	/* Prepare per connection plugin env vars */
	setenvvars_munin();
	setenvvars_conf();

	printf("# munin node at %s\n", host);
	while (fflush(stdout), fgets(line, LINE_MAX, stdin) != NULL) {
		char* cmd;
		char* arg;

		cmd = strtok(line, " \t\n");
		if(cmd == NULL)
			arg = NULL;
		else
			arg = strtok(NULL, " \t\n");

		if (!cmd || strlen(cmd) == 0) {
			printf("# empty cmd\n");
		} else if (strcmp(cmd, "version") == 0) {
			printf("munin c node version: %s\n", VERSION);
		} else if (strcmp(cmd, "nodes") == 0) {
			printf("%s\n", host);
			printf(".\n");
		} else if (strcmp(cmd, "quit") == 0) {
			return(0);
		} else if (strcmp(cmd, "list") == 0) {
			struct dirent* dp;
			DIR* dirp = opendir(plugin_dir);
			if (dirp == NULL) {
				printf("# Cannot open plugin dir\n");
				return(0);
			}
			while ((dp = readdir(dirp)) != NULL) {
				char cmdline[LINE_MAX];
				char* plugin_filename = dp->d_name;;

				if (plugin_filename[0] == '.') {
					/* No dotted plugin */
					continue;
				}

				snprintf(cmdline, LINE_MAX, "%s/%s", plugin_dir, plugin_filename);
				if (access(cmdline, X_OK) == 0) {
					if(extension_stripping) {
						/* Strip after the last . */
						char *last_dot_idx = strrchr(plugin_filename, '.');
						if (last_dot_idx != NULL) {
							*last_dot_idx = '\0';
						}
					}
					printf("%s ", plugin_filename);
				}
			}
			closedir(dirp);
			putchar('\n');
		} else if (
				strcmp(cmd, "config") == 0 ||
				strcmp(cmd, "fetch") == 0
			) {
			char cmdline[LINE_MAX];
			pid_t pid;
			if(arg == NULL) {
				printf("# no plugin given\n");
				continue;
			}
			if(arg[0] == '.' || strchr(arg, '/')) {
				printf("# invalid plugin character\n");
				continue;
			}
			if (! extension_stripping || find_plugin_with_basename(cmdline, plugin_dir, arg) == 0) {
				/* extension_stripping failed, using the plain method */
				snprintf(cmdline, LINE_MAX, "%s/%s", plugin_dir, arg);
			}
			if (access(cmdline, X_OK) == -1) {
				printf("# unknown plugin: %s\n", arg);
				continue;
			}
			if(0 == (pid = vfork())) {
				execl(cmdline, arg, cmd, NULL);
				/* according to vfork(2) we must use _exit */
				_exit(1);
			} else if(pid < 0) {
				printf("# fork failed\n");
				continue;
			} else {
				waitpid(pid, NULL, 0);
			}
			printf(".\n");
		} else if (strcmp(cmd, "cap") == 0) {
			printf("cap ");
			if ('\0' != *spoolfetch_dir) {
				printf("spool ");
			}
			printf("\n");
		} else if (strcmp(cmd, "spoolfetch") == 0) {
			printf("# not implem yet cmd: %s\n", cmd);
		} else {
			printf("# unknown cmd: %s\n", cmd);
		}
	}

	return 0;
}
