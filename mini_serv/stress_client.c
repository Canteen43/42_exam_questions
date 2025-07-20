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
	if (argc != 3)
	{
		printf("[stress_client] Wrong number of args\n");
		return 1;
	}

	// Loop connecting multiple times to the server
	int reps = atoi(argv[2]);
	for (int i = 0; i < reps; ++i)
	{
		// Create socket
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1)
		{
			printf("[stress_client] [Rep: %d] Error creating socket\n", i);
			return 1;
		}
		else
			printf("[stress_client] [Rep: %d] Created socket\n", i);

		// Connect socket to server's port
		struct sockaddr_in address;

		address.sin_family = AF_INET;
		address.sin_addr.s_addr = inet_addr("127.0.0.1");
		address.sin_port = htons(atoi(argv[1]));

		int connect_ret = connect(sockfd, (struct sockaddr *)&address, sizeof(address));
		if (connect_ret == -1)
		{
			printf("[stress_client] [Rep: %d] Error during connection\n", i);
			return 1;
		}
		else
			printf("[stress_client] [Rep: %d] Connected to server\n", i);


		// Close socket
		close(sockfd);
		printf("[stress_client] [Rep: %d] Closed socket\n", i);
	}

	return 0;
}