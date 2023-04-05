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
  printf("%s inFile [outFile]\n", s);
  exit(EXIT_FAILURE);
}

int setBit(int byte, char bit) {
  /* sets bit of input buffer*/
  int mask;
  if(bit == '1') 
    mask = 1;
  else if(bit == '0')
    mask = 0;
  byte = byte << 1;
  return (byte | mask);
}

void writeBits(int in, int out, unsigned char *buf, char **codeList) {
  /* write character codes to output file (v2.0)*/
  int finCheck, i, bufInd=0, byte=0;
  while( (finCheck = read(in, buf, 1)) > 0) {
    /* read chars in from input file*/
    for(i = 0; i < strlen(codeList[(int)*buf]); i++) {
      byte = setBit(byte, codeList[(int)*buf][i]);
      if(bufInd == 7) {
	/* if byte is filled, write it to output and blank it*/
	if( write(out, &byte, 1) == -1) {
	  perror("write");
	  exit(1);
	}
	bufInd = 0;
	byte = 0;
      } else {
	bufInd++;
      }
    }
  }
  if(finCheck == -1) {
    perror("read");
    exit(1);
  }
  if(bufInd) {
    /* pad unfilled bytes with zeros*/
    while(bufInd < 8) {
      byte = setBit(byte, '0');
      bufInd++;
    }
    if( write(out, &byte, 1) == -1) {
      perror("write");
      exit(1);
    }
  }
}

int main(int argc, char *argv[]) {
  int i, buf2, in, out, finCheck, stdoutCheck = 0, numChars = 0, j = 0;
  node *np, *root, **list, **list2;
  char **codeList, *code;
  unsigned char buf[SIZE];

  if(! (list = malloc(sizeof(node *) * CHAR_COUNT))) {
    perror("malloc");
    exit(1);
  }
  for(i = 0; i < CHAR_COUNT; i++)
    list[i] = NULL;
  
  /* parse arguments */
  if(argc < 2 || argc > 3)
    usage("hencode");
  
  if( (in = open(argv[1], O_RDONLY)) == -1) {
    perror("open");
    exit(1);
  }
  if(argc == 3) {
    if( (out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1) {
      perror("open");
      exit(1);
    }
  } else {
    stdoutCheck = 1;
    out = STDOUT_FILENO;
  }
    
  
  /* create list of nodes with all characters from file*/
  while( (finCheck = read(in, buf, 1)) > 0) {
    if( list[(int)*buf] ) {
      list[(int)*buf]->freq++;
    } else {
      np = makeNode(*buf, 1);
      list[(int)*buf] = np;
      numChars++;
    }
  }
  if(finCheck == -1) {
    perror("read");
    exit(1);
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
  
  /*dt(root, 0);*/ /* used for troubleshooting */

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
    
    /* create header*/
    *buf = numChars-1;
    if(write(out, buf, 1) == -1){ /* write total count*/
      perror("write");
      exit(1);
    }
    for(i = 0; i < CHAR_COUNT; i++) { /* write count of each char*/
      if(list[i]) {
	*buf = list[i]->val;
	if(write(out, buf, 1) == -1){
	  perror("write");
	  exit(1);
	}
	buf2 = htonl(list[i]->freq);
	if(write(out, &buf2, 4) == -1){
	  perror("write");
	  exit(1);
	}
      }
    }
   
    /* write body*/
    lseek(in, 0, SEEK_SET); /* return to start of input file*/
    writeBits(in, out, buf, codeList); 

    /* huffcodes table*/
    for(i = 0; i < CHAR_COUNT; i++) {
      if(codeList[i]) {
	/*printf("0x%02x: %s\n", i, codeList[i]);*/ /* 
	  used for troubleshooting */
	free(codeList[i]);
      }
    }
    
    /* cleanup */
    free(codeList);
  }
  
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
