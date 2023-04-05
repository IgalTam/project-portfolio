#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#define LARGE 10000000

int main(int argc, char *argv[]) {
    char *t, *u;
    // char *s = strdup("bad_mal"); /* should call malloc() implicitly */
    u = malloc(8*sizeof(char));
    sprintf(u, "bad_mal");
    puts(u);
    t = realloc(u, 70000*sizeof(char));
    sprintf(t, "bad_mal");
    puts(t);
    free(t);

    // t = calloc(10, sizeof(char));

    // char *s_arr[LARGE];
    // int i;
    // for(i = 0; i < LARGE; i++) {
    //     s_arr[i] = malloc(8*sizeof(char));
    //     sprintf(s_arr[i], "bad_mal\n");
    //     puts(s_arr[i]);
    //     free(s_arr[i]);
    // }

    return 0;
}