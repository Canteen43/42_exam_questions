# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Find all C files in the current directory
SRCS = $(wildcard *.c)
# Strip the .c extension to get the binary names
BINS = $(SRCS:.c=)

# Default target: build all binaries
all: $(BINS)

# Rule to build each binary from its .c file
%: %.c
	$(CC) $(CFLAGS) -o $@ $<

# Clean up all binaries
clean:
	rm -f $(BINS)

# Clean and rebuild everything
re: clean all
