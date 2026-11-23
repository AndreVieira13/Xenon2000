// ============================================================================
// Missile.h
// Player light missile fired upward from the Spaceship (row 0 of missile.bmp).
// ============================================================================
#ifndef XENON_MISSILE_H
#define XENON_MISSILE_H

#include "Projectile.h"

namespace xenon {

class Missile final : public Projectile {
public:
    // Configures an upward-moving light missile using the first row of missile.bmp.
    Missile(engine::Vec2 position,
            std::shared_ptr<engine::Texture> texture,
            std::shared_ptr<engine::Texture> explosionTexture);
};

} // namespace xenon

#endif
