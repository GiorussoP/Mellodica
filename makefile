
OPTIMIZATION_LEVEL = 1

# Compiler and flags
CXX = g++
CC = gcc

CXXFLAGS = -Wall -Wextra -O$(OPTIMIZATION_LEVEL) -std=c++17
CFLAGS = -Wall -Wextra -O$(OPTIMIZATION_LEVEL)

LDFLAGS = -lSDL2 -lSDL2_image -lGLEW -lGL -lfluidsynth

# Directories
EXEC_NAME = a.out
INCLUDE_DIR = ./include
SOURCE_DIR = ./src
OBJ_DIR = ./obj

# Automatically find all .cpp and .c files in src/ and subdirectories
CPP_SOURCES = $(shell find $(SOURCE_DIR) -name '*.cpp')
C_SOURCES = $(shell find $(SOURCE_DIR) -name '*.c')

# Generate object file names from source files
CPP_OBJECTS = $(CPP_SOURCES:$(SOURCE_DIR)/%.cpp=$(OBJ_DIR)/%.o)
C_OBJECTS = $(C_SOURCES:$(SOURCE_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJECTS = $(CPP_OBJECTS) $(C_OBJECTS)

# Automatically find all include directories
INCLUDE_FLAGS = $(shell find $(INCLUDE_DIR) -type d -exec echo -I{} \;)

# Default target
all: build

# Build executable
build: $(EXEC_NAME)

$(EXEC_NAME): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile C++ source files to object files
$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

# Compile C source files to object files
$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

# Run the program
run: build
	./$(EXEC_NAME)

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(EXEC_NAME)

# Rebuild everything
rebuild: clean build

# Print variables for debugging
debug:
	@echo "CPP Sources: $(CPP_SOURCES)"
	@echo "C Sources: $(C_SOURCES)"
	@echo "Objects: $(OBJECTS)"
	@echo "Include flags: $(INCLUDE_FLAGS)"

.PHONY: all build run clean rebuild debug