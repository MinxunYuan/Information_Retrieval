#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "invertedIndex.h"

#define MAX_WD_SIZE 100
#define MAX_LINE_SIZE 512
#define COLLECTION_FILE_NAME "collection.txt"

#define isInValidPunc(c) ((c) == '.' || (c) == ',' || (c) == ';' || (c) == '?')
/// Helper function prototypes

char* normaliseWord(char* str);
struct InvertedIndexNode* createInvertedIndexNode(char* wd, char* fileName, double tf);
// struct InvertedIndexNode* createInvertedIndexNode(char* wd, char* fileName);
// 在某一个word所在的InvertedIndex的FileList中找是否有一个FileListNode，其filename为fileName
struct FileListNode* findFileListNode(FileList flst, char* fileName);
struct FileListNode* createFileListNode(char* fileName, double tf);

FileList insertFileNode(FileList flst, char* fileName, double tf);
InvertedIndexBST invertedIndexInsert(InvertedIndexBST root, char* word, char* fileName, double tf);
typedef struct wf {
    char* word;
    int freq;
    // struct wf* next;
} wf;


int main() {
    // 为啥char *s = "  123  "这样就不行啊
    // char s[MAX_WD_SIZE];

    // sprintf(s, "%s", "abc.net.au.");

    // s = normaliseWord(s);
    // printf("%s\n", normaliseWord(s));

    InvertedIndexBST inverted_index = generateInvertedIndex(COLLECTION_FILE_NAME);
    // printf("\n");
    // printf("%s %s\n", inverted_index->right->word, inverted_index->right->fileList->filename);

    return 0;
}

/**
 * This function opens the collection file with the given name, and then
 * generates an inverted index from those files listed in the collection
 * file,  as  discussed  in  the spec. It returns the generated inverted
 * index.
 */
InvertedIndexBST generateInvertedIndex(char* collectionFilename) {
    InvertedIndexBST bst = NULL;

    // reads data from a given collection of files in collection.txt
    char file_names[MAX_WD_SIZE];
    FILE* fp = fopen(collectionFilename, "r");
    int c, i = 0;
    // 把空格换行什么的去掉，读到一个buffer里面
    while ((c = fgetc(fp)) != EOF) {
        if (!isspace(c)) file_names[i++] = c;
    }
    // finish reading collectionFilename
    fclose(fp);

    // 搓一个tok可以是str的strtok()
    // char* pos;
    char* pos = strstr(file_names, ".txt");
    char file_name[MAX_WD_SIZE];

    // while (ch < fileNanes + strlen(fileNanes)) {
    for (char* ch = file_names; pos && ch < file_names + strlen(file_names); ch = pos + 4, pos = strstr(ch, ".txt")) {
        memset(file_name, 0, sizeof(file_name));

        // len(each_file) without suffix
        int len = pos - ch;
        for (int i = 0; i < len; i++) file_name[i] = ch[i];

        strcat(file_name, ".txt"); // file1 -> file1.txt
        // read each file in collectionFilename
        FILE* each_file = fopen(file_name, "r");

        char line[MAX_LINE_SIZE];

        int wd_cnt = 0, wf_cnt = 0; // each_file里单词总数wd_cnt去重后的wf_cnt用来统计词频
        // set up variables for iterating over tokens ' ' and '\n'
        char *wd, *input;

        wf wfs[100]; // TODO:
        memset(wfs, 0, sizeof(wfs));

        while (fgets(line, MAX_LINE_SIZE, each_file) != NULL) {
            input = strdup(line); // backup
            while ((wd = strsep(&input, " \n")) != NULL) {
                // eliminate blank space and line
                if ((isalnum(*wd))) {
                    wd_cnt++;
                    // normalise word before add
                    wd = normaliseWord(wd);
                    // 先去wfs里遍历一下康康有没有之前见过的
                    int wf_num;
                    for (wf_num = 0; wf_num < wf_cnt; wf_num++) {
                        if (strcmp(wfs[wf_num].word, wd) == 0) {
                            wfs[wf_num].freq++;
                            break;
                        }
                    }

                    // no
                    if (wf_num == wf_cnt) {
                        wf_cnt++;
                        wfs[wf_num].word = strdup(wd);
                        wfs[wf_num].freq++;
                    }
                }
            }
        }

        // printf("wd_cnt %d wf_cnt %d\n", wd_cnt, wf_cnt);
        for (int i = 0; i < wf_cnt; i++) {
            // create InvertedIndexNode
            bst = invertedIndexInsert(bst, wfs[i].word, file_name, (double)wfs[i].freq / wd_cnt);
            // printf("%s %d, tf=%f\n", wfs[i].word, wfs[i].freq, (double)wfs[i].freq / wd_cnt);
        }
        // grab next file
        wf_cnt = wd_cnt = 0;
        fclose(each_file);
    }
    return bst;
}

