// ============================================================================
// Explosion.h
// Purely visual one-shot explosion actor played upon impact or destruction.
// ============================================================================
#ifndef XENON_EXPLOSION_H
#define XENON_EXPLOSION_H

#include "engine/Actor.h"
#include "engine/SpriteAnimation.h"
#include "engine/Texture.h"
#include <memory>

namespace xenon {

class Explosion final : public engine::Actor {
public:
    // Spawns a 10-frame non-looping explosion at position using explode64.bmp.
    Explosion(engine::Vector2D position, std::shared_ptr<engine::Texture> texture);

    // Advances the explosion animation and deactivates this actor once the last frame finishes.
    void update(float deltaTime, const engine::InputManager& input) override;

    // Draws the current 64x64 frame of the explosion sprite sheet.
    void render(engine::Renderer& renderer) const override;

private:
    std::shared_ptr<engine::Texture> texture_;
    engine::SpriteAnimation animation_;
};

} // namespace xenon

#endif
