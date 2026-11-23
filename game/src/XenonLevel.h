// ============================================================================
// XenonLevel.h
// Concrete Level implementing the vertical Xenon 2000 space shooter.
// ============================================================================
#ifndef XENON_XENON_LEVEL_H
#define XENON_XENON_LEVEL_H

#include "engine/Level.h"
#include "engine/Texture.h"
#include <memory>
#include <random>

namespace xenon {

class XenonLevel final : public engine::Level {
public:
    // Initializes a zero-gravity space level of size (width x height) pixels.
    XenonLevel(float width, float height);

    // Preloads and caches all Xenon 2000 textures and spawns the Spaceship + opening enemy wave.
    void onLoad(engine::ResourceManager& resources) override;

    // Scrolls both parallax layers, handles Spaceship respawning, spawns waves, and updates Level.
    void update(float deltaTime, const engine::InputManager& input) override;

protected:
    // Renders the slow-scrolling galaxy2 layer followed by the faster bluish-black stones layer.
    void renderBackground(engine::Renderer& renderer) const override;

private:
    // Spawns the player Spaceship at bottom-center with its Box2D kinematic sensor body.
    void spawnSpaceship(float invulnerableSeconds);

    // Spawns a horizontal Loner enemy at (x, y) moving at horizontal 'speed'.
    void spawnLoner(float x, float y, float speed);

    // Spawns a vertical Rusher enemy at (x, y) descending at vertical 'speed'.
    void spawnRusher(float x, float y, float speed);

    std::shared_ptr<engine::Texture> farBackgroundTexture_;
    std::shared_ptr<engine::Texture> nearBackgroundTexture_;
    std::shared_ptr<engine::Texture> spaceshipTexture_;
    std::shared_ptr<engine::Texture> missileTexture_;
    std::shared_ptr<engine::Texture> explosionTexture_;
    std::shared_ptr<engine::Texture> lonerTexture_;
    std::shared_ptr<engine::Texture> enemyProjectileTexture_;
    std::shared_ptr<engine::Texture> rusherTexture_;

    float farScrollOffset_{0.0f};
    float nearScrollOffset_{0.0f};
    float respawnTimer_{0.0f};
    float lonerSpawnTimer_{1.0f};
    float rusherSpawnTimer_{0.5f};
    std::mt19937 rng_{42};

    static constexpr float kFarScrollSpeed = 0.04f;
    static constexpr float kNearScrollSpeed = 0.16f;
    static constexpr float kRespawnDelay = 1.5f;
    static constexpr float kRespawnInvulnerability = 2.0f;
    static constexpr float kLonerSpawnInterval = 4.5f;
    static constexpr float kRusherSpawnInterval = 2.2f;
};

} // namespace xenon

#endif
