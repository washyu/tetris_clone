# Tetris Clone

This project is a Tetris clone that I coded to test out SDL2 and C++. It includes basic Tetris gameplay, such as rotating and moving tetrominoes, clearing full rows, and handling game over conditions.

## Features

- Basic Tetris gameplay
- Tetromino rotation and movement
- Row clearing and scoring
- Game over detection
- Text rendering using SDL_ttf

## Requirements

- SDL2
- SDL2_ttf
- C++ compiler (e.g., GCC)

## Installation

1. Clone the repository:
    ```sh
    git clone https://github.com/yourusername/tetris-clone.git
    cd tetris-clone
    ```

2. Install SDL2 and SDL2_ttf:
    - Download and install SDL2 from [SDL2](https://www.libsdl.org/download-2.0.php)
    - Download and install SDL2_ttf from [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/)

3. Build the project using CMake:
    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```

## Usage

Run the executable generated in the `build` directory:
```sh
./tetris