### Secretkeeper Device Driver
Project from Operating Systems course

##### Overview of Functionality
This program is a character device driver designed for use in a MINIX 3.1.8 operating system. The secretkeeper device driver is to be stored within the 
filesystem in the directory ```/dev/Secret```. The secretkeeper allows the first user to open it for writing to store data up to a specific capacity, and transfers ownership
to that user. Then, the owner can read the data stored in the secretkeeper, with the stored data being removed after reading. No other user can open the device for 
reading or writing until the owner reads all stored data, in which case the device resets.

#### Usage
[WIP]

##### Requirements
[WIP]
