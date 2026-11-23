// ============================================================================
// Missile.cpp
// Player light missiles (row 0 of missile.bmp) flying upward at 550 px/s.
// ============================================================================
#include "Missile.h"
#include <utility>

namespace xenon {

// Initializes Projectile with upward velocity (550 px/s) and loops over row 0
// (frames 0 and 1, light missiles) of missile.bmp's 2x3 grid (16x16 px per tile).
Missile::Missile(engine::Vector2D position,
                 std::shared_ptr<engine::Texture> texture,
                 std::shared_ptr<engine::Texture> explosionTexture)
    : Projectile{position,
                 engine::Vector2D{0.0f, 550.0f},
                 engine::Vector2D{20.0f, 20.0f},
                 0.0f,
                 std::move(texture),
                 std::move(explosionTexture)} {
    animation_.cols = 2;
    animation_.rows = 3;
    animation_.startFrame = 0;
    animation_.frameCount = 2;
    animation_.fps = 12.0f;
    animation_.loop = true;
}

} // namespace xenon
