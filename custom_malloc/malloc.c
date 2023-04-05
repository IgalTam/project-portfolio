#include "malloc.h"

static hdr *heap;

hdr *extract_chunk() {
    /* extracts a chunk of memory from the stack
    and allocates it to the heap struct; the new node
    contains a memory segment of size "size"*/
    hdr *new_node;

     /* retrieve memory from the stack*/
    if((void *)-1 == (new_node = sbrk(MEM_CHUNK*sizeof(hdr)))) {
        return NULL;
    }

    /* set new struct properties*/
    new_node->free_flag = FREE_MEM;
    new_node->alloc_size = MEM_CHUNK;
    new_node->padding = 0;

    return new_node;
}

hdr *merge_allocs(hdr *prev_node, hdr *post_node) {
    /* merges two linked list nodes together to combine
    allocated memory segments*/

    // combined node has the combined memory of both nodes plus the header
    // space of the post node
    prev_node->alloc_size += (post_node->alloc_size + sizeof(hdr));
    
    // redirect previous node next ptr to remove post node from the structure
    prev_node->next = post_node->next;

    // return combined node
    return prev_node;
}

void *malloc_func(size_t size) {
    /* implementation of malloc(3) */
    uintptr_t mult_check;
    hdr *cur_node;
    hdr *new_node;

    /* if heap is empty, check if there is available memory in
    an allocated chunk and allocate it to start the heap*/
    if(!heap) {
        // ensure heap start is aligned with 16 byte boundaries
        if((void *)-1 == (heap = sbrk(0))) {
            errno = ENOMEM;
            return NULL;
        }
        if( (mult_check = ((uintptr_t) heap % 16)) ) {
            heap += (mult_check - 16);
        }

        // construct the first list struct
        // with the heap pointing to it
        if(!(heap = extract_chunk())) {
            errno = ENOMEM;
            return NULL;
        }
    }

    /* adjust size so that it is in multiples of 16 bytes
    i.e. increase to be multiple of 16 bytes if not already*/
    if((mult_check = size % 16)) {
        size += (16 - mult_check);
    }

    // traverse linked list to the last generated segment in the
    // allocated memory chunk
    cur_node = heap;
    while(cur_node) {
        if(cur_node->free_flag == FREE_MEM && cur_node->alloc_size >= size) { 
            // allocated space is sufficient
            if(cur_node->alloc_size > (size + sizeof(hdr))) {
                /* if memory has more than exact, use some 
                from tail end and adjust struct accordingly*/
                cur_node->alloc_size -= (size + sizeof(hdr));
                new_node = (hdr *) ((uintptr_t) (cur_node + 
                                                cur_node->alloc_size));
                new_node->alloc_size = size;
                new_node->next = cur_node->next;
                new_node->padding = 0;
                cur_node->next = new_node;
            } else {
                /* exact memory requirement found*/
                new_node = cur_node;
            }
            /* flag memory as in use */
            new_node->free_flag = NO_FREE_MEM;

            /* return pointer to free memory space */
            return (void *) ((uintptr_t) new_node + sizeof(hdr));
        } else if(!cur_node->next) {
            /* if no memory can be found in the linked list, carve out more
            memory and resume the loop*/
            do {
                if (!(new_node = extract_chunk())) {
                    errno = ENOMEM;
                    return NULL;
                }
                /* if more memory is needed than MEM_CHUNK, continue extracting
                memory and merging nodes until a large enough block is formed*/
                if(!cur_node->next)
                    cur_node->next = new_node;
                else {
                    cur_node->next = merge_allocs(cur_node->next, new_node);
                }
            } while(cur_node->next->alloc_size < size);
        }
        /* go to next node in the list if memory in use or too small
        keep track of previous node in case no space can be found*/
        // prev_node = cur_node;
        cur_node = cur_node->next;
    }

    /* if malloc got here, something went wrong */
    errno = ENOMEM;
    return NULL;
}

