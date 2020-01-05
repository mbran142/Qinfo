#include "qinfo.h"

void splay(snode *to_splay, snode **root, int level) {

	snode **thread = (snode **)malloc(sizeof(snode*) * (level + 1));
	snode *p = *root;

	for (int i = 0; i < level + 1; i++) {
		thread[i] = p;
		p = str_cmp(p->title, to_splay->title) > 0 ? p->left : p->right;
	}

	while (level > 0) {

		if (level == 1) {

			p = NULL;

			if (thread[0]->left == thread[1])	//p->left = c
				rotate_right(p, thread[0], thread[1]);
			else	//p->right = c
				rotate_left(p, thread[0], thread[1]);

			thread[0] = thread[1];
			level--;
		}

		else {

			int rotation_type = 0;
			p = level == 2 ? NULL : thread[level - 3];

			if (thread[level - 1]->right == thread[level])
				rotation_type += 1;
			if (thread[level - 2]->right == thread[level - 1])
				rotation_type += 2;

			switch(rotation_type) {

				case 0:		//g->left = p->left = c
					rotate_right(p, thread[level - 2], thread[level - 1]);
					rotate_right(p, thread[level - 1], thread[level]);
					break;

				case 1:		//g->left = p->right = c
					rotate_left(thread[level - 2], thread[level - 1], thread[level]);
					rotate_right(p, thread[level - 2], thread[level]);
					break;

				case 2:		//g->right = p->left = c
					rotate_right(thread[level - 2], thread[level - 1], thread[level]);
					rotate_left(p, thread[level - 2], thread[level]);
					break;

				case 3:		//g->right = g->right = c
					rotate_left(p, thread[level - 2], thread[level - 1]);
					rotate_left(p, thread[level - 1], thread[level]);
					break;
			}

			thread[level - 2] = thread[level];
			level -=2 ;
		}
	}

	*root = thread[0];

	free(thread);
}

void rotate_left(snode *grandparent, snode *parent, snode *child) {

	snode *p = child->left;
	child->left = parent;
	parent->right = p;

	if (grandparent != NULL) {
		if (grandparent->left == parent)
			grandparent->left = child;
		else
			grandparent->right = child;
	}
}

void rotate_right(snode *grandparent, snode *parent, snode *child) {

	snode *p = child->right;
	child->right = parent;
	parent->left = p;

	if (grandparent != NULL) {
		if (grandparent->left == parent)
			grandparent->left = child;
		else
			grandparent->right = child;
	}
}