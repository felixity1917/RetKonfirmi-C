# Makefile for RetKonfirmi-C project

# Compiler and flags
CC := clang
CFLAGS := -Wall -Wextra -std=c99 -O2 -g
LDFLAGS := -lcurl

# Directories
SRC_DIR := src
BIN_DIR := bin
INCLUDE_DIR := include
LOG_DIR := logs

# Project details
TARGET := retkonfirmi-c
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:.c=.o)

# Default target
all: $(BIN_DIR)/$(TARGET)

# Compile and link
$(BIN_DIR)/$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< -I$(INCLUDE_DIR)

# Create bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean up object files and binaries
clean:
	rm -rf $(BIN_DIR) $(SRC_DIR)/*.o

# Install target (optional, to install in /usr/local/bin)
install: $(BIN_DIR)/$(TARGET)
	install -m 755 $(BIN_DIR)/$(TARGET) /usr/local/bin/

# Uninstall target (optional)
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Help message
help:
	@echo "Makefile for RetKonfirmi-C project"
	@echo "Usage:"
	@echo "  make          - Compile the project"
	@echo "  make clean    - Clean object files and binaries"
	@echo "  make install  - Install the binary to /usr/local/bin"
	@echo "  make uninstall- Uninstall the binary from /usr/local/bin"
	@echo "  make help     - Show this help message"

.PHONY: all clean install uninstall help
