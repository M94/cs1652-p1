/*
 * Austin Choi
 * CS1652 F15
 */

/* UNCOMMENT FOR MINET 
 * #include "minet_socket.h"
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define BUFSIZE 1024

int main(int argc, char * argv[]) {

    char * server_name = NULL;
    char * server_port = NULL;
    char * server_path = NULL;
    char * req         = NULL; // request
    int status         = 0;

    int sock;

    /*parse args */
    if (argc != 5) {
	fprintf(stderr, "usage: http_client k|u server port path\n");
	exit(-1);
    }

    server_name = argv[2];
    server_port = argv[3];
    server_path = argv[4];

    req = (char *)malloc(strlen("GET  HTTP/1.0\r\n\r\n") + strlen(server_path) + 2); // sets request for later

    /* initialize */
    if (toupper(*(argv[1])) == 'K') { 
        minet_init(MINET_KERNEL);
    } else if (toupper(*(argv[1])) == 'U') { 
        minet_init(MINET_USER);
    } else {
	   fprintf(stderr, "First argument must be k or u\n");
	   exit(-1);
    }

    /* make socket */
    if(sock = minet_socket(SOCK_STREAM) < 0) {
        fprintf(stderr, "Error making socket");
    }

    /* get host IP address  */
    /* Hint: use gethostbyname() */
    struct hostent *hosty;
    hosty = gethostbyname(server_name);

    /* set address */
    struct sockaddr_in sock_addr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(server_port); // changes byte order for server?
    sockaddr.sin_addr.saddr = host_addr; 
    
    /* connect to the server socket */
    if (minet_connect(sock, &sockaddr) < 0) {
        fprintf(stderr, "Error connecting socket");
    }

    /* send request message */
    sprintf(req, "GET /%s HTTP/1.0\r\n\r\n", server_path);
    if(minet_write(sock, req, strlen(req)) < 0) {
        fprintf(stderr, "Error sending request");
    }

    /* wait till socket can be read */
    /* Hint: use select(), and ignore timeout for now */
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(sock, &rfds);

    if (status = select(sock + 1, &rfds, NULL, NULL, NULL) < 0) {
        fprintf(stderr, "Error waiting for socket to be read")
    }
    /* first read loop -- read headers */
    char received[BUFSIZE];
    char totalReceived[BUFSIZE * BUFSIZE]; // Is there a more proper size to use?, just used large buffer


    int bytesRead = minet_read(sock, received, BUFSIZE);
    int index = 0;

    // Loops to read all bytes
    while(bytesRead > 0 && (index < (BUFSIZE*BUFSIZE))) { // First condition to ensure bytes are read, Second condition to ensure no reading over buffer size
        memcpy(totalReceived + index, received, bytesRead);
        index = index + bytesRead;  // Ensure the same positions in buffer aren't overwritten

        bytesRead = minet_read(newsock, received, BUFSIZE); // Reads in more bytes
    }
    char * header = strstr(totalReceived, "\r\n\r\n");
    /* examine return code */
    // Get status code

    // Normal reply has return code 200
    if(status == 200) {
    /* print first part of response: header, error code, etc. */
        // minet_write()
    }
    /* second read loop -- print out the rest of the response: real web content */
        // minet_write()

    /* close socket and deinitialize */

    minet_close(sock);
    minet_deinit();
}
