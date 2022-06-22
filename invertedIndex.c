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

InvertedIndexBST generateInvertedIndex(char* collectionFilename) {
	InvertedIndexBST invertedIndex;
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

		// There is no limit on the number of words in a file.
		// for eachFile calculate tf(word, eachFile) for InvertedIndexNode
		wf* head = NULL;
		int cnt = 0;
		while (fscanf(colFiles, "%s", fileName) != EOF) {
			printf("%s\n", fileName);

			// read word from each fileName
			FILE* eachFile = fopen(fileName, "r");

			// assume all files listed collectionFilename are valid
			char wd[MAX_WD_LEN];
			// char* str = wd;
			while (fscanf(eachFile, "%s", wd) != EOF) {
				int wd_num = 0;
				// tf(t, d) = frequency of t in d / num of word in d

				// printf("%s - ", wd);
				// remove punctuation marks(. , : ; ? *) from the end of the words
				char* word = normaliseWord(wd);
				// If a word becomes empty after removing punctuation marks, then it should not count as a word
				if (strlen(word) == 0) continue;

				// add word to wf list
				wf* cur_wf = find_wf(head, word); // check whether the word exists
				if (cur_wf) cur_wf->cnt++;
				else {
					head = add_wf(head, word);
					cnt++;
				}
				// printf("%s\n", head->word);

				// add word to invertedIndex

				// for (wf* cur = head; cur; cur = cur->next)
				// 	invertedIndex = insertInvertedIndexNode(invertedIndex, word, fileName, cur->cnt / wd_num);


			}
			print_wfs(head);


			free_wfs(head);
			fclose(eachFile);
		}
		fclose(colFiles);

	} else {
		fprintf(stderr, "no such file %s\n", collectionFilename);
	}
	return NULL;
}

void printInvertedIndex(InvertedIndexBST tree, char* filename) {

}

void freeInvertedIndex(InvertedIndexBST tree) {

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
	char* i = str;
	char* j = str + strlen(str) - 1;
	while (i <= j) *i++ = tolower(*i);

	// remove tailing specified punctuation
	while (isPunc(*j)) j--;
	// j++;
	*(++j) = 0;
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
		printf("%s %d\n", head->word, head->cnt);
		head = head->next;
	}
}

void free_wfs(wf* head) {
	wf* prev;
	while (head) {
		prev = head;
		head = head->next;
		free(prev->word);
		free(prev);
		prev->word = NULL;
		prev = NULL;
	}
}

// Part 2

TfIdfList searchOne(InvertedIndexBST tree, char* searchWord, int D) {
	return NULL;
}

TfIdfList searchMany(InvertedIndexBST tree, char* searchWords[], int D) {
	return NULL;
}

void freeTfIdfList(TfIdfList list) {

}

