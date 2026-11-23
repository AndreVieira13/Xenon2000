// ============================================================================
// Explosion.cpp
// One-shot 10-frame explosion animation using explode64.bmp (5 cols x 2 rows).
// ============================================================================
#include "Explosion.h"
#include "engine/Renderer.h"
#include <utility>

namespace xenon {

// Configures a non-looping 10-frame animation matching explode64.bmp's 5x2 grid (64x64 px per tile).
Explosion::Explosion(engine::Vec2 position, std::shared_ptr<engine::Texture> texture)
    : Actor{position}
    , texture_{std::move(texture)} {
    animation_.cols = 5;
    animation_.rows = 2;
    animation_.startFrame = 0;
    animation_.frameCount = 10;
    animation_.fps = 20.0f;
    animation_.loop = false;
}

// Steps the explosion animation forward and marks this actor inactive as soon as the last frame ends.
void Explosion::update(float deltaTime, const engine::InputManager& /*input*/) {
    animation_.update(deltaTime);
    if (animation_.finished) {
        deactivate();
    }
}

// Renders the current 64x64 explosion tile at this actor's screen position.
void Explosion::render(engine::Renderer& renderer) const {
    if (!texture_) {
        return;
    }
    renderer.drawSprite(*texture_,
                        getRenderPosition(),
                        engine::Vec2{64.0f, 64.0f},
                        0.0f,
                        animation_.currentCol(),
                        animation_.currentRow(),
                        animation_.cols,
                        animation_.rows);
}

} // namespace xenon
