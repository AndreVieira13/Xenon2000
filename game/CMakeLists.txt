# ---------------------------------------------------------------------------
# Xenon2000 -- Vertical space shooter clone built exclusively on Engine
# ---------------------------------------------------------------------------
set(GAME_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/CollisionLayers.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Enemy.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/EnemyProjectile.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Explosion.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Loner.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Missile.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Projectile.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Rusher.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Spaceship.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/XenonLevel.h"
)

set(GAME_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Enemy.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/EnemyProjectile.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Explosion.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Loner.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Missile.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Projectile.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Rusher.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Spaceship.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/XenonLevel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
)

add_executable(Xenon2000
    ${GAME_HEADERS}
    ${GAME_SOURCES}
)

# Only link Engine -- the game cannot include SDL3, OpenGL, GLAD, GLM, or Box2D directly
target_link_libraries(Xenon2000
    PRIVATE
        Engine
)

if(MSVC)
    target_compile_options(Xenon2000 PRIVATE /W4)
else()
    target_compile_options(Xenon2000 PRIVATE -Wall -Wextra)
endif()

# Copy Engine shaders and Game assets next to Xenon2000 executable on every build
get_target_property(ENGINE_SHADERS_DIR Engine ENGINE_SHADERS_DIR)
add_custom_command(TARGET Xenon2000 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Xenon2000>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ENGINE_SHADERS_DIR}"
        "$<TARGET_FILE_DIR:Xenon2000>/shaders"
)

# Run from the executable directory so copied assets/ and shaders/ always resolve
set_target_properties(Xenon2000 PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY   "$<TARGET_FILE_DIR:Xenon2000>"
    XCODE_SCHEME_WORKING_DIRECTORY  "$<TARGET_FILE_DIR:Xenon2000>"
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES
    ${GAME_HEADERS}
    ${GAME_SOURCES}
)
