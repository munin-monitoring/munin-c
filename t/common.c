/* Simmle plugin framework */
#include <string.h>

#include "common.h"

int main(int argc, const char *argv[])
{
	int is_config = (argc == 2) && (strcmp(argv[1], "config") == 0);
	return is_config ? emit_config() : emit_fetch();
}
