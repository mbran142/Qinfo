#include "qinfo.h"

snode* search(snode *root, char *search_str, int *level) {

	if (root == NULL)
		return NULL;

	snode *p = root;
	*level = 0;

	while (p != NULL && str_cmp(p->title, search_str)) {
		p = str_cmp(p->title, search_str) > 0 ? p->left : p->right;
		(*level)++;
	}

	return p;
}

