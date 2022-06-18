#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "invertedIndex.h"

#define MAX_WD_SIZE 100
#define COLLECTION_FILE_NAME "collection1.txt"

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

// wf* wf_create(char* word, wf* next) {
//     wf* node = malloc(sizeof(wf));
//     (*node) = (wf){
//         .word = strdup(word),
//         .freq = 1,
//         .next = next,
//     };
//     return node;
// }

// void wf_add(wf* head, char* word) {
//     wf* cur = head;
//     while (cur) {
//         if (strcmp(cur->word, word)) {
//             cur->freq++;
//             return;
//         }
//         else
//             cur = cur->next;
//     }
//     head

// }

int main() {
    // 为啥char *s = "  123  "这样就不行啊
    char s[MAX_WD_SIZE];

    // sprintf(s, "%s", "abc.net.au.");

    // s = normaliseWord(s);
    // printf("%s\n", normaliseWord(s));
    // InvertedIndexBST inverted_index = generateInvertedIndex(COLLECTION_FILE_NAME);

    printf("testing generateInvertedIndex\n");

    // reads data from a given collection of files in collection.txt
    char fileNanes[MAX_WD_SIZE];
    FILE* fp = fopen(COLLECTION_FILE_NAME, "r");
    int c, i = 0;
    while ((c = fgetc(fp)) != EOF) {
        if (!isspace(c)) {
            fileNanes[i++] = c;
        }
    }
    fclose(fp);

    // 搓一个tok可以是str的strtok
    char* pos;
    char fileName[MAX_WD_SIZE];

    // while (ch < fileNanes + strlen(fileNanes)) {
    for (char* ch = fileNanes; ch < fileNanes + strlen(fileNanes);) {
        memset(fileName, 0, sizeof(fileName));
        pos = strstr(ch, ".txt");
        if (!pos) break;

        int len = pos - ch;

        // pos-ch为去掉.txt的那一段fileName（我寻思不可能有.txt.txt这种东西吧）
        for (int i = 0; i < len; i++) fileName[i] = ch[i];
        // printf("%d", strlen(fileName));

        // 到这里相当于就已经有fileName了，然后直接strcat
        strcat(fileName, ".txt");
        // 拿到FileName之后进去读
        printf("%s\n", fileName);

        FILE* each_file = fopen(fileName, "r");
        // no need to impose any restriction on a line length
        char line[512];
        // each_file里单词总数wd_cnt去重后的wf_cnt
        int wd_cnt = 0, wf_cnt = 0;
        // set up variables for iterating over tokens ' ' '\n'
        char *wd, *input;

        // 算了，each_file直接配一个wf数组统计词频算tf好了
        wf wfs[100];
        memset(wfs, 0, sizeof(wfs));

        // each_file的每一行
        while (fgets(line, 512, each_file) != NULL) {
            input = strdup(line); // backup
            while ((wd = strsep(&input, " \n")) != NULL) {
                // eliminate blank space and line
                if ((isalnum(*wd))) {
                    wd_cnt++;

                    wd = normaliseWord(wd);
                    // printf("%s\n", wd);
                    // 先去wfs里遍历一下康康有没有之前见过的
                    int wf_num;
                    for (wf_num = 0; wf_num < wf_cnt; wf_num++) {
                        if (strcmp(wfs[wf_num].word, wd) == 0) {
                            wfs[wf_num].freq++;
                            break;
                        }
                    }

                    // 没有
                    if (wf_num == wf_cnt) {
                        wf_cnt++;
                        wfs[wf_num].word = strdup(wd);
                        wfs[wf_num].freq++;
                    }

                    // Before inserting words in your inverted index, you need to normaliseWord
                    // wd = normaliseWord(wd);
                    // add word to fileListNode
                }
            }
        }
        printf("%d %d\n", wd_cnt, wf_cnt);
        for (int i = 0; i < wf_cnt; i++) {
            // create InvertedIndexNode
            // 到这里
            // bst = invertedIndexInsert(bst, wfs[i].word, fileName, (double)wfs[i].freq / wd_cnt);
            printf("%s %d, tf=%f\n", wfs[i].word, wfs[i].freq, (double)wfs[i].freq / wd_cnt);
        }

        // grab next file
        wf_cnt = wd_cnt = 0;
        fclose(each_file);
        // find next file name
        ch = pos + 4;
    }

    InvertedIndexBST bst = NULL;

    puts("\ntesting bst...");
    // same word
    bst = invertedIndexInsert(bst, "ha", "file1.txt", 0.01);
    bst = invertedIndexInsert(bst, "hahaha", "file2.txt", 0.01);
    bst = invertedIndexInsert(bst, "hahaha", "file1.txt", 0.01);

    // same word, different file
    printf("%s %s\n", bst->word, bst->fileList->filename);
    printf("%s %s\n", bst->right->word, bst->right->fileList->filename);

    return 0;
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

/**
 * This function opens the collection file with the given name, and then
 * generates an inverted index from those files listed in the collection
 * file,  as  discussed  in  the spec. It returns the generated inverted
 * index.
 */
// InvertedIndexBST generateInvertedIndex(char *collectionFilename) {

// }

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
    printf("find word\n");
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
    printf("createInvertedIndexNode\n");
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
    printf("insertFileNode,");

    if (!flst) return createFileListNode(fileName, tf);
    printf("create a FileNode\n");
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