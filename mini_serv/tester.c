#include <unistd.h>		// for write
#include <string.h>		// for strlen
#include <stdio.h>		// for sprintf
#include <sys/types.h>	// sometimes needed for legacy reasons
#include <sys/socket.h>	// defines socket(), AF_INET, SOCK_STREAM, etc.
#include <arpa/inet.h>	// for inet_addr(), htons()
#include <poll.h>		// for poll()
#include <stdlib.h>		// realloc()
#include <errno.h>		// for errno
#include <fcntl.h>		// for fcntl()

#define NBR_TEST_CLIENTS 10
#define PORT 8080

int main()
{
	// Declare reusable return variable
	int ret = 0;
	
	// Prepare log file
	remove("tester.log");
	FILE* logfile = fopen("tester.log", "a");

	// Creating array of client file descriptors
	int* client_fd_arr = malloc(sizeof(int) * NBR_TEST_CLIENTS);

	// Preparing server's address
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(PORT);

	// Loop creating and connecting test clients
	for (int i = 0; i < NBR_TEST_CLIENTS; ++i)
	{
		// Create socket
		ret = socket(AF_INET, SOCK_STREAM, 0);
		if (ret == -1)
		{
			fprintf(logfile, "[Fatal Error] Context: creating socket %d, Description: %s\n", i, strerror(errno));
			fflush(logfile);
			return 1;
		}
		else
			client_fd_arr[i] = ret;

		// Connect socket to server
		ret = connect(client_fd_arr[i], (struct sockaddr *)&address, sizeof(address));
		if (ret == -1)
		{
			fprintf(logfile, "[Fatal Error] Context: connecting socket %d, Description: %s\n", i, strerror(errno));
			fflush(logfile);
			return 1;
		}
	}
	fprintf(logfile, "Finished creating and connecting %d clients\n", NBR_TEST_CLIENTS);
	fflush(logfile);

	// Loop sending messages to server
	char buf[100];
	for (int i = 0; i < NBR_TEST_CLIENTS; ++i)
	{
		sprintf(buf, "Hello from client %d", i);
		ret = send(client_fd_arr[i], buf, strlen(buf), 0);
		if (ret == -1)
		{
			fprintf(logfile, "[Error] Context: sending message from client %d, Description: %s\n", i, strerror(errno));
			fflush(logfile);
		}
	}
	fprintf(logfile, "Finished sending messages from all clients\n");
	fflush(logfile);

	// Loop closing all client sockets except the first one
	for (int i = 1; i < NBR_TEST_CLIENTS; ++i)
	{
		ret = close(client_fd_arr[i]);
		if (ret == -1)
		{
			fprintf(logfile, "[Error] Context: closing socket %d, Description: %s\n", i, strerror(errno));
			fflush(logfile);
		}
	}
	fprintf(logfile, "Finished closing non-zero client sockets\n");
	fflush(logfile);

	// Reading the first client socket and printing to the log file
	fprintf(logfile, "Reading from socket of client 0:\n\"\"\"\n");
	fflush(logfile);
	
	sleep(3);
	
	char read_buf[4000];
	ret = recv(client_fd_arr[0], read_buf, sizeof(read_buf) - 1, 0);
	if (ret < 0)
	{
		fprintf(logfile, "[Error] Context: reading socket of client 0, Description: %s\n", strerror(errno));
		fflush(logfile);
	}
	fprintf(logfile, "%.*s\n", ret, read_buf);
	fprintf(logfile, "\"\"\"\n");
	fflush(logfile);

	// Close the first client socket
	close(client_fd_arr[0]);
	fprintf(logfile, "Tester finished. Closed socket of client 0\n");
	fflush(logfile);
	fclose(logfile);

	return 0;
}