void *malloc(size_t size) {
    /* malloc wrapper to enable debugging*/
    char out_str[200];
    void *ret_ptr;

    // error handling
    if(!(ret_ptr = malloc_func(size)))
        return NULL;
    
    /* if DEBUG_MALLOC is defined, narrate malloc() behavior
    by outputting a string to stdout */
    if(getenv("DEBUG_MALLOC")) {
        snprintf(out_str, 100, 
            "MALLOC: malloc(%d)\t=>   (ptr=%p, size=%d)\n", 
            (int) size, ret_ptr, 
            (int) ((hdr *)((uintptr_t) ret_ptr - sizeof(hdr)))->alloc_size);
        write(STDERR_FILENO, out_str, strlen(out_str));
    }

    return ret_ptr;
}

void *calloc(size_t nmemb, size_t size) {
    /* implementation of calloc(3p) */
    char out_str[200];
    void *ret_ptr;

    /* if either argument is 0, return NULL*/
    if(!nmemb || !size)
        return NULL;

    /* allocate memory */
    if(!(ret_ptr = malloc_func(nmemb * size))) {
        return NULL;
    }

    /* set allocated memory to all 0's*/
    memset(ret_ptr, 0, nmemb*size);

    /* if DEBUG_MALLOC is defined, narrate calloc() behavior
    by outputting a string to stdout */
    if(getenv("DEBUG_MALLOC")) {
        snprintf(out_str, 100, 
            "MALLOC: calloc(%d,%d)\t=>   (ptr=%p, size=%d)\n", 
            (int) nmemb, (int) size, ret_ptr, 
            (int) ((hdr *)((uintptr_t) ret_ptr - sizeof(hdr)))->alloc_size);
        write(STDERR_FILENO, out_str, strlen(out_str));
    }

    return ret_ptr;
}

hdr *locate_hdr(void *ptr) {
    /* determines the address of the hdr pointer
    that indicates the memory pointed to by ptr*/
    hdr *prev_ptr, *cur_node = heap;

    /* if ptr addr is before the malloc list, return NULL*/
    if((uintptr_t) heap > (uintptr_t) ptr)
        return NULL;

    /* starting at the heap, continue until the node pointer
    address is greater than ptr, in which case the previous
    hdr struct contains the memory pointed to by ptr*/
    while(cur_node && (uintptr_t) cur_node < (uintptr_t) ptr) {
        prev_ptr = cur_node;
        cur_node = cur_node->next;
    }

    if(!cur_node) {
        /* if ptr addr is beyond the malloc list and the allocated
        memory of the last node, return NULL*/
        if((uintptr_t) ptr > ((uintptr_t) prev_ptr + 
            prev_ptr->alloc_size + sizeof(hdr)))
            return NULL;
    }
    return prev_ptr;
}

hdr *defragment_allocs(hdr *cur_node, size_t size, int free_check) {
    /* merges free allocations adjacent to cur_node and
    returns the combined memory segment
    free_check is a special case for free(): DO_FREE indicates to free
    all adjacent blocks no matter the requested size*/
    hdr *prev_node;

    /* merge forward segments*/
    while(cur_node->next && cur_node->next->free_flag == FREE_MEM && 
        (cur_node->alloc_size < size || free_check == DO_FREE)) {
        cur_node = merge_allocs(cur_node, cur_node->next);
    }
    /* merge backward segments*/
    if(cur_node != heap && (cur_node->alloc_size < size || 
        free_check == DO_FREE)) {
        // only search behind if not at start of heap
        prev_node = locate_hdr(--cur_node); // locate previous hdr struct
        while(prev_node->free_flag == FREE_MEM &&
            (cur_node->alloc_size < size || free_check == DO_FREE)) {
            if(heap == (cur_node = merge_allocs(prev_node, cur_node)))
                break; // if the heap is reached, stop backtracing
            prev_node = locate_hdr(--cur_node);
        }
    }

    /* return the merged and defragmented memory allocation*/
    return cur_node;
}

