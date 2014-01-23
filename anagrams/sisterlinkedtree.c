#include "sisterlinkedtree.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// static size_t block_size = 512;
#define block_size 512

struct list_node {
    struct list_node *next;
    char data[0];
};

struct slt_node {
    struct slt_node *sister;
    struct slt_node *child;
    struct list_node *data;
    char key;
};

struct slt_tree {
    struct slt_node *root;
    struct slt_tree *nextblock;
    struct slt_tree *lastblock;
    size_t count;
    struct slt_node nodes[block_size];
};

struct slt_tree *slt_new() {
    struct slt_tree *tree = calloc(1, sizeof(struct slt_tree));
    tree->lastblock = tree;
    return tree;
}

static struct list_node *new_list_node(const char *data, 
                                       struct list_node *next) {
    struct list_node *node = 
	calloc(1, sizeof(struct list_node) + strlen(data) + 1);
    node->next = next;
    strcpy(node->data, data);
    return node;
}

static struct slt_node *new_slt_node(struct slt_tree *tree, char key, 
				     struct slt_node *sister) {
    struct slt_tree *block = tree->lastblock;

    if (block->count == block_size) {
	block->nextblock = calloc(1, sizeof(struct slt_tree));
	block = block->nextblock;
	tree->lastblock = block;
    }

    struct slt_node *node = &block->nodes[(block->count)++];
    node->key = key;
    node->sister = sister;

    return node;
}

static struct slt_node *slt_add_(struct slt_tree *tree, 
				 struct slt_node *root,
				 const char *key,
				 const char *data) {
    assert(key != NULL);
    assert(data != NULL);

    struct slt_node *next = NULL;
    if (root == NULL || key[0] < root->key ) {
        next = root = new_slt_node(tree, key[0], root);
    } else {
        next = root;
        while (next->sister != NULL && key[0] >= next->sister->key )
            next = next->sister;
        if (key[0] > next->key) {
            next->sister = new_slt_node(tree, key[0], next->sister);
            next = next->sister;
        }
    }

    if (key[1] == '\0') {
        // Push to front is the easiest way
        next->data = new_list_node(data, next->data);
    } else {
        next->child = slt_add_(tree, next->child, key + 1, data);
    }
    return root;
}

void slt_add(struct slt_tree *tree, 
	     const char *key,
	     const char *data) {
    tree->root = slt_add_(tree, tree->root, key, data);
}

void print_node(const struct list_node *node) {
    while (node != NULL) {
        printf("%s ", node->data);
        node = node->next;
    }
    printf("\n");
}

void slt_print(const struct slt_node *node) {
    if (node->data != NULL) 
        print_node(node->data);
    if (node->child != NULL)
        slt_print(node->child);
    if (node->sister != NULL)
        slt_print(node->sister);
}

void slt_print_anagrams(const struct slt_tree *tree) {
    while (tree != NULL) {
	for (size_t i = 0; i < tree->count; i++) {
	    const struct slt_node *node = &(tree->nodes[i]);
	    if (node->data != NULL && node->data->next != NULL)
		print_node(node->data);
	}
	tree = tree->nextblock;
    }
}
