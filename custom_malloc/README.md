### Custom Malloc Library
Project from Operating Systems course

##### Overview of Functionality
This project consists of a library of functions that have simplified functionality of ```malloc(3)```, ```calloc(3)```, ```realloc(3)```, and ```free(3)```. The library
makes use of a linked list structure to keep track of allocated memory within the heap, and allocates chunks of memory using ```mmap(2)```. The library also
defragments sections of freed memory for efficiency.

##### Requirements
This program is intended to be used in an environment with the following versions of Linux and gcc:<br />
```Linux 3.10.0-1160.15.2.el7.x86_64```<br />
```gcc (GCC) 4.8.5 20150623 (Red Hat 4.8.5-44)```<br />

#### Usage
[WIP]