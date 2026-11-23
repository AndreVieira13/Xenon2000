// ============================================================================
// Projectile.h
// Shared base class for both player Missiles and enemy EnemyProjectiles.
// ============================================================================
#ifndef XENON_PROJECTILE_H
#define XENON_PROJECTILE_H

#include "engine/Actor.h"
#include "engine/SpriteAnimation.h"
#include "engine/Texture.h"
#include <memory>

namespace xenon {

class Projectile : public engine::Actor {
public:
    // Initializes projectile transform, rendered size, spin speed, sprite texture, and impact explosion texture.
    Projectile(engine::Vector2D position,
               engine::Vector2D velocity,
               engine::Vector2D size,
               float rotationSpeedRad,
               std::shared_ptr<engine::Texture> texture,
               std::shared_ptr<engine::Texture> explosionTexture);

    // Advances sprite animation and spin rotation, and deactivates once outside screen bounds.
    void update(float deltaTime, const engine::InputManager& input) override;

    // Draws the current animated frame at the interpolated render position with visualRotation_.
    void render(engine::Renderer& renderer) const override;

    // Spawns an Explosion at impact position and marks this projectile inactive.
    void onOverlapBegin(engine::Actor& other) override;

protected:
    engine::SpriteAnimation animation_;

private:
    std::shared_ptr<engine::Texture> texture_;
    std::shared_ptr<engine::Texture> explosionTexture_;
    engine::Vector2D size_{16.0f, 16.0f};
    float rotationSpeed_{0.0f};
    float visualRotation_{0.0f};
};

} // namespace xenon

#endif
