// COMP2521 Assignment 1

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "invertedIndex.h"

#define MAX_WD_LEN 100
#define PUNC_LEN 6

char* normaliseWord(char* str);
bool isPunc(char ch);

// Part 1

static char punc[] = { '.',',',':',';','?','*' };

typedef struct wf {
	int cnt;
	char* word;
	struct wf* next; // linked list
} wf;
typedef wf* wfs;

wf* create_wf(char*, wf*);
wf* add_wf(wf*, char*);
wf* find_wf(wf*, char*);
void print_wfs(wf*);
void free_wfs(wf*);

InvertedIndexBST insertInvertedIndexNode(InvertedIndexBST root, char* word, char* fileName, double tf);
struct InvertedIndexNode* createInvertedIndexNode(char* wd, char* fileName, double tf);

FileList insertFileNode(FileList flst, char* fileName, double tf);

void printFileList(FileList head, char* filename);
int countFileList(FileList head);
struct FileNode* createFileListNode(char* fileName, double tf, FileList next);

InvertedIndexBST findInvertedIndexNodeByWord(InvertedIndexBST tree, char* searchWord);
TfIdfList createTfIdfNode(char* filename, double tfidfSum, TfIdfList next);
TfIdfList insertTfIdfNode(TfIdfList head, char* filename, double tfidf);

double findTfIdfNodeByFilename(TfIdfList* head, char* filename);

InvertedIndexBST generateInvertedIndex(char* collectionFilename) {
	InvertedIndexBST invertedIndex = NULL;
	// scan file name from collectionFilename

	/*
	Assumptions:
		Filenames do not contain whitespace characters.
		Filenames do not contain whitespace characters.
		All filenames in the collection file are distinct.
		There is no limit on the number of filenames in the collection file.
		There is no limit on the number of words in a file.
		The printInvertedIndex function should always behave as if the given file was empty or didn't exist
	*/

	FILE* colFiles = fopen(collectionFilename, "r");
	if (colFiles) {
		// file exists
		char fileName[MAX_WD_LEN];
		memset(fileName, 0, MAX_WD_LEN);

		// There is no limit on the number of words in a file.
		// for eachFile calculate tf(word, eachFile) for InvertedIndexNode

		// read eachFile in collectionFilename
		while (fscanf(colFiles, "%s", fileName) != EOF) {
			// printf("%s\n", fileName);
			// read word from each fileName
			FILE* eachFile = fopen(fileName, "r");

			// assume all files listed collectionFilename are valid
			char wd[MAX_WD_LEN];
			memset(wd, 0, MAX_WD_LEN);
			// char* str = wd;
			int wd_num = 0; // total num of word in eachFile

			// read words in eachFile
			wf* head = NULL;
			while (fscanf(eachFile, "%s", wd) != EOF) {
				// tf(t, d) = frequency of t in d / num of word in d

				// remove punctuation marks(. , : ; ? *) from the end of the words
				char* word = normaliseWord(wd);
				// printf("%s - ", word);

				// If a word becomes empty after removing punctuation marks, then it should not count as a word
				if (strlen(word)) {
					wd_num++;

					// add word to wf list
					wf* cur_wf = find_wf(head, word); // check whether the word exists
					if (cur_wf) cur_wf->cnt++;
					else head = add_wf(head, word);
				}
			}
			// add words to invertedIndex
			for (wf* cur = head; cur; cur = cur->next)
				invertedIndex = insertInvertedIndexNode(invertedIndex, cur->word, fileName, (double) cur->cnt / wd_num);
			free_wfs(head);
			fclose(eachFile);
		}
		fclose(colFiles);

	} else {
		fprintf(stderr, "no such file %s\n", collectionFilename);
	}
	return invertedIndex;
}

/**
 * Creates node which contains `wd` in fileName
 */
struct InvertedIndexNode* createInvertedIndexNode(char* wd, char* fileName, double tf) {
	// printf("createInvertedIndexNode\n");
	struct InvertedIndexNode* node = (struct InvertedIndexNode*) malloc(sizeof(struct InvertedIndexNode));
	memset(node, 0, sizeof(struct InvertedIndexNode)); // fix bug: Uninitialised value was created by a heap allocation
	(*node) = (struct InvertedIndexNode){
		.word = strdup(wd),
		.fileList = insertFileNode(node->fileList, fileName, tf),
		.left = NULL,
		.right = NULL,
	};
	// node->fileList = insertFileNode(node->fileList, fileName, tf);
	return node;
}

