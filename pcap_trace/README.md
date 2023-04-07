### Network Packet Analyzer
Project from Computer Networks course

##### Overview of Functionality
This program takes as input a PCAP file (```.pcap``` extension) and analyzes its contents to provide a report on the network packet data. The PCAP analyzer can
recognize and report on the following protocols: Ethernet, ARP, IPv4, TCP, UDP, ICMP.

##### Requirements
This program is intended to be used in an environment with the following versions of Linux and gcc:<br />
```Linux 3.10.0-1160.15.2.el7.x86_64```<br />
```gcc (GCC) 4.8.5 20150623 (Red Hat 4.8.5-44)```<br />

#### Usage
Building: ```$ make all``` or ```$ make trace-<OS version>-<processor type>```<br />
Running: ```$ ./trace <PCAP file>```