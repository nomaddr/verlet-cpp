#!/bin/bash
# Simple build script for the Verlet Integration project

echo "=== Verlet Integration Build Script ==="
echo ""

# Check if compiler exists
if ! command -v g++ &> /dev/null; then
    echo "Error: g++ compiler not found!"
    echo "Please install a C++ compiler (g++, clang++, etc.)"
    exit 1
fi

echo "Found compiler: $(g++ --version | head -n1)"
echo ""

# Function to try different raylib configurations
try_build() {
    local config_name="$1"
    local cxx_flags="$2"
    local include_flags="$3"
    local lib_flags="$4"
    
    echo "Trying configuration: $config_name"
    echo "Command: g++ $cxx_flags $include_flags main.cpp particle.cpp stick.cpp motor.cpp -o verlz $lib_flags"
    
    if g++ $cxx_flags $include_flags main.cpp particle.cpp stick.cpp motor.cpp -o verlz $lib_flags 2>/dev/null; then
        echo "✓ SUCCESS: Build completed with $config_name"
        echo "Executable created: verlz"
        return 0
    else
        echo "✗ FAILED: $config_name didn't work"
        return 1
    fi
}

# Test syntax compilation first
echo "=== Testing syntax compilation (no linking) ==="
if g++ -std=c++14 -Wall -c main.cpp particle.cpp stick.cpp motor.cpp 2>&1; then
    echo "✓ Syntax compilation successful"
    echo ""
else
    echo "✗ Syntax errors found. Please fix these first."
    exit 1
fi

echo "=== Attempting different raylib configurations ==="
echo ""

# Configuration 1: System-wide raylib (Linux/Mac)
if try_build "System raylib (Linux/Mac)" \
    "-std=c++14 -Wall -O2" \
    "" \
    "-lraylib -lGL -lm -lpthread -ldl -lrt -lX11"; then
    exit 0
fi

echo ""

# Configuration 2: pkg-config raylib
if command -v pkg-config &> /dev/null && pkg-config --exists raylib; then
    if try_build "pkg-config raylib" \
        "-std=c++14 -Wall -O2" \
        "$(pkg-config --cflags raylib)" \
        "$(pkg-config --libs raylib)"; then
        exit 0
    fi
fi

echo ""

# Configuration 3: Windows raylib (common paths)
for raylib_path in "C:/raylib" "C:/raylib/raylib" "/c/raylib" "raylib"; do
    if [ -d "$raylib_path" ]; then
        if try_build "Windows raylib ($raylib_path)" \
            "-std=c++14 -Wall -O2" \
            "-I$raylib_path/include" \
            "-L$raylib_path/lib -lraylib -lopengl32 -lgdi32 -lwinmm"; then
            exit 0
        fi
    fi
done

echo ""

# Configuration 4: Local raylib build
if [ -d "raylib" ]; then
    if try_build "Local raylib directory" \
        "-std=c++14 -Wall -O2" \
        "-Iraylib/include" \
        "-Lraylib/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11"; then
        exit 0
    fi
fi

echo ""
echo "=== All configurations failed ==="
echo ""
echo "Possible solutions:"
echo "1. Install raylib system-wide:"
echo "   - Ubuntu/Debian: sudo apt install libraylib-dev"
echo "   - Fedora: sudo dnf install raylib-devel"
echo "   - Arch: sudo pacman -S raylib"
echo "   - macOS: brew install raylib"
echo ""
echo "2. Download raylib manually:"
echo "   - Go to: https://github.com/raysan5/raylib/releases"
echo "   - Download and extract to C:/raylib (Windows) or ./raylib (Linux/Mac)"
echo ""
echo "3. Use the original Makefile if you have the exact raylib setup it expects"
echo ""
echo "4. Check the last compilation error by running:"
echo "   g++ -std=c++14 -Wall main.cpp particle.cpp stick.cpp motor.cpp -o verlz -lraylib"