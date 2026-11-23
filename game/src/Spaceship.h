// ============================================================================
// Spaceship.h
// Player-controlled ship with 7-frame left/right banking animations, missile
// firing, destruction on hit, and post-respawn invulnerability blinking.
// ============================================================================
#ifndef XENON_SPACESHIP_H
#define XENON_SPACESHIP_H

#include "engine/Actor.h"
#include "engine/Texture.h"
#include <memory>

namespace xenon {

class Spaceship final : public engine::Actor {
public:
    // Creates the player Spaceship at position with an optional initial invulnerability duration.
    Spaceship(engine::Vector2D position,
              std::shared_ptr<engine::Texture> spaceshipTexture,
              std::shared_ptr<engine::Texture> missileTexture,
              std::shared_ptr<engine::Texture> explosionTexture,
              float invulnerableSeconds = 0.0f);

    // Processes movement input, clamps to screen edges, steps banking frames, and fires missiles.
    void update(float deltaTime, const engine::InputManager& input) override;

    // Draws the current Ship1.bmp banking tile, blinking translucently while invulnerable.
    void render(engine::Renderer& renderer) const override;

    // Destroys the ship with an Explosion on enemy/projectile overlap unless invulnerable.
    void onOverlapBegin(engine::Actor& other) override;

    // Returns true while the post-respawn invulnerability timer has not yet expired.
    bool isInvulnerable() const noexcept { return invulnerableTimer_ > 0.0f; }

private:
    std::shared_ptr<engine::Texture> texture_;
    std::shared_ptr<engine::Texture> missileTexture_;
    std::shared_ptr<engine::Texture> explosionTexture_;

    float speed_{280.0f};
    float fireCooldown_{0.18f};
    float fireTimer_{0.0f};
    int currentFrame_{3};
    float bankTimer_{0.0f};
    float invulnerableTimer_{0.0f};

    static constexpr float kScreenMargin = 32.0f;
    static constexpr float kBankInterval = 0.06f;
};

} // namespace xenon

#endif
