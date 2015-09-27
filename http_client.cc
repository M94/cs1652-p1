/*
 * Austin Choi
 * CS1652 F15
 */


#include "minet_socket.h"


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
    char * req         = NULL;
    int status         = 0;

    /*parse args */
    if (argc != 5) {
	fprintf(stderr, "usage: http_client k|u server port path\n");
	exit(-1);
    }

    server_name = argv[2];
    server_port = argv[3];
    server_path = argv[4];
    req = (char *)malloc(strlen("GET  HTTP/1.0\r\n\r\n") 
			 + strlen(server_path) + 2);  

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
    //int socket = socket(AF_INET, SOCK_STREAM);
    /* get host IP address  */
    /* Hint: use gethostbyname() */
    //int host_addr = gethostbyname(server_name)->h_addr;
    /* set address */
    //struct sockaddr_in sock_addr;
    //sockaddr.sin_family = AF_INET;
    //sockaddr.sin_port = server_port;
    //sockaddr.sin_addr.saddr = host_addr; 
    
    /* better way to make socket */
    struct addrinfo hints, *res;
    int sock;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(server_name, server_port, &hints, &res);
    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    /* connect to the server socket */
    status = connect(sock, res->ai_addr, res->ai_addrlen);
    if (status == -1) {
       	fprintf(stderr, "Error connecting to server socket\n"); 
    	exit(-1); 
    } else fprintf(stdout, "Connected to server socket\n");
    /* send request message */
    sprintf(req, "GET /%s HTTP/1.0\r\n\r\n", server_path);
    fprintf(stdout, "Sending request:\n%s", req);
    status = minet_write(sock, req, strlen(req));
    if (status < 0) {
    	fprintf(stderr, "Error sending request");
    	exit(-1);
    } else fprintf(stdout, "Request sent\n");
    /* wait till socket can be read. */
    /* Hint: use select(), and ignore timeout for now. */
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(sock, &rfds);
    status = minet_select(sock + 1, &rfds, NULL, NULL, NULL);
    if (status < 0) {
    	fprintf(stderr, "Error waiting for socket to be read\n");
    	exit(-1);
    } else fprintf(stdout, "Socket read\n");
    	
    /* first read loop -- read headers */
    char * header = (char *)malloc(128); 
    FILE * fsock = fdopen(sock, "r"); 
    fgets(header, 128, fsock);
    /* examine return code */
    char * temp = (char *)malloc(128);
    char * ret_code = NULL;    
    strcpy(temp, header);
    strtok(temp, " ");
    ret_code = strtok(NULL, " ");
    // Normal reply has return code 200
    status = strcmp(ret_code, "200");
    /* print first part of response: header, error code, etc. */
    fprintf(stdout, "Response:\n");
    if (status == 0) fprintf(stdout, "%s\n", header);
    else fprintf(stderr, "%s\n", header);	
    /* second read loop -- print out the rest of the response: real web content */
    char * content = (char *)malloc(256);
    do {
    	content = fgets(content, 256, fsock);
    	if (content != NULL) {
		if (status == 0) fprintf(stdout, "%s", content);
		else fprintf(stderr, "%s", content);
	}
    } while (content != NULL);
    /*close socket and deinitialize */
    minet_close(sock);
    return status;
}
