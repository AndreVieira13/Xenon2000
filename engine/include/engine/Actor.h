// ============================================================================
// Actor.h
// Abstract base class for every entity living inside a Level.
// Caches physics transforms for O(1) rendering with fixed-timestep interpolation
// and owns an optional RAII Box2DBody.
// ============================================================================
#ifndef ENGINE_ACTOR_H
#define ENGINE_ACTOR_H

#include "engine/Vector2D.h"
#include <cstdint>
#include <memory>

namespace engine {

class Box2DBody;
class InputManager;
class Level;
class PhysicsWorld;
class Renderer;

enum class BodyType {
    Static,
    Kinematic,
    Dynamic
};

struct BodyConfig {
    BodyType type{BodyType::Kinematic};
    Vector2D halfExtents{16.0f, 16.0f};
    float density{1.0f};
    bool isSensor{false};
    bool isBullet{false};
    bool enableSensorEvents{true};
    bool lockRotation{true};
    std::uint64_t categoryBits{0x0001ULL};
    std::uint64_t maskBits{0xFFFFFFFFFFFFFFFFULL};
};

class Actor {
public:
    // Initializes an active actor at the given starting pixel position and velocity.
    explicit Actor(Vector2D position = {0.0f, 0.0f}, Vector2D velocity = {0.0f, 0.0f});

    // Destroys the actor and automatically frees its Box2DBody via std::unique_ptr.
    virtual ~Actor();

    Actor(const Actor&) = delete;
    Actor& operator=(const Actor&) = delete;
    Actor(Actor&&) = delete;
    Actor& operator=(Actor&&) = delete;

    // Runs per-frame game logic, input handling, and animation updates for this actor.
    virtual void update(float deltaTime, const InputManager& input) = 0;

    // Submits this actor's visual representation to the abstract Renderer.
    virtual void render(Renderer& renderer) const = 0;

    // Invoked by PhysicsWorld when this actor's shape starts overlapping another actor.
    // Never delete actors inside this callback; call deactivate() for deferred cleanup.
    virtual void onOverlapBegin(Actor& /*other*/) {}

    // Returns the latest authoritative physics/logic position in pixels.
    Vector2D getPosition() const noexcept { return position_; }

    // Returns the smoothly interpolated pixel position for stutter-free rendering.
    Vector2D getRenderPosition() const noexcept { return renderPosition_; }

    // Teleports the actor (and its physics body, if present) to pos without interpolation smear.
    void setPosition(Vector2D pos) noexcept;

    // Returns the current linear velocity in pixels/second.
    Vector2D getVelocity() const noexcept { return velocity_; }

    // Updates linear velocity on both the actor cache and its Box2D body.
    void setVelocity(Vector2D vel) noexcept;

    // Returns the current rotation angle in radians.
    float getRotation() const noexcept { return rotation_; }

    // Updates rotation angle on both the actor cache and its Box2D body.
    void setRotation(float radians) noexcept;

    // Returns true while this actor is alive and participating in updates/rendering.
    bool isActive() const noexcept { return active_; }

    // Marks this actor for deferred destruction at the end of Level::update().
    void deactivate() noexcept { active_ = false; }

    // Returns a non-owning pointer to the owning Level (nullptr if detached).
    Level* getLevel() const noexcept { return level_; }

    // Creates and attaches an RAII Box2DBody inside the owning Level's PhysicsWorld.
    void initPhysicsBody(const BodyConfig& config);

private:
    friend class Level;
    friend class PhysicsWorld;

    // Associates this actor with its owning Level and PhysicsWorld upon spawning.
    void attachToLevel(Level* level, PhysicsWorld* physics) noexcept;

    // Records current position_ as previousPosition_ right before a fixed physics step.
    void snapshotPreviousTransform() noexcept;

    // Pulls the latest position, velocity, and angle from Box2D into cached fields.
    void syncFromPhysics() noexcept;

    // Computes renderPosition_ by interpolating between previousPosition_ and position_.
    void updateRenderPosition(float alpha) noexcept;

    Vector2D previousPosition_{0.0f, 0.0f};
    Vector2D position_{0.0f, 0.0f};
    Vector2D renderPosition_{0.0f, 0.0f};
    Vector2D velocity_{0.0f, 0.0f};
    float rotation_{0.0f};
    bool active_{true};

    Level* level_{nullptr};
    PhysicsWorld* physics_{nullptr};
    std::unique_ptr<Box2DBody> body_;
};

} // namespace engine

#endif
