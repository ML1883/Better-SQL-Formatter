# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

# Files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)
TARGET = main

# Rules
all: $(TARGET)

# Link the final executable
$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files to object files
# Note: All headers are included as prerequisites
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Run tests
test: $(TARGET)
	@./$(TARGET) < build/input.sql

# Run the program
run: $(TARGET)
	@echo "Running the program..."
	@./$(TARGET)

# Phony targets
.PHONY: all clean test run