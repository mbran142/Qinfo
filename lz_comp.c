#include "qinfo.h"

char* lz_comp(char *input) {

	int not_found;
	char *output = (char *)calloc(str_len(input) + 1, sizeof(char));

	for (int i = 0, o = 0, n; input[i]; i++, o++) {

		not_found = 1;

		for (int j = i > 0x3ff ? i - 0x3ff : 0; j < i && not_found; j++) {

			for (n = 0; input[j + n] == input[i + n] && n + j < i - 1 && n < 0x3f; n++);

			if (n > 3) {

			/*	3 byte pointer; first 8 bits is a pointer char
				next 10 bits is skip-back value
				last 6 bits is amnt of chars to copy	*/
			output[o++] = PTR_CHAR;
			output[o++] = (i - j) >> 2;
			output[o] = (((i - j) << 6) & 0xc0) | n;

			i += n - 1;
			not_found = 0;

			}
		}	

		if (not_found)
			output[o] = input[i];
	}

	return output;
}