# 🔨 Build and Setup Guide

## 📋 **Prerequisites**

### **Required Software**
- **C++ Compiler**: GCC 7.0+, Clang 6.0+, or MSVC 2017+
- **Build System**: Make, CMake, or Visual Studio
- **Raylib**: Version 5.5.1 or newer
- **Git**: For version control and dependency management

### **Platform-Specific Requirements**

#### **Windows**
- MinGW-w64 or Visual Studio 2017+
- Windows 10 or newer recommended

#### **Linux**
- GCC or Clang compiler
- Development packages: `build-essential`, `cmake`
- X11 development libraries

#### **macOS**
- Xcode Command Line Tools
- Homebrew (recommended for package management)

---

## 🚀 **Quick Start**

### **Option 1: Using the Provided Build Scripts**

```bash
# Windows
build.bat

# Linux/macOS
chmod +x build.sh
./build.sh
```

### **Option 2: Manual Build**

```bash
# Clone the repository
git clone <your-repo-url>
cd verlet-physics-engine

# Build with the simple Makefile
make -f Makefile.simple

# Or build manually
g++ -std=c++14 -Wall -O2 main.cpp particle.cpp stick.cpp motor.cpp -o verlz -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

---

## 📦 **Raylib Installation**

### **Windows Installation**

#### **Method 1: Download Pre-built**
1. Go to [Raylib Releases](https://github.com/raysan5/raylib/releases)
2. Download `raylib-X.X.X_win64_mingw-w64.zip`
3. Extract to `C:\raylib`
4. Update your build script paths

#### **Method 2: Using vcpkg**
```cmd
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg install raylib
```

#### **Method 3: Using MSYS2**
```bash
pacman -S mingw-w64-x86_64-raylib
```

### **Linux Installation**

#### **Ubuntu/Debian**
```bash
sudo apt update
sudo apt install libraylib-dev
```

#### **Fedora**
```bash
sudo dnf install raylib-devel
```

#### **Arch Linux**
```bash
sudo pacman -S raylib
```

#### **Build from Source**
```bash
git clone https://github.com/raysan5/raylib.git
cd raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo make install
```

### **macOS Installation**

#### **Using Homebrew**
```bash
brew install raylib
```

#### **Using MacPorts**
```bash
sudo port install raylib
```

---

## 🛠️ **Build Configuration**

### **Debug Build**
```bash
# Add debug flags
g++ -std=c++14 -Wall -g -O0 -DDEBUG main.cpp particle.cpp stick.cpp motor.cpp -o verlz_debug -lraylib [system_libs]
```

### **Release Build**
```bash
# Optimized for performance
g++ -std=c++14 -Wall -O3 -DNDEBUG main.cpp particle.cpp stick.cpp motor.cpp -o verlz -lraylib [system_libs]
```

### **Profile Build**
```bash
# For performance profiling
g++ -std=c++14 -Wall -O2 -pg main.cpp particle.cpp stick.cpp motor.cpp -o verlz_profile -lraylib [system_libs]
```

---

## 🔧 **CMake Setup (Advanced)**

### **Create CMakeLists.txt**
```cmake
cmake_minimum_required(VERSION 3.10)
project(VerletPhysicsEngine)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(raylib REQUIRED)

# Add executable
add_executable(verlz
    main.cpp
    particle.cpp
    stick.cpp
    motor.cpp
)

# Link libraries
target_link_libraries(verlz raylib)

# Compiler-specific options
if(MSVC)
    target_compile_options(verlz PRIVATE /W4)
else()
    target_compile_options(verlz PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Debug/Release configurations
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(verlz PRIVATE DEBUG)
    if(NOT MSVC)
        target_compile_options(verlz PRIVATE -g -O0)
    endif()
else()
    target_compile_definitions(verlz PRIVATE NDEBUG)
    if(NOT MSVC)
        target_compile_options(verlz PRIVATE -O3)
    endif()
endif()
```

### **Build with CMake**
```bash
# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build .

# Or for specific configuration
cmake --build . --config Release
```

---

## 🧪 **Development Environment Setup**

### **Visual Studio Code**
Create `.vscode/tasks.json`:
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build",
            "type": "shell",
            "command": "g++",
            "args": [
                "-std=c++14",
                "-Wall",
                "-g",
                "main.cpp",
                "particle.cpp", 
                "stick.cpp",
                "motor.cpp",
                "-o",
                "verlz",
                "-lraylib",
                "-lGL",
                "-lm",
                "-lpthread",
                "-ldl",
                "-lrt",
                "-lX11"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            }
        }
    ]
}
```

Create `.vscode/launch.json`:
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/verlz",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "preLaunchTask": "build"
        }
    ]
}
```

### **CLion/Qt Creator**
- Import as CMake project
- Set build directory to `build/`
- Configure run configuration to execute from project root

---

## 🔍 **Troubleshooting**

### **Common Build Errors**

#### **"raylib.h not found"**
```bash
# Solution 1: Install raylib system-wide
sudo apt install libraylib-dev  # Linux
brew install raylib             # macOS

# Solution 2: Specify include path
g++ -I/path/to/raylib/include ...

# Solution 3: Use pkg-config
g++ $(pkg-config --cflags raylib) ... $(pkg-config --libs raylib)
```

#### **Linking Errors**
```bash
# Linux: Add missing libraries
-lGL -lm -lpthread -ldl -lrt -lX11

# Windows: Use Windows libraries
-lopengl32 -lgdi32 -lwinmm

# macOS: Use macOS frameworks
-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
```

#### **"undefined reference to" errors**
- Check that all source files are included in build
- Verify library linking order
- Ensure all dependencies are installed

### **Runtime Issues**

#### **"Failed to initialize graphics device"**
- Check graphics drivers are installed
- Try running with different graphics settings
- Verify display is available (not SSH without X forwarding)

#### **Poor Performance**
- Build in Release mode (`-O3`)
- Check for debug prints in tight loops
- Profile with `gprof` or similar tools

---

## 📊 **Performance Optimization**

### **Compiler Optimizations**
```bash
# Basic optimizations
-O2 -march=native

# Aggressive optimizations
-O3 -march=native -flto -ffast-math

# Profile-guided optimization
# 1. Build with profiling
g++ -O2 -fprofile-generate ...
# 2. Run typical workload
./verlz
# 3. Rebuild with profile data
g++ -O2 -fprofile-use ...
```

### **Link-Time Optimization**
```bash
# Enable LTO for better optimization across translation units
g++ -O3 -flto main.cpp particle.cpp stick.cpp motor.cpp -o verlz -lraylib
```

---

## 🧪 **Testing Setup**

### **Unit Testing with Catch2**
```bash
# Install Catch2
git clone https://github.com/catchorg/Catch2.git
cd Catch2
cmake -B build -S .
cmake --build build
sudo cmake --install build

# Create test file
# tests/test_particle.cpp
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../particle.h"

TEST_CASE("Particle creation", "[particle]") {
    Particle p(100, 200, 5.0f, 1.0f);
    REQUIRE(p.get_x() == 100);
    REQUIRE(p.get_y() == 200);
    REQUIRE(p.get_radius() == 5.0f);
    REQUIRE(p.get_mass() == 1.0f);
}

# Build tests
g++ -std=c++14 tests/test_particle.cpp particle.cpp -o test_runner -lcatch2
```

### **Integration Testing**
```bash
# Create integration test script
#!/bin/bash
echo "Running integration tests..."

# Test 1: Basic startup
timeout 5s ./verlz --test-mode
if [ $? -eq 0 ]; then
    echo "✓ Basic startup test passed"
else
    echo "✗ Basic startup test failed"
fi

# Test 2: Performance test
echo "Testing performance with 100 particles..."
timeout 10s ./verlz --benchmark --particles=100
```

---

## 📦 **Distribution**

### **Creating Release Package**
```bash
#!/bin/bash
# build_release.sh

# Clean previous builds
rm -rf build release

# Create release build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

# Create distribution package
cd ..
mkdir release
cp build/verlz release/
cp README.md release/
cp LICENSE release/
cp -r assets release/ # If you have assets

# Create archive
tar -czf verlz-v1.0.0-linux.tar.gz release/
```

### **Windows Installer (NSIS)**
```nsis
; verlz_installer.nsi
!define APP_NAME "Verlet Physics Engine"
!define APP_VERSION "1.0.0"

Name "${APP_NAME}"
OutFile "verlz_installer.exe"
InstallDir "$PROGRAMFILES\${APP_NAME}"

Section "Main"
    SetOutPath $INSTDIR
    File "verlz.exe"
    File "README.md"
    
    CreateDirectory "$SMPROGRAMS\${APP_NAME}"
    CreateShortCut "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" "$INSTDIR\verlz.exe"
SectionEnd
```

This comprehensive build guide should help you set up the development environment and build the project on any platform.