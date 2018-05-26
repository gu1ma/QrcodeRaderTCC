# QRCode C++ project

### First, install openssl, zbar, qrencode and opencv libraries
### On a linux machine run the following commands to install the libraries
### OpenSSL
`sudo apt-get install openssl`
### ZBar
`sudo apt-get install zbar-tools`
`sudo apt-get install libzbar-dev`
### Qrencode
`sudo apt-get install qrencode`

### OpenCV - follow the oficial guide for install on your system
https://docs.opencv.org/trunk/d7/d9f/tutorial_linux_install.html

### Compile the file 
``` g++ -o teste3 teste3.cpp -lzbar `pkg-config --cflags --libs opencv` ```
### Run 
`./teste3`

> Type "h" to histogram esqualizaton, "s" to segmentation image (Otsu) or "q" to apply histogram equalization and image segmentation to correct reading on low light environments. To cancel image corrections, type "n".

### Examples 
> Original QR Code

![Alt text](https://github.com/gu1ma/qrcode-reader/blob/master/results-examples/qrcode-subexposto.jpg "Original QR Code")
> QR Code after histogram equalization

![Alt text](https://github.com/gu1ma/qrcode-reader/blob/master/results-examples/qrcode-apos-histograma.jpg "Equalized QR Code")
> QR code after histogram equalization and Otsu segmentation

![Alt text](https://github.com/gu1ma/qrcode-reader/blob/master/results-examples/qrcode-apos-histograma-e-segmentacao.jpg "Equalized/Segmented QR Code")
