#include "qinfo.h"

int print_all(snode *root, char *start, char *end) {

	snode **thread = (snode **)malloc(sizeof(long*) * (get_depth(root, 0)));

	snode *p = root;
	int level = 0;
	int printed = 0;

	while (p != NULL || level > 0) {

		if (p != NULL) {

			if (end && str_cmp(p->title, end) > 0)
				break;

			thread[level++] = p;
			p = p->left;
		}

		else {
			p = thread[--level];

			if (start && str_cmp(p->title, start) >= 0) {

				if ((printed++ % 5) == 0)
					putchar('\n');

				if (str_len(p->title) > 15) {

					char print[17] = {0};

					for (int i = 0; i < 13; i++)
						print[i] = p->title[i];

					for (int i = 13; i < 16; i++)
						print[i] = '.';

					printf("%-18s", print);

				}

				else
					printf("%-18s", p->title);

			}

			p = p->right;

		}

	}

	free(thread);
	return printed;
}