#include "qinfo.h"

int get_num(void) {

	char *input = get_msg('\0');
	int output = 0;

	for (int i = 0; input[i]; i++) {

		if (!(input[i] >= '0' && input[i] <= '9')) {
			free(input);
			return -1;
		}

		output *= 10;
		output += input[i] & 0xf;
	}

	free(input);
	return output;
}