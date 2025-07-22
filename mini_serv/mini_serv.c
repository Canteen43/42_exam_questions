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
char send_buf[20000];
char msg_src[20000];
int ret = 0;
int arr_size = 0;
int next_client_id = 0;
struct pollfd* poll_arr = NULL;
struct socket* socket_arr = NULL;
FILE* logfile;


void exit_fatal()
{
	ret = write(2, "Fatal error\n", 12);
	for (int i = 0; i < arr_size; ++i)
	{
		if (socket_arr[i].fd != -1)
			close(socket_arr[i].fd);
	}
	exit(1);
}

void broadcast(int bytes)
{
	for (int i = 1; i < arr_size; ++i)
	{
		ret = send(socket_arr[i].fd, send_buf, bytes, MSG_NOSIGNAL);
	}

	// Log the broadcast message without its newline
	fprintf(logfile, "\tBroadcasted message: \"");
	if (send_buf[bytes - 1] == '\n')
		fprintf(logfile, "%.*s\\n\"\n", bytes - 1, send_buf);
	else
		fprintf(logfile, "%.*s\"\n", bytes, send_buf);
	fflush(logfile);
}

void accept_client()
{
	// Accept new client (address not used but required by function)
	struct sockaddr_in client_address;
	socklen_t addr_len = sizeof(client_address);
	ret = accept(socket_arr[0].fd, (struct sockaddr *)&client_address, &addr_len);
	if (ret == -1)
	{
		fprintf(logfile, "[Error] Context: accept() failed, Description: %s\n", strerror(errno));
		fflush(logfile);
		exit_fatal();
	}
	int client_fd = ret;

	// Set client id
	int client_id = next_client_id;
	++next_client_id;

	// Increase array size and add client to arrays
	++arr_size;

	socket_arr = realloc(socket_arr, sizeof(struct socket) * arr_size);
	if (socket_arr == NULL)
	{
		fprintf(logfile, "[Error] Context: realloc for socket_arr, Description: %s\n", strerror(errno));
		fflush(logfile);
		exit_fatal();
	}
	socket_arr[arr_size - 1].fd = client_fd;
	socket_arr[arr_size - 1].id = client_id;

	poll_arr = realloc(poll_arr, sizeof(struct pollfd) * arr_size);
	if (poll_arr == NULL)
	{
		fprintf(logfile, "[Error] Context: realloc for poll_arr, Description: %s\n", strerror(errno));
		fflush(logfile);
		exit_fatal();
	}
	poll_arr[arr_size - 1].fd = client_fd;
	poll_arr[arr_size - 1].events = POLLIN;
	poll_arr[arr_size - 1].revents = 0;

	// Log for debugging
	fprintf(logfile, "Accepted client connection: ID: %d, FD: %d\n", client_id, client_fd);
	fflush(logfile);

	// Prepare and send message
	ret = sprintf(send_buf, "New Client connected: %d\n", client_id);
	broadcast(ret);
}

