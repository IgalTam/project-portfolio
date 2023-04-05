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

node *makeNode(int c, int freq) {
  node *nPtr;
  if( !(nPtr = malloc(sizeof(node)))) {
    perror("malloc");
    exit(1);
  }
  nPtr->val = c;
  nPtr->freq = freq;
  nPtr->left = nPtr->right = NULL;
  return nPtr;
}

int compNode(const void *node1p, const void *node2p) {
  /* comparator function for qsort on list of nodes*/
  node *node1 = *(node **)node1p;
  node *node2 = *(node **)node2p;

  if(node1->freq == node2->freq)
    return node1->val - node2->val;
  return node1->freq - node2->freq;
}

node *buildTree(node **list, int numNodes) {
  /* construct huffman tree and return its root*/
  int i, listPos = 1;
  node *newNode, *tempNode;

  while(numNodes > 1) {
    /* create new node combining two smallest nodes*/
    newNode = makeNode(list[0]->val, list[0]->freq + list[1]->freq);
    newNode->left = list[0];
    newNode->right = list[1];
    list[0] = newNode;
    list[1] = NULL;

    /* stop if there are only 2 nodes in list*/
    if(numNodes == 2) {
      list[0] = makeNode(newNode->val, newNode->freq);
      list[0]->left = newNode->left;
      list[0]->right = newNode->right;
      break;
    }
      
    /* resort list*/
    for(i = 2; i < numNodes; i++) {
      list[i-1] = list[i];
    }
    list[i-1] = NULL;
    while(list[listPos] && (newNode->freq > list[listPos]->freq)) {
      /* swap nodes until sorted*/
      tempNode = list[listPos];
      list[listPos] = newNode;
      list[listPos-1] = tempNode;
      listPos++;
    }
    listPos = 1;
    numNodes--;
  }
  free(newNode);
  return list[0];
}

char *crtCodeStr(char *code, int size) {
  /* allocates memory for a new copy of code*/
  char *outStr;
  int i;

  if(! (outStr = malloc(size))) {
    perror("malloc");
    exit(1);
  }
  for(i = 0; i < size/sizeof(char); i++)
    outStr[i] = '\0';
  return outStr;
}

void extractCodes(char **list, node *node, char *code, int size) {
  /* recursively DFS into tree, inserting huffman codes into list*/
  char *leftCode, *rightCode;
  int i;
  if(!node->left && !node->right) {
    strcat(code, "\0");
    list[(int)node->val] = crtCodeStr(code, size);
    for(i = 0; i < strlen(code); i++) {
      list[(int)node->val][i] = code[i];
    }
  }

  if(node->left) {
    leftCode = crtCodeStr(code, size);
    strcat(leftCode, code);
    strcat(leftCode, "0");
    extractCodes(list, node->left, leftCode, size);
  }
  if(node->right) {
    rightCode = crtCodeStr(code, size);
    strcat(rightCode, code);
    strcat(rightCode, "1");
    extractCodes(list, node->right, rightCode, size);
  }
  free(code);
}

void freeTree(node *node) {
  /* free all nodes in tree*/
  if(node->left)
    freeTree(node->left);
  if(node->right)
    freeTree(node->right);
  free(node);
}

void dt(node *r, int d) {
  /* prints out tree sideways
   used only for testing purposes, is not part of output of this program*/
  if(r) {
    if(r->left)
      dt(r->left, d+1);
    printf("%*s 0x%02x\n", d*3, "", r->val);
    if(r->right)
      dt(r->right, d+1);
  }
}