InvertedIndexBST insertInvertedIndexNode(InvertedIndexBST root, char* word, char* fileName, double tf) {
	if (!root)
		return createInvertedIndexNode(word, fileName, tf);
	// printf("find word\n");
	if (strcmp(word, root->word) < 0)
		root->left = insertInvertedIndexNode(root->left, word, fileName, tf);
	else if (strcmp(word, root->word) > 0)
		root->right = insertInvertedIndexNode(root->right, word, fileName, tf);
	else
		root->fileList = insertFileNode(root->fileList, fileName, tf);

	return root;
}

struct FileNode* createFileListNode(char* fileName, double tf, FileList next) {
	struct FileNode* node = malloc(sizeof(struct FileNode));
	(*node) = (struct FileNode){
		.filename = strdup(fileName),
		.tf = tf,
		.next = next };
	return node;
}


FileList insertFileNode(FileList flst, char* fileName, double tf) {
	if (!flst) return createFileListNode(fileName, tf, NULL);
	FileList head = flst, prev = NULL;

	// each file list must be sorted alphabetically (ascending) by filename
	while (flst && strcmp(fileName, flst->filename) > 0) {
		prev = flst;
		flst = flst->next;
	}

	struct FileNode* node = createFileListNode(fileName, tf, NULL);

	if (flst == head) {
		node->next = head;
		return node;
	}
	node->next = flst;
	prev->next = node;

	return head;
}

struct FileNode* findFileListNode(FileList flst, char* fileName) {
	while (flst) {
		if (strcmp(flst->filename, fileName) == 0)
			return flst;
		else
			flst = flst->next;
	}
	return NULL;
}

void printInvertedIndex(InvertedIndexBST tree, char* filename) {
	if (!tree) return;
	FILE* fp = fopen(filename, "a");
	printInvertedIndex(tree->left, filename);
	fprintf(fp, "%s", tree->word);
	FileList head = tree->fileList;
	while (head) {
		fprintf(fp, " %s (%.7f)", head->filename, head->tf);
		head = head->next;
	}
	fprintf(fp, "\n");
	fclose(fp);
	printInvertedIndex(tree->right, filename);
}


void freeFileList(FileList head) {
	if (head) {
		freeFileList(head->next);
		free(head->filename);
		free(head);
	}
}

void freeInvertedIndex(InvertedIndexBST tree) {
	if (tree) {
		freeInvertedIndex(tree->left);
		freeInvertedIndex(tree->right);
		freeFileList(tree->fileList);
		free(tree->word);
		free(tree);
	}
}


bool isPunc(char ch) {
	for (int i = 0; i < 6; i++)
		if (ch == punc[i]) return true;
	return false;
}

/**
 * @brief
 * Converting all letters to lowercase
 * and removing the following punctuation marks from the end of the words
 * without modify the original string
 * @param str given word
 * @return a char* referring to the normalised word
 */
char* normaliseWord(char* str) {
	// converting all letters to lowercase
	char* end = str + strlen(str) - 1;
	for (int i = 0; str[i]; i++) {
		str[i] = tolower(str[i]);
	}

	// remove tailing specified punctuation
	while (isPunc(*end)) end--;
	*(++end) = 0;
	return str;
}

wf* create_wf(char* word, wf* next) {
	wf* node = malloc(sizeof(wf));
	*node = (wf){
		.word = strdup(word),
		.cnt = 1,
		.next = next,
	};
	return node;
}


wf* add_wf(wf* head, char* word) {
	if (!head) return create_wf(word, NULL);
	return create_wf(word, head);
}

wf* find_wf(wf* head, char* word) {
	while (head) {
		if (strcmp(head->word, word) == 0) return head;
		head = head->next;
	}
	return NULL;
}

void print_wfs(wf* head) {
	while (head) {
		printf("%s-%d ", head->word, head->cnt);
		head = head->next;
	}
	printf("\n");
}

void free_wfs(wf* head) {
	if (head) {
		free_wfs(head->next);
		free(head->word);
		free(head);
	}
}

// Part 2

