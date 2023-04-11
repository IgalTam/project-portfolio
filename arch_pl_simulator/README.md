### Computer Architecture Pipelining Emulator
Project from Computer Architecture course

##### Overview of Functionality
This project is intended to emulate the functionality of a CPU executing MIPS assembly instructions with pipelining implemented. The project builds upon the previous two projects from the course: ```lab2```
is an assembler that converts assembly instructions into a binary file; ```lab3``` emulates a CPU executing MIPS assembly instructions without pipelining. ```lab4``` implements
pipelining operations and also reports the contents of the pipeline for each instruction execution. The final output of the emulator is a report of the input assembly program's
runtime statistics, including CPI, total clock cycles operated on, and total instructions executed.<br />
```lab4``` optionally takes as input a .script file which contains single characters separated by newlines. See the Usage section for more details on individual commands. If no script file is provided, the emulator will function in interactive mode and take input commands in that way instead. <br />
See ```lab4_test1.asm```, ```lab4_test1.script```, and ```lab4_test1.output``` for example input and output. 

##### Requirements
This program is capable of running in an environment with the following version of Java:<br />
```openjdk version "1.8.0_282"```<br />
Other details:<br />
```OpenJDK Runtime Environment (build 1.8.0_282-b08)```<br />
```OpenJDK 64-Bit Server VM (build 25.282-b08, mixed mode)```

#### Usage
Building: ```$ make lab4```<br />
Usage: ```$ /bin/java lab4 <input .asm file> <input .script file>```, output printed to STDOUT<br />
Command Guide (from ```execCmd()``` in ```lab4.java```):<br />
1) ```d```: display current register state
2) ```p```: display current pipeline register values
3) ```s```: step through a single clock cycle
4) ```s <number>```: step through _number_ clock cycles
5) ```r```: run to program completion and display timing summary
6) ```m <num1> <num2>```: display data memory from address _num1_ to address _num2_
7) ```c```: clear and set to 0 all registers, memory, and program counter
8) ```q```: exit program
