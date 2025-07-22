# Mini Server

A simple TCP chat server implementation in C that allows multiple clients to connect and broadcast messages to all connected clients.

> ⚠️ **Warning:** This project is still under construction. Features may be incomplete or unstable.


## Overview

This project implements a minimal chat server using TCP sockets and the `poll()` system call for handling multiple clients concurrently. The server listens on a specified port and relays messages from any client to all other connected clients.

## Files

- `mini_serv.c` - Main server implementation
- `micro_serv.c` - Like mini_serv.c but without error checks (faster in exam)
- `tester.c` - Test client program that creates multiple connections and sends messages
- `Makefile` - Build configuration for compiling the server and tester
- `mini_serv.log` - Server log file (generated during runtime)
- `tester.log` - Tester log file (generated during testing)

## Building

Use the provided Makefile to build the project:

```bash
make all       # Build both mini_serv and tester
make mini_serv # Build only the server
make tester    # Build only the tester
make clean     # Remove compiled binaries
make re        # Clean and rebuild
```

## Usage

### Starting the Server

```bash
./mini_serv <port>
```

Example:
```bash
./mini_serv 8080
```

The server will:
1. Bind to localhost (127.0.0.1) on the specified port
2. Start listening for client connections
3. Log all operations to `mini_serv.log`

### Testing with the Tester

The included tester program can be used to simulate multiple clients:

```bash
./tester
```

The tester will:
1. Create 10 simultaneous client connections to port 8080
2. Send a message from each client
3. Close 9 connections, leaving one to receive broadcasts
4. Log all operations to `tester.log`

## Exercise Requirements

- **Single file implementation**: The entire server must be implemented in one `.c` file
- **No printf()**: Output must be done using `write()` system call only
- **No signal handling**: The program cannot handle signals (must be terminated with Ctrl+C)
- **Error handling**: System call errors that prevent server operation must result in fatal exit with "Fatal error\n" message
- **Memory management**: Must properly allocate and deallocate memory for dynamic arrays
- **Port binding**: Must bind to 127.0.0.1 (localhost) on the port specified as command line argument

## Logging

Both the server and tester create detailed log files:

- `mini_serv.log` - Server operations, client connections, and message broadcasts
- `tester.log` - Test client operations and received messages

## System Requirements

- Linux/Unix system with TCP socket support
- GCC compiler
- Standard C library
- POSIX-compliant system calls (socket, poll, etc.)

## License

This is an educational project and part of the 42 School curriculum.
