#include "qinfo.h"

char* huff_comp(int *out_count, char *text) {
	
	char *huff = gen_tree(text);
	int out = 3;

	/*lookup table for amount of bits required for each char
	the table is of format: [char][amnt_bits][string of bits]
	all chars that don't show up in the text will point to NULL by the table*/
	char *table[STAT_SIZE];
	char temp;
	char char_count = '\0';
	int num_bits = 0;
	int max = 0;

	for (int i = 0, j = 0, amnt = 0; i < STAT_SIZE; i++) {

		if (i == huff[j]) {
			temp = huff[j++];
			amnt = 0;

			while (huff[j] < 3 && huff[j]) {
				j++;
				amnt++;
			}

			table[i] = (char *)malloc(sizeof(char) * (amnt + 3));
			table[i][0] = temp;
			num_bits += table[i][1] = amnt;

			for (int l = amnt + 1, k = 1; l > 1; l--, k++)
				table[i][l] = huff[j - k];
			
			table[i][amnt + 2] = '\0';
			char_count++;

			if (amnt > max)
				max = amnt;
		}
		else
			table[i] = NULL;
	}

	//calculating size of output string
	num_bits += (char_count + 1) << 4;
	for (int i = 0; text[i]; i++) {
		num_bits += table[(int)text[i]][1];
		if (text[i] == PTR_CHAR) {
			num_bits += 16;
			i += 2;
		}
	}

	char *compressed = (char *)malloc(sizeof(char) * ((num_bits >> 3) + 2));

	compressed[1] = char_count;

	//number of bits needed to store huffman encoding for each char
	temp = '\0';
	while (max) {
		max >>= 1;
		temp++;
	}
	compressed[2] = max = temp;

	char temp_bits = '\0';
	for (int i = 0, bits = 0; huff[i]; i++) {

		//put char into file
		temp = huff[i];
		for (int j = 0; j < 8; j++) {
			if (temp & (1 << j))
				temp_bits |= 1 << bits;
			bits++;
			if (bits == 8) {
				compressed[out++] = temp_bits;
				bits = 0;
				temp_bits = '\0';
			}
		}

		//put num bits that char's huffman code takes up into file
		temp = table[(int)temp][1];
		for (int j = 0; j < max; j++) {
			if (temp & (1 << j))
				temp_bits |= 1 << bits;
			bits++;	
			if (bits == 8) {
				compressed[out++] = temp_bits;
				bits = 0;
				temp_bits = '\0';
			}
		}

		//put individual bits into chars
		for (int j = 0; j < temp; j++) {
			if (huff[++i] == 2)
				temp_bits |= 1 << bits;
			bits++;
			if (bits == 8) {
				compressed[out++] = temp_bits;
				bits = 0;
				temp_bits = '\0';
			}
		}
	}
	compressed[out++] = temp_bits;

	//storing the huffman encoding into the file
	int bit = 0;
	temp_bits = '\0';

	for (int i = 0; text[i]; i++) {

		temp = text[i];

		for (int j = 2; j - 2 < table[(int)temp][1]; j++) {

			if (table[(int)temp][j] == 2)
				temp_bits |= 1 << bit;
			bit++;

			if (bit == 8) {
				compressed[out++] = temp_bits;
				temp_bits = '\0';
				bit = 0;
			}
		}

		//LZ ptr char scenario
		if (text[i] == PTR_CHAR) {

			for (int j = 1; j < 3; j++)

				for (int k = 0; k < 8; k++) {

					if (text[i + j] & (1 << k))
						temp_bits |= 1 << bit;
					bit++;

					if (bit == 8) {
						compressed[out++] = temp_bits;
						temp_bits = '\0';
						bit = 0;
					}
				}

			i += 2;
		}
	}
	compressed[out++] = temp_bits;
	*out_count = out;

	//put bit skip at front of string
	compressed[0] = bit;

	//free allocated variables
	free(huff);
	for (int i = 0; i < STAT_SIZE; i++)
		if (table[i] != NULL)
			free(table[i]);

	return compressed;
}

