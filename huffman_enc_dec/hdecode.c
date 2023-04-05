#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "huffTree.h"

#define CHAR_COUNT 256
#define SIZE 1
#define SIZE2 8

void usage(const char *s) {
  printf("%s [(inFile | -)] [outFile]\n", s);
  exit(EXIT_FAILURE);
}

int getBit(unsigned char *buf, int bufInd) {
  /* retrieve individual bits from buffer via mask, return 1 if 1, 0 if 0*/
  int bit;
  int offset = SIZE2 - bufInd - 1;
  int mask = 1 << offset;
  bit = ((*buf&mask)>>offset);
  if(bit)
    return 1;
  return 0;

}

void writeChars(int in, int out, int totalChars, node *root, char **codeList) {
  /* read through input file, navigating tree to leaves by following huffman 
     codes, outputting resulting char to output*/
  node *tree = root;
  int finCheck, buf2, bit, charCount=0, bufInd;
  unsigned char buf[SIZE];
  /* if only one char in file, just write it to output without going 
     through rest of func.*/
  while( (finCheck = read(in, &buf, SIZE)) > 0) {
    /* navigate tree, getting bits along the way*/
    bufInd = 0;
    while(bufInd < SIZE2 && charCount < totalChars) {
      bit = getBit(buf, bufInd);
      bufInd++;
      if(bit == 1) {
	tree = tree->right;
      } else if(!bit) {
	tree = tree->left;
      }
      if(!tree->left && !tree->right) {
	/* when encountering leaf node, output associated char to output and 
	   return to node*/
	buf2 = tree->val;
	if(write(out, &buf2, 1) == -1) {
	  perror("write");
	  exit(1);
	}
	tree = root;
	charCount++;
      }
    }
  }
  if(finCheck == -1) {
    perror("read");
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  int i, buf2, in, out, numChars, testIn, totalChars=0;
  int countTracker = 0, stdinCheck = 0, stdoutCheck = 0, j = 0;
  node *root, **list, **list2;
  char **codeList, *code; 
  unsigned buf[SIZE];

  if(! (list = malloc(sizeof(node *) * CHAR_COUNT))) {
    perror("malloc");
    exit(1);
  }
  for(i = 0; i < CHAR_COUNT; i++)
    list[i] = NULL;
  
  /* parse arguments */
  if(argc < 1 || argc > 3)
    usage("hdecode");
  
  if(argc >= 2 && argv[1] && strcmp(argv[1], "-") != 0) {
    if( (in = open(argv[1], O_RDONLY)) == -1) {
      perror("open");
      exit(1);
    }  
  } else {
    stdinCheck = 1;
    in = STDIN_FILENO;
  }
  if(argc == 3 && argv[2]) {
    if( (out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1) {
      perror("open");
      exit(1);
    }
  } else {
    stdoutCheck = 1;
    out = STDOUT_FILENO;
  }
  
  /* read and process header*/
  
  /* read total char count*/
  if( (testIn = read(in, buf, 1)) == -1) {
    perror("read");
    exit(1);
  }
  if(!testIn)
    numChars = 0;
  else
    numChars = *buf + 1;
  while(  countTracker < numChars) {
    if(read(in, buf, 1) == -1) {
      perror("read");
      exit(1);
    }
    if(read(in, &buf2, 4) == -1) {
      perror("read");
      exit(1);
    }
    list[(int)*buf] = makeNode(*buf, ntohl(buf2));
    totalChars += ntohl(buf2);
    countTracker++;
  }

  /* create new list of just nodes*/
  if(! (list2 = malloc(sizeof(node *) * numChars))) {
    perror("malloc");
    exit(1);
  }
  for(i = 0; i < CHAR_COUNT; i++) {
    if(list[i])
      list2[j++] = list[i];
  }

  /* sort list and build tree*/
  qsort(list2, numChars, sizeof(node *), compNode);
  root = buildTree(list2, numChars);
  
  /*dt(root, 0);*/ /* used for troubleshooting*/

  /* if root is not NULL*/
  if(root) {
  /* extract codes from tree*/
    if(! (codeList = malloc(sizeof(char *) * CHAR_COUNT))) {
      perror("malloc");
      exit(1);
    }
    for(i = 0; i < CHAR_COUNT; i++)
      codeList[i] = NULL;
  
    if(! (code = malloc(sizeof(char *) * (numChars+1)))) {
      perror("malloc");
      exit(1);
    }
    for(i = 0; i < numChars+1; i++)
      code[i] = '\0';
   
    extractCodes(codeList, root, code, (sizeof(char *) * (numChars+1)));
    
    /* write decompressed charactes to output*/
    if(!root->left && !root->right) {
      *buf = root->val;
      i = 0;
      for(i = 0; i < root->freq; i++) {
	if(write(out, &buf, 1) == -1) {
	  perror("write");
	  exit(1);
	}
      }
    } else {
      writeChars(in, out, totalChars, root, codeList); 
    }

    /* huffcodes table*/
    for(i = 0; i < CHAR_COUNT; i++) {
      if(codeList[i]) {
	/*printf("0x%02x: %s\n", i, codeList[i]);*/ /* used for 
	  troubleshooting*/
	free(codeList[i]);
      }
    }
    
    /* cleanup */
    free(codeList);
  }

  if(!stdinCheck)
    close(in);
  if(!stdoutCheck)
    close(out);
  free(list);
  free(list2);
  if(!root)
    free(root);
  else
    freeTree(root);
  return 0;
}
