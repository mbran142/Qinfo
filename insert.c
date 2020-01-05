#include "qinfo.h"

void insert(snode **root, snode *insert_node, int splay_status) {

	snode *p = *root;

	int level = 0;
	snode *q = NULL;

	//search for placement in tree
	while (p != NULL) {
		q = p;
		p = str_cmp(p->title, insert_node->title) > 0 ? p->left : p->right;
		level++;
	}

	if (q == NULL)
		*root = insert_node;

	else {

		//connect new node to tree
		if (str_cmp(q->title, insert_node->title) > 0)
			q->left = insert_node;
		else
			q->right = insert_node;

		if (splay_status)
			splay(insert_node, root, level);
	}

}