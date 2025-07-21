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
int return_value = 0;
int arr_size = 0;
int next_client_id = 0;
struct pollfd* poll_arr = NULL;
struct socket* socket_arr = NULL;
FILE* logfile;


void exit_fatal()
{
	return_value = write(2, "Fatal error\n", 12);
	for (int i = 0; i < arr_size; ++i)
	{
		if (socket_arr[i].fd != -1)
			close(socket_arr[i].fd);
	}
	exit(1);
}

void send_buf()
{
	for (int i = 1; i < arr_size; ++i)
	{
		return_value = send(socket_arr[i].fd, buf, strlen(buf), MSG_NOSIGNAL);
		// if (return_value == -1)
		// 	fprintf(logfile, "send() failed with errno: %d, %s\n", errno, strerror(errno));
		// 	fflush(logfile);
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
		fprintf(logfile, "Accept error\n");
		fflush(logfile);
		exit_fatal();
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
		fprintf(logfile, "Memory allocation error for socket_arr\n");
		fflush(logfile);
		exit_fatal();
	}
	socket_arr[arr_size - 1].fd = client_fd;
	socket_arr[arr_size - 1].id = client_id;

	poll_arr = realloc(poll_arr, sizeof(struct pollfd) * arr_size);
	if (poll_arr == NULL)
	{
		fprintf(logfile, "Memory allocation error for poll_arr\n");
		fflush(logfile);
		exit_fatal();
	}
	poll_arr[arr_size - 1].fd = client_fd;
	poll_arr[arr_size - 1].events = POLLIN;
	poll_arr[arr_size - 1].revents = 0;

	
	// Prepare and send message
	sprintf(buf, "New Client connected: %d", client_id);
	send_buf();
	
	// Log for debugging
	fprintf(logfile, "Accepted client connection: ID: %d, FD: %d\n", client_id, client_fd);
	fflush(logfile);
}

void handle_client(int index)
{	
	// Get client fd and id
	int client_fd = socket_arr[index].fd;
	int client_id = socket_arr[index].id;
	
	// Before reading, add message prefix to buffer
	int prefix_length = sprintf(buf, "Client %d says: ", client_id);
	
	// Read message from client
	return_value = read(client_fd, buf + prefix_length, sizeof(buf) - prefix_length - 1);
	if (return_value == -1)
	{
		fprintf(logfile, "Read error for client %d\n", client_id);
		fflush(logfile);
		exit_fatal();
	}
	
	// If read returns 0, client disconnected and needs to be removed
	if (return_value == 0)
	{
		// Close the socket fd
		return_value = close(client_fd);
		if (return_value == -1)
		{
			fprintf(logfile, "Close error for client %d\n", client_id);
			fflush(logfile);
		}

		// Remove client from arrays by swapping with the last element
		socket_arr[index] = socket_arr[arr_size - 1];
		poll_arr[index] = poll_arr[arr_size - 1];
		--arr_size;
		socket_arr = realloc(socket_arr, sizeof(struct socket) * arr_size);
		if (socket_arr == NULL)
		{
			fprintf(logfile, "Memory allocation error for socket_arr\n");
			fflush(logfile);
			exit_fatal();
		}
		poll_arr = realloc(poll_arr, sizeof(struct pollfd) * arr_size);
		if (poll_arr == NULL)
		{
			fprintf(logfile, "Memory allocation error for poll_arr\n");
			fflush(logfile);
			exit_fatal();
		}

		// Prepare and send message about disconnection
		sprintf(buf, "Client %d disconnected", client_id);
		send_buf();
	
		// Log for debugging
		fprintf(logfile, "Client disconnected: ID: %d, FD: %d\n", client_id, client_fd);
		fflush(logfile);
	}
	// Otherwise, the client sent a message that needs to be broadcast
	else
	{
		// Log for debugging
		fprintf(logfile, "Received message from client. ID: %d, FD: %d, Message: \"%s\"\n", 
			client_id, client_fd, buf + prefix_length);
		fflush(logfile);
		
		buf[prefix_length + return_value] = '\0';
		send_buf();

		// Log for debugging
		fprintf(logfile, "Broadcasted client message: ID: %d, FD: %d, Message: \"%s\"\n", client_id, client_fd, buf);
		fflush(logfile);
	}
}

int main(int argc, char *argv[])
{
	// Check nbr of args
	if (argc < 2)
	{
		return_value = write(2, "Wrong number of args\n", 22);
		return 1;
	}

	// Prepare log file
	remove("mini_serv.log");
	logfile = fopen("mini_serv.log", "a");

	// Prepare listening socket
	{
		// Create socket
		int listening_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (listening_fd == -1)
		{
			fprintf(logfile, "Socket creation error\n");
			fflush(logfile);
			exit_fatal();
		}

		// Bind socket to port
		struct sockaddr_in address;

		address.sin_family = AF_INET;
		address.sin_addr.s_addr = inet_addr("127.0.0.1");
		address.sin_port = htons(atoi(argv[1]));

		return_value = bind(listening_fd, (struct sockaddr *)&address, sizeof(address));
		if (return_value == -1)
		{
			fprintf(logfile, "Bind error\n");
			fflush(logfile);
			exit_fatal();
		}

		// Start listening
		return_value = listen(listening_fd, 10);
		if (return_value == -1)
		{
			fprintf(logfile, "Listen error\n");
			fflush(logfile);
			exit_fatal();
		}

		// Add listening socket fd to arrays
		socket_arr = malloc(sizeof(struct socket));
		if (socket_arr == NULL)
		{
			fprintf(logfile, "Memory allocation error for socket_arr\n");
			fflush(logfile);
			exit_fatal();
		}
		socket_arr[0].fd = listening_fd;
		socket_arr[0].id = -1;

		poll_arr = malloc(sizeof(struct pollfd));
		if (poll_arr == NULL)
		{
			fprintf(logfile, "Memory allocation error for poll_arr\n");
			fflush(logfile);
			exit_fatal();
		}
		poll_arr[0].fd = listening_fd;
		poll_arr[0].events = POLLIN;
		poll_arr[0].revents = 0;

		arr_size = 1;

		// Log for debugging
		fprintf(logfile, "Server listening on: %s\n", argv[1]);
		fflush(logfile);
	}


	// Server loop
	while (1)
	{
		// Wait until event occurs
		return_value = poll(poll_arr, arr_size, -1);
		if (return_value == -1)
		{
			fprintf(logfile, "Poll error\n");
			fflush(logfile);
			exit_fatal();
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