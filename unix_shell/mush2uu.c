#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <sys/wait.h>
#include <pwd.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <fcntl.h>
#include <mush.h>
#include "mush2uu.h"

int updatePipes(int which, pipes *ps) {
  /* pipe() one of two pipes in ps*/
  if(which < 1 || which > 2) {
    fprintf(stderr, "%s: %s", "makePipes", "invalid pipe chosen");
    return -1;
  }

  if(which == 1) {
    if(-1 == pipe(ps->pd1)) {
      perror("pipe");
      return -1;
    }
  } else if(which == 2) {
    if(-1 == pipe(ps->pd2)) {
      perror("pipe");
      return -1;
    }
  }
  return 0;
}


pipes *makePipes() {
  /* create and allocate memory for a new pipes struct,
   which contains 2 pipes (int arrays of lenght 2) in it*/
  pipes *ps;
  if(!(ps = malloc(sizeof(struct pipelineHolder)))) {
    perror("malloc");
    return NULL;
  }
  if(-1 == updatePipes(1, ps))
    return NULL;
  if(-1 == updatePipes(2, ps))
    return NULL;
  
  return ps;
}

void freePipes(pipes *ps) {
  /* free a pipes struct*/
  close(ps->pd1[0]);
  close(ps->pd1[1]);
  close(ps->pd2[0]);
  close(ps->pd2[1]);
  free(ps);
}
