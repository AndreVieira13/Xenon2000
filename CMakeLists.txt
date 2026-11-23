cmake_minimum_required(VERSION 3.24)

# Automatically pick up vcpkg when VCPKG_ROOT is set (even without --preset)
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

project(EngineProject LANGUAGES C CXX)

# ---------------------------------------------------------------------------
# Global settings
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Organize targets neatly into folders in Visual Studio / Xcode
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ---------------------------------------------------------------------------
# Third-party dependencies (provided by vcpkg via vcpkg.json manifest)
# ---------------------------------------------------------------------------
find_package(SDL3 CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# GLAD -- OpenGL 3.3 Core loader (pre-generated in external/glad)
add_subdirectory(external/glad)

# OpenGL -- provided by the OS / graphics driver
find_package(OpenGL REQUIRED)

set_target_properties(glad PROPERTIES FOLDER "external")

# ---------------------------------------------------------------------------
# Separate Engine library and Game executable targets
# ---------------------------------------------------------------------------
add_subdirectory(engine)
add_subdirectory(game)

# Make Xenon2000 the default startup project in Visual Studio (F5)
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Xenon2000)
