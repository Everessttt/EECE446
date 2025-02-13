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
 #include <stdbool.h>
 
 #define SERVER_PORT "80"
 
 /*
  * Lookup a host IP address and connect to it using service. Arguments match the first two
  * arguments to getaddrinfo(3).
  *
  * Returns a connected socket descriptor or -1 on error. Caller is responsible for closing
  * the returned socket.
  */
 int lookup_and_connect( const char *host, const char *service );
 
 int main(int argc, char *argv[]) {
	 int chunk_size = atoi(argv[1]);
	 char *host = "www.ecst.csuchico.edu";
	 char buf[chunk_size];
	 int s;
 
	 /* Lookup IP and connect to server */
	 if ( ( s = lookup_and_connect( host, SERVER_PORT ) ) < 0 ) {
		 exit( 1 );
	 }
 
	 char* request = "GET /~kkredo/file.html HTTP/1.0\r\n\r\n";
	 strcpy(buf, request);
	 send(s, buf, strlen(buf), 0);
 
	 ssize_t incoming_data, additional_data;
	 unsigned char num_h1_tags = 0;
	 size_t num_bytes = 0;
	 bool exit = false; //set to true to exit while loop
	 while(!exit) {
		 incoming_data = 0;
		 //request data until entire chunk is recieved
		 while(incoming_data != chunk_size) {
			 additional_data = recv(s, buf + incoming_data, chunk_size - incoming_data, 0);
 
			 //connection closed
			 if(additional_data == 0) {
				 incoming_data += additional_data;
				 exit = true;
				 break;
			 }
			 //error
			 else if(additional_data == -1) {
				 exit = true;
				 perror("ERROR: INCOMING_DATA\n");
				 break;
			 }
			 //accumulate data
			 else {
				 incoming_data += additional_data;
			 }
		 }
 
		 //process incoming data
		 num_bytes += incoming_data;
		 buf[incoming_data] = '\0';
		 for(int i = 0; i < incoming_data - 3; i++) {
			 if(buf[i] == '<' && buf[i+1] == 'h' &&
				 buf[i+2] == '1' && buf[i+3] == '>') {
					 num_h1_tags += 1;
			 }
		 }
	 }
 
	 printf("Number of <h1> tags: %u\n", num_h1_tags);
	 printf("Number of bytes: %zu\n", num_bytes);
	 return num_h1_tags;
 
	 close( s );
 
	 return 0;
 }
 
 int lookup_and_connect( const char *host, const char *service ) {
	 struct addrinfo hints;
	 struct addrinfo *rp, *result;
	 int s;
 
	 /* Translate host name into peer's IP address */
	 memset( &hints, 0, sizeof( hints ) );
	 hints.ai_family = AF_INET;
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
 