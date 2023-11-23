<h1 align="center">CrossFire</h1>
<div align="center">A game engine written in C++ for various platforms.</div>

## What is CrossFire?

CrossFire is a game engine written in C++ for various platforms. It is designed to be simple and easy to use, while still being powerful enough to create games with. It is currently in development, and is not ready for use yet.

CrossFire will be used primarily in the development of the CrossCraft project, a Minecraft implementation written in C++.

## Platform Support List

✔️ Supported

🚧 Work In Progress

❌ Not Supported (yet)

| Platform    | Status |
|-------------|--------|
| Windows     | ✔️     |
| MacOS (x86) | ❌      |
| MacOS (ARM) | ❌      |
| Linux       | ✔️     |
| Android     | ❌      |
| PSP         | ❌      |
| PS Vita     | ❌      |
| PS2         | ❌      |
| PS3         | ❌      |
| PS4         | ❌      |
| 3DS         | ❌      |
| Switch      | ❌      |
| Wii         | ❌      |
| Wii U       | ❌      |
| Xbox 360    | ❌      |
| Xbox One    | ❌      |

## Building

CrossFire uses CMake to generate build files. To build CrossFire, you will need to install CMake and a C++ compiler.
CrossFire uses C++17 features and git submodules to build, so you will need to make sure you've cloned the repository with `git clone --recursive`.

### Windows 

#### Visual Studio

To build CrossFire on Windows with Visual Studio, you will need to install Visual Studio 2019 and CMake. You can then open the CMake project in Visual Studio and build it.

#### MinGW

To build CrossFire on Windows with MinGW, you will need to install MinGW and CMake. You can then run the following commands in the CrossFire directory:

```
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```

### Linux

To build CrossFire on Linux, you will need to install CMake and a C++ compiler. You can then run the following commands in the CrossFire directory:

```
mkdir build
cd build
cmake ..
make
```

You will need some additional dependencies to build CrossFire on Linux.

Check the GLFW documentation for more information: https://www.glfw.org/docs/latest/compile_guide.html

