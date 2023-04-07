### Huffman File Compression
Project from Systems Programming course

##### Overview of Functionality
This project consists of two programs. ```hencode``` uses Huffman encoding to compress an input file to a smaller size
based on the frequency of individual characters in the file. ```hdecode``` uses Huffman decoding to restore a file compressed with ```hencode``` to its original state.

##### Requirements
This program is intended to be used in an environment with the following versions of Linux and gcc:<br />
```Linux 3.10.0-1160.15.2.el7.x86_64```<br />
```gcc (GCC) 4.8.5 20150623 (Red Hat 4.8.5-44)```<br />

#### Usage
Building: ```$ make all``` for both programs, ```$ make hencode``` for Huffman encoder, and ```$ make hdecode``` for Huffman decoder.<br />
File Compression: ```$ ./hencode <input text file> [output file, defaults to STDOUT]```<br />
File Decompression: ```$ ./hdecode <input Huffman-encoded file, or '-' for STDIN> [output file, defaults to STDOUT]``` <br />