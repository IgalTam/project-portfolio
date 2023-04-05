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

#ifndef MUSH2UU2_H
#define MUSH2UU2_H

typedef struct pipelineHolder {
  int pd1[2];
  int pd2[2];
} pipes;

pipes *makePipes();
int updatePipes(int which, pipes *ps);
void freePipes(pipes *ps);

#endif
