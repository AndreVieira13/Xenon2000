// ============================================================================
// Spaceship.cpp
// Implements player movement clamping, 7-frame Ship1.bmp banking transitions,
// missile firing with cooldown, destruction, and blinking respawn protection.
// ============================================================================
#include "Spaceship.h"
#include "CollisionLayers.h"
#include "Explosion.h"
#include "Missile.h"
#include "engine/InputManager.h"
#include "engine/Level.h"
#include "engine/Renderer.h"
#include <algorithm>
#include <cmath>
#include <utility>

namespace xenon {

// Initializes the player ship with shared sprite/missile/explosion textures and optional shield time.
Spaceship::Spaceship(engine::Vec2 position,
                     std::shared_ptr<engine::Texture> spaceshipTexture,
                     std::shared_ptr<engine::Texture> missileTexture,
                     std::shared_ptr<engine::Texture> explosionTexture,
                     float invulnerableSeconds)
    : Actor{position}
    , texture_{std::move(spaceshipTexture)}
    , missileTexture_{std::move(missileTexture)}
    , explosionTexture_{std::move(explosionTexture)}
    , invulnerableTimer_{invulnerableSeconds} {}

// Updates invulnerability timer, computes clamped movement velocity from InputManager,
// smoothly transitions banking frames on Ship1.bmp, and spawns Missiles when Fire is held.
void Spaceship::update(float deltaTime, const engine::InputManager& input) {
    if (invulnerableTimer_ > 0.0f) {
        invulnerableTimer_ = std::max(0.0f, invulnerableTimer_ - deltaTime);
    }

    // Normalize diagonal input so moving diagonally isn't sqrt(2) times faster than axis-aligned motion;
    // lengthSquared() avoids invoking std::sqrt() when moving purely horizontally, vertically, or idle.
    engine::Vec2 axis = input.getMoveAxis();
    if (engine::lengthSquared(axis) > 1.0f) {
        axis = engine::normalized(axis);
    }

    engine::Vec2 vel = axis * speed_;

    // Prevent the ship from leaving the visible arena by cancelling outward velocity at each border.
    const auto* level = getLevel();
    const float w = level ? level->getWidth() : 800.0f;
    const float h = level ? level->getHeight() : 600.0f;
    const engine::Vec2 pos = getPosition();

    if ((pos.x <= kScreenMargin && vel.x < 0.0f) || (pos.x >= w - kScreenMargin && vel.x > 0.0f)) {
        vel.x = 0.0f;
    }
    if ((pos.y <= kScreenMargin && vel.y < 0.0f) || (pos.y >= h - kScreenMargin && vel.y > 0.0f)) {
        vel.y = 0.0f;
    }

    setVelocity(vel);

    // Banking state machine across Ship1.bmp's 7 horizontal tiles:
    // frames 0..2 = turning left, frame 3 = level center, frames 4..6 = turning right.
    // Stepping one tile every kBankInterval seconds creates a smooth roll into/out of turns.
    int targetFrame = 3;
    if (axis.x < -0.1f) {
        targetFrame = 0;
    } else if (axis.x > 0.1f) {
        targetFrame = 6;
    }

    bankTimer_ += deltaTime;
    if (bankTimer_ >= kBankInterval) {
        bankTimer_ = 0.0f;
        if (currentFrame_ < targetFrame) {
            ++currentFrame_;
        } else if (currentFrame_ > targetFrame) {
            --currentFrame_;
        }
    }

    // Enforce fire cooldown and spawn an upward dynamic bullet Missile when Action::Fire is held.
    if (fireTimer_ > 0.0f) {
        fireTimer_ = std::max(0.0f, fireTimer_ - deltaTime);
    }

    if (input.isActionHeld(engine::Action::Fire) && fireTimer_ <= 0.0f) {
        fireTimer_ = fireCooldown_;
        if (auto* lvl = getLevel()) {
            // Dynamic bullet body performs CCD so fast missiles never skip over moving enemy hitboxes.
            engine::BodyConfig cfg{};
            cfg.type = engine::BodyType::Dynamic;
            cfg.halfExtents = {8.0f, 8.0f};
            cfg.density = 1.0f;
            cfg.isBullet = true;
            cfg.isSensor = false;
            cfg.enableSensorEvents = true;
            cfg.lockRotation = true;
            cfg.categoryBits = layer::PlayerShot;
            cfg.maskBits = layer::Enemy;

            const engine::Vec2 spawnPos{pos.x, pos.y + 36.0f};
            lvl->spawnActorWithPhysics<Missile>(cfg, spawnPos, missileTexture_, explosionTexture_);
        }
    }
}

// Draws the active banking tile of Ship1.bmp, oscillating opacity at 5 Hz while invulnerable.
void Spaceship::render(engine::Renderer& renderer) const {
    if (!texture_) {
        return;
    }
    // Square-wave blink pattern alternating between 35% and 100% opacity during respawn protection.
    float alpha = 1.0f;
    if (invulnerableTimer_ > 0.0f) {
        alpha = (std::fmod(invulnerableTimer_ * 10.0f, 2.0f) < 1.0f) ? 0.35f : 1.0f;
    }

    renderer.drawSprite(*texture_,
                        getRenderPosition(),
                        engine::Vec2{64.0f, 64.0f},
                        0.0f,
                        currentFrame_,
                        0,
                        7,
                        1,
                        alpha);
}

// Spawns an Explosion and deactivates the Spaceship on enemy/projectile impact (ignored while shielded).
void Spaceship::onOverlapBegin(engine::Actor& /*other*/) {
    if (!isActive() || isInvulnerable()) {
        return;
    }
    if (auto* lvl = getLevel()) {
        lvl->spawnActor<Explosion>(getPosition(), explosionTexture_);
    }
    deactivate();
}

} // namespace xenon
