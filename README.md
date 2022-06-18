# FTP-Client-Server-with-SSL-OpenSSL-Application
Final Project for CENG421 Network Programming Course

IBRAHIM ESER

## Abstract

This project aims to develop a File Transfer Protocol Server/Client Application securely. This security
process will be obtained with Secure Sockets Layer (SSL) and Transport Layer Security (TLS). This
server/client application sends a file from server to client. In this project, we will transfer a text and
image file from server to client. We will use OpenSSL.

## Introduction

One of the most used applications for file transfer is FTP. "File Transfer Protocol", or FTP, was
developed as a file transfer protocol. It provides file transfer between two computers connected to
the Internet. If you want to transfer your files to your website, you can do it easily thanks to the FTP
application. It provides fast transfer of high-dimensional data between two computers. You can use
the FTP application to download files as well as upload files.
Security is also the other important term for Network Applications. Many security protocols give us
encrypted keys, and authentication information then we have the more secure client/server or peerto-peer communication. OpenSSL is an open-source implementation of the SSL and TLS protocols. The
main library built with the C programming language implements the basic encryption graph.
## Implementation of the Project
### A. Required Tools
### Installing GCC
The first step is to get the C compiler, gcc, installed.

```sudo apt-get install build-essential```

or

```sudo apt-get install gcc```

### Installing OpenSSL
OpenSSL can be tricky. You can try your distribution's package manager with the
following commands:

```sudo apt-get install openssl libssl-dev```

### NOTE
Since Linux distribution that I use in Virtualbox is constantly crashing, I have completed some
of the processes I have done there through Windows Subsystem for Linux (WSL). Ubuntu and
Debian WSL.

### 1. Creating Authentication Certificate for SSL
### 2. Developing an FTP server and an FTP client application.
#### 2.1. Compiling server and client
##### Compiling server side
Compilation requires that the crypto and ssl libraries from the OpenSSL kit be linked in with

```gcc -o ftpserver ftpserver.c -lcrypto -lssl```

##### Compiling client side

```gcc -o ftpclient ftpclient.c -lcrypto -lssl```

Running the server examples requires a PEM-style certificate. Running the server requires that the
certificate be in the same directory as the server executable.

### 3. Running FTP server/client app with OpenSSL
```./ftpserver
./ftpclient [IP(Local Network)] [File at Server] [Destination File]
./ftpclient 127.0.0.1 text.txt /mnt/c/ceng421_project/file_client/new_text_1.txt
./ftpclient 127.0.0.1 image.txt /mnt/c/ceng421_project/file_client/new_image_1.txt
```

## Resources
[1] Davis, K., Turner, J. W., & Yocum, N. (2004). The Definitive Guide to Linux Network
Programming. Apress.

[2] Winkle, L. V. (2019). Hands-on network programming with C: Learn socket programming
in C and write secure and optimized network code. Packt Publishing. 
