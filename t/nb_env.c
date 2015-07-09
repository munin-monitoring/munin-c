#include <stdio.h>
#include <unistd.h>

#include "common.h"

extern char **environ;

int count_env_nb() {
	int env_nb = 0;
	char **cur_environ = environ;
	while (*cur_environ) {
		env_nb ++;
		cur_environ ++;
	}

	return env_nb;
}

int emit_config() {
	printf("graph_title " __FILE__ "\n");
	printf("env_nb.label Number of env vars\n");

	return 0;
}

int emit_fetch() {
	char **cur_environ = environ;

	printf("env_nb.value %d\n", count_env_nb());
	printf("env_nb.ext_info ");

	while (*cur_environ) {
		printf("{%s},", *cur_environ);
		cur_environ ++;
	}

	printf("\n");

	return 0;
}
