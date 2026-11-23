// ============================================================================
// CollisionLayers.h
// Bitmask constants for Box2D collision filtering (categoryBits / maskBits).
// Two shapes collide only when each shape's category bit is enabled in the
// other shape's maskBits.
// ============================================================================
#ifndef XENON_COLLISION_LAYERS_H
#define XENON_COLLISION_LAYERS_H

#include <cstdint>

namespace xenon::layer {

inline constexpr std::uint64_t Player     = 1ULL << 0;
inline constexpr std::uint64_t Enemy      = 1ULL << 1;
inline constexpr std::uint64_t PlayerShot = 1ULL << 2;
inline constexpr std::uint64_t EnemyShot  = 1ULL << 3;

} // namespace xenon::layer

#endif
