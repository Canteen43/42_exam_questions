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
	int return_value;
	
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
		printf("[printing_client] Error creating socket\n");
		return 1;
	}
	else
		printf("[printing_client] Created socket\n");

	// Connect socket to server's port
	struct sockaddr_in address;

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(atoi(argv[1]));

	return_value = connect(sockfd, (struct sockaddr *)&address, sizeof(address));
	if (return_value == -1)
	{
		printf("[printing_client] Error during connection\n");
		return 1;
	}
	else
		printf("[printing_client] Connected to server\n");

	
	// Prepare print array
	struct pollfd* poll_arr = malloc(sizeof(struct pollfd));
	poll_arr[0].fd = sockfd;
	poll_arr[0].events = POLLIN;
	poll_arr[0].revents = 0;
	
	char buf[20000];


	// Print loop
	while (1)
	{
		// Wait until event occurs
		return_value = poll(poll_arr, 1, -1);
		if (return_value == -1)
		{
			printf("[printing_client] Error creating socket\n");
			return 1;
		}

		// Check for and accept new client
		if (poll_arr[0].revents & POLLIN)
		{
			return_value = read(sockfd, buf, sizeof(buf) - 1);
			if (return_value == -1)
			{
				printf("[printing_client] Read error\n");
				return 1;
			}
			else if (return_value == 0)
			{
				printf("[printing_client] Server closed connection\n");
				break;
			}
			buf[return_value] = '\0';
			printf("[printing_client] Received: \"%s\"\n", buf);
		}
	}



	// Close socket
	close(sockfd);
	printf("[printing_client] Closed socket\n");

	return 0;
}