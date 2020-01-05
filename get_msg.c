#include "qinfo.h"

//if delim = NULL, the message is terminated by a newline char
char* get_msg(char delim) {

	int current_size = 2;
	int index_size = 1;
	char temp;
	char *text = (char *)malloc(sizeof(char) * current_size);

	if (text == NULL) {
		printf("Memory allocation error\n");
		exit(1);
	}

	*text = getc(stdin);
	if (*text == '\n' && !delim)
		return text;

	do {

		temp = getc(stdin);

		if (temp < 0)
			continue;

		//if the string "~\n" is entered, msg will end
		if (temp == '\n' && (!delim || *(text + index_size - 1) == delim)) {

			temp = '\0';

			if (delim)
				--index_size;
		}

		else if (index_size >= current_size - 1) {
			current_size *= 2;
			text = (char *)realloc(text, current_size * sizeof(char));

			if (text == NULL) {
				printf("Memory allocation error\n");
				exit(1);
			}
		}

		*(text + index_size++) = temp;

	} while (temp); //while temp != NULL

	if (text[0] == '\n' && !text[1])
		text[0] = '\0';

	return text;
}
