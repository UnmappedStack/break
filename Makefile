# Compiler and flags
CC = clang
CFLAGS = -Iinclude -Wall -g

# Directories
SRC_DIR = src
OBJ_DIR = obj

# Output executable
TARGET = break

# Find all .c files in the src directory and corresponding .o files in the obj directory
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Default target
all: $(TARGET)

# Rule to create the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to create object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(OBJ_DIR) break

run: all
	 ./$(TARGET)

.PHONY: all clean
