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
#include "mush2uu2.h"

#define WRITE 1
#define READ 0

static int sigChk;

void handler(int signum) {
  /* catch SIGINT*/
  printf("\n");
  sigChk = 1;
}

int mushCd(int argc, char *argv[]) {
  /* built-in cd implementation, if no args
   chdir to home directory, otherwise try to
  chdir to included path*/
  char *path;

  /* parse input*/
  if(argc < 1 || argc > 2) {
    fprintf(stderr, "%s: %s\n", "cd", "invalid arguments");
    return -1;
  }

  if(argc == 1) {
    /* if no arguments, chdir to home directory*/
    if( !(path = getenv("HOME"))) {
      /* if HOME is undefined, use user's
       password entry*/
      if(!(path = getpwuid(getuid())->pw_dir)) {
	fprintf(stderr, "%s\n", "unable to determine home directory");
	return -1;
      }
    }
    if(-1 == chdir(path)) {
      perror(path);
      return -1;
    }
  } else {
    /* otherwise, chdir to path*/
    if(-1 == chdir(argv[1])) {
      perror(argv[1]);
      return -1;
    }
  }
  
  return 0;
}

int interOp(pipeline pl) {
  /* launch and execute pipeline, forking for each stage in pipeline*/
  int childs, status=0, inf, outf;
  pipes *ps;
  pid_t pidArr[pl->length];
  struct sigaction sa;
  sigset_t set;
  
  /* set up signal handling*/
  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if(-1 == sigaction(SIGINT, &sa, NULL)) {
    perror("sigaction");
    return -1;
  }

  /* set up set masking to block SIGINT until after pipeline launches*/
  sigfillset(&set);
  sigprocmask(SIG_SETMASK, &set, NULL);

  /* malloc pipe struct*/
  if(!(ps = makePipes()))
    return -1;

  /* fork as many times as needed*/
  for(childs = 0; childs < pl->length; childs++) {
    if(-1 == (pidArr[childs] = fork())) {
      perror("fork");
      freePipes(ps);
      return -1;
    } else if(!pidArr[childs]) {
      /* duplicate pipe file descriptors*/
      if(!childs) {
	      /* first child, redirect to pipe if needed*/
	      if(pl->length > 1) {
          if(-1 == dup2(ps->pd1[WRITE], STDOUT_FILENO)) {
            perror("dup2");
            freePipes(ps);
            exit(1);
          }
	      }
        /* redirected input and output*/
        if(pl->stage[childs].inname) {
          if(-1 == (inf = open(pl->stage[childs].inname, O_RDONLY))) {
            perror(pl->stage[childs].inname);
            freePipes(ps);
            exit(1);
          }
          if(-1 == dup2(inf, STDIN_FILENO)) {
            perror("dup2");
            freePipes(ps);
            return -1;
          }
	      }
        if(pl->stage[childs].outname) {
          if(-1 == (outf = open(pl->stage[childs].outname, O_RDWR | O_TRUNC | 
              O_CREAT, 0666))) {
            perror(pl->stage[childs].outname);
            freePipes(ps);
            exit(1);
          }
          if(-1 == dup2(outf, STDOUT_FILENO)) {
            perror("dup2");
            freePipes(ps);
            exit(1);
          }
        } 
      } else if(childs%2){
        /* child is odd in order*/
        if(-1 == dup2(ps->pd1[READ], STDIN_FILENO)) {
          perror("dup2");
          freePipes(ps);
          exit(1);
        }
        /* continuing to next pipe*/
        if(childs < (pl->length - 1)) {
          if(-1 == dup2(ps->pd2[WRITE], STDOUT_FILENO)) {
            perror("dup2");
            freePipes(ps);
            exit(1);
          }
        } else if(pl->stage[childs].outname) {
          /* redirected output*/
          if(-1 == (outf = open(pl->stage[childs].outname, O_RDWR | O_TRUNC | 
              O_CREAT, 0666 ))) {
            perror(pl->stage[childs].outname);
            freePipes(ps);
            exit(1);
          }
          if(-1 == dup2(outf, STDOUT_FILENO)) {
            perror("dup2");
            freePipes(ps);
            exit(1);
          }
        }
      } else {
        /* child is even in order*/
        if(-1 == dup2(ps->pd2[READ], STDIN_FILENO)) {
          perror("dup2");
          freePipes(ps);
          exit(1);
        }
        /* continuing to next pipe*/
        if(childs < (pl->length - 1)) {
          if(-1 == dup2(ps->pd1[WRITE], STDOUT_FILENO)) {
            perror("dup2");
            freePipes(ps);
            exit(1);
          }
        } else if(pl->stage[childs].outname) {
          /* redirected output*/
          if(-1 == (outf = open(pl->stage[childs].outname, O_RDWR | O_TRUNC | 
              O_CREAT, 0666 ))) {
            perror(pl->stage[childs].outname);
            freePipes(ps);
            exit(1);
          }
          if(-1 == dup2(outf, STDOUT_FILENO)) {
            perror("dup2");
            freePipes(ps);
            exit(1);
          }
        }
      }
      break;
    } else {
      /* if parent, make new pipes as needed*/
      if(childs%2 == 0 && childs > 1) {
	      /* for all odd children after 2nd, 
	      open new output pipe*/
        close(ps->pd1[READ]);
        close(ps->pd1[WRITE]);
        if(-1 == updatePipes(1, ps))
          return -1;
      } else if(childs%2 && childs > 2) {
        /* for all even children after 3rd, 
        open new output pipe*/
        close(ps->pd2[READ]);
        close(ps->pd2[WRITE]);
        if(-1 == updatePipes(2, ps))
          return -1;
      }
    }
  }

  /* close unnecessary file descriptors*/
  close(ps->pd1[0]);
  close(ps->pd1[1]);
  close(ps->pd2[0]);
  close(ps->pd2[1]);

  /* unblock SIGINT*/
  sigdelset(&set, SIGINT);
  sigprocmask(SIG_SETMASK, &set, NULL);

  /* forked running*/
  if(childs == pl->length) { /* parent waits*/
    while(childs > 0) {
      if(-1 != wait(&status)) {
	      /* if EINTR, wait again*/
	      childs--;
      }
    }
  } else { /* child executes command*/
    if(!strcmp(pl->stage[childs].argv[0], "cd")) {
      /* cd implementation*/
      if(-1 == mushCd(pl->stage[childs].argc, pl->stage[childs].argv)) {
        freePipes(ps);
        exit(1);
      }
    } else {
      /* exec any other command*/
        if(-1 == execvp(pl->stage[childs].argv[0], pl->stage[childs].argv)) {
	  perror(pl->stage[childs].argv[0]);
	  freePipes(ps);
	  exit(1);
	}
    }
  }

  /* cleanup*/
  freePipes(ps);
  return 0;
}

