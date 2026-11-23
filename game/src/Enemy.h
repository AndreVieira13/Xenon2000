// ============================================================================
// Enemy.h
// Shared base class for all Xenon enemies (Loner, Rusher, etc.).
// ============================================================================
#ifndef XENON_ENEMY_H
#define XENON_ENEMY_H

#include "engine/Actor.h"
#include "engine/SpriteAnimation.h"
#include "engine/Texture.h"
#include <memory>

namespace xenon {

class Enemy : public engine::Actor {
public:
    // Initializes an enemy with starting transform, pixel size, positive health, and textures.
    // Throws std::invalid_argument if initialHealth <= 0.
    Enemy(engine::Vector2D position,
          engine::Vector2D velocity,
          engine::Vector2D size,
          int initialHealth,
          std::shared_ptr<engine::Texture> texture,
          std::shared_ptr<engine::Texture> explosionTexture);

    // Advances the enemy's looping sprite-sheet animation.
    void update(float deltaTime, const engine::InputManager& input) override;

    // Draws the enemy's current animated frame at its interpolated render position.
    void render(engine::Renderer& renderer) const override;

    // Reduces health by 1 on collision; spawns an Explosion and deactivates when health reaches 0.
    void onOverlapBegin(engine::Actor& other) override;

protected:
    // Returns true once the enemy has completely exited the level viewport plus 'margin' pixels.
    bool isOutOfBounds(float margin = 64.0f) const noexcept;

    // Returns the shared explosion texture for subclasses that spawn projectiles.
    const std::shared_ptr<engine::Texture>& getExplosionTexture() const noexcept { return explosionTexture_; }

    engine::SpriteAnimation animation_;
    engine::Vector2D size_{64.0f, 64.0f};
    int health_{1};

private:
    std::shared_ptr<engine::Texture> texture_;
    std::shared_ptr<engine::Texture> explosionTexture_;
};

} // namespace xenon

#endif
