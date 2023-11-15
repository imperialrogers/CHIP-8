# CHIP-8 EMULATOR 🕹️

This is a Chip-8 compiler designed to emulate and run Chip-8 programs.

|           ![Space Invaders](images/space-invaders.png)           |     ![Space Invaders Game](images/space-invaders-game.png)     |
| :--------------------------------------------: | :------------------------------------: |
| ![Soccer](images/soccer.png) | ![Tetris](images/Tetris.png) |

## Overview

The Chip-8 is an interpreted programming language used in the 1970s and early 1980s on early microcomputers. This project aims to provide a compiler that can translate Chip-8 programs into executable code.

## Features

- **Chip-8 Emulation:** Compiles and runs Chip-8 programs.
- **Simple Interface:** Minimal setup to compile and execute programs.

## Getting Started

### Prerequisites


- **C++ compiler**

  - **Linux:** GCC 9
  - **Windows:** MinGW-w64 8.0 (GCC 9.2)
  - **macOS:** Install XCode command line tools

- **SDL2**

  - **Linux:** install using `sudo apt install libsdl2-dev`.
  - **Windows:** download the [SDL2-2.0.10 development libraries](https://www.libsdl.org/download-2.0.php) and place them under a new `external` folder in the root of this project. To run, download the [SDL2 runtime binaries](https://www.libsdl.org/download-2.0.php) and put `SDL2.dll` into the folder with your compiled binary.
  - **macOS:** install using `brew install sdl2`.

- **Emscripten**
  - [Emscripten](https://emscripten.org/docs/getting_started/index.html) required if compiling to WebAssembly.
    
### Building
Clone the repository into your system and go to the root directory of the project.
```console
git clone https://github.com/imperialrogers/CHIP-8.git
```

```console
cd CHIP-8
```

#### Windows
Just Run the following command in root directory.

```console
g++ -I src/include -L src/lib main.cpp Chip8.cpp Platform.cpp -lmingw32 -lSDl2main -lSDl2 -o chip8
```

## Usage

`./chip8 <Scale> <Delay> <ROM>`

- Some ROMs are provided in the /ROMs directory.

## References

- [Cowgod's Chip-8 Technical Reference v1.0](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)

- [CHIP-8 on Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)

- [Explanation of differing opcode behaviours on CHIP8/CHIP-48/SCHIP](https://www.reddit.com/r/programming/comments/3ca4ry/writing_a_chip8_interpreteremulator_in_c14_10/csuepjm/)

- [Austin Morlan's Building a Chip-8 Emulator(C++)](https://austinmorlan.com/posts/chip8_emulator/#source-code)

- [MDN Web Docs](https://developer.mozilla.org/en-US/docs/WebAssembly/C_to_wasm)
