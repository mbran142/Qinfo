#include "qinfo.h"

char* store_data(snode *root) {
	
	int temp = get_depth(root, 0);

	snode **thread = (snode **)malloc(sizeof(snode *) * temp);
	int *thread_helper = (int *)malloc(sizeof(int) * temp);

	for (int i = 0; i < temp; i++)
		thread_helper[i] = 0;

	char *output = (char *)malloc(sizeof(char) * CHUNK);
	snode *p = root;

	int level = 0;
	for (int i = 0, current_size = CHUNK, done = 0; !done; i++) {

		for (int j = 0; p->title[j]; i++, j++) {
			if (i >= current_size - 2) {
				current_size <<= 1;
				output = (char *)realloc(output, sizeof(char) * current_size);
			}
			output[i] = p->title[j];
		}
		output[i++] = NODE_CHAR;

		for (int j = 0; p->info[j]; i++, j++) {
			if (i >= current_size - 2) {
				current_size <<= 1;
				output = (char *)realloc(output, sizeof(char) * current_size);
			}
			output[i] = p->info[j];
		}
		output[i] = NODE_CHAR;
		output[i + 1] = '\0';

		thread[level] = p;

		if (!(thread_helper[level] & 0x1) && p->left != NULL) {
			thread_helper[level++] |= 0x1;
			p = p->left;
		}

		else if (!(thread_helper[level] & 0x2) && p->right != NULL) {
			thread_helper[level++] |= 0x2;
			p = p->right;
		}

		else {
			thread_helper[level] |= 0x2;

			while (level >= 0 && (thread_helper[level] & 0x2 || thread[level]->right == NULL))
				level--;

			if (level == -1)
				done = 1;
			
			else {
				thread_helper[level] |= 0x2;
				p = thread[level]->right;
			}
		}
	}

	free(thread);
	free(thread_helper);
	return output;
}

int get_depth(snode *root, int level) {
	if (root != NULL) {
		int l = get_depth(root->left, level + 1);
		int r = get_depth(root->right, level + 1);
		return l > r ? l : r;
	}
	else
		return level;
}

void free_data_tree(snode *root) {
	if (root != NULL) {
		free_data_tree(root->left);
		free_data_tree(root->right);
		free(root->title);
		free(root->info);
		free(root);
	}
}