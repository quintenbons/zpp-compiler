# Z++: The C++ Compiler Project from Zero

Welcome to Z++, a custom C++ compiler project built from scratch

## Getting Started Quickly

```bash
./build.sh  # Build the Z++ compiler
./cmake-build/bin/z++ ./cpp_testbase/simple.cpp  # Compile a simple C++ program
```

## Dependencies

```bash
sudo apt install nasm
sudo apt install libboost-all-dev
sudo apt-get install cmake
```

## Development Setup

Setting up your development environment is straightforward and enables you to launch programs with a sub-version of the standard library compiled by z++.

```bash
source ./setenv.sh                  # Will update your PATH, and add zpp_* commands
zpp_commands                        # List of available commands
zpp_build -c                        # Compile the Z++ compiler
z++ ./cpp_testbase/simple.cpp       # Use Z++ to compile a C++ program
```

## Additional Development Tools

Z++ also provides tools for working with assembly language:

```bash
zpp_assemble_binary ./asm_testbase/simple.asm       # Compile an assembly file
zpp_run_nasm ./asm_testbase/simple.asm              # Execute the compiled assembly code
```
