#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAXC 128

typedef struct TrieNode
{
    struct TrieNode *children[MAXC];
    int end_index;
    int length;
} TrieNode;

TrieNode *getNode();
void insert(TrieNode *root, const char *key, const int index);
bool search(struct TrieNode *root, const char *key, const int length);
TrieNode *next_node(struct TrieNode *node, const char ch);