int interMode() {
  /* shell interactive mode, runs shell
   until SIGINT is caught*/
  char *cl;
  pipeline pl;
  struct sigaction sa;
  sigset_t set;

  /* set up signal handling*/
  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if(-1 == sigaction(SIGINT, &sa, NULL)) {
    perror("sigaction");
    return -1;
  }

  /* set up set masking*/
  sigfillset(&set);
  sigdelset(&set, SIGINT);

  /* check for stdin and stdout tty before printing prompt*/
  if(isatty(STDIN_FILENO) && isatty(STDOUT_FILENO))
    printf("8-P ");

  /* read input until SIGINT is caught or EOF*/
  while((cl = readLongString(stdin))) {
    if(!(pl = crack_pipeline(cl))) {
	perror("crack_pipeline");
    }
    interOp(pl);
    free(cl);
    free_pipeline(pl);
    /* check tty again*/
    if(isatty(STDIN_FILENO) && isatty(STDOUT_FILENO))
      printf("8-P ");
  }

  /* if SIGINT is caught, start another instance of shell*/
  if(sigChk) {
    sigChk = 0;
    if(-1 == interMode())
      return -1;
  }

  return 0;
}

int batchMode(char *cl) {
  /* shell batch mode*/
  pipeline pl;
  
  if(!(pl = crack_pipeline(cl))) {
      perror("crack_pipeline");
      return -1;
  }
  interOp(pl);
  free_pipeline(pl);
  return 0;
}