void handle_client(int index)
{	
	// Get client fd and id
	int client_fd = socket_arr[index].fd;
	int client_id = socket_arr[index].id;
	
	// Read message from client
	ret = recv(client_fd, msg_src, sizeof(msg_src) - 1, 0);
	if (ret == -1)
	{
		fprintf(logfile, "[Error] Context: recv() failed, Description: %s\n", strerror(errno));
		fflush(logfile);
		exit_fatal();
	}
	
	// If read returns 0, client disconnected and needs to be removed
	else if (ret == 0)
	{
		// Close the socket fd
		ret = close(client_fd);
		if (ret == -1)
		{
			fprintf(logfile, "[Error] Context: close() failed, Description: %s\n", strerror(errno));
			fflush(logfile);
		}

		// Remove client from arrays by swapping with the last element
		socket_arr[index] = socket_arr[arr_size - 1];
		poll_arr[index] = poll_arr[arr_size - 1];
		--arr_size;
		socket_arr = realloc(socket_arr, sizeof(struct socket) * arr_size);
		if (socket_arr == NULL)
		{
			fprintf(logfile, "[Fatal Error] Context: realloc for socket_arr, Description: %s\n", strerror(errno));
			fflush(logfile);
			exit_fatal();
		}
		poll_arr = realloc(poll_arr, sizeof(struct pollfd) * arr_size);
		if (poll_arr == NULL)
		{
			fprintf(logfile, "[Fatal Error] Context: realloc for poll_arr, Description: %s\n", strerror(errno));
			fflush(logfile);
			exit_fatal();
		}

		// Log for debugging
		fprintf(logfile, "Client disconnected: ID: %d, FD: %d\n", client_id, client_fd);
		fflush(logfile);

		// Prepare and send message about disconnection
		ret = sprintf(send_buf, "Client %d disconnected\n", client_id);
		broadcast(ret);
	}
	// Otherwise, the client sent a message that needs to be broadcast
	else
	{
		// Saving number of bytes read
		int bytes_read = ret;

		// Adding prefix to the message
		int prefix_length = sprintf(send_buf, "Client %d says: ", client_id);
		char *msg_dst = send_buf + prefix_length;

		// Log for debugging
		fprintf(logfile, "Received message from client. ID: %d, FD: %d, Message: \"%.*s\"\n",
			client_id, client_fd, bytes_read, msg_src);
		fflush(logfile);

		for (int i = 0, j = 0; i < bytes_read; ++i, ++j)
		{
			msg_dst[j] = msg_src[i];
			if (msg_src[i] == '\n' || i == bytes_read - 1)
			{
				broadcast(prefix_length  + j + 1);
				j = -1;
			}
		}
		
		// Log for debugging
		fprintf(logfile, "Received message from client. ID: %d, FD: %d, Message: \"%.*s\"\n",
			client_id, client_fd, ret, send_buf + prefix_length);
		fflush(logfile);
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

	// Prepare log file
	remove("mini_serv.log");
	logfile = fopen("mini_serv.log", "a");

	// Prepare listening socket
	{
		// Create socket
		int listening_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (listening_fd == -1)
		{
			fprintf(logfile, "[Fatal Error] Context: socket() failed, Description: %s\n", strerror(errno));
			fflush(logfile);
			exit_fatal();
		}

		// Bind socket to port
		struct sockaddr_in address;

		address.sin_family = AF_INET;
		address.sin_addr.s_addr = inet_addr("127.0.0.1");
		address.sin_port = htons(atoi(argv[1]));

		ret = bind(listening_fd, (struct sockaddr *)&address, sizeof(address));
		if (ret == -1)
		{
			fprintf(logfile, "[Fatal Error] Context: bind() failed, Description: %s\n", strerror(errno));
			fflush(logfile);
			exit_fatal();
		}

		// Start listening
		ret = listen(listening_fd, 10);
		if (ret == -1)
		{
			fprintf(logfile, "[Fatal Error] Context: listen() failed, Description: %s\n", strerror(errno));
			fflush(logfile);
			exit_fatal();
		}

		// Add listening socket fd to arrays
		socket_arr = malloc(sizeof(struct socket));
		if (socket_arr == NULL)
		{
			fprintf(logfile, "[Fatal Error] Context: malloc for socket_arr, Description: %s\n", strerror(errno));
			fflush(logfile);
			exit_fatal();
		}
		socket_arr[0].fd = listening_fd;
		socket_arr[0].id = -1;

		poll_arr = malloc(sizeof(struct pollfd));
		if (poll_arr == NULL)
		{
			fprintf(logfile, "[Fatal Error] Context: malloc for poll_arr, Description: %s\n", strerror(errno));
			fflush(logfile);
			exit_fatal();
		}
		poll_arr[0].fd = listening_fd;
		poll_arr[0].events = POLLIN;
		poll_arr[0].revents = 0;

		arr_size = 1;

		// Log for debugging
		fprintf(logfile, "Setup finished. Server listening on port %s\n", argv[1]);
		fflush(logfile);
	}


	// Server loop
	while (1)
	{
		// Wait until event occurs
		ret = poll(poll_arr, arr_size, -1);
		if (ret == -1)
		{
			fprintf(logfile, "[Fatal Error] Context: poll() failed, Description: %s\n", strerror(errno));
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