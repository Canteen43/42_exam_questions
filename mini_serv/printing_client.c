#include <unistd.h>		// for write
#include <string.h>		// for strlen
#include <stdio.h>		// for sprintf
#include <sys/types.h>	// sometimes needed for legacy reasons
#include <sys/socket.h>	// defines socket(), AF_INET, SOCK_STREAM, etc.
#include <arpa/inet.h>	// for inet_addr(), htons()
#include <poll.h>		// for poll()
#include <stdlib.h>		// realloc()

// System call errors are intentionally ignored

int main(int argc, char *argv[])
{
	// Check nbr of args
	if (argc < 2)
	{
		printf("Wrong number of args\n");
		return 1;
	}

	// Create socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf("[simple_client] Error creating socket\n");
		return 1;
	}
	else
		printf("[simple_client] Created socket\n");

	// Connect socket to server's port
	struct sockaddr_in address;

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(atoi(argv[1]));

	int connect_ret = connect(sockfd, (struct sockaddr *)&address, sizeof(address));
	if (connect_ret == -1)
	{
		printf("[simple_client] Error during connection\n");
		return 1;
	}
	else
		printf("[simple_client] Connected to server\n");


	// Close socket
	close(sockfd);
	printf("[simple_client] Closed socket\n");

	return 0;
}