/* This code is an updated version of the sample code from "Computer Networks: A Systems
 * Approach," 5th Edition by Larry L. Peterson and Bruce S. Davis. Some code comes from
 * man pages, mostly getaddrinfo(3). */
<<<<<<< HEAD
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
	 if(chunk_size < 4 || chunk_size > 1000) {
=======
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define SERVER_PORT "80"

int lookup_and_connect(const char *host, const char *service);

int recvall(int s, char *buf, int *len, int chunk_size)
{
	int total = 0;							// how many bytes we've received
	int bytesleft = chunk_size; // how many more we need to receive
	int n;

	while (bytesleft > 0) // Ensure it only requests what’s left
	{
		n = recv(s, buf + total, bytesleft, 0);
		if (n <= 0)
		{
			break; // Stop on error or connection closed
		}

		//printf("n: %u \n", n);

		total += n;

		//printf("Total: %u \n", total);

		bytesleft -= n;

		//printf("bytes left: %u \n", bytesleft);
	}

	*len = total; // Return number actually received

	return (n < 0) ? -1 : 0; // Ensure correct return value
}

int sendall(int s, const char *buf, int *len)
{
	int total = 0;				// how many bytes we've sent
	int bytesleft = *len; // how many we have left to send
	int n;

	while (total < *len)
	{
		n = send(s, buf + total, bytesleft, 0);
		if (n == -1)
		{
			break;
		}
		total += n;
		bytesleft -= n;
	}

	*len = total; // return number actually sent here

	return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}

int main(int argc, char *argv[])
{
	int chunk_size = atoi(argv[1]);

	if (chunk_size < 4 || chunk_size > 1000)
	{
>>>>>>> 1e1defa (Updated recvall and sendall functions, passing partial tests)
		printf("ERROR: INVALID CHUNK SIZE\n");
		printf("4 <= CHUNK SIZE <= 1000\n");
		return -1;
	 }

<<<<<<< HEAD
	 char *host = "www.ecst.csuchico.edu";
	 char buf[chunk_size + 1];
	 int s;
 
	 /* Lookup IP and connect to server */
	 if ( ( s = lookup_and_connect( host, SERVER_PORT ) ) < 0 ) {
		 exit( 1 );
	 }
 
	 //sent request to host
	 char *request = "GET /~kkredo/file.html HTTP/1.0\r\n\r\n";
	 strcpy(buf, request);
	 ssize_t data_sent, total_data_sent = 0;
	 size_t remaining_data = strlen(buf);
	 while(remaining_data > 0) {
		data_sent = send(s, buf + total_data_sent, remaining_data, 0);
		//printf("SEND: %zu\n", remaining_data);
		if(data_sent == -1) {
			perror("ERROR: OUTGOING_DATA\n");
			return -1;
		}
		else {
			total_data_sent += data_sent;
			remaining_data -= data_sent;
		}
	 }
	 //printf("CONNECTION ESTABLISHED\n");
 
	 //recieve data from host
	 ssize_t incoming_data, additional_data;
	 unsigned char num_h1_tags = 0;
	 size_t num_bytes = 0;
	 bool exit = false; //set to true to exit while loop
	 while(!exit) {
		 incoming_data = 0;
		 //request data until entire chunk is recieved
		 while(incoming_data != chunk_size) {
			 additional_data = recv(s, buf + incoming_data, chunk_size - incoming_data, 0);
			 //printf("RECV: %zu\n", additional_data);
 
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
 
=======
	char *host = "www.ecst.csuchico.edu";
	char buf[chunk_size + 1]; // Buffer with space for null termination
	int s;

	if ((s = lookup_and_connect(host, SERVER_PORT)) < 0)
	{
		exit(1);
	}

	const char *request = "GET /~kkredo/file.html HTTP/1.0\r\n\r\n";
	int req_len = strlen(request);

	// Replacing sendall with simple send
	if (sendall(s, request, &req_len) == -1)
	{
		perror("send");
		close(s);
		exit(1);
	}

	int amount_read = 0;
	unsigned char num_h1_tags = 0;
	size_t num_bytes = 0;

	while (true)
	{
		amount_read = 0;

		if (recvall(s, buf, &amount_read, chunk_size) == -1)
		{
			perror("recvall");
			break;
		}

		if (amount_read == 0)
		{
			break; // Server closed connection
		}

		num_bytes += amount_read;

		// Ensure safe null termination
		if (amount_read < chunk_size)
		{
			buf[amount_read] = '\0'; // Prevent overflow
		}

		// Process the chunk, replace with library
		for (ssize_t i = 0; i < amount_read - 3; i++)
		{
			if (buf[i] == '<' && buf[i + 1] == 'h' && buf[i + 2] == '1' && buf[i + 3] == '>')
			{
				num_h1_tags++;
			}
		}
	}

	printf("Number of <h1> tags: %u\n", num_h1_tags);
	printf("Number of bytes: %zu\n", num_bytes);

	close(s);
	return 0;
}

int lookup_and_connect(const char *host, const char *service)
{
	struct addrinfo hints, *rp, *result;
	int s;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // use AF_UNSPEC
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	if ((s = getaddrinfo(host, service, &hints, &result)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		if ((s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1)
		{
			continue;
		}

		if (connect(s, rp->ai_addr, rp->ai_addrlen) != -1)
		{
			break;
		}

		close(s);
	}

	if (rp == NULL)
	{
		perror("connect");
		return -1;
	}

	freeaddrinfo(result);
	return s;
}
>>>>>>> 1e1defa (Updated recvall and sendall functions, passing partial tests)
