#include <unistd.h>		// for write
#include <string.h>		// for strlen
#include <stdio.h>		// for sprintf
#include <sys/types.h>	// sometimes needed for legacy reasons
#include <sys/socket.h>	// defines socket(), AF_INET, SOCK_STREAM, etc.
#include <arpa/inet.h>	// for inet_addr(), htons()
#include <poll.h>		// for poll()

// System call errors are intentionally ignored

// Client struct
struct client
{
	int	fd;
	int	id;
};

// Shared variables are global for simplicity
char buf[20000];
int server_fd;
int nbr_clients = 0;
int next_client_id = 0;
struct pollfd* poll_fds = NULL;
struct client* clients = NULL;

void print_buf(int fd)
{
	write(fd, buf, strlen(buf));
}

void send_buf()
{
	for (int i = 0; i < nbr_clients; ++i)
	{
		write(clients[i].fd, buf, strlen(buf));
	}
}

void add_to_poll(int fd)
{
	poll_fds = realloc(poll_fds, sizeof(struct pollfd) * (nbr_clients + 1));
	poll_fds[nbr_clients].fd = fd;
	poll_fds[nbr_clients].events = POLLIN;
	poll_fds[nbr_clients].revents = 0;
}

void remove_from_poll(int fd)
{
	// Find the index of the fd to remove
	int index;
	for (int i = 0; i < nbr_clients; ++i)
	{
		if (poll_fds[i].fd == fd)
		{
			index = i;
			break;
		}
	}

	// Swap the element to remove with the last element
	poll_fds[index] = poll_fds[nbr_clients];

	// Delete the last element
	poll_fds = realloc(poll_fds, sizeof(struct pollfd) * (nbr_clients));
}

int fd_to_id(int fd)
{
	for (int i = 0; i < nbr_clients; ++i)
	{
		if (clients[i].fd == fd)
		{
			return clients[i].id;
		}
	}
	return -1;
}

void accept_client()
{
	// Accept new client (address not used but required by function)
	struct sockaddr_in client_address;
	socklen_t addr_len = sizeof(client_address);
	int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &addr_len);

	// Set client id
	int client_id = next_client_id;
	++next_client_id;

	// Increase client count and array size
	nbr_clients++;
	clients = realloc(clients, sizeof(struct client) * nbr_clients);

	// Add new client to array
	clients[nbr_clients - 1].fd = client_fd;
	clients[nbr_clients - 1].id = client_id;

	// Add new client to poll
	add_to_poll(client_fd);

	// Prepare and send message
	sprintf(buf, "New Client connected: %d\n", client_id);
	send_buf();
}

void remove_client(int client_id)
{
	// Find client index and fd
	int client_index;
	int client_fd;
	for (int i = 0; i < nbr_clients; ++i)
	{
		if (clients[i].id == client_id)
		{
			client_index = i;
			client_fd = clients[i].fd;
			break;
		}
	}

	// Close the client's socket
	close(clients[client_index].fd);

	// Remove client from poll
	remove_from_poll(client_fd);

	// Remove client from array
	clients[client_index] = clients[nbr_clients - 1];
	clients = realloc(clients, sizeof(struct client) * (nbr_clients - 1));
	nbr_clients--;

	// Send message
	sprintf(buf, "Client %d disconnected\n", client_id);
	send_buf();
}

void handle_client(int client_fd)
{	
	// Find client id from fd
	int client_id = fd_to_id(client_fd);
	
	// Before reading add message prefix to buffer
	sprintf(buf, "Client %d says: ", client_id);
	
	// Read message from client
	int bytes_read = read(client_fd, buf + strlen(buf), sizeof(buf) - strlen(buf));
	
	// If read returns 0, client disconnected
	if (bytes_read == 0)
		remove_client(client_id);
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
		server_fd = socket(AF_INET, SOCK_STREAM, 0);

		// Bind socket to port
		struct sockaddr_in address;

		address.sin_family = AF_INET;
		address.sin_addr.s_addr = inet_addr("127.0.0.1");
		address.sin_port = htons(atoi(argv[1]));

		bind(server_fd, (struct sockaddr *)&address, sizeof(address));

		// Start listening
		listen(server_fd, 10);

		// Add server_fd to poll
		add_to_poll(server_fd);
	}

	// Server loop
	while (1)
	{
		// Wait until event occurs
		poll(poll_fds, nbr_clients + 1, -1);

		// Check for and accept new client
		if (poll_fds[0].revents & POLLIN)
		{
			accept_client();
			continue;
		}

		// Check for and handle client messages
		for (int i = 1; i <= nbr_clients; ++i)
		{
			if (poll_fds[i].revents & POLLIN)
			{
				handle_client(poll_fds[i].fd);
				continue;
			}
		}
	}
	return 0;
}