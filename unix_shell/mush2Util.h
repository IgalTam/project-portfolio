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
#include <mush.h>
#include "mush2uu.h"

void handler(int signum);
int mushCd(int argc, char *argv[]);
int interMode();
int interOp(pipeline pl);
int batchMode(char *cl);
