#include "qinfo.h"

void delete_root(snode **root, int dealloc) {

	snode *p = *root;
	snode *q = *root;

	if (p->right != NULL) {
		//inorder successor
		p = get_extreme(p->right, &q, EXTREME_MIN);

		//shifting parent's pointer to successor's child
		if (q->left == p)
			q->left = p->right;
		else
			q->right = p->right;

		//root gets the successor
		p->left = (*root)->left;
		p->right = (*root)->right;

		if (dealloc) {
			free((*root)->title);
			free((*root)->info);
			free(*root);
		}
		else
			(*root)->left = (*root)->right = NULL;

		*root = p;
	}

	else if (p->left != NULL) {
		//inorder predecessor
		p = get_extreme(p->left, &q, EXTREME_MAX);
		//replace p with root

		//shifting parent's pointer to successor's child
		if (q->left == p)
			q->left = p->right;
		else
			q->right = p->right;

		//root gets the successor
		p->left = (*root)->left;
		p->right = (*root)->right;

		if (dealloc) {
			free((*root)->title);
			free((*root)->info);
			free(*root);
		}
		else
			(*root)->left = (*root)->right = NULL;			
		*root = p;
	}

	else {

		if (dealloc) {
			free((*root)->title);
			free((*root)->info);
			free(*root);
		}
		else
			(*root)->left = (*root)->right = NULL;

		*root = NULL;
	}
}

snode* get_extreme(snode *root, snode **parent, int min_max) {

	int temp = get_depth(root, 0);

	snode **thread = (snode **)malloc(sizeof(snode *) * temp);
	int *thread_helper = (int *)malloc(sizeof(int) * temp);

	for (int i = 0; i < temp; i++)
		thread_helper[i] = 0;

	snode *output = root;
	snode *p = root;

	int level = 0;
	for (int i = 0, done = 0; !done; i++) {

		if ((min_max == EXTREME_MIN && str_cmp(p->title, output->title) < 0) ||
			(min_max == EXTREME_MAX && str_cmp(p->title, output->title) > 0)) {
			*parent = thread[level - 1];
			output = p;
		}

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