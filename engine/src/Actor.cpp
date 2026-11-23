// ============================================================================
// Actor.cpp
// Implements Actor transform caching, Box2D body synchronization, and smooth
// fixed-timestep position interpolation.
// ============================================================================
#include "engine/Actor.h"
#include "PhysicsWorld.h"

namespace engine {

// Initializes all three position snapshots to the same starting coordinates so
// newly spawned actors never interpolate from (0, 0) on their first frame.
Actor::Actor(Vector2D position, Vector2D velocity)
    : previousPosition_{position}
    , position_{position}
    , renderPosition_{position}
    , velocity_{velocity} {}

// Defined out-of-line so std::unique_ptr<Box2DBody> sees the complete definition of Box2DBody.
Actor::~Actor() = default;

// Stores non-owning pointers to the owning Level and PhysicsWorld when spawned.
void Actor::attachToLevel(Level* level, PhysicsWorld* physics) noexcept {
    level_ = level;
    physics_ = physics;
}

// Allocates this actor's Box2DBody once inside the owning Level's PhysicsWorld.
void Actor::initPhysicsBody(const BodyConfig& config) {
    if (!physics_ || body_) {
        return;
    }
    body_ = physics_->createBody(this, config, position_, velocity_);
}

// Teleports the actor to pos, resetting interpolation snapshots and moving the Box2D body.
void Actor::setPosition(Vector2D pos) noexcept {
    previousPosition_ = pos;
    position_ = pos;
    renderPosition_ = pos;
    if (body_) {
        body_->setPositionPx(pos);
    }
}

// Updates both the cached velocity and the underlying Box2D body's linear velocity.
void Actor::setVelocity(Vector2D vel) noexcept {
    velocity_ = vel;
    if (body_) {
        body_->setVelocityPx(vel);
    }
}

// Updates both the cached rotation angle and the underlying Box2D body's transform angle.
void Actor::setRotation(float radians) noexcept {
    rotation_ = radians;
    if (body_) {
        body_->setRotationRad(radians);
    }
}

// Saves the pre-step position right before Box2D advances so we can interpolate afterwards.
void Actor::snapshotPreviousTransform() noexcept {
    previousPosition_ = position_;
}

// Copies the latest simulated position, velocity, and rotation out of Box2D into cached members
// so render() and game queries read them in O(1) without repeated Box2D API calls.
void Actor::syncFromPhysics() noexcept {
    if (body_) {
        position_ = body_->getPositionPx();
        velocity_ = body_->getVelocityPx();
        rotation_ = body_->getRotationRad();
    }
}

// Blends between previousPosition_ (alpha = 0) and position_ (alpha = 1) using the
// leftover physics accumulator fraction to eliminate visual stutter on >60 Hz displays.
void Actor::updateRenderPosition(float alpha) noexcept {
    renderPosition_ = body_ ? lerp(previousPosition_, position_, alpha) : position_;
}

} // namespace engine
