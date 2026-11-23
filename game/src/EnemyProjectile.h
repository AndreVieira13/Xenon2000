// ============================================================================
// EnemyProjectile.h
// Visibly rotating energy shot fired by Loner enemies toward the player ship.
// ============================================================================
#ifndef XENON_ENEMY_PROJECTILE_H
#define XENON_ENEMY_PROJECTILE_H

#include "Projectile.h"

namespace xenon {

class EnemyProjectile final : public Projectile {
public:
    // Configures a rotating enemy energy projectile along the supplied velocity vector.
    EnemyProjectile(engine::Vec2 position,
                    engine::Vec2 velocity,
                    std::shared_ptr<engine::Texture> texture,
                    std::shared_ptr<engine::Texture> explosionTexture);
};

} // namespace xenon

#endif
