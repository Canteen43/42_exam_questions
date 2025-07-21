#include <unistd.h>		// for write
#include <string.h>		// for strlen
#include <stdio.h>		// for sprintf
#include <sys/types.h>	// sometimes needed for legacy reasons
#include <sys/socket.h>	// defines socket(), AF_INET, SOCK_STREAM, etc.
#include <arpa/inet.h>	// for inet_addr(), htons()
#include <poll.h>		// for poll()
#include <stdlib.h>		// realloc()
#include <errno.h>		// for errno

// Printing:
// The exercise does not allow printf(). It is used for debugging only. 
// "Real" printing has to be done with write().

// Error handling:
// The exercise dicates that relevant system call errors should lead to a fatal exit.
// Irrelevant errors are deliberately ignored.
// printf() is added for debugging.

// Signal handling:
// The exercise forbids signal handling.
// Therefore, the program has to be terminated with Ctrl+C and resources cannot be freed.

// Client struct
struct socket
{
	int	fd;
	int	id;
};

// Shared variables are global for simplicity
char buf[20000];
int ret = 0;
int next_client_id = 0;
int arr_size = 0;
struct pollfd* poll_arr = NULL;
struct socket* socket_arr = NULL;


void exit_fatal()
{
	ret = write(2, "Fatal error\n", 12);
	exit(1);
}

void send_buf()
{
	for (int i = 1; i < arr_size; ++i)
		ret = send(socket_arr[i].fd, buf, strlen(buf), MSG_NOSIGNAL);
}

void accept_client()
{
	// Accept new client (address not used but required by function)
	struct sockaddr_in client_address;
	socklen_t addr_len = sizeof(client_address);
	ret = accept(socket_arr[0].fd, (struct sockaddr *)&client_address, &addr_len);
	int client_fd = ret;

	// Set client id
	int client_id = next_client_id;
	++next_client_id;

	// Increase array size and add client to arrays
	++arr_size;

	socket_arr = realloc(socket_arr, sizeof(struct socket) * arr_size);
	socket_arr[arr_size - 1].fd = client_fd;
	socket_arr[arr_size - 1].id = client_id;

	poll_arr = realloc(poll_arr, sizeof(struct pollfd) * arr_size);
	poll_arr[arr_size - 1].fd = client_fd;
	poll_arr[arr_size - 1].events = POLLIN;
	poll_arr[arr_size - 1].revents = 0;

	// Prepare and send message
	sprintf(buf, "New Client connected: %d\n", client_id);
	send_buf();
}

void handle_client(int index)
{	
	// Get client fd and id
	int client_fd = socket_arr[index].fd;
	int client_id = socket_arr[index].id;
	
	// Before reading, add message prefix to buffer
	int prefix_length = sprintf(buf, "Client %d says: ", client_id);
	
	// Read message from client
	ret = read(client_fd, buf + prefix_length, sizeof(buf) - prefix_length - 1);
	
	// If read returns 0, client disconnected and needs to be removed
	if (ret == 0)
	{
		// Close the socket fd
		ret = close(client_fd);

		// Remove client from arrays by swapping with the last element
		socket_arr[index] = socket_arr[arr_size - 1];
		poll_arr[index] = poll_arr[arr_size - 1];
		--arr_size;
		socket_arr = realloc(socket_arr, sizeof(struct socket) * arr_size);
		poll_arr = realloc(poll_arr, sizeof(struct pollfd) * arr_size);

		// Prepare and send message about disconnection
		sprintf(buf, "Client %d disconnected\n", client_id);
		send_buf();
	}
	// Otherwise, the client sent a message that needs to be broadcast
	else
	{
		buf[prefix_length + ret] = '\n';
		buf[prefix_length + ret + 1] = '\0';
		send_buf();
	}
}

int main(int argc, char *argv[])
{
	// Check nbr of args
	if (argc < 2)
	{
		ret = write(2, "Wrong number of args\n", 22);
		return 1;
	}

	// Prepare listening socket
	{
		// Create socket
		int listening_fd = socket(AF_INET, SOCK_STREAM, 0);

		// Bind socket to port
		struct sockaddr_in address;

		address.sin_family = AF_INET;
		address.sin_addr.s_addr = inet_addr("127.0.0.1");
		address.sin_port = htons(atoi(argv[1]));

		ret = bind(listening_fd, (struct sockaddr *)&address, sizeof(address));

		// Start listening
		ret = listen(listening_fd, 10);

		// Add listening socket fd to arrays
		socket_arr = malloc(sizeof(struct socket));
		socket_arr[0].fd = listening_fd;
		socket_arr[0].id = -1;

		poll_arr = malloc(sizeof(struct pollfd));
		poll_arr[0].fd = listening_fd;
		poll_arr[0].events = POLLIN;
		poll_arr[0].revents = 0;

		arr_size = 1;
	}

	// Server loop
	while (1)
	{
		// Wait until event occurs
		ret = poll(poll_arr, arr_size, -1);

		// Check for and accept new client
		if (poll_arr[0].revents & POLLIN)
		{
			accept_client();
			continue;
		}

		// Check for and handle client messages
		for (int i = 1; i <= arr_size; ++i)
		{
			if (poll_arr[i].revents & POLLIN)
			{
				handle_client(i);
				continue;
			}
		}
	}
}