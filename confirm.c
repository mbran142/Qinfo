#include "qinfo.h"

int confirm(int num_space, char *msg) {

		int redo = 0;
		int save;
		char *input;

		do {

		for (int i = 0; i < num_space; i++)
			putchar(' ');

		printf("%s: ", redo ? "Please enter [y or n]" : msg);
		input = get_msg('\0');
		
		redo = !((input[0] | ' ') == 'y' || (input[0] | ' ') == 'n');
		save = (input[0] | ' ') == 'y' ? 1 : 0;

		free(input);
		input = NULL;

		} while (redo);

		return save;
}