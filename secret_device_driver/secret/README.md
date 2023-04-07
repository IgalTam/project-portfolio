### Secretkeeper Device Driver
Project from Operating Systems course

##### Overview of Functionality
This program is a character device driver designed for use in a MINIX 3.1.8 operating system, and is derived from the Hello driver that can also be found in a MINIX 3.1.8 installation. The secretkeeper device driver is to be stored within the filesystem, and be accessible via the special device file ```/dev/Secret```. The secretkeeper allows the first user to open it for writing to store data up to a specific capacity, and transfers ownership
to that user. Then, the owner can read the data stored in the secretkeeper, with the stored data being removed after reading. No other user can open the device for 
reading or writing until the owner reads all stored data, in which case the device resets.

##### Requirements
The device driver is designed to operate in a MINIX 3.1.8 operating system. Specifically, the driver was tested using a virtual machine created with Oracle VirtualBox 7.0.6 and a MINIX 3.1.8 image file. Additionally, the virtual machine should contain either ```cc``` or ```gcc```. After installation, create the following files within the filesystem:<br />
1) Create a file ```/usr/src/include/sys/ioc_secret.h```. Populate this file with the lines ```#include <minix/ioctl.h>```and ```#define SSGRANT	_IOW(‘K’, 1, uid_t)```. Copy this file to ```/usr/include/sys/ioc_secret.h```.
2) In ```/etc/system.conf```, include the line ```service secretkeeper {};``` within the main scope of the file, similar to the other listed services.
3) In ```/usr/src/include/sys/ioctl.h```, add the include line ```#include <sys/ioc_secret.h>``` with the other includes at the top of the file.
4) Download this source directory to ```/src/drivers/```.

#### Usage
Building: ```$ make```.<br />
For use in the aforementioned MINIX 3.1.8 environment:<br />
1) If ```/dev/Secret``` does not yet exist in the filesystem, create it using ```$ mknod /dev/Secret c 20 0```.
2) To activate the device driver, navigate to the directory of the built driver source and run ```$ service up pwd/secretkeeper -dev /dev/Secret```.
3) The device is now active and can be accessed with open, read, write, and close calls.
4) The device supports an ioctl call designated SSGRANT, which allows for the exchanging of ownership of the device without deleting stored data.
5) To deactivate the device driver, run ```$ service down secretkeeper```.
