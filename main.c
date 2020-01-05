#include "qinfo.h"

/*
TODO:
	- Improve LZ encoding by connecting it to huffman encoding tree, and only going to LZ encoding if the huffman encoding is less efficient
	- -p [nodename] commandline arg
	- Add a password
*/

int main(int argc, char *argv[]) {

	FILE *fp;

	int save = 0;
	int done = 0;
	int print_menu = 1;
	char *start;
	char *halfway;
	char *finished;
	char *input;
	char file_name[FILE_SIZE] = {0};
	str_cpy(file_name, "default.qfo");

	if (argc > 2) {
		printf("Error: Too many commandline arguments\n");
		exit(1);
	}

	else if (argc == 2) {
		if (str_len(argv[1]) > FILE_SIZE - str_len(file_name) - 1) {
			printf("Error: Filename too long\n");
			exit(1);
		}
		else
			str_cpy(file_name, argv[1]);
	}
	
	if (!(fp = fopen(file_name, "r"))) {

		printf("Error: \'%s\' could not be opened.\n", file_name);
		
		if (!str_cmp(file_name, "default.qfo") && confirm(0, "Create new default.qfo file? (y/n)")) {
			fp = fopen(file_name, "a");
			printf("File created. Please restart the program.\n");
		}
		else printf("The program will now exit.\n");

		exit(1);
	}

	printf("Loading data... ");

	//extract data from file
	int file_index = 0;
	start = (char *)malloc(sizeof(char) * CHUNK);

	for (int current_size = CHUNK; !feof(fp); file_index++) {
		if (file_index >= current_size) {
			current_size *= 2;
			start = (char *)realloc(start, current_size * sizeof(char));
		}
		start[file_index] = fgetc(fp);
	}

	start[--file_index] = '\0';
	fclose(fp);

	snode *root = NULL;

	if (file_index) {

		//decompress file data
		halfway = huff_decomp(file_index, start);
		free(start);
		start = NULL;

		//FIX LZ DECOMP
		/*finished = lz_decomp(halfway);
		free(halfway);
		halfway = NULL;*/
		finished = halfway;

		//generate splay tree from file data
		gen_data_tree(&root, finished);
		free(finished);
		finished = NULL;

	}
	else {
		free(start);
		start = NULL;
	}

	if (root == NULL)
		printf("(%s is empty) ", file_name);
	printf("Done.\n");

	do {

		char *title;
		char *info;
		snode *p;
		int level;

		//pre_trav(root, 0);

		if (print_menu) {

			printf("[1] Search entry\n");
			printf("[2] Store entry\n");
			printf("[3] Modify entry\n");
			printf("[4] Delete entry\n");
			printf("[5] Find entry\n");
			printf("[0] Exit program\n");

		}

		printf("   Enter selection: ");
		input = get_msg('\0');
		print_menu = 1;

		switch(input[0]) {

			case '0':
				done = 1;
				break;

			case '1':
				//search for title
				printf("   Enter title to search for: ");
				title = get_msg('\0');

				//print result
				p = search(root, title, &level);
				if (p == NULL)
					printf("      \'%s\' not found\n", title);
				else {
					putchar('\n');
					printf("%s\n", p->info);
					splay(p, &root, level);
				}

				free(title);
				title = NULL;
				p = NULL;
				break;

			case '2':
				//geting user input for storage information
				printf("   Enter seatch title: ");
				title = get_msg('\0');

				p = search(root, title, &level);
				if (p != NULL) {
					printf("      Entry under title \'%s\' already exists.\n", title);
					free(title);
					title = NULL;
				}

				else {
					printf("   Enter information stored. Terminate entry with \'~\\n\'\n\n");
					info = get_msg('~');

					//creating new node
					snode *new = (snode *)malloc(sizeof(snode));
					new->left = new->right = NULL;

					new->title = (char *)realloc(title, sizeof(char) * (str_len(title) + 1));
					new->info = (char *)realloc(info, sizeof(char) * (str_len(info) + 1));

					title = info = NULL;

					//splayed to root
					insert(&root, new, 1);
					save = 1;
				}
				break;

			case '3':

				//getting user input modification
				printf("   Enter title to modify: ");
				title = get_msg('\0');
				p = search(root, title, &level);

				if (p == NULL) {
					printf("      \'%s\' not found\n", title);
					free(title);
				}

				else {

					//print the start of the info
					printf("      ");
					for (int i = 0; p->info[i] && i < PRINT_AMNT; i++) {
						putchar(p->info[i]);
						if (i == (PRINT_AMNT - 1) || !p->info[i] || p->info[i + 1] == '\n') {
							printf("...");
							break;
						}
					}
					putchar('\n');

					free(title);
					title = NULL;

					free(input);
					input = NULL;

					printf("      [1] Change title\n");
					printf("      [2] Rewrite information\n");
					printf("      [3] Append information\n");
					printf("      [4] Modify by line\n");
					printf("      [0] Cancel\n");

					do {

						printf("         Enter selection: ");
						input = get_msg('\0');

						switch(input[0]) {

							case '0':
								done = 1;
								break;

							case '1':
								printf("            Enter new title: ");
								title = get_msg('\0');

								if (confirm(12, "Confirm modification (y/n)")) {

									free(p->title);
									int temp = str_len(title);
									p->title = (char *)realloc(title, sizeof(char) * (temp + 1));
									p->title[temp] = '\0';
									title = NULL;

									//delete node from tree since it may be in a different place due to title change
									if (!(!level && root->left == NULL && root->right == NULL)) {
									splay(p, &root, level);
									delete_root(&root, 0);

									//splay to root
									insert(&root, p, 1);
									}

									printf("               \'%s\' modified.\n", p->title);
									save = 1;
								}

								else {
									printf("               Modification canceled.\n");
									free(title);
									title = NULL;
								}

								done = 1;
								break;

							case '2':
								printf("            Enter new information. Terminate entry with \'~\\n\'\n\n");
								info = get_msg('~');
								putchar('\n');

								if (confirm(12, "Confirm modification (y/n)")) {

									free(p->info);
									p->info = (char *)realloc(info, sizeof(char) * (str_len(info) + 1));
									info = NULL;

									splay(p, &root, level);

									printf("\n            \'%s\' modified.\n", p->title);
									save = 1;
								}

								else {
									printf("               Modification canceled.\n");
									free(info);
									info = NULL;
								}

								p = NULL;
								done = 1;
								break;

							case '3':

								printf("\n%s [Enter appendation, end with \'~\\n\']: ", p->info);
								info = get_msg('~');
							
								if (confirm(12, "Confirm modification (y/n)")) {

									done = str_len(p->info);

									//concatanation
									p->info = (char *)realloc(p->info, sizeof(char) * (done + str_len(info) + 2));
									p->info[done++] = ' ';
									str_cpy(p->info + done, info);
									p->info[done + str_len(info)] = '\0';

									free(info);
									info = NULL;

									splay(p, &root, level);

									printf("\n            \'%s\' modified.\n", p->title);
									save = 1;
								}

								else {
									printf("               Modification canceled.\n");
									free(info);
									info = NULL;
								}

								done = 1;
								break;

							case '4':

								free(title);
								title = NULL;

								if (str_nlen(p->info) == str_len(p->info)) 
									printf("            By-line modifications cannot be done on one-line entries\n");					

								else {
									putchar('\n');
									printf("   [1]   ");
									int nl_count = 1;
									for (int i = 0; p->info[i]; i++) {
										putchar(p->info[i]);
										if (p->info[i] == '\n')
											printf("   [%d]   ", ++nl_count);
									}
									putchar('\n');
									putchar('\n');

									printf("      [1] Remove line\n");
									printf("      [2] Add line\n");
									printf("      [3] Edit line\n");
									printf("      [0] Cancel\n");

									do {

										free(input);
										printf("         Enter selection: ");
										input = get_msg('\0');

										switch(input[0]) {

											case '0':
												done = 1;
												break;

											case '1':

												do {

													printf("            Enter line number to remove (or 0 to cancel): ");
													done = get_num();

													if (done == -1) {
														printf("               Invalid input\n\n");
														done = 0;
													}

													else if (done > nl_count) {
														printf("               Error: Line input out of bounds\n\n");
														done = 0;
													}

													else if (!done)
														done = 1;

													else if (!(confirm(15, "Confirm deletion (y/n)")))
														done = 1;

													else {

														if (done == 1)
															done = 0;

														char *temp = (char *)malloc(sizeof(char) * (str_len(p->info) + 1));

														register int n = 0;
														for (int i = done ? 0 : str_nlen(p->info) + 1, j = 1; p->info[i]; i++) {

															if (p->info[i] == '\n') {
																j++;

																if (j == done) {
																	i += str_nlen(p->info + i + 1);
																	j++;
																}
															}

															else temp[n++] = p->info[i];
														}
														temp[n] = '\0';

														free(p->info);
														p->info = temp;

														done = 1;
														save = 1;
													}

												} while (!done);

												break;

											case '2':

												do {

													printf("            Enter line number to add after (or 0 to cancel): ");
													done = get_num();

													if (done == -1) {
														printf("               Invalid input\n\n");
														done = 0;
													}

													else if (done > nl_count) {
														printf("               Error: Line input out of bounds\n\n");
														done = 0;
													}

													else if (!done)
														done = 1;

													else {

														printf("               Enter new line here: ");
														info = get_msg('\0');

														if (confirm(15, "Confirm insertion (y/n)")) {

															char *temp = (char *)malloc(sizeof(char) * (str_len(p->info) + str_len(info) + 2));

															register int n;

															if (done != nl_count) {

																n = 0;
																for (int i = 0, j = 0; p->info[i]; i++, n++) {

																	temp[n] = p->info[i];

																	if (p->info[i] == '\n') {

																		j++;

																		if (j == done) {

																			n += 1;

																			for (int k = 0; info[k]; n++, k++)
																				temp[n] = info[k];

																			temp[n] = '\n';

																			j++;
																		}
																	}
																}
															}

															else {
																str_cpy(temp, p->info);

																n = str_len(p->info);
																temp[n++] = '\n';

																for (int i = 0; info[i]; i++, n++)
																	temp[n] = info[i];
															}

															temp[n] = '\0';

															free(p->info);
															free(info);
															info = NULL;
															p->info = temp;
														}

														else
															printf("                  Insertion canceled\n");
													
														free(info);
														info = NULL;
														done = 1;
														save = 1;
													}

												} while (!done);										

												break;

											case '3':
									
												do {

													printf("            Enter line number to replace (or 0 to cancel): ");
													done = get_num();

													if (done == -1) {
														printf("               Invalid input\n\n");
														done = 0;
													}

													else if (done > nl_count) {
														printf("               Error: Line input out of bounds\n\n");
														done = 0;
													}

													else if (!done)
														done = 1;

													else {

														printf("               Enter replacement line here: ");
														info = get_msg('\0');

														if (confirm(15, "Confirm replacement (y/n)")) {

															char *temp = (char *)malloc(sizeof(char) * (str_len(p->info) + str_len(info) + 1));

															register int n = 0;
															for (int i = done ? 0 : str_nlen(p->info) + 1, j = 1; p->info[i]; i++, n++) {

																if (p->info[i] == '\n') {
																	j++;

																	if (j == done) {
																		temp[n++] = '\n';
																		for (int k = 0; info[k]; k++, n++)
																			temp[n] = info[k];
																		if (done == nl_count) {
																			temp[n] = '\0';
																			break;
																		}
																		else
																			temp[n] = '\n';

																		i += str_nlen(p->info + i + 1) + 1;
																		j++;
																	}
																}

																temp[n] = p->info[i];
															}

															temp[n] = '\0';
															free(p->info);
															p->info = temp;

															done = 1;
															save = 1;
														}

														else
															printf("                  Replacement canceled\n");

														free(info);
														info = NULL;
														done = 1;
														save = 1;
													}

												} while (!done);		
												break;

											case 'q':
												done = 1;
												break;

											default:
												printf("                Selection not recognized\n\n");
										}

									} while (!done);

								}

								done = 1;
								break;

							case 'q':
								done = 1;
								break;

							default:
								printf("            Selection not recognized\n\n");
						}

					} while (!done);

					free(input);
					input = NULL;
				}

				if (p != NULL) {
					splay(p, &root, level);
					p = NULL;
				}
				done = 0;
				break;

			case '4':
				//getting user input for deletion
				printf("   Enter title to delete: ");
				title = get_msg('\0');
				p = search(root, title, &level);

				if (p == NULL)
					printf("      \'%s\' not found\n", title);

				else {

					printf("      ");
					for (int i = 0; p->info[i] && i < PRINT_AMNT; i++) {
						putchar(p->info[i]);
						if (i == (PRINT_AMNT - 1) || !p->info[i] || p->info[i + 1] == '\n') {
							printf("...");
							break;
						}
					}
					putchar('\n');

					if (confirm(6, "Confirm deletion (y/n)")) {
						splay(p, &root, level);
						delete_root(&root, 1);
						printf("         \'%s\' deleted.\n", title);
						save = 1;
					}

					else
						printf("         Deletion canceled.\n");
				}

				free(title); 
				title = NULL;
				p = NULL;
				break;

			case '5':

				printf("      Enter start of search: ");
				title = get_msg('\0');
				printf("      Enter end of search: ");
				info = get_msg('\0');

				if (print_all(root, title, info))
					putchar('\n');
				else
					printf("         No entries found\n");

				free(title);
				free(info);
				title = info = NULL;

				break;

			case 'q':
				done = 1;
				break;

			default:
				printf("      Error: Selection not recognized\n");
				print_menu = 0;
		}

		free(input);
		input = NULL;
		putchar('\n');

	} while (!done);

	if (save)
		save = confirm(0, "Save changes? (y/n)");

	if (save) {

		if (!(fp = fopen(file_name, "w"))) {
			printf("Error: \'%s\' could not be accessed for saving\n", file_name);
			exit(1);
		}

		int char_count;
		printf("Saving... ");
		
		if (root == NULL) {
			fclose(fp);
			printf("(nothing stored) Done.\n");
			return 0;
		}

		//converting data in tree into a storable string
		start = store_data(root);
		free_data_tree(root);
		root = NULL;

		//TODO: FIX LZCOMP (?)
		/*halfway = lz_comp(start);
		free(start);
		start = NULL;*/
		halfway = start;

		finished = huff_comp(&char_count, halfway);
		free(halfway);
		halfway = NULL;

		//storing data
		for (int i = 0; i < char_count; i++)
			fputc((int)finished[i], fp);

		free(finished);
		finished = NULL;
		fclose(fp);

		printf("Done.\n");
	}

	else
		free_data_tree(root);

	return 0;
}

int str_len(char *text) {
	register int i = 0;
	while (text[i])
		i++;
	return i;
}

int str_nlen(char *text) {
	register int i = 0;
	while (text[i] && text[i] != '\n')
		i++;
	return i;
}

int str_cmp(char *str_1, char *str_2) {

	register int i = 0;

	while (str_1[i] && str_2[i] && (str_1[i] | ' ') == (str_2[i] | ' '))
		i++;

	return ((int)str_1[i] | ' ' ) - ((int)str_2[i] | ' ');
}

void str_cpy(char *dest, char *src) {
	for (int i = 0; src[i]; i++)
		dest[i] = src[i];
}

void pre_trav(snode *root, int num) {
	if (root != NULL) {
		printf("%s : %s - lv.%d\n",root->title, root->info, num);
		pre_trav(root->left, num + 1);
		pre_trav(root->right, num + 1);
	}
	else {
		printf("NUL - lv.%d\n", num);
	}
}