#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>

typedef struct node_ht node;
struct node_ht {
  unsigned char val;
  int freq;
  node *left;
  node *right;
};

node *makeNode(int c, int freq);
int compNode(const void *node1p, const void *node2p);
node *buildTree(node **list, int numNodes);
char *crtCodeStr(char *code, int size);
void extractCodes(char **list, node *node, char *code, int size);
void freeTree(node *node);
void dt(node *r, int d);
