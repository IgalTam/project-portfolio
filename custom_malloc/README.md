### Custom Malloc Library
Project from Operating Systems course

##### Overview of Functionality
This project consists of a library of functions that have simplified functionality of ```malloc(3)```, ```calloc(3)```, ```realloc(3)```, and ```free(3)```. The library
makes use of a linked list structure to keep track of allocated memory within the heap, and allocates chunks of memory using ```mmap(2)```. The library also
defragments sections of freed memory for efficiency. The example C program ```bad_mal``` can be built using the library's Makefile for testing.

##### Requirements
This program is intended to be used in an environment with the following versions of Linux and gcc:<br />
```Linux 3.10.0-1160.15.2.el7.x86_64```<br />
```gcc (GCC) 4.8.5 20150623 (Red Hat 4.8.5-44)```<br />

#### Usage
Building: ```$ make lib/libmalloc.so``` for 32-bit supported library, ```$ make lib64/libmalloc.so``` for 64-bit supported library, ```$ make intel-all``` for both libraries.<br />
Linking: See Makefile section on building ```bad_mal``` for linking the custom malloc library during compilation. Note the commented steps after building ```bad_mal```, as the environment variable ```LD_LIBRARY_PATH``` must be set in the terminal that will be used for running programs with the library.<br />
Usage: Once built and linked, the functions in this malloc library will replace their respective POSIX counterparts. See the man pages for ```malloc(3)```, ```calloc(3)```, ```realloc(3)```, and ```free(3)``` for more information on using the library functions.