/**
 * normalization
 * modify the given string - do not create a copy of it.
 */
char* normaliseWord(char* str) {
    // removing leading and trailing spaces
    char* i = str;
    char* j = i + strlen(str) - 1;
    while (isspace(*i)) i++;
    while (isspace(*j)) j--;
    *(++j) = 0;

    // converting all characters to lowercase
    char* k;
    for (k = i; k < i + strlen(i); k++)
        *k = tolower(*k);
    k--;

    // remove the {. , ; ?}, if they appear at the end of a word.
    if (isInValidPunc(*k)) *k = 0;

    return i;
}

// InvertedIndexBST findInvertedIndexNodeByWord(InvertedIndexBST root, char* word) {
//     if (!root) return NULL;
//     printf("findInvertedIndexNodeByWord");
//     if (root->left && strcmp(word, root->word) > 0)
//         return findInvertedIndexNodeByWord(root->right, word);
//     else if (root->right && strcmp(word, root->word) < 0)
//         return findInvertedIndexNodeByWord(root->left, word);
//     else
//         return root;
// }

InvertedIndexBST invertedIndexInsert(InvertedIndexBST root, char* word, char* fileName, double tf) {
    // 如果不空，先去找找有没有word所在的InvertedIndexNode
    // InvertedIndexBST node = findInvertedIndexNodeByWord(root, word);
    InvertedIndexBST node = root;
    // if (node) {
    //     // 有的话，就只需要改其fileList
    //     // fileList没dummyhead，涉及到改指针值，要么返回地址，要么传指针的地址进去
    //     root->fileList = insertFileNode(root->fileList, fileName, tf);
    //     return root;
    // }
    // printf("get");

    // create a new InvertedIndexNode
    // 找一个叶子
    if (!root) {
        // printf("get1");
        return createInvertedIndexNode(word, fileName, tf);
    }
    // printf("find word\n");
    if (strcmp(word, root->word) < 0)
        root->left = invertedIndexInsert(root->left, word, fileName, tf);
    else if (strcmp(word, root->word) > 0)
        root->right = invertedIndexInsert(root->right, word, fileName, tf);
    else
        root->fileList = insertFileNode(root->fileList, fileName, tf);

    return root;
}

/**
 * Creates node which contains `wd` in fileName
 */
// struct InvertedIndexNode* createInvertedIndexNode(char* wd, char* fileName) {
struct InvertedIndexNode* createInvertedIndexNode(char* wd, char* fileName, double tf) {
    // printf("createInvertedIndexNode\n");
    struct InvertedIndexNode* node = (struct InvertedIndexNode*)malloc(sizeof(struct InvertedIndexNode));

    // node->word = strdup(wd);
    // node->left = NULL;
    // node->right = NULL;
    // node->fileList = NULL;
    // node->fileList = insertFileNode(node->fileList, fileName, tf);
    (*node) = (struct InvertedIndexNode){
        .word = strdup(wd),
        .fileList = insertFileNode(node->fileList, fileName, tf),
        .left = NULL,
        .right = NULL,
    };
    return node;
}

struct FileListNode* createFileListNode(char* fileName, double tf) {
    struct FileListNode* node = malloc(sizeof(struct FileListNode));
    (*node) = (struct FileListNode){
        .filename = strdup(fileName),
        .tf = tf,
        .next = NULL};
    return node;
}

/**
 *
 * 将FileNode插入InvertedIndexNode
 */
FileList insertFileNode(FileList flst, char* fileName, double tf) {
    // 遍历each_file已经对word去重，所以放心insert
    // if (findFileListNode(flst, fileName)) return;
    // printf("insertFileNode,");

    if (!flst) return createFileListNode(fileName, tf);
    // printf("create a FileNode\n");
    FileList head = flst, prev;

    // 如果fileName大于flst->filename，就往后走
    while (flst && strcmp(fileName, flst->filename) > 0) {
        prev = flst;
        flst = flst->next;
    }
    // 出来之后flst是小于
    struct FileListNode* node = createFileListNode(fileName, tf);
    if (flst == head) {
        node->next = head;
        return node;
    }
    node->next = flst;
    prev->next = node;

    return head;
}

struct FileListNode* findFileListNode(FileList flst, char* fileName) {
    while (flst) {
        if (strcmp(flst->filename, fileName) == 0)
            return flst;
        else
            flst = flst->next;
    }
    return NULL;
}