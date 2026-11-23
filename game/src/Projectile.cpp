// ============================================================================
// Projectile.cpp
// Shared runtime behavior for both player Missiles and Loner EnemyProjectiles.
// ============================================================================
#include "Projectile.h"
#include "Explosion.h"
#include "engine/Level.h"
#include "engine/Renderer.h"
#include <utility>

namespace xenon {

namespace {
constexpr float kBoundsMargin = 32.0f;
} // namespace

// Initializes projectile transform, size, spin rate, and sprite/explosion textures.
Projectile::Projectile(engine::Vector2D position,
                       engine::Vector2D velocity,
                       engine::Vector2D size,
                       float rotationSpeedRad,
                       std::shared_ptr<engine::Texture> texture,
                       std::shared_ptr<engine::Texture> explosionTexture)
    : Actor{position, velocity}
    , texture_{std::move(texture)}
    , explosionTexture_{std::move(explosionTexture)}
    , size_{size}
    , rotationSpeed_{rotationSpeedRad} {}

// Advances the sprite-sheet animation and visual spin angle, then deactivates the
// projectile once it exits the viewport (linear motion itself is integrated by Box2D).
void Projectile::update(float deltaTime, const engine::InputManager& /*input*/) {
    animation_.update(deltaTime);
    visualRotation_ += rotationSpeed_ * deltaTime;

    // Cull off-screen projectiles so shots never accumulate indefinitely in the physics world.
    const auto* level = getLevel();
    const float w = level ? level->getWidth() : 800.0f;
    const float h = level ? level->getHeight() : 600.0f;
    const engine::Vector2D pos = getPosition();

    if (pos.x < -kBoundsMargin || pos.x > w + kBoundsMargin ||
        pos.y < -kBoundsMargin || pos.y > h + kBoundsMargin) {
        deactivate();
    }
}

// Draws the projectile at its smoothly interpolated physics position with visualRotation_.
void Projectile::render(engine::Renderer& renderer) const {
    if (!texture_) {
        return;
    }
    renderer.drawSprite(*texture_,
                        getRenderPosition(),
                        size_,
                        visualRotation_,
                        animation_.currentCol(),
                        animation_.currentRow(),
                        animation_.cols,
                        animation_.rows);
}

// Spawns an Explosion at the impact coordinates and marks this projectile for removal.
void Projectile::onOverlapBegin(engine::Actor& /*other*/) {
    if (!isActive()) {
        return;
    }
    if (auto* lvl = getLevel()) {
        lvl->spawnActor<Explosion>(getPosition(), explosionTexture_);
    }
    deactivate();
}

} // namespace xenon