void *realloc_func(void *ptr, size_t size) {
    /* implementation of realloc(3p) */
    hdr *op_hdr, *new_ptr;
    uintptr_t mult_check;

    /* locate hdr struct for the memory that ptr points to */
    if(!(op_hdr = locate_hdr(ptr))) {
        return NULL; // unable to locate ptr in malloc list
    }

    if(op_hdr->free_flag == FREE_MEM) {
        /* undefined behavior: attempting to access
        freed memory -> return NULL*/
        return NULL;
    }

    /* adjust size so that it is in multiples of 16 bytes
    i.e. increase to be multiple of 16 bytes if not already*/
    if((mult_check = size % 16)) {
        size += (16 - mult_check);
    }

    /* if possible, attempt in-place size adjustment*/
    if(op_hdr->alloc_size >= size) {
        /* special case: if allocation size is reduced to 0,
        mark the segment as free (realloc(ptr, 0) == free(ptr))
        NOTE: this is most likely redundant, the main body of realloc()
        already has a special case for this*/
        if(!(op_hdr->alloc_size = size))
            op_hdr->free_flag = FREE_MEM;
        return (void *) ((uintptr_t) op_hdr + sizeof(hdr));
    }

    /* otherwise, merge as many adjacent free chunks as possible
    to create enough memory for in-place size adjustment;
    start with allocations ahead of op_hdr, then turn to those behind*/
    op_hdr = defragment_allocs(op_hdr, size, DONT_FREE);

    /* if possible, attempt in-place size adjustment (again)*/
    if(op_hdr->alloc_size >= size) {
        if((uintptr_t) op_hdr < ((uintptr_t)ptr - sizeof(hdr))) {
            /* if the new location was backshifted, copy data 
            to starting point of reallocation*/
            memcpy((hdr *)((uintptr_t) op_hdr + sizeof(hdr)), ptr, size);
        }
        return (void *) ((uintptr_t) op_hdr + sizeof(hdr));
    }
    
    /* at this point, a new block has to be malloc'd
    afterwards, copy the data from ptr to the new block*/
    if(!(new_ptr = malloc(size))) {
        return NULL;
    }
    memcpy(new_ptr, ptr, size);
    /* since the defragmented memory wasn't used,
    might as well free it */
    op_hdr->free_flag = FREE_MEM;
    return new_ptr;

}

void *realloc(void *ptr, size_t size) {
    /* realloc wrapper to enable debugging*/
    char out_str[200];
    void *ret_ptr;
    
    /* special cases*/
    if(!ptr) {
        /* if ptr is NULL, call malloc(size)*/
        if(!(ret_ptr = malloc(size)))
            return NULL;
    } else if(!size) {
        /* if ptr is not NULL and size is 0, call free(ptr)*/
        free(ptr);
    } else {
        if(!(ret_ptr = realloc_func(ptr, size)))
            return NULL;
    }

    /* if DEBUG_MALLOC is defined, narrate realloc() behavior
    by outputting a string to stdout */
    if(getenv("DEBUG_MALLOC")) {
        snprintf(out_str, 100, 
            "MALLOC: realloc(%p,%d)\t=>   (ptr=%p, size=%d)\n", 
            ptr, (int) size, ret_ptr, 
            (int) ((hdr *)((uintptr_t) ret_ptr - sizeof(hdr)))->alloc_size);
        write(STDERR_FILENO, out_str, strlen(out_str));
    }

    return ret_ptr;
}

void free(void *ptr) {
    /* implementation of free(3p) */
    char out_str[200];
    hdr *op_hdr;

    if(!ptr || !(op_hdr = locate_hdr(ptr)) || op_hdr->free_flag == FREE_MEM){
        /* undefined behavior: 
            free(NULL) is called
            ptr address has not been malloc'd yet
            attempting to free a freed location
        in these cases, do not perform operation and exit early*/
        return;
    }
    
    /* mark the target allocation as free
    then defragment with adjacent free allocations*/
    op_hdr->free_flag = FREE_MEM;
    defragment_allocs(op_hdr, 0, DO_FREE);

     /* if DEBUG_MALLOC is defined, narrate free() behavior
    by outputting a string to stdout */
    if(getenv("DEBUG_MALLOC")) {
        snprintf(out_str, 100, 
            "MALLOC: free(%p)\n", ptr);
        write(STDERR_FILENO, out_str, strlen(out_str));
    }

}

int main(int argc, char *argv[]) {
    /* pretty much just here to help with compiling*/
    return 0;
}