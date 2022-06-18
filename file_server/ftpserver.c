// CENG421_PROJECT - File Transfer Protocol Server/Client with OpenSSL
// 220206018 - İBRAHİM ESER - EEE
// Lecturer: TURGUT KALFAOĞLU

/* File transfer server with OPENSSL */
// Standard Includes
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
// SSL includes
 
#include <openssl/ssl.h>
#include <openssl/err.h>

#define SERVERPORT	8888
#define MAXBUF		1024

int main()
{
    SSL_METHOD *SSL_method;                          // The SSL/TLS method to negotiate
    SSL_CTX *SSL_ctx;                                // The CTX object for SSL
    SSL *SSL_my;                                        // The actual SSL connection

    OpenSSL_add_all_algorithms();                       // Initialize the OpenSSL library
    SSL_load_error_strings();                           // Have the OpenSSL library load its error strings

    SSL_method = TLSv1_server_method();              // Function indicates that the application is a server and supports Transport Layer Security version 1.0 (TLSv1.0).

    if((SSL_ctx = SSL_CTX_new(SSL_method)) == NULL) {
        ERR_print_errors_fp(stderr);
        exit(-1);
    }

    SSL_CTX_use_certificate_file(SSL_ctx,"server.pem",SSL_FILETYPE_PEM);  //loads the first certificate stored in file into ctx.
    SSL_CTX_use_PrivateKey_file(SSL_ctx,"server.pem",SSL_FILETYPE_PEM);

    if(!SSL_CTX_check_private_key(SSL_ctx)) {
        fprintf(stderr,"Private key does not match certificate!\n");
        exit(-1);
    }

    int socket1,socket2;
    int address_length;
    struct sockaddr_in ftpServer, ftpClient;
    int returnStatus;

    /* create a socket */
    socket1 = socket(AF_INET, SOCK_STREAM, 0);     
    if (socket1 == -1)
    {
        fprintf(stderr, "Could not create socket!\n");
        exit(1);
    }

    /*SO_REUSEADDR enables local address reuse*/
    int flag = 1;  
    if (-1 == setsockopt(socket1, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)))
    {  
        perror("setsockopt failed!");  
    }

    /* binding to a socket and using INADDR_ANY for local addresses */
    ftpServer.sin_family = AF_INET;
    ftpServer.sin_port = htons(8888);
    ftpServer.sin_addr.s_addr = INADDR_ANY;

    returnStatus = bind(socket1, (struct sockaddr*)&ftpServer, sizeof(ftpServer));
  
    if (returnStatus == -1)
    {
        fprintf(stderr, "Could not bind to socket!\n");
        exit(1);
    }

    returnStatus = listen(socket1, 5);

    if (returnStatus == -1)
    {
        fprintf(stderr, "Could not listen on socket!\n");
        exit(1);
    }

    for(;;)
    {

        int fd;                              //initiliazing a file descriptor
        int i, readCounter, writeCounter;
        char* bufptr;
        char buf[MAXBUF];
        char filename[MAXBUF];
  
        // waiting for any connection
        address_length = sizeof(ftpClient);

        // accept() is used to handle connection requests            
        // free original socket for other incoming requests   

        socket2 = accept(socket1, (struct sockaddr*)&ftpClient, &address_length);
    
        if (socket2 == -1)
        {
            fprintf(stderr, "Could not accept connection!\n");
            exit(1);
        }
        
        if((SSL_my = SSL_new(SSL_ctx)) == NULL) {
            ERR_print_errors_fp(stderr);
            exit(-1);
        }

        SSL_set_fd(SSL_my,socket2); //sets the file descriptor fd as the input/output facility for the TLS/SSL (encrypted) side of ssl.

        if(SSL_accept(SSL_my) <= 0) {
            ERR_print_errors_fp(stderr);
            exit(-1);
        }

        printf("Connection made with [version,cipher]: [%s,%s] \n",SSL_get_version(SSL_my),SSL_get_cipher(SSL_my));

        // get the filename from the client over the socket
        i = 0;

        if ((readCounter = read(socket2, filename + i, MAXBUF)) > 0)
        {
            i += readCounter;
        }
    
        if (readCounter == -1)
        {
            fprintf(stderr, "Could not read filename from socket!\n");
            close(socket2);
            continue;
        }

        //Limiting the characters of filename
        int j = 0;
        for(j=0; j<strlen(filename); j++)
        {
            if( !((filename[j] == 46) || (48 <= filename[j] && filename[j] <= 57) || (65 <= filename[j] && filename[j] <= 90) || (97 <= filename[j] && filename[j] <= 122)) )
            {
                fprintf(stderr,"Filename can only contain these character: '.','A-Z' and 'a-z' !\n");
                close(socket2);
            }
            else if(filename[j] == 46 && filename[j+1] == 46)
            {
                fprintf(stderr,"Filename can not include '..' !\n");
                close(socket2);
            }
        }

        //Checking whether the file exists or not.
        if( access(filename, F_OK ) == 0 )
        {
            // file exists
            printf("Reading file %s\n", filename);
        }
        else
        {
            // file doesn't exist
            fprintf(stderr,"The file '%s' does not exist!\n",filename);
            close(socket2);
        }
 
        //open the file for reading
        fd = open(filename, O_RDONLY); 

        if (fd == -1)
        {
            fprintf(stderr, "Could not open file for reading!\n");
            close(socket2);
            continue;
        }

        //reset the reading counter
        readCounter = 0;

        //read the file, and send it to the client in chunks of size MAXBUF
        while((readCounter = read(fd, buf, MAXBUF)) > 0)
        {
            writeCounter = 0;
            bufptr = buf;

            while (writeCounter < readCounter)
            {
                readCounter -= writeCounter;
                bufptr += writeCounter;
                writeCounter = write(socket2, bufptr, readCounter);
        
                if (writeCounter == -1) 
                {
                    fprintf(stderr, "Could not write file to client!\n");
                    close(socket2); 
                    continue;
                }
            }
        }


        close(fd);                          //Closing a file descriptor
        SSL_shutdown(SSL_my);               //shut down a TLS/SSL connection
        SSL_free(SSL_my);                   //Free an allocated SSL structure
        close(socket2);                     //Closing child socket
    }
        
    close (socket1);                        //Closing server socket

    SSL_CTX_free(SSL_ctx);               //Free an allocated SSL_CTX object

    return 0;
}
