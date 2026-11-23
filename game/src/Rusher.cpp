// ============================================================================
// Rusher.cpp
// Vertical enemy descending straight down until leaving the bottom edge.
// ============================================================================
#include "Rusher.h"
#include <utility>

namespace xenon {

// Initializes Enemy with downward velocity, 1 HP, and rusher.bmp's 4x6 grid (64x32 px per tile).
Rusher::Rusher(engine::Vec2 position,
               float verticalSpeed,
               std::shared_ptr<engine::Texture> texture,
               std::shared_ptr<engine::Texture> explosionTexture)
    : Enemy{position,
            engine::Vec2{0.0f, -verticalSpeed},
            engine::Vec2{64.0f, 32.0f},
            1,
            std::move(texture),
            std::move(explosionTexture)} {
    animation_.cols = 4;
    animation_.rows = 6;
    animation_.startFrame = 0;
    animation_.frameCount = 24;
    animation_.fps = 16.0f;
    animation_.loop = true;
}

// Steps animation and deactivates once the Rusher exits the screen bounds.
void Rusher::update(float deltaTime, const engine::InputManager& input) {
    Enemy::update(deltaTime, input);

    if (isOutOfBounds(64.0f)) {
        deactivate();
    }
}

} // namespace xenon
