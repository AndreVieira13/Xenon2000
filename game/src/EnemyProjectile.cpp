// ============================================================================
// EnemyProjectile.cpp
// Visibly rotating energy projectile fired by Loners (EnWeap6.bmp, 8 frames).
// ============================================================================
#include "EnemyProjectile.h"
#include <utility>

namespace xenon {

// Initializes Projectile along the aimed velocity vector with continuous 6.0 rad/s rotation
// and loops across all 8 frames of EnWeap6.bmp's 8x1 strip (16x16 px per tile).
EnemyProjectile::EnemyProjectile(engine::Vector2D position,
                                 engine::Vector2D velocity,
                                 std::shared_ptr<engine::Texture> texture,
                                 std::shared_ptr<engine::Texture> explosionTexture)
    : Projectile{position,
                 velocity,
                 engine::Vector2D{24.0f, 24.0f},
                 6.0f,
                 std::move(texture),
                 std::move(explosionTexture)} {
    animation_.cols = 8;
    animation_.rows = 1;
    animation_.startFrame = 0;
    animation_.frameCount = 8;
    animation_.fps = 14.0f;
    animation_.loop = true;
}

} // namespace xenon
