// ============================================================================
// Rusher.h
// Vertical enemy that dives straight downward until it exits the bottom edge.
// ============================================================================
#ifndef XENON_RUSHER_H
#define XENON_RUSHER_H

#include "Enemy.h"

namespace xenon {

class Rusher final : public Enemy {
public:
    // Creates a Rusher descending at verticalSpeed with 1 HP and 4x6 rusher.bmp animation.
    Rusher(engine::Vec2 position,
           float verticalSpeed,
           std::shared_ptr<engine::Texture> texture,
           std::shared_ptr<engine::Texture> explosionTexture);

    // Advances animation and deactivates once the Rusher flies off the bottom of the screen.
    void update(float deltaTime, const engine::InputManager& input) override;
};

} // namespace xenon

#endif
