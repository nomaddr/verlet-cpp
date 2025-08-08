# Simple Makefile for Verlet Integration Project
# This is a simplified version that should work with most systems

# Project settings
PROJECT_NAME = verlz
CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -Wshadow -O2

# Source files
SOURCES = main.cpp particle.cpp stick.cpp motor.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Raylib settings - adjust these paths for your system
# Option 1: If raylib is installed system-wide (Linux/Mac with package manager)
RAYLIB_LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Option 2: If using raylib from a local installation (Windows/custom install)
# Uncomment and adjust these lines if raylib is in a specific directory:
# RAYLIB_PATH = C:/raylib
# INCLUDE_PATHS = -I$(RAYLIB_PATH)/include
# LIBRARY_PATHS = -L$(RAYLIB_PATH)/lib
# RAYLIB_LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

# Option 3: If using pkg-config (recommended if available)
# Uncomment this line if raylib is installed with pkg-config support:
# RAYLIB_LIBS = $(shell pkg-config --libs raylib)
# INCLUDE_PATHS = $(shell pkg-config --cflags raylib)

# Default target
all: $(PROJECT_NAME)

# Build the executable
$(PROJECT_NAME): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(PROJECT_NAME) $(LIBRARY_PATHS) $(RAYLIB_LIBS)

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJECTS) $(PROJECT_NAME) $(PROJECT_NAME).exe

# Install raylib (Linux/Mac only)
install-raylib:
	@echo "Installing raylib..."
	@echo "On Ubuntu/Debian: sudo apt install libraylib-dev"
	@echo "On Fedora: sudo dnf install raylib-devel"
	@echo "On Arch: sudo pacman -S raylib"
	@echo "On macOS: brew install raylib"

# Test compilation without linking (useful for checking syntax)
test-compile:
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $(SOURCES)

# Show current configuration
show-config:
	@echo "Project: $(PROJECT_NAME)"
	@echo "Compiler: $(CXX)"
	@echo "Flags: $(CXXFLAGS)"
	@echo "Sources: $(SOURCES)"
	@echo "Include paths: $(INCLUDE_PATHS)"
	@echo "Library paths: $(LIBRARY_PATHS)"
	@echo "Libraries: $(RAYLIB_LIBS)"

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build the project (default)"
	@echo "  clean        - Remove build files"
	@echo "  test-compile - Test compilation without linking"
	@echo "  show-config  - Show current build configuration"
	@echo "  install-raylib - Show raylib installation commands"
	@echo "  help         - Show this help"
	@echo ""
	@echo "To customize for your system:"
	@echo "1. Edit the RAYLIB_LIBS, INCLUDE_PATHS, and LIBRARY_PATHS variables"
	@echo "2. Choose the appropriate option (1, 2, or 3) based on your raylib installation"

.PHONY: all clean install-raylib test-compile show-config help