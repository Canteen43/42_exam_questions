#include <unistd.h>		// for write
#include <string.h>		// for strlen
#include <stdio.h>		// for sprintf
#include <sys/types.h>	// sometimes needed for legacy reasons
#include <sys/socket.h>	// defines socket(), AF_INET, SOCK_STREAM, etc.
#include <arpa/inet.h>	// for inet_addr(), htons()
#include <poll.h>		// for poll()
#include <stdlib.h>		// realloc()
#include <errno.h>		// for errno

// System call errors are intentionally ignored
// Print statements with "[mini_serv]" prefix are for debugging purposes

// Client struct
struct socket
{
	int	fd;
	int	id;
};

// Shared variables are global for simplicity
char buf[20000];
int return_value = 0;
int arr_size = 0;
int next_client_id = 0;
struct pollfd* poll_arr = NULL;
struct socket* socket_arr = NULL;


void exit_fatal(const char* func, const char* msg)
{
	return_value = write(2, "[mini_serv] Fatal error in: ", 28);
	return_value = write(2, func, strlen(func));
	return_value = write(2, ": ", 2);
	return_value = write(2, msg, strlen(msg));
	return_value = write(2, "\n", 1);
	(void) return_value;
	exit(1);
}

void print_buf(int fd)
{
	return_value = write(fd, buf, strlen(buf));
	if (return_value == -1)
	{
		exit_fatal(__FUNCTION__, "Write error");
	}
}

void send_buf()
{
	for (int i = 1; i < arr_size; ++i)
	{
		return_value = send(socket_arr[i].fd, buf, strlen(buf), MSG_NOSIGNAL);
		if (return_value == -1 && errno == EPIPE)
		{
			printf("[mini_serv] send() returned EPIPE, skipping that client\n");
		}
		else if (return_value == -1)
		{
			perror("send");
			exit_fatal(__FUNCTION__, "Write error\n");
		}
	}
}

void accept_client()
{
	// Accept new client (address not used but required by function)
	struct sockaddr_in client_address;
	socklen_t addr_len = sizeof(client_address);
	return_value = accept(socket_arr[0].fd, (struct sockaddr *)&client_address, &addr_len);
	if (return_value == -1)
	{
		exit_fatal(__FUNCTION__, "Accept error");
	}
	int client_fd = return_value;

	// Set client id
	int client_id = next_client_id;
	++next_client_id;

	// Increase array size and add client to arrays
	++arr_size;

	socket_arr = realloc(socket_arr, sizeof(struct socket) * arr_size);
	if (socket_arr == NULL)
	{
		exit_fatal(__FUNCTION__, "Memory allocation error for socket_arr");
	}
	socket_arr[arr_size - 1].fd = client_fd;
	socket_arr[arr_size - 1].id = client_id;

	poll_arr = realloc(poll_arr, sizeof(struct pollfd) * arr_size);
	if (poll_arr == NULL)
	{
		exit_fatal(__FUNCTION__, "Memory allocation error for poll_arr");
	}
	poll_arr[arr_size - 1].fd = client_fd;
	poll_arr[arr_size - 1].events = POLLIN;
	poll_arr[arr_size - 1].revents = 0;

	
	// Prepare and send message
	sprintf(buf, "New Client connected: %d", client_id);
	send_buf();
	
	// Log for debugging
	printf("[mini_serv] Accepted client connection: ID: %d, FD: %d\n", client_id, client_fd);
}

void handle_client(int index)
{	
	// Get client fd and id
	int client_fd = socket_arr[index].fd;
	int client_id = socket_arr[index].id;
	
	// Before reading, add message prefix to buffer
	sprintf(buf, "Client %d says: ", client_id);
	int prefix_length = strlen(buf);
	
	// Read message from client
	return_value = read(client_fd, buf + prefix_length, sizeof(buf) - prefix_length - 1);
	if (return_value == -1)
	{
		exit_fatal(__FUNCTION__, "Read error");
	}
	
	// If read returns 0, client disconnected and needs to be removed
	if (return_value == 0)
	{
		// Close the socket fd
		return_value = close(client_fd);
		if (return_value == -1)
		{
			exit_fatal(__FUNCTION__, "Close error");
		}

		// Remove client from arrays by swapping with the last element
		socket_arr[index] = socket_arr[arr_size - 1];
		poll_arr[index] = poll_arr[arr_size - 1];
		--arr_size;
		socket_arr = realloc(socket_arr, sizeof(struct socket) * arr_size);
		if (socket_arr == NULL)
		{
			exit_fatal(__FUNCTION__, "Memory allocation error for socket_arr");
		}
		poll_arr = realloc(poll_arr, sizeof(struct pollfd) * arr_size);
		if (poll_arr == NULL)
		{
			exit_fatal(__FUNCTION__, "Memory allocation error for poll_arr");
		}

		// Prepare and send message about disconnection
		sprintf(buf, "Client %d disconnected", client_id);
		send_buf();
	
		// Log for debugging
		printf("[mini_serv] Client disconnected: ID: %d, FD: %d\n", client_id, client_fd);
	}
	// Otherwise, the client sent a message that needs to be broadcast
	else
	{
		buf[prefix_length + return_value] = '\0';
		send_buf();

		// Log for debugging
		printf("[mini_serv] Broadcasted client message: ID: %d, FD: %d\n", client_id, client_fd);
	}
}

int main(int argc, char *argv[])
{
	// Check nbr of args
	if (argc < 2)
	{
		sprintf(buf, "Wrong number of args\n");
		print_buf(2);
		return 1;
	}

	// Prepare listening socket
	{
		// Create socket
		int listening_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (listening_fd == -1)
		{
			exit_fatal(__FUNCTION__, "Socket creation error");
		}

		// Bind socket to port
		struct sockaddr_in address;

		address.sin_family = AF_INET;
		address.sin_addr.s_addr = inet_addr("127.0.0.1");
		address.sin_port = htons(atoi(argv[1]));

		return_value = bind(listening_fd, (struct sockaddr *)&address, sizeof(address));
		if (return_value == -1)
		{
			exit_fatal(__FUNCTION__, "Bind error");
		}

		// Start listening
		return_value = listen(listening_fd, 10);
		if (return_value == -1)
		{
			exit_fatal(__FUNCTION__, "Listen error");
		}

		// Add listening socket fd to arrays
		socket_arr = malloc(sizeof(struct socket));
		if (socket_arr == NULL)
		{
			exit_fatal(__FUNCTION__, "Memory allocation error for socket_arr");
		}
		socket_arr[0].fd = listening_fd;
		socket_arr[0].id = -1;

		poll_arr = malloc(sizeof(struct pollfd));
		if (poll_arr == NULL)
		{
			exit_fatal(__FUNCTION__, "Memory allocation error for poll_arr");
		}
		poll_arr[0].fd = listening_fd;
		poll_arr[0].events = POLLIN;
		poll_arr[0].revents = 0;

		arr_size = 1;

		// Log for debugging
		printf("[mini_serv] Server listening on: %s\n", argv[1]);
	}


	// Server loop
	while (1)
	{
		// Wait until event occurs
		return_value = poll(poll_arr, arr_size, -1);
		if (return_value == -1)
		{
			exit_fatal(__FUNCTION__, "Poll error");
		}

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