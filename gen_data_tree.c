#include "qinfo.h"

void gen_data_tree(snode **root, char *text) {

	for (int i = 0, j = 0; text[i]; i++, j = 0) {

		snode *new = (snode *)malloc(sizeof(snode));
		new->left = new->right = NULL;

		//memory for title
		while (text[i] != NODE_CHAR) {
			i++;
			j++;
		}

		new->title = (char *)malloc(sizeof(char) * (j + 1));

		i -= j;

		for (j = 0; text[i] != NODE_CHAR; i++, j++)
			new->title[j] = text[i];
		new->title[j] = '\0';

		i++;
		j = 0;

		//memory for data
		while (text[i] != NODE_CHAR) {
			i++;
			j++;
		}

		new->info = (char *)malloc(sizeof(char) * (j + 1));

		i -= j;

		for (j = 0; text[i] != NODE_CHAR; i++, j++)
			new->info[j] = text[i];
		new->info[j] = '\0';

		insert(root, new, 0); //not splayed
	}
}