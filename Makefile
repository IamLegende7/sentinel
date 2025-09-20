# Compiler and flags
CXX = g++
CXXFLAGS = -Iinclude -Wall -Wextra -std=c++17
LDFLAGS = -lSDL3 -lSDL3_image

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin
LIB_DIR = lib

# Source files and object files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

INCLUDES = $(shell find $(INC_DIR) -type d)
CXXFLAGS += $(addprefix -I,$(INCLUDES))

LIBS = $(shell find $(LIB_DIR) -type d)
CXXFLAGS += $(addprefix -I,$(LIBS))
LDFLAGS += $(addprefix -L,$(LIBS))

# Executable name
EXECUTABLE_NAME = sentinel
TARGET = $(BIN_DIR)/$(EXECUTABLE_NAME)

all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CXX) -o $@ $^ $(LDFLAGS)

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run new