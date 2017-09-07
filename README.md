# QrcodeRaderTCC

### First, install openssl, zbar and opencv library
### On a linux machine run the following commands to install the libraries
### OpenSSL
`sudo apt-get install openssl`
### ZBar
`sudo apt-get install zbar-tools`
### OpenCV - follow the oficial guide for install on your system
http://docs.opencv.org/2.4/doc/tutorials/introduction/linux_install/linux_install.html

### Compile the file 
``` g++ -o teste3 teste3.cpp -lzbar `pkg-config --cflags --libs opencv` ```
### Run 
`./teste3`
