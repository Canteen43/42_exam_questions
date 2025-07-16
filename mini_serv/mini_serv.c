#include <unistd.h>		// for write
#include <string.h>		// for strlen
#include <stdio.h>		// for sprintf
#include <sys/types.h>	// sometimes needed for legacy reasons
#include <sys/socket.h>	// defines socket(), AF_INET, SOCK_STREAM, etc.
#include <arpa/inet.h>	// for inet_addr(), htons()

// System call errors are intentionally ignored

// Variables are global for simplicity
char buf[20000];
int server_fd;

void print_buf(int fd)
{
	write(fd, buf, strlen(buf));
}

int main(int argc, char *argv[])
{
	// Check nbr of args
	if (argc < 2)
	{
		sprintf(buf, "Wrong number of args\n", argv[0]);
		print_buf(2);
		return 1;
	}

	// Prepare listening socket
	{
		// Create socket
		server_fd = socket(AF_INET, SOCK_STREAM, 0);

		// Bind socket to port
		struct sockaddr_in address;

		address.sin_family = AF_INET;
		address.sin_addr.s_addr = inet_addr("127.0.0.1");
		address.sin_port = htons(atoi(argv[1]));

		bind(server_fd, (struct sockaddr *)&address, sizeof(address));

		// Start listening
		listen(server_fd, 10);
	}

	// Server loop
	while (1)
	{
	}
	return 0;
}