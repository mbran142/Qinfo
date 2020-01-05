#ifndef _QINFO_H
#define _QINFO_H

#include <stdio.h>
#include <stdlib.h>

#define CHUNK 1
#define STAT_SIZE 128
#define FILE_SIZE 100
#define BUF_SIZE 30
#define PTR_CHAR 31
#define NODE_CHAR 30
#define EXTREME_MIN 0
#define EXTREME_MAX 1
#define PRINT_AMNT 25

typedef struct HuffmanTreeNode {
	char data;
	double chance;
	struct HuffmanTreeNode* left;
	struct HuffmanTreeNode* right;
} hnode;

typedef struct SplayTreeNode {
	char *title;
	char *info;
	struct SplayTreeNode* left;
	struct SplayTreeNode* right;
} snode;

/* get_msg: Gets message from user
Args:	- delim: Delimiter used to end input. It must follow a newline char (ex: if '~' is delim, "~\n" will
			terminate input). If set to null, newline alone will end string
Return: Heap-allocated string of user input */
char* get_msg(char delim);

/* huff_comp: Compresses message using dynamic huffman encoding
Args:	- out_count: Address of variable holding the amount of characters in output compressed string
		- text: Input message to be compressed
Retrun: Heap-allocated string of compressed data */	
char* huff_comp(int *char_count, char *text);

/* gen_tree: Generates huffman codes for input text
Args:	- msg: Input text for huffman code generation
Return: Heap-allocated string of huffan codes (1s and 0s of code are stored as (char)2 and (char)1 */
char* gen_tree(char *msg);

/* free_htree: Recursively frees allocated storage for huffman tree
Args:	- root: Node of tree to be freed */
void free_htree(hnode* root);

/* huff_decomp: Decompresses huffman encoded file text
Args:	- size: amount of character in file_data string
		- file: string of compressed data extracted from file
Return: Heap-allocated string of the decompressed message */
char* huff_decomp(int size, char *file_data);

/* lz_comp: Compresses text using a psuedo LZ77 compression scheme
Args:	- input: Input message to be compressed
Return: String that will contain compressed message */
char* lz_comp(char *input);

/* lz_decomp: Decompresses LZ-encoded file text
Args	- input: Input data to be decompressed
Return: String that will contain decompressed message */
char* lz_decomp(char *input);

/* gen_data_tree: Generates binary tree for data to be stored in
Args	- root: pointer to root of tree 
		- text: string to make binary tree out of */
void gen_data_tree(snode **root, char *text);

/* insert: Inserts item into splay tree
Args	- root: Root of tree being inserted into
		- insert_node: Node being inserted into tree
		- splay_status: Boolean whether inserted node is to be splayed to the root */
void insert(snode **root, snode *insert_node, int splay_status);

/* search: Search for item in the splay tree
Args:	- root: Root of tree being searched
		- search_str: Title of node being seatched for
		- level: Address of variable to store level of node in
Return: Node of searched title string, or NULL if not found */
snode* search(snode *root, char *search_str, int *level);

/* delete: Deletes node from splay tree
Args	- root: Pointer to root of tree
		- dealloc: Whether the root should be deallocated */
void delete_root(snode **root, int dealloc);

/* splay: Splays a node to the root of a tree
Args	- to_splay: Node being splayed
		- root: Root of tree of node being splayed
		- level: Level in the free of to_splay node */
void splay(snode *to_splay, snode **root, int level);

/* rotate_left: Rotate left operation of splay tree
Args	- grandparent: Grandparent node to node being rotated up
		- parent: Parent node to node being rotated up
		- child: Node being rotated up */
void rotate_left(snode *grandparent, snode *parent, snode *child);

/* rotate_right: Rotate right operation of splay tree
Args	- grandparent: Grandparent node to node being rotated up
		- parent: Parent node to node being rotated up
		- child: Node being rotated up */
void rotate_right(snode *grandparent, snode *parent, snode *child);

/* free_data_tree: Frees binary tree holding the program's data
Args	- root: Root of tree to be freed */
void free_data_tree(snode *root);

/* store_data: Stores data from splay tree into file
Args	- root: Root of splay tree
		- fp: pointer to storage file
Return: String containing the information from the tree */
char* store_data(snode *root);

/* get_depth: Gets the maximum depth of tree
Args:	- root: Root of tree to get depth of
		- level: Current recursive level of the tree */
int get_depth(snode *root, int level);

/* get_extreme: Gets the minimum or maximum of the tree
Args:	- root: Root of the tree to get extreme
		- min_max: Code whether to get the minimum or maximum value
		- parent: Pointer to parent node of min/max value
Return: Node with the min or max value */
snode* get_extreme(snode *root, snode **parent, int min_max);

/* confirm: Get user yes-or-no confirmation
Args:	- msg: Message to be displayed when asked to save
		- num_space: Number of spaces to print before printing msg
Return: True or false based on user input */
int confirm(int num_space, char *msg);

/* print_all: Prints all the nodes in the tree
Args:	- root: Root of tree to travers
		- start: The starting character to print entries at
		- end: The ending char to print entries at
Return: Boolean if print was successful */
int print_all(snode *root, char *start, char *end);

/* pre_trav: Prints a preorder traversal of the tree. For testing purposes
Args: 	- root: Root of tree
		- num: Level of tree to be printed */
void pre_trav(snode *root, int num);

/* str_cmp: Compares two strings
Args	- str_1: First string
		- str_2: Second string
Return: Comparison between the two strings */
int str_cmp(char *str_1, char *str_2);

/* str_len: Gets the length of input string
Args	- text: String input
Return: Amount of characters in string (minus the null terminator) */
int str_len(char *text);

/* str_cpy: Copies one string to another
Args	- dest: Destination string
		- src: Source string */
void str_cpy(char *dest, char *src);

/* get_num: Gets a positive number input from user
Return: Number from user. -1 as a failed input value */
int get_num(void);

/* str_nlen: Gets the length of string, terminated by either newline or null
Args	- text: Text to find length of
Return: Length of string */
int str_nlen(char *text);

#endif