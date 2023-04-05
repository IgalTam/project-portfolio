#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM_CHUNK (uintptr_t) 64000
#define FREE_MEM 0      // flags allocated memory as free to use
#define NO_FREE_MEM 1   // flags allocated memory as in use
#define DO_FREE 0       // indicates that free() is defragging
#define DONT_FREE 1     // indicates that realloc() is defragging

typedef struct header {
    uintptr_t alloc_size;
    uintptr_t free_flag;
    struct header *next;
    uintptr_t padding;
} hdr;

hdr *extract_chunk();
hdr *merge_allocs(hdr *prev_node, hdr *post_node);
void *malloc_func(size_t size);
void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
hdr *locate_hdr(void *ptr);
hdr *defragment_allocs(hdr *cur_node, size_t size, int free_check);
void *realloc_func(void *ptr, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);