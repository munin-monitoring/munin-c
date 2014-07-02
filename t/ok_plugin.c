#include <stdio.h>

#include "common.h"

int emit_config() {
	printf("graph_title " __FILE__ "\n");
	printf("first_f.label This is the first field\n");
	printf("second_f.label This is the second field\n");

	return 0;
}

int emit_fetch() {
	printf("first_f.value %f\n", 1234.567);
	printf("second_f.value %f\n", -2345.678);

	return 0;
}
