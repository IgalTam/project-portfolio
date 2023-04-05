#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <sys/wait.h>
#include <pwd.h>
#include <mush.h>
#include "mush2Util.h"
#include "mush2uu.h"

#define CMDLEN 100

int main(int argc, char *argv[]) {
  int i, cmdLen1=CMDLEN, cmdLen2, batChk=0;
  char *cl;

  /* parse arguments*/
  if(argc == 1) {
    /* interactive processing*/
    batChk=0;
  } else {
    /* batch processing*/
    batChk = 1;
    if(!(cl = malloc(sizeof(char) * cmdLen1))) {
      perror("malloc");
      exit(1);
    }
    memset(cl, '\0', 100);
    /* build command line string*/
    for(i=1; i < argc; i++) {
      cmdLen2 = strlen(argv[i]) + 1;
      if( cmdLen2 + strlen(cl) > cmdLen1) {
        cmdLen1 += cmdLen2;
	if(!(cl = realloc(cl, cmdLen1))) {
	  perror("realloc");
	  exit(1);
	}
      } else {
	cmdLen1 += cmdLen2;
      }
      if(!strlen(cl))
	cl = argv[i];
      else
	strcat(cl, argv[i]);
      if(i < (argc - 1))
	strcat(cl, " ");
    }
  }

  if(!batChk) {
    /* interactive mode*/
    interMode();
  } else {
    /* batch mode*/
    batchMode(cl);
  }

  /* cleanup*/
  yylex_destroy();
  return 0;
}
