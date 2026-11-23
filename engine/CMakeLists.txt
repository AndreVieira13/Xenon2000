# ---------------------------------------------------------------------------
# Engine -- Generic, reusable 2D Game Engine static library
# ---------------------------------------------------------------------------
set(ENGINE_PUBLIC_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/Actor.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/Application.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/InputManager.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/Level.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/Renderer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/ResourceManager.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/Shader.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/SpriteAnimation.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/Texture.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/engine/Vector2D.h"
)

set(ENGINE_PRIVATE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/OpenGLRenderer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PhysicsWorld.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/QuadMesh.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/SDLWrappers.h"
)

set(ENGINE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Actor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Application.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/InputManager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Level.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/OpenGLRenderer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PhysicsWorld.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/QuadMesh.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ResourceManager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Shader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Texture.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Vector2D.cpp"
)

add_library(Engine STATIC
    ${ENGINE_PUBLIC_HEADERS}
    ${ENGINE_PRIVATE_HEADERS}
    ${ENGINE_SOURCES}
)

# Expose only engine/include/ publicly; SDL3, GLAD, OpenGL, Box2D and GLM stay PRIVATE
target_include_directories(Engine
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(Engine
    PRIVATE
        SDL3::SDL3
        glad
        OpenGL::GL
        box2d::box2d
        glm::glm
)

if(MSVC)
    target_compile_options(Engine PRIVATE /W4)
else()
    target_compile_options(Engine PRIVATE -Wall -Wextra)
endif()

# Expose engine shader directory path so any game target linking Engine can copy them
set_target_properties(Engine PROPERTIES
    ENGINE_SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES
    ${ENGINE_PUBLIC_HEADERS}
    ${ENGINE_PRIVATE_HEADERS}
    ${ENGINE_SOURCES}
)
