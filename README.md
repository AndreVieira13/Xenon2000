# 2D Game Engine & Xenon 2000 Clone

Common Project 1 for **Advanced Game Programming Topics (ATGP)** and **Computer Graphics (CG)** — 2026/27.

## Project Structure

The repository is split into two independent CMake targets so the engine can be reused for any 2D game (e.g., a 2D platformer):

```text
.
├── CMakeLists.txt           # Root CMake setup (C++17, vcpkg find_package, GLAD)
├── CMakePresets.json        # Presets wired to $env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
├── vcpkg.json               # vcpkg manifest (sdl3, box2d, glm)
├── engine/                  # Target 1: 'Engine' static library (game-agnostic)
│   ├── CMakeLists.txt
│   ├── include/engine/      # Public headers (zero SDL3/OpenGL/Box2D/GLM includes)
│   ├── src/                 # Private implementation (SDL3, OpenGL 3.3 Core, Box2D, GLM)
│   └── shaders/             # GLSL vertex & fragment shaders (sprite.vert, sprite.frag)
├── game/                    # Target 2: 'Xenon2000' executable (links only Engine)
│   ├── CMakeLists.txt
│   ├── src/                 # Xenon 2000 level, actors, and main.cpp
│   └── assets/graphics/     # Xenon 2000 BMP sprite sheets and parallax backgrounds
└── external/glad/           # OpenGL 3.3 Core loader
```

## Building from a Clean Clone

### Prerequisites
- CMake 3.24+
- C++17 compiler (Visual Studio 2022 on Windows, Xcode on macOS, or GCC/Clang + Ninja)
- [vcpkg](https://github.com/microsoft/vcpkg) with the `VCPKG_ROOT` environment variable set to your vcpkg directory

### Configure & Build (using CMake Presets)
vcpkg automatically reads `vcpkg.json` and installs `sdl3`, `box2d`, and `glm` during configure:

```bash
# Windows (Visual Studio 2022)
cmake --preset vs2022
cmake --build --preset vs2022

# Windows / Linux / macOS (Ninja)
cmake --preset ninja
cmake --build --preset ninja

# macOS (Xcode)
cmake --preset xcode
cmake --build --preset xcode
```

*(Or open the folder in Visual Studio 2022, select `Xenon2000.exe` as the startup item, and press **F5**. `POST_BUILD` automatically copies `assets/` and `shaders/` next to the executable.)*

### Run
```bash
# From Visual Studio preset:
./build-vs/game/Debug/Xenon2000.exe

# From Ninja preset:
./build/game/Xenon2000.exe
```

## Controls
- **Move**: Arrow keys, `WASD`, or Gamepad D-Pad / Left Analog Stick
- **Fire**: `Space`, Xbox `A`, or PlayStation `Cross (X)`
- **Quit**: `Esc`
