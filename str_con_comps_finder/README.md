### Strongly Connected Component Identifier
Project from Design & Analysis of Algorithms course

##### Overview of Functionality
This program takes as input a specially formatted file containing the total number of nodes, followed by edges defined by the graph nodes they connect. See ```it_test_dir/```
for sample input files. The program outputs a list containing each node in the graph grouped by strongly connected component. <br />
Additionally, I wrote a test case creator to better test my program during development. This script generates a randomized input file for a given number of nodes, and guarantees
each node in the graph to have at least one edge. <br />
```sort_component_list()``` and ```read_file()``` were provided as part of the assignment. 

##### Requirements
This program is intended to be used in an environment with the following version Python:<br />
```Python 3.11.1```

#### Usage
(assuming Linux CLI)<br />
Standard operation: ```python3 itamarki_lab2.py <input file>```<br />
Test case creator: ```python3 it_test_writer.py <number of nodes>```<br />
