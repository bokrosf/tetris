# Tetris

Tetris game.

## Getting started

The project uses [CMake](https://cmake.org/) to generate the build system.

### 1. Cloning the repositoy:
```git clone https://github.com/bokrosf/tetris.git```

### 2. Setting up the project:
Build system generation uses CMake.

```
cmake -DCMAKE_BUILD_TYPE=Release -B build/release -S .
```

### 3. Building the project:
```
cmake --build build/release
```

### Extra

If necessary specify the generator. For example to use [MinGW](https://github.com/niXman/mingw-builds-binaries/releases) on Windows specify the generator with the -G option.
```
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -B build/release -S .
```

Debug build system generation:
```
cmake -DCMAKE_BUILD_TYPE=Debug -B build/debug -S .
```

