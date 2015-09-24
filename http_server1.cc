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
    int sock        = -1;
    
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

    /* initialize */
    /*
    if (toupper(*(argv[1])) == 'K') { 
	/* UNCOMMENT FOR MINET 
	 * minet_init(MINET_KERNEL);
         */
    } else if (toupper(*(argv[1])) == 'U') { 
	/* UNCOMMENT FOR MINET 
	 * minet_init(MINET_USER);
	 */
    } else {
	fprintf(stderr, "First argument must be k or u\n");
	exit(-1);
    }
    */


    /* initialize and make socket */
    char type_ku = argv[1]; // Grabs the 2nd argument, kernel or user

    if(type_ku == 'k') {
        minet_init(MINET_KERNEL);
    } else if (type_ku == 'u') {
        minet_init(MINET_USER);
    } else {
        fprintf(stderr, "usage: http_server1 k|u port\n");
    }

    /* set server address*/
    memset(&sa, 0, sizeof(sa));
    sa.sin_port = htons(server_port);
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_family = AF_INET;

    /* bind listening socket */
    if(minet_bind(sock, &sa) < 0) {
        fprintf(stderr, "ENTER ERROR MESSAGE HERE\n");
    }

    /* start listening */
    if(minet_listen(sock, 5) < 0) { // What to put in backlog argument? 5 just placeholder
        fprintf(stderr, "ENTER ERROR MESSAGE HERE\n");
    }

    /* connection handling loop: wait to accept connection */

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
    int index; // So the same data isn't read over and over

    while(bytesRead > 0 && (index < (BUFSIZE*BUFSIZE))) {
        //memcpy(destination, source, size)
        memcpy(totalReceived + index, received, BUFSIZE);
        index = index + received;

        bytesRead = minet_read(newsock, received, BUFSIZE); // Reads in more bytes
    }

    
    /* parse request to get file name */
    /* Assumption: this is a GET request and filename contains no spaces*/
    char fileName[FILENAMESIZE]; // defined as 100
    // how the fuck....
    // Would involve totalReceived and fileName

    /* try opening the file */

    /* send response */
    if (ok) {
	/* send headers */
	
	/* send file */
	
    } else {
	// send error response
    }
    
    /* close socket and free space */
  
    if (ok) {
	return 0;
    } else {
	return -1;
    }
}
