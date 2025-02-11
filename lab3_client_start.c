/* This code is an updated version of the sample code from "Computer Networks: A Systems
 * Approach," 5th Edition by Larry L. Peterson and Bruce S. Davis. Some code comes from
 * man pages, mostly getaddrinfo(3). */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

/*
 * Lookup a host IP address and connect to it using service. Arguments match the first two
 * arguments to getaddrinfo(3).
 *
 * Returns a connected socket descriptor or -1 on error. Caller is responsible for closing
 * the returned socket.
 */

int HTML_tag_and_byte_counter();
int lookup_and_connect( const char *host, const char *service );


int main( ) {
	int s;
	const char *host = "www.ecst.csuchico.edu";
	const char *port = "80";

	/* Lookup IP and connect to server */
	if ( ( s = lookup_and_connect( host, port ) ) < 0 ) {
		exit( 1 );
	}

	//start of Program 1: HTML Tag and Byte Counter
	HTML_tag_and_byte_counter();

	close( s );

	return 0;
}

int HTML_tag_and_byte_counter(const int *s) {
	//allow user to select chunk size
	int chunk_size;
	printf("Chunk size?: ");
	scanf("%d", &chunk_size);

	//request files from server
	char buf[256];
	char request_files[] = "GET /~kkredo/file.html HTTP/1.0\r\n\r\n";
	strcpy(buf, request_files);

	//check if connection was established
	ssize_t outgoing_data = send(*s, buf, strlen(buf), 0);
	if(outgoing_data == -1) {
		perror("OUTGOING_DATA\n")
		return -1;
	}

	//connection established, recieve files from server
	ssize_t incoming_data;
	while(1) {
		incoming_data = recv(*s, buf, sizeof(buf) - 1, 0);

		//connection closed
		if(incoming_data == 0) {
			break;
		}
		//error
		else if(incoming_data == -1) {
			perror("INCOMING_DATA\n");
			break;
		}
		//process incoming data
		else {
			for(int i = 0; i < sizeof(incoming_data) - 3; i++) {
				if(incoming_data[i] == '<' && incoming_data[i+1] == 'h' &&
					incoming_data[i] == '1' && incoming_data[i+1] == '>' &&) {

				}
			}
		}
	}
}

int lookup_and_connect( const char *host, const char *service ) {
	struct addrinfo hints;
	struct addrinfo *rp, *result;
	int s;

	/* Translate host name into peer's IP address */
	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	if ( ( s = getaddrinfo( host, service, &hints, &result ) ) != 0 ) {
		fprintf( stderr, "stream-talk-client: getaddrinfo: %s\n", gai_strerror( s ) );
		return -1;
	}

	/* Iterate through the address list and try to connect */
	for ( rp = result; rp != NULL; rp = rp->ai_next ) {
		if ( ( s = socket( rp->ai_family, rp->ai_socktype, rp->ai_protocol ) ) == -1 ) {
			continue;
		}

		if ( connect( s, rp->ai_addr, rp->ai_addrlen ) != -1 ) {
			break;
		}

		close( s );
	}
	if ( rp == NULL ) {
		perror( "stream-talk-client: connect" );
		return -1;
	}
	freeaddrinfo( result );

	return s;
}