char* gen_tree(char *msg) {

	int stats[STAT_SIZE] = {0};
	int char_count = 0;
	int non_zero_count = 0;

	//gets all character occurences
	for (int i = 0; *(msg + i); i++) { //while not NULL char
		stats[(int)msg[i]]++;
		char_count++;

		if (msg[i] == PTR_CHAR)
			i += 2;
	}

	//counts how many unique characters exist in the string
	for (int i = 0; i < STAT_SIZE; i++)
		if (stats[i]) //stats[i] != 0
			non_zero_count++;

	hnode **huff_leaves = (hnode **)malloc(sizeof(hnode*) * non_zero_count);

	//for every char (except for '\n', create a new node containing the char and chance
	for (int i = 0, j = 0; i < STAT_SIZE; i++) {
		if (stats[i]) { //stats[i] != 0
			*(huff_leaves + j) = (hnode *)malloc(sizeof(hnode));
			(*(huff_leaves + j))->data = (char)i;
			(*(huff_leaves + j))->chance = (double)stats[i] / char_count;
			(*(huff_leaves + j))->left = NULL;
			(*(huff_leaves + j++))->right = NULL;
		}
	}

	//sort nodes in order of occurence (high -> low)
	hnode* temp;
	for (int i = 0, j; i < non_zero_count - 1; i++) {
		temp = huff_leaves[i + 1];
		for (j = i; j >= 0 && temp->chance >= huff_leaves[j]->chance; j--)
			huff_leaves[j + 1] = huff_leaves[j];
		huff_leaves[j + 1] = temp;
	}

	//generates huffman tree using the nodes
	int huff_index = non_zero_count - 1;
	while (huff_index > 0) {

		hnode* new_node = (hnode *)malloc(sizeof(hnode));
		new_node->data = 0; //NULL
		new_node->chance = huff_leaves[huff_index]->chance + huff_leaves[huff_index - 1]->chance;
		new_node->left = huff_leaves[huff_index];
		new_node->right = huff_leaves[huff_index - 1];

		huff_leaves[huff_index] = NULL;
		huff_leaves[--huff_index] = NULL;

		register int i;
		for (i = huff_index - 1; i >= 0 && new_node->chance >= huff_leaves[i]->chance; i--) 
			huff_leaves[i + 1] = huff_leaves[i];
		huff_leaves[i + 1] = new_node;
	}

	hnode* root = huff_leaves[0];
	free(huff_leaves);

	//some variables to help non-recursively navigate tree
	char **huffman_code = (char **)malloc(sizeof(char**) * non_zero_count);
	char *code = (char *)calloc(non_zero_count, sizeof(char*));
	hnode **thread = (hnode **)malloc(sizeof(hnode**) * non_zero_count);

	hnode *p = root;
	*thread = p;

	//traverse tree to get codes
	for (int i = 0, level = 0; i < non_zero_count; i++) {

		while (p->data < 3) {
			(p->data)++;
			if (p->data == 1) {
				p = p->left;
				code[level] = 1;
			}
			else {
				p = p->right;
				code[level] = 2;
			}
			thread[++level] = p;
		}

		huffman_code[i] = (char *)malloc(sizeof(char) * (level + 3));
		if (str_len(code) != level)
			printf("i == %d | str_len(code) == %d, level == %d\n", i, str_len(code), level);
		huffman_code[i][0] = p->data;
		str_cpy(huffman_code[i] + 1, code);
		huffman_code[i][level + 1] = 0; //NULL

		//go up tree
		level--;
		while (level > 0 && thread[level]->data == 2)
			code[level--] = '\0';

		p = thread[level];
	}

	//variables freed
	free(code);
	free(thread);
	free_htree(root);

	//sort huffman code poiners
	char *temp_code;
	for (int i = 0, j; i < non_zero_count - 1; i++) {
		temp_code = huffman_code[i + 1];
		for (j = i; j >= 0 && temp_code[0] <= huffman_code[j][0]; j--) {
			huffman_code[j + 1] = huffman_code[j];
		}
		huffman_code[j + 1] = temp_code;
	}

	//get total amount of chars in huffman codes
	int amnt = 0;
	for (int i = 0; i < non_zero_count; i++)
		for (int j = 0; huffman_code[i][j]; j++)
			amnt++;

	char *huffman_string = (char *)malloc(sizeof(char) * (amnt + 2));
	huffman_string[amnt] = '\0';

	//turn the double-pointer into a simple string
	for (int i = 0, amnt = 0; i < non_zero_count; i++)
		for (int j = 0; huffman_code[i][j]; j++)
			huffman_string[amnt++] = huffman_code[i][j];

	//free the double-pointer and everything in it
	for (int i = 0; i < non_zero_count; i++)
		free(huffman_code[i]);
	free(huffman_code);

	return huffman_string;
}

void free_htree(hnode* root) {
	if (root != NULL) {
		free_htree(root->left);
		free_htree(root->right);
		free(root);
	}
}