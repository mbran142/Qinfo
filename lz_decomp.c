#include "qinfo.h"

char* lz_decomp(char *input) {

	//calculating how much space is needed
	int byte_count = 0;
	for (int i = 0; input[i]; i++) {

		if (input[i] == PTR_CHAR) {
			i += 2;
			byte_count += input[i] & 0x3f;
		}
		else
			byte_count++;
	}

	char *output = (char *)malloc(sizeof(char) * (byte_count + 2));
	int backtrack;
	int count;

	register int o = 0;
	for (int i = 0; input[i]; i++, o++) {

		if (input[i] == 31) {

			//extract pointer values from bits
			i += 2;
			backtrack = (input[i - 1] << 2) | ((input[i] & 0xc0) >> 6);
			count = input[i] & 0x3f; 

			//copy over text
			for (int j = 0; j < count; j++, o++)
				output[o] = output[o - backtrack];
			o--;
		}

		else
			output[o] = input[i];
	}

	output[byte_count] = '\0';

	return output;
}