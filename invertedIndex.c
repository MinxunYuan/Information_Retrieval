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

InvertedIndexBST generateInvertedIndex(char* collectionFilename) {
	InvertedIndexBST bst;
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

		while (fscanf(colFiles, "%s", fileName) != EOF) {
			printf("%s\n", fileName);

			// read word from each fileName
			FILE* eachFile = fopen(fileName, "r");

			// assume all files listed collectionFilename are valid
			char wd[MAX_WD_LEN];
			// char* str = wd;
			while (fscanf(eachFile, "%s", wd) != EOF) {
				printf("%s - ", wd);
				// remove punctuation marks(. , : ; ? *) from the end of the words
				char* word = normaliseWord(wd);
				// If a word becomes empty after removing punctuation marks, then it should not count as a word
				if (strlen(word) == 0) continue;


			}
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

// Part 2

TfIdfList searchOne(InvertedIndexBST tree, char* searchWord, int D) {
	return NULL;
}

TfIdfList searchMany(InvertedIndexBST tree, char* searchWords[], int D) {
	return NULL;
}

void freeTfIdfList(TfIdfList list) {

}

