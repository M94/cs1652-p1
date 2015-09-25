#include "minet_socket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netdb.h>


#define BUFSIZE 1024
#define FILENAMESIZE 100

int handle_connection(int sock);

int main(int argc, char * argv[]) {
    int server_port = -1;
    int rc          =  0;
    int sock        = -1; // Is this the socket file descriptor?

    int newsock; // Is this necessary?
    struct sockaddr_in sa, ca; // Server address, client address?


    /* parse command line args */
    if (argc != 3) {
	fprintf(stderr, "usage: http_server1 k|u port\n");
	exit(-1);
    }

    server_port = atoi(argv[2]);

    if (server_port < 1500) {
	fprintf(stderr, "INVALID PORT NUMBER: %d; can't be < 1500\n", server_port);
	exit(-1);
    }

    /* initialize and make socket */            
    char type_ku = argv[1]; // Grabs the 2nd argument, kernel or user

    if(type_ku == 'k') {
        minet_init(MINET_KERNEL);
    } else if (type_ku == 'u') {
        minet_init(MINET_USER);
    } else {
        fprintf(stderr, "usage: http_server1 k|u port\n");
    }

    if(sock = minet_socket(SOCK_STREAM) < 0) {
        fprintf(stderr, "Error making socket\n");
    }
    
    /* set server address*/
    memset(&sa, 0, sizeof(sa));
    sa.sin_port = htons(server_port);
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_family = AF_INET;

    /* bind listening socket */
    if(minet_bind(sock, &sa) < 0) {
        fprintf(stderr, "Error binding socket\n");
    }

    /* start listening */
    if(minet_listen(sock, 5) < 0) { // What value to put for backlog?
        fprintf(stderr, "Error listening to socket\n");
    }

    /* connection handling loop: wait to accept connection */
    // add wait to connect
    // newsock = minet_accept(sockfd, &ca); // Doesn't make sense to accept here? Need to connect then accept?

    while (1) {
	/* handle connections */
	rc = handle_connection(sock);
    }
}

int handle_connection(int sock) {
    bool ok = false;

    const char * ok_response_f = "HTTP/1.0 200 OK\r\n"	\
	"Content-type: text/plain\r\n"			\
	"Content-length: %d \r\n\r\n";
 
    const char * notok_response = "HTTP/1.0 404 FILE NOT FOUND\r\n"	\
	"Content-type: text/html\r\n\r\n"			\
	"<html><body bgColor=black text=white>\n"		\
	"<h2>404 FILE NOT FOUND</h2>\n"
	"</body></html>\n";

    // Accepting a connection
    int newsock = minet_accept(sock, 0);

    /* first read loop -- get request and headers*/
    char received[BUFSIZE]; // defined as 1024, Takes data in chunks of 1024 bytes?
    char totalReceived[BUFSIZE * BUFSIZE]; // don't know what size to use, just used large buffer
    

    //int minet_read(int fd, char *buf, int len);
    int bytesRead = minet_read(newsock, bufbuf, BUFSIZE); // Returns number of bytes read
    int index = 0; // So the same data isn't read over and over

    while(bytesRead > 0 && (index < (BUFSIZE*BUFSIZE))) {
        //memcpy(destination, source, size)
        memcpy(totalReceived + index, received, bytesRead);
        index = index + bytesRead;

        bytesRead = minet_read(newsock, received, BUFSIZE); // Reads in more bytes
    }
    
    printf("%s\n", totalReceived); // prints the bytes received

    /* parse request to get file name */
    /* Assumption: this is a GET request and filename contains no spaces*/

    char fileName[FILENAMESIZE]; // defined as 100
    char * copy = new char[strlen(totalReceived)] // +1? 

    strcpy(copy, totalReceived);

    if(copy[4] == '/') {
        strcpy(fileName, copy + 5);
    }

    //////
    // Add other cases of finding file name
    //////

    /* try opening the file */
    FILE * fileRequest = 0;
    int fileNameLength = strlen(fileName);

    fileRequest = fopen(fileName, "rb");

    /////
    //  Add other cases of fopen
    /////

    // Reads data of file into buffer
    char * data = 0;

    // Get length of file to determine size of buffer
    fseek(fileRequest, 0, SEEK_END);
    int lengthOfFile = ftell(fileRequest);
    fseek(fileRequest, 0, SEEK_SET); // Point back to beginning no file
    
    data = new char[lengthOfFile];
    memset(lengthOfFile, 0, lengthOfFile);

    // size_t fread(void * ptr, size_t size, size_t count, FILE * stream)
    fread(data, 1, lengthOfFile, fileRequest);
    fclose(fileRequest);


    /* send response */
    if (ok) {

	/* send headers */
	//char * toSend = new char[strlen(ok_response_f)];
    //memset(toSend, 0, strlen(ok_response_f));

	/* send file */
	
    } else {
	// send error response
        minet_write(newsock, notok_response, strlen(notok_response)); // +1 ? 
    }

    /* close socket and free space */
  
    if (ok) {
	return 0;
    } else {
	return -1;
    }
}
