/*
 * Austin Choi
 * CS1652 F15
*/

#include "minet_socket.h"
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>

#include <list>
#include <map>

#define BUFSIZE 1024
#define FILENAMESIZE 100

struct connection {
    int sock;
};

int handle_connection(int sock);

int main(int argc, char * argv[]) {
    int server_port = -1;
    int rc          =  0;
    int sock        = -1; // listen for new connections here

    /* parse command line args */
    if (argc != 3) {
	fprintf(stderr, "usage: http_server2 k|u port\n");
	exit(-1);
    }

    server_port = atoi(argv[2]);

    if (server_port < 1500) {
	fprintf(stderr, "INVALID PORT NUMBER: %d; can't be < 1500\n", server_port);
	exit(-1);
    }
    
    if(tolower(*(argv[1])) == 'k') {
        minet_init(MINET_KERNEL);
    } else if (tolower(*(argv[1])) == 'u') {
        minet_init(MINET_USER);
    } else {
        fprintf(stderr, "usage: http_server2 k|u port\n");
	exit(-1);
    }
    
    /* initialize and make socket */
    sock = minet_socket(SOCK_STREAM);
    if (sock < 0) {
    	fprintf(stderr, "Error making socket.\n");
        exit(-1);
    } else fprintf(stdout, "Socket initialized.\n");
    /* set server address*/
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_port = htons(server_port);
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_family = AF_INET;
    /* bind listening socket */
    if(minet_bind(sock, &sa) < 0) {
        fprintf(stderr, "Error binding socket\n");
	minet_perror(NULL);
    	exit(-1);
    }
    /* start listening */
    if(minet_listen(sock, SOMAXCONN) < 0) { // somaxconn = 128
        fprintf(stderr, "Error listening to socket\n");
	minet_perror(NULL);
    	exit(-1);
    } else fprintf(stdout, "Listening on port %d...\n", server_port);
    /* make list of open connections indexed by socket fds */
    std::map<int, struct connection> open_conn;
    /* socket read list */
    fd_set read_list; 
    int read_list_maxfd;;
    /* connection handling loop */
    while (1) {
	
	/* create read list from list of open connections */
    	FD_ZERO(&read_list);
    	read_list_maxfd = 0;
    	std::map<int, struct connection>::iterator it;
    	for (it = open_conn.begin(); it != open_conn.end(); it++) {
    		int s = it->second.sock;
    		FD_SET(s, &read_list);
    		if (s > read_list_maxfd) read_list_maxfd = s;
    	}
    	/* add accept socket to read list (if no open connections) */
    	if (open_conn.size() == 0) minet_set_blocking(sock);
    	else minet_set_nonblocking(sock);
	struct sockaddr_in a_sockaddr;
	int a_sock = minet_accept(sock, &a_sockaddr);
	if (a_sock > 0) {
		FD_SET(a_sock, &read_list);
		if (a_sock > read_list_maxfd) read_list_maxfd = a_sock;
		fprintf(stdout, "Sock %d accepted\n", a_sock); 
	}  	
	/* do a select */
	minet_select(read_list_maxfd + 1, &read_list, NULL, NULL, NULL);
	/* process sockets that are ready */
	for (int s = 0; s <= read_list_maxfd; s++) {
		/* Socket not ready */
		if (FD_ISSET(s, &read_list) == 0) continue; 		
		/* for the accept socket, add accepted connection to connections */
		if (s == a_sock) {
			struct connection a_conn;
			a_conn.sock = s;
			open_conn.insert(std::pair<int,struct connection>(s,a_conn));
			// make new accept socket
			fprintf(stdout, "Sock %d is now an open connection\n", s);
		/* for a connection socket, handle the connection */
		} else {
			rc = handle_connection(s);
			open_conn.erase(s); // remove connection
		}
	}
    }
}

int handle_connection(int sock) {
    fprintf(stdout, "Handling sock %d...\n", sock);
    bool ok = false;

    const char * ok_response_f = "HTTP/1.0 200 OK\r\n"	\
	"Content-type: text/plain\r\n"			\
	"Content-length: %d \r\n\r\n";
    
    const char * notok_response = "HTTP/1.0 404 FILE NOT FOUND\r\n"	\
	"Content-type: text/html\r\n\r\n"			\
	"<html><body bgColor=black text=white>\n"		\
	"<h2>404 FILE NOT FOUND</h2>\n"				\
	"</body></html>\n";
	
    
    /* first read loop -- get request and headers*/
    
    /*
    char received[1024]; // defined as 1024, Takes data in chunks of 1024 bytes?
    char totalReceived[1024 * 1024]; // Is there a more proper size to use?, just used large buffer

    int bytesRead = minet_read(sock, received, BUFSIZE); // minet_read() returns the number of bytes read
    int index = 0;

    // Loops to read all bytes
    while(bytesRead > 0 && (index < (BUFSIZE*BUFSIZE))) { // First condition to ensure bytes are read, Second condition to ensure no reading over buffer size
        memcpy(totalReceived + index, received, bytesRead);
        index = index + bytesRead;  // Ensure the same positions in buffer aren't overwritten

        bytesRead = minet_read(sock, received, BUFSIZE); // Reads in more bytes
    }
    *
    
    /* parse request to get file name */
    /* Assumption: this is a GET request and filename contains no spaces*/
    
    /*
    char fileName[100];
    char * fileStart = strchr(totalReceived, ' ') + 1;
    char * fileEnd = strchr(fileStart, ' ');
    int fileNameLength = strlen(fileStart) - strlen(fileEnd);
    strcpy(fileName, fileStart);
    int toCut = strlen(fileName) - fileNameLength;
    fileName[strlen(fileName) - toCut] = 0;
    */
    
    /* try opening the file */
    
    /*
	FILE * fileRequest = 0;
	fileRequest = fopen(fileName, "rb");
	// Reads data of file into buffer
	char * data = 0;
	// Get length of file to determine size of buffer for data
	fseek(fileRequest, 0, SEEK_END);
	int lengthOfFile = ftell(fileRequest);
	fseek(fileRequest, 0, SEEK_SET); // Point back to beginning no file 
	data = new char[lengthOfFile];
	memset(data, 0, lengthOfFile);
	if(fread(data, 1, lengthOfFile, fileRequest)) { // size_t fread(void * ptr, size_t size, size_t count, FILE * stream)
		ok = true; // We want true no matter what because we have to output an error message
	}
	fclose(fileRequest);
    */
	
    /* send response */
    if (ok) {
	/* send headers */
	ok = send(sock, ok_response_f, strlen(notok_response), 0);
	/* send file */
	
	/*
        if(minet_write(sock, data, lengthOfFile) < 0) {
           fprintf(stderr, "Error sending file\n");
        }
        */
        
    } else {
	// send error response
	ok = send(sock, notok_response, strlen(notok_response), 0);
    }

    /* close socket and free space */
    minet_close(sock);
    if (ok) {
    	fprintf(stdout, "Response sent to sock %d\n", sock);
	return 0;
    } else {
        fprintf(stderr, "Error responding to sock %d\n", sock);
	return -1;
    }
}
