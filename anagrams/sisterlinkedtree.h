#ifndef SISTERLINKEDTREE_H
#define SISTERLINKEDTREE_H

struct list_node;
struct slt_node;
struct slt_tree;

struct slt_tree *slt_new();
void slt_add(struct slt_tree *tree,
	     const char *key,
	     const char *data);

void slt_print(const struct slt_node *node);
void slt_print_anagrams(const struct slt_tree *tree);

#endif
