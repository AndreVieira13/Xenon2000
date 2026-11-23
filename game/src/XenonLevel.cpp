// ============================================================================
// XenonLevel.cpp
// Orchestrates the Xenon 2000 vertical shooter: resource loading, two-layer
// parallax scrolling, periodic Loner/Rusher spawning, and Spaceship respawning.
// ============================================================================
#include "XenonLevel.h"
#include "CollisionLayers.h"
#include "Loner.h"
#include "Rusher.h"
#include "Spaceship.h"
#include "engine/Renderer.h"
#include "engine/ResourceManager.h"
#include <cmath>

namespace xenon {

// Constructs the base Level with zero gravity for top-down space flight.
XenonLevel::XenonLevel(float width, float height)
    : Level{width, height, engine::Vec2{0.0f, 0.0f}} {}

// Preloads all Xenon 2000 sprite sheets and parallax backgrounds into ResourceManager
// and spawns the player ship along with an opening wave of enemies.
void XenonLevel::onLoad(engine::ResourceManager& resources) {
    // galaxy2.bmp uses GL_MIRRORED_REPEAT so top/bottom edges meet seamlessly with zero visible seam;
    // parallax_stones.bmp uses GL_REPEAT so rock formations scroll upright.
    farBackgroundTexture_   = resources.getTexture("assets/graphics/galaxy2.bmp", engine::TextureWrap::MirroredRepeat);
    nearBackgroundTexture_  = resources.getTexture("assets/graphics/parallax_stones.bmp", engine::TextureWrap::Repeat);
    spaceshipTexture_       = resources.getTexture("assets/graphics/Ship1.bmp");
    missileTexture_         = resources.getTexture("assets/graphics/missile.bmp");
    explosionTexture_       = resources.getTexture("assets/graphics/explode64.bmp");
    lonerTexture_           = resources.getTexture("assets/graphics/LonerA.bmp");
    enemyProjectileTexture_ = resources.getTexture("assets/graphics/EnWeap6.bmp");
    rusherTexture_          = resources.getTexture("assets/graphics/rusher.bmp");

    spawnSpaceship(0.0f);

    spawnLoner(-32.0f, getHeight() - 90.0f, 120.0f);
    spawnLoner(getWidth() + 32.0f, getHeight() - 160.0f, -120.0f);
    spawnRusher(getWidth() * 0.5f, getHeight() + 40.0f, 150.0f);
}

// Advances parallax UV offsets, monitors player death to schedule respawning, triggers
// periodic Loner and Rusher waves, and runs base Level::update().
void XenonLevel::update(float deltaTime, const engine::InputManager& input) {
    // Wrap scroll offsets cleanly to preserve float precision:
    // GL_MIRRORED_REPEAT repeats every 2.0 UV units (1 forward + 1 mirrored), GL_REPEAT every 1.0.
    farScrollOffset_  = std::fmod(farScrollOffset_  + kFarScrollSpeed  * deltaTime, 2.0f);
    nearScrollOffset_ = std::fmod(nearScrollOffset_ + kNearScrollSpeed * deltaTime, 1.0f);

    // Wait kRespawnDelay seconds after destruction (allowing the explosion to finish) before respawning.
    if (!findActor<Spaceship>()) {
        respawnTimer_ += deltaTime;
        if (respawnTimer_ >= kRespawnDelay) {
            respawnTimer_ = 0.0f;
            spawnSpaceship(kRespawnInvulnerability);
        }
    } else {
        respawnTimer_ = 0.0f;
    }

    // Periodically spawn a Loner entering horizontally from either the left or right edge.
    lonerSpawnTimer_ += deltaTime;
    if (lonerSpawnTimer_ >= kLonerSpawnInterval) {
        lonerSpawnTimer_ = 0.0f;
        std::uniform_real_distribution<float> distY{getHeight() * 0.6f, getHeight() - 70.0f};
        const bool fromLeft = (rng_() % 2) == 0;
        spawnLoner(fromLeft ? -32.0f : getWidth() + 32.0f,
                   distY(rng_),
                   fromLeft ? 120.0f : -120.0f);
    }

    // Periodically spawn a Rusher diving straight down from a random X coordinate above the top edge.
    rusherSpawnTimer_ += deltaTime;
    if (rusherSpawnTimer_ >= kRusherSpawnInterval) {
        rusherSpawnTimer_ = 0.0f;
        std::uniform_real_distribution<float> distX{64.0f, getWidth() - 64.0f};
        spawnRusher(distX(rng_), getHeight() + 40.0f, 160.0f);
    }

    Level::update(deltaTime, input);
}

// Draws the single slow-scrolling galaxy2 far layer first, then overlays the faster
// bluish-black stones layer whose magenta background was keyed to transparent.
void XenonLevel::renderBackground(engine::Renderer& renderer) const {
    if (farBackgroundTexture_) {
        renderer.drawScrollingBackground(*farBackgroundTexture_, farScrollOffset_, 1.0f, 1.0f, 1.0f);
    }
    if (nearBackgroundTexture_) {
        renderer.drawScrollingBackground(*nearBackgroundTexture_, nearScrollOffset_, 1.0f, 1.0f, 1.0f);
    }
}

// Creates the player Spaceship at bottom-center with a kinematic sensor hitbox filtered to enemies/shots.
void XenonLevel::spawnSpaceship(float invulnerableSeconds) {
    engine::BodyConfig cfg{};
    cfg.type = engine::BodyType::Kinematic;
    cfg.halfExtents = {22.0f, 22.0f};
    cfg.isSensor = true;
    cfg.enableSensorEvents = true;
    cfg.lockRotation = true;
    cfg.categoryBits = layer::Player;
    cfg.maskBits = layer::Enemy | layer::EnemyShot;

    spawnActorWithPhysics<Spaceship>(
        cfg,
        engine::Vec2{getWidth() * 0.5f, 80.0f},
        spaceshipTexture_,
        missileTexture_,
        explosionTexture_,
        invulnerableSeconds);
}

// Creates a Loner at (x, y) with a kinematic sensor hitbox filtered to player missiles and ship.
void XenonLevel::spawnLoner(float x, float y, float speed) {
    engine::BodyConfig cfg{};
    cfg.type = engine::BodyType::Kinematic;
    cfg.halfExtents = {24.0f, 24.0f};
    cfg.isSensor = true;
    cfg.enableSensorEvents = true;
    cfg.lockRotation = true;
    cfg.categoryBits = layer::Enemy;
    cfg.maskBits = layer::PlayerShot | layer::Player;

    spawnActorWithPhysics<Loner>(
        cfg,
        engine::Vec2{x, y},
        speed,
        lonerTexture_,
        enemyProjectileTexture_,
        explosionTexture_);
}

// Creates a Rusher at (x, y) with a 48x28 sensor hitbox matching its 64x32 sprite silhouette.
void XenonLevel::spawnRusher(float x, float y, float speed) {
    engine::BodyConfig cfg{};
    cfg.type = engine::BodyType::Kinematic;
    cfg.halfExtents = {24.0f, 14.0f};
    cfg.isSensor = true;
    cfg.enableSensorEvents = true;
    cfg.lockRotation = true;
    cfg.categoryBits = layer::Enemy;
    cfg.maskBits = layer::PlayerShot | layer::Player;

    spawnActorWithPhysics<Rusher>(
        cfg,
        engine::Vec2{x, y},
        speed,
        rusherTexture_,
        explosionTexture_);
}

} // namespace xenon
