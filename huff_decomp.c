#include "qinfo.h"

char* huff_decomp(int size, char *file_data) {

	int index = 0;
	int bit_skip = (int)file_data[index++];
	int num_chars = (int)file_data[index++];
	int bit_per_char = (int)file_data[index++];

	//create table of huffman codes
	char **table = (char **)malloc(sizeof(char *) * num_chars);
	char buf[BUF_SIZE] = {0};

	char c = file_data[index++];

	for (int i = 0, bit = 0; i < num_chars; i++) {

		char table_char = '\0';
		char table_num = '\0';

		//get char for huff code
		for (int j = 0; j < 8; j++) {
			if (c & (1 << bit))
				table_char |= 1 << j;
			bit++;
			if (bit == 8) {
				c = file_data[index++];
				bit = 0;
			}
		}

		//get amnt of bits for that char
		for (int j = 0; j < bit_per_char; j++) {
			if (c & (1 << bit))
				table_num |= 1 << j;
			bit++;
			if (bit == 8) {
				c = file_data[index++];
				bit = 0;
			}
		}

		//generate a string for the table
		table[i] = (char *)malloc(sizeof(char) * (table_num + 2));
		table[i][0] = table_char;
		table[i][table_num + 1] = '\0';
		for (int j = 0; j < table_num; j++) {
			table[i][j + 1] = c & (1 << bit++) ? '1' : '0';
			if (bit == 8) {
				c = file_data[index++];
				bit = 0;
			}
		}
	}

	char *output = (char *)malloc(sizeof(char) * CHUNK);

	//decompressing data from file
	c = file_data[index++];
	int out_index = 0;
	for (int i = 0, bit = 0, boolean = 0, current_size = CHUNK; index < size || bit < bit_skip; i = 0, boolean = 0) {

		do {

			if (bit == 8) {
				c = file_data[index++];
				bit = 0;
			}

			buf[i++] = c & (1 << bit++) ? '1' : '0';
			buf[i] = '\0';

			for (int j = 0; j < num_chars; j++)

				if (!str_cmp(buf, table[j] + 1)) {

					if (out_index >= current_size - 2) {
						current_size <<= 1;
						output = (char *)realloc(output, sizeof(char) * current_size);
					}

					output[out_index] = table[j][0];

					//LZ ptr char scenario
					if (output[out_index++] == PTR_CHAR) {
						char temp_bits = '\0';

						for (int n = 0; n < 16; n++) {

							if (bit == 8) {
								c = file_data[index++];
								bit = 0;
							}
						
							if (c & (1 << bit++))
								temp_bits |= 1 << (n & 0x7);

							if ((n & 0x7) == 7) {
								output[out_index++] = temp_bits;
								temp_bits = '\0';
							}
						}
					}

					boolean = 1;
					break;
				}

		} while (!boolean);

		if (bit == 8) {
			c = file_data[index++];
			bit = 0;
		}
	}

	output[out_index] = '\0';

	for (int i = 0; i < num_chars; i++)
		free(table[i]);
	free(table);

	return output;
}