TfIdfList searchOne(InvertedIndexBST tree, char* searchWord, int D) {
	// init a TfIdfList
	TfIdfList lst = NULL;
	// find searchWord in tree
	InvertedIndexBST bst = findInvertedIndexNodeByWord(tree, searchWord);

	// idf(t, D) = log(total number of documents / number of documents containing the term)

	if (!bst) return NULL;
	int docNum = countFileList(bst->fileList);
	// log10(1) == 0
	if (docNum != D) {
		double idf = log10((double) D / (double) docNum);
		// traverse fileList
		for (FileList head = bst->fileList; head; head = head->next)
			lst = insertTfIdfNode(lst, head->filename, head->tf * idf);
	}
	return lst;
}


TfIdfList insertTfIdfNode(TfIdfList head, char* filename, double tfidf) {
	// The list must be in descending order of tf-idf. 
	// Files with the same tf-idf should be ordered alphabetically by filename in ascending order.
	if (head == NULL) return createTfIdfNode(filename, tfidf, NULL);

	if (head->tfIdfSum > tfidf)
		head->next = insertTfIdfNode(head->next, filename, tfidf);
	else if (head->tfIdfSum < tfidf)
		return createTfIdfNode(filename, tfidf, head);
	else {
		if (strcmp(filename, head->filename) > 0)
			head->next = insertTfIdfNode(head->next, filename, tfidf);
		else
			return createTfIdfNode(filename, tfidf, head);
	}
	return head;
}

TfIdfList createTfIdfNode(char* filename, double tfidfSum, TfIdfList next) {
	TfIdfList node = malloc(sizeof(struct TfIdfNode));
	memset(node, 0, sizeof(struct TfIdfNode));
	(*node) = (struct TfIdfNode){
		.filename = strdup(filename),
		.tfIdfSum = tfidfSum,
		.next = next,
	};
	return node;
}

int countFileList(FileList head) {
	int num = 0;
	while (head) {
		num++;
		head = head->next;
	}
	return num;
}

InvertedIndexBST findInvertedIndexNodeByWord(InvertedIndexBST tree, char* searchWord) {
	if (!tree) return NULL;
	if (strcmp(searchWord, tree->word) < 0)
		return findInvertedIndexNodeByWord(tree->left, searchWord);
	else if (strcmp(searchWord, tree->word) > 0)
		return findInvertedIndexNodeByWord(tree->right, searchWord);
	else return tree;
}

TfIdfList searchMany(InvertedIndexBST tree, char* searchWords[], int D) {
	TfIdfList lst = NULL;
	// scan words
	for (int i = 0; searchWords[i]; i++) {
		if (i == 0) {
			lst = searchOne(tree, searchWords[i], D);
		} else {
			InvertedIndexBST bst = NULL;
			bst = findInvertedIndexNodeByWord(tree, searchWords[i]);
			if (!bst) continue; // searchWords[i] doesn't appears in bst

			int docNum = countFileList(bst->fileList);
			if (docNum == D)  continue; // lg1 = 0

			double idf = log10((double) D / (double) docNum);
			
			// All files containing searchWords[i]
			for (FileList head = bst->fileList; head; head = head->next) {
				// check filename has already appears in the TfIdfList
				double prevTfIdf = findTfIdfNodeByFilename(&lst, head->filename);
				double curTfIdf = head->tf * idf;
				// calculate tfidf based on the previous check
				if (prevTfIdf != 0) {
					curTfIdf += prevTfIdf;
				}
				// reconstruct TfIdfList
				lst = insertTfIdfNode(lst, head->filename, curTfIdf);
			}
		}
	}
	return lst;
}

/**
 * @brief
 * Find whether fileName appears in TfIdfList
 * and delete the existed TfIdfNode
 * @return The tf-idf value of the existed TfIdfNode
 */
double findTfIdfNodeByFilename(TfIdfList* head, char* filename) {
	TfIdfList cur = *head, prev = NULL;
	while (cur) {
		if (strcmp(cur->filename, filename) == 0) {
			double tfidf = cur->tfIdfSum;
			if (cur == (*head)) {
				// delete head, modify list from callee
				*head = (*head)->next;
			} else {
				// not head
				prev->next = cur->next;
			}
			free(cur->filename);
			free(cur);
			return tfidf;
		}
		prev = cur;
		cur = cur->next;
	}
	// filename doesn't appear in the given list
	return 0.0;
}

void freeTfIdfList(TfIdfList list) {
	if (list) {
		freeTfIdfList(list->next);
		free(list->filename);
		free(list);
	}
}

