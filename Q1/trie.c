#include "trie.h"

TrieNode *getNode()
{
    TrieNode *n = (TrieNode *)malloc(sizeof(TrieNode));
    for (int i = 0; i < MAXC; i++)
        n->children[i] = NULL;
    n->end_index = -1;
    n->length = 0;
    return n;
}

void insert(TrieNode *root, const char *key, const int index)
{
    TrieNode *temp = root;

    int i;
    for (i = 0; key[i] != '\0'; i++)
    {
        if (!temp->children[key[i]])
            temp->children[key[i]] = getNode();

        temp = temp->children[key[i]];
    }
    temp->end_index = index;
    temp->length = i;
}

bool search(struct TrieNode *root, const char *key, const int length)
{
    TrieNode *temp = root;

    for (int i = 0; key[i] != '\0'; i++)
    {
        if (!temp->children[key[i]])
            return false;
        temp = temp->children[key[i]];
    }

    return (!temp && (temp->end_index >= 0));
}

TrieNode *next_node(struct TrieNode *node, const char ch)
{
    if (!node || !(node->children[ch]))
        return NULL;
    return node->children[ch];
}