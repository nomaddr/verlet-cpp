@echo off
REM Simple build script for the Verlet Integration project (Windows)

echo === Verlet Integration Build Script (Windows) ===
echo.

REM Check if compiler exists
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: g++ compiler not found!
    echo Please install MinGW-w64 or similar C++ compiler
    echo You can get it from: https://www.mingw-w64.org/
    pause
    exit /b 1
)

echo Found compiler: 
g++ --version | findstr "g++"
echo.

echo === Testing syntax compilation (no linking) ===
g++ -std=c++14 -Wall -c main.cpp particle.cpp stick.cpp motor.cpp
if %errorlevel% neq 0 (
    echo X Syntax errors found. Please fix these first.
    pause
    exit /b 1
)
echo + Syntax compilation successful
echo.

echo === Attempting different raylib configurations ===
echo.

REM Configuration 1: Raylib in C:\raylib
echo Trying configuration: C:\raylib
if exist "C:\raylib\include\raylib.h" (
    g++ -std=c++14 -Wall -O2 -IC:\raylib\include main.cpp particle.cpp stick.cpp motor.cpp -o verlz.exe -LC:\raylib\lib -lraylib -lopengl32 -lgdi32 -lwinmm
    if %errorlevel% equ 0 (
        echo + SUCCESS: Build completed with C:\raylib
        echo Executable created: verlz.exe
        goto success
    )
)
echo X FAILED: C:\raylib configuration didn't work
echo.

REM Configuration 2: Raylib in current directory
echo Trying configuration: Local raylib directory
if exist "raylib\include\raylib.h" (
    g++ -std=c++14 -Wall -O2 -Iraylib\include main.cpp particle.cpp stick.cpp motor.cpp -o verlz.exe -Lraylib\lib -lraylib -lopengl32 -lgdi32 -lwinmm
    if %errorlevel% equ 0 (
        echo + SUCCESS: Build completed with local raylib
        echo Executable created: verlz.exe
        goto success
    )
)
echo X FAILED: Local raylib configuration didn't work
echo.

REM Configuration 3: w64devkit path (from original Makefile)
echo Trying configuration: w64devkit raylib
if exist "C:\raylib\w64devkit\bin\gcc.exe" (
    C:\raylib\w64devkit\bin\g++.exe -std=c++14 -Wall -O2 -IC:\raylib\include main.cpp particle.cpp stick.cpp motor.cpp -o verlz.exe -LC:\raylib\lib -lraylib -lopengl32 -lgdi32 -lwinmm
    if %errorlevel% equ 0 (
        echo + SUCCESS: Build completed with w64devkit raylib
        echo Executable created: verlz.exe
        goto success
    )
)
echo X FAILED: w64devkit raylib configuration didn't work
echo.

REM Configuration 4: Try system PATH raylib
echo Trying configuration: System PATH raylib
g++ -std=c++14 -Wall -O2 main.cpp particle.cpp stick.cpp motor.cpp -o verlz.exe -lraylib -lopengl32 -lgdi32 -lwinmm
if %errorlevel% equ 0 (
    echo + SUCCESS: Build completed with system PATH raylib
    echo Executable created: verlz.exe
    goto success
)
echo X FAILED: System PATH raylib configuration didn't work
echo.

echo === All configurations failed ===
echo.
echo Possible solutions:
echo 1. Download raylib for Windows:
echo    - Go to: https://github.com/raysan5/raylib/releases
echo    - Download raylib-X.X.X_win64_mingw-w64.zip
echo    - Extract to C:\raylib
echo.
echo 2. Use raylib installer:
echo    - Download from: https://www.raylib.com/
echo    - Follow the installation instructions
echo.
echo 3. Install raylib with a package manager:
echo    - vcpkg: vcpkg install raylib
echo    - MSYS2: pacman -S mingw-w64-x86_64-raylib
echo.
echo 4. Check the last compilation error by running:
echo    g++ -std=c++14 -Wall main.cpp particle.cpp stick.cpp motor.cpp -o verlz.exe -lraylib -lopengl32 -lgdi32 -lwinmm
echo.
pause
exit /b 1

:success
echo.
echo === Build successful! ===
echo You can now run: verlz.exe
pause