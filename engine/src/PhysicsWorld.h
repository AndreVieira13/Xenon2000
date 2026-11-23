// ============================================================================
// PhysicsWorld.h (Private Engine Header)
// Wraps Box2D v3.2 C handles inside RAII C++ classes (Box2DWorld, Box2DBody),
// manages MKS pixel-to-meter scaling, steps at a fixed 60 Hz rate, and
// dispatches Box2D sensor overlap events back to owning Actors.
// ============================================================================
#ifndef ENGINE_PHYSICS_WORLD_H
#define ENGINE_PHYSICS_WORLD_H

#include "engine/Actor.h"
#include "engine/Vector2D.h"
#include <box2d/box2d.h>
#include <memory>

namespace engine {

// Box2D is tuned for metric sizes (0.1 m to 10 m). Mapping 32 px = 1 m turns
// a 64x64 spaceship into a stable 2 m x 2 m rigid body.
namespace units {
    inline constexpr float pixelsPerMeter = 32.0f;

    // Converts a distance or speed from Box2D meters to screen pixels.
    inline constexpr float toPixels(float meters) noexcept { return meters * pixelsPerMeter; }

    // Converts a distance or speed from screen pixels to Box2D meters.
    inline constexpr float toMeters(float pixels) noexcept { return pixels / pixelsPerMeter; }
} // namespace units

// Move-only RAII wrapper owning a Box2D v3.2 b2WorldId handle.
class Box2DWorld {
public:
    // Creates a Box2D physics world with the given gravity vector in m/s^2.
    explicit Box2DWorld(b2Vec2 gravity = b2Vec2{0.0f, 0.0f});

    // Destroys the Box2D world automatically when leaving scope.
    ~Box2DWorld();

    Box2DWorld(const Box2DWorld&) = delete;
    Box2DWorld& operator=(const Box2DWorld&) = delete;

    // Transfers world ownership from other and nulls other's handle.
    Box2DWorld(Box2DWorld&& other) noexcept;

    // Destroys any currently owned world and takes ownership from other.
    Box2DWorld& operator=(Box2DWorld&& other) noexcept;

    // Advances the Box2D solver by timeStep seconds using subStepCount internal sub-steps.
    void step(float timeStep, int subStepCount = 4) noexcept;

    // Returns the raw b2WorldId handle.
    b2WorldId getId() const noexcept { return worldId_; }

private:
    // Destroys the Box2D world if still valid and resets worldId_ to b2_nullWorldId.
    void release() noexcept;

    b2WorldId worldId_{b2_nullWorldId};
};

// Move-only RAII wrapper owning a Box2D v3.2 b2BodyId handle with pixel/meter conversion.
class Box2DBody {
public:
    // Allocates a rigid body inside worldId configured by bodyDef.
    Box2DBody(b2WorldId worldId, const b2BodyDef& bodyDef);

    // Removes this body (and all attached shapes) from its Box2D world automatically.
    ~Box2DBody();

    Box2DBody(const Box2DBody&) = delete;
    Box2DBody& operator=(const Box2DBody&) = delete;

    // Transfers body ownership from other and nulls other's handle.
    Box2DBody(Box2DBody&& other) noexcept;

    // Destroys any currently owned body and takes ownership from other.
    Box2DBody& operator=(Box2DBody&& other) noexcept;

    // Reads the body's world position from Box2D and converts meters to pixels.
    Vector2D getPositionPx() const noexcept;

    // Teleports the body to positionPx (converted to meters) while preserving rotation.
    void setPositionPx(Vector2D positionPx) noexcept;

    // Reads the body's linear velocity from Box2D and converts m/s to px/s.
    Vector2D getVelocityPx() const noexcept;

    // Sets the body's linear velocity in px/s (converted to m/s).
    void setVelocityPx(Vector2D velocityPx) noexcept;

    // Reads the body's Z-axis rotation angle in radians from Box2D's b2Rot complex number.
    float getRotationRad() const noexcept;

    // Sets the body's rotation angle in radians while preserving its position.
    void setRotationRad(float radians) noexcept;

    // Returns the raw b2BodyId handle.
    b2BodyId getId() const noexcept { return bodyId_; }

private:
    // Destroys the Box2D body if still valid and resets bodyId_ to b2_nullBodyId.
    void release() noexcept;

    b2BodyId bodyId_{b2_nullBodyId};
};

// Manages the Box2DWorld, fixed-timestep accumulator, body factory, and collision event dispatch.
class PhysicsWorld {
public:
    // Initializes a Box2D world converting gravity from px/s^2 to m/s^2.
    explicit PhysicsWorld(Vector2D gravity = {0.0f, 0.0f});
    ~PhysicsWorld() = default;

    PhysicsWorld(const PhysicsWorld&) = delete;
    PhysicsWorld& operator=(const PhysicsWorld&) = delete;
    PhysicsWorld(PhysicsWorld&&) = delete;
    PhysicsWorld& operator=(PhysicsWorld&&) = delete;

    // Reports whether accumulating deltaTime will cross kTimeStep and trigger at least one step.
    bool willStep(float deltaTime) const noexcept;

    // Accumulates deltaTime and executes fixed 60 Hz physics steps; returns the step count run.
    int step(float deltaTime);

    // Returns the leftover accumulator fraction in [0, 1) used for smooth render interpolation.
    float getInterpolationAlpha() const noexcept { return accumulator_ / kTimeStep; }

    // Builds an RAII Box2DBody with a configured box polygon shape and collision filter bits.
    std::unique_ptr<Box2DBody> createBody(Actor* owner,
                                          const BodyConfig& config,
                                          Vector2D positionPx,
                                          Vector2D velocityPx);

private:
    // Queries Box2D sensor begin-touch events after a step and invokes onOverlapBegin on both Actors.
    void processSensorEvents();

    Box2DWorld world_;
    float accumulator_{0.0f};
    static constexpr float kTimeStep = 1.0f / 60.0f;
    static constexpr float kMaxFrameTime = 0.25f;
    static constexpr int kSubStepCount = 4;
};

} // namespace engine

#endif
