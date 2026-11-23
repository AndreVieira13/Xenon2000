// ============================================================================
// Enemy.cpp
// Shared base implementation for Xenon enemies: precondition check, animation,
// rendering, hit/explosion handling, and viewport bounds testing.
// ============================================================================
#include "Enemy.h"
#include "Explosion.h"
#include "engine/Level.h"
#include "engine/Renderer.h"
#include <stdexcept>
#include <utility>

namespace xenon {

// Validates positive initial health and stores enemy size, health, and textures.
Enemy::Enemy(engine::Vector2D position,
             engine::Vector2D velocity,
             engine::Vector2D size,
             int initialHealth,
             std::shared_ptr<engine::Texture> texture,
             std::shared_ptr<engine::Texture> explosionTexture)
    : Actor{position, velocity}
    , size_{size}
    , health_{initialHealth}
    , texture_{std::move(texture)}
    , explosionTexture_{std::move(explosionTexture)} {
    if (initialHealth <= 0) {
        throw std::invalid_argument("Enemy initial health must be positive");
    }
}

// Advances the enemy's looping sprite-sheet animation.
void Enemy::update(float deltaTime, const engine::InputManager& /*input*/) {
    animation_.update(deltaTime);
}

// Renders the active animation frame at the enemy's smoothly interpolated render position.
void Enemy::render(engine::Renderer& renderer) const {
    if (!texture_) {
        return;
    }
    renderer.drawSprite(*texture_,
                        getRenderPosition(),
                        size_,
                        0.0f,
                        animation_.currentCol(),
                        animation_.currentRow(),
                        animation_.cols,
                        animation_.rows);
}

// Applies 1 point of collision damage; spawns an Explosion and deactivates the enemy at 0 HP.
// Box2D collision masks ensure only PlayerShot or Player shapes can trigger this overlap.
void Enemy::onOverlapBegin(engine::Actor& /*other*/) {
    if (!isActive()) {
        return;
    }
    --health_;
    if (health_ <= 0) {
        if (auto* lvl = getLevel()) {
            lvl->spawnActor<Explosion>(getPosition(), explosionTexture_);
        }
        deactivate();
    }
}

// Tests whether the enemy has traveled completely past any edge of the level plus 'margin' pixels.
bool Enemy::isOutOfBounds(float margin) const noexcept {
    const auto* level = getLevel();
    const float w = level ? level->getWidth() : 800.0f;
    const float h = level ? level->getHeight() : 600.0f;
    const engine::Vector2D pos = getPosition();
    return pos.x < -margin || pos.x > w + margin || pos.y < -margin || pos.y > h + margin;
}

} // namespace xenon
