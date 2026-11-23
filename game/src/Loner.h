// ============================================================================
// Loner.h
// Horizontal enemy flying across the screen in a straight line while firing
// aimed EnemyProjectiles toward the Spaceship every 2 seconds.
// ============================================================================
#ifndef XENON_LONER_H
#define XENON_LONER_H

#include "Enemy.h"

namespace xenon {

class Loner final : public Enemy {
public:
    // Creates a Loner flying horizontally at horizontalSpeed with 2 HP and LonerA.bmp animation.
    Loner(engine::Vec2 position,
          float horizontalSpeed,
          std::shared_ptr<engine::Texture> texture,
          std::shared_ptr<engine::Texture> projectileTexture,
          std::shared_ptr<engine::Texture> explosionTexture);

    // Advances animation, cleans up when out of bounds, and fires an aimed shot every 2 seconds.
    void update(float deltaTime, const engine::InputManager& input) override;

private:
    std::shared_ptr<engine::Texture> projectileTexture_;
    float fireTimer_{0.0f};
    static constexpr float kFireInterval = 2.0f;
    static constexpr float kProjectileSpeed = 220.0f;
};

} // namespace xenon

#endif
