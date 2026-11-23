// ============================================================================
// Loner.cpp
// Horizontal enemy flying across the screen in a straight line and firing an
// aimed EnemyProjectile toward the active Spaceship every 2 seconds.
// ============================================================================
#include "Loner.h"
#include "CollisionLayers.h"
#include "EnemyProjectile.h"
#include "Spaceship.h"
#include "engine/Level.h"
#include <utility>

namespace xenon {

// Initializes Enemy with horizontal velocity, 2 HP, and LonerA.bmp's 4x4 grid (64x64 px per tile).
Loner::Loner(engine::Vector2D position,
             float horizontalSpeed,
             std::shared_ptr<engine::Texture> texture,
             std::shared_ptr<engine::Texture> projectileTexture,
             std::shared_ptr<engine::Texture> explosionTexture)
    : Enemy{position,
            engine::Vector2D{horizontalSpeed, 0.0f},
            engine::Vector2D{64.0f, 64.0f},
            2,
            std::move(texture),
            std::move(explosionTexture)}
    , projectileTexture_{std::move(projectileTexture)} {
    animation_.cols = 4;
    animation_.rows = 4;
    animation_.startFrame = 0;
    animation_.frameCount = 16;
    animation_.fps = 12.0f;
    animation_.loop = true;
}

// Steps animation, deactivates after crossing out of bounds, and fires a dynamic bullet
// EnemyProjectile aimed at the player's current position every 2 seconds.
void Loner::update(float deltaTime, const engine::InputManager& input) {
    Enemy::update(deltaTime, input);

    if (isOutOfBounds(64.0f)) {
        deactivate();
        return;
    }

    fireTimer_ += deltaTime;
    if (fireTimer_ >= kFireInterval) {
        fireTimer_ -= kFireInterval;
        if (auto* lvl = getLevel()) {
            const engine::Vector2D pos = getPosition();
            engine::Vector2D dir{0.0f, -1.0f};

            // Dynamically query the active Spaceship right when firing instead of storing a raw
            // Spaceship* across frames (prevents dangling pointers while the player is respawning).
            if (const auto* ship = lvl->findActor<Spaceship>()) {
                dir = engine::normalized(ship->getPosition() - pos);
            }

            // Dynamic bullet body enables Box2D continuous collision detection (CCD) against the player.
            engine::BodyConfig cfg{};
            cfg.type = engine::BodyType::Dynamic;
            cfg.halfExtents = {8.0f, 8.0f};
            cfg.density = 1.0f;
            cfg.isBullet = true;
            cfg.isSensor = false;
            cfg.enableSensorEvents = true;
            cfg.lockRotation = true;
            cfg.categoryBits = layer::EnemyShot;
            cfg.maskBits = layer::Player;

            lvl->spawnActorWithPhysics<EnemyProjectile>(
                cfg,
                pos,
                dir * kProjectileSpeed,
                projectileTexture_,
                getExplosionTexture());
        }
    }
}

} // namespace xenon
