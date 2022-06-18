// CENG421_PROJECT - File Transfer Protocol Server/Client with OpenSSL
// 220206018 - İBRAHİM ESER - EEE
// Lecturer: TURGUT KALFAOĞLU

/* File transfer client with OPENSSL*/

//Standard includes
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
/*
    SSL includes
 */
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/e_os2.h>

#define SERVERPORT	8888
#define MAXBUF		1024

int main(int argc, char* argv[])
{
    
    SSL_METHOD *SSL_method;                 // The SSL/TLS method to negotiate
    SSL_CTX *SSL_ctx;                       // The CTX object for SSL
    SSL *SSL_my;                            // The actual SSL connection

    int sockd;
    int counter;
    int fd;
    struct sockaddr_in ftpClient;
    char buf[MAXBUF];
    int returnStatus;

    char filename_buff[MAXBUF];
    char destination_buf[MAXBUF];


    OpenSSL_add_all_algorithms();           // Initialize the OpenSSL library
    SSL_load_error_strings();               // Have the OpenSSL library load its error strings

    SSL_method = TLSv1_client_method();     // Function indicates that the application is a server and supports Transport Layer Security version 1.0 (TLSv1.0).

    if((SSL_ctx = SSL_CTX_new(SSL_method)) == NULL) {
        ERR_print_errors_fp(stderr);
        exit(-1);
    }

    if((SSL_my = SSL_new(SSL_ctx)) == NULL) {
        ERR_print_errors_fp(stderr);
        exit(-1);
    }

    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <ip address> <filename> [dest filename]\n", argv[0]);
        exit(1);
    }

    /* create a socket */
    sockd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockd == -1)
    {
        fprintf(stderr, "Could not create socket!\n");
        exit(1);
    }

    /* ++++ SO_REUSEADDR Enables local address reuse */
    int flag = 1;  
    if (-1 == setsockopt(sockd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)))
    {  
        perror("setsockopt fail");  
    }

    bzero(&ftpClient,sizeof(ftpClient));
    //setting up the server 
    ftpClient.sin_family = AF_INET;
    ftpClient.sin_addr.s_addr = inet_addr(argv[1]);
    ftpClient.sin_port = htons(8888);

    //connecting to the server
    returnStatus = connect(sockd, (struct sockaddr*)&ftpClient, sizeof(ftpClient));

    if (returnStatus == -1)
    {
        fprintf(stderr, "Could not connect to server!\n");
        exit(1);
    }

    SSL_set_fd(SSL_my,sockd);

    if(SSL_connect(SSL_my) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(-1);
    }

    printf("Connection made with [version,cipher]: [%s,%s]\n",SSL_get_version(SSL_my),SSL_get_cipher(SSL_my));


    //sending the name of the file we want to the server
    returnStatus = write(sockd, argv[2], strlen(argv[2])+1);

    if (returnStatus == -1)
    {
        fprintf(stderr, "Could not send filename to server!\n");
        exit(1);
    }
    else
    {
        //Limiting the characters of filename
        strncpy(filename_buff,argv[2],MAXBUF-1);
        filename_buff[MAXBUF-1] = '\n';
        //printf("%s\n %d\n",filename_buff,strlen(argv[2]));
        int j = 0;
        for(j=0; j<strlen(filename_buff); j++)
        {
            if( !((filename_buff[j] == 46) || (48 <= filename_buff[j] && filename_buff[j] <= 57) || (65 <= filename_buff[j] && filename_buff[j] <= 90) || (97 <= filename_buff[j] && filename_buff[j] <= 122)) )
            {
                fprintf(stderr,"Filename can only contain these character: '.','A-Z' and 'a-z' !\n");
                //close(sockd);
                exit(1);
            }
            else if(filename_buff[j] == 46 && filename_buff[j+1] == 46)
            {
                fprintf(stderr,"Filename can not include '..' !\n");
                //close(sockd);
                exit(1);
            }
        }
    }

    strncpy(destination_buf,argv[3],MAXBUF-1);
    destination_buf[MAXBUF-1] = '\n';
    // controlling destination file

    int k = 0;
    for(k=0; k<strlen(destination_buf); k++)
    {
        if( (destination_buf[k] == 46) && (destination_buf[k+1] == 46) )
       {
            fprintf(stderr,"Destination file can not include '..' !\n");
            close(sockd);
            exit(1);
            return 0;
       }
    }

    if( access( argv[3], F_OK ) != -1)
    {
        printf("Destination file is found and please enter a file that does not exist!\n");
        close(sockd);
        exit(1);
        return 0;
    }
    else
    {
        // call the shutdown to set our socket to read only
        shutdown(sockd, SHUT_WR);
        // open a handle to our destination file to get the content from the server
        fd = open(argv[3], O_WRONLY | O_CREAT | O_APPEND);

        if (fd == -1)
        {
            fprintf(stderr, "Could not open destination file, using stdout.\n");
            fd = 1;
        }
     
        // reading the file from the socket as long as it has data
        while ((counter = read(sockd, buf, MAXBUF)) > 0)
        {
            // sending the contents to stdout
            write(fd, buf, counter);
        }

        if (counter == -1)
        {
            fprintf(stderr, "Could not read file from socket!\n");
            exit(1);
        }

    }
    SSL_shutdown(SSL_my);
    SSL_free(SSL_my);
    SSL_CTX_free(SSL_ctx);    
    close(sockd);
    
    return 0;
}