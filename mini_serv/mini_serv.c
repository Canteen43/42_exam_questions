#include <unistd.h>		// for write
#include <string.h>		// for strlen
#include <stdio.h>		// for sprintf
#include <sys/types.h>	// sometimes needed for legacy reasons
#include <sys/socket.h>	// defines socket(), AF_INET, SOCK_STREAM, etc.
#include <arpa/inet.h>	// for inet_addr(), htons()
#include <poll.h>		// for poll()

// System call errors are intentionally ignored

// Client struct
struct socket
{
	int	fd;
	int	id;
};

// Shared variables are global for simplicity
char buf[20000];
int arr_size = 0;
int next_client_id = 0;
struct pollfd* poll_arr = NULL;
struct socket* socket_arr = NULL;




void print_buf(int fd)
{
	write(fd, buf, strlen(buf));
}

void send_buf()
{
	for (int i = 0; i < arr_size; ++i)
	{
		write(socket_arr[i].fd, buf, strlen(buf));
	}
}

void accept_client()
{
	// Accept new client (address not used but required by function)
	struct sockaddr_in client_address;
	socklen_t addr_len = sizeof(client_address);
	int client_fd = accept(socket_arr[0].fd, (struct sockaddr *)&client_address, &addr_len);

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
	sprintf(buf, "Client %d says: ", client_id);
	
	// Read message from client
	int bytes_read = read(client_fd, buf + strlen(buf), sizeof(buf) - strlen(buf));
	
	// If read returns 0, client disconnected and needs to be removed
	if (bytes_read == 0)
	{
		// Close the socket fd
		close(client_fd);

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
		send_buf();
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
		int listening_fd = socket(AF_INET, SOCK_STREAM, 0);

		// Bind socket to port
		struct sockaddr_in address;

		address.sin_family = AF_INET;
		address.sin_addr.s_addr = inet_addr("127.0.0.1");
		address.sin_port = htons(atoi(argv[1]));

		bind(listening_fd, (struct sockaddr *)&address, sizeof(address));

		// Start listening
		listen(listening_fd, 10);

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
		poll(poll_arr, arr_size, -1);

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
	return 0;
}