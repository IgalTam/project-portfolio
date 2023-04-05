### Custom Unix Shell
Project from Systems Programming Course

##### Overview of Functionality
This project is intended to emulate a Unix shell utility, complete with pipeline handling. Generally, the program uses forking to create
child processes for handling each command and associated arguments in the pipeline, and uses the ```exec(3)``` family of functions to execute said commands. 
The shell has two modes of operation: interactive mode and batch mode. In interactive mode, the shell behaves like a standard command line shell, prompting the user
for input commands and pipelines and executing them as described earlier; the shell starts another instance of itself to continue operation after execution. In batch mode,
the shell runs a single instance of itself to execute an input pipeline.

#### Usage
Building: ```make``` or ```make mush2```<br />
Interactive Mode: ```./mush2``` Use Ctrl-D to terminate the shell.<br />
Batch Mode: ```./mush2 <Unix command/pipeline, arguments, etc.>```<br />

##### Requirements
This program is intended to be used in an environment with the following versions of Linux and gcc:<br />
```Linux 3.10.0-1160.15.2.el7.x86_64```<br />
```gcc (GCC) 4.8.5 20150623 (Red Hat 4.8.5-44)```<br />
