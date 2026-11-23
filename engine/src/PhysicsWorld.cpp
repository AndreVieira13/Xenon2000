// ============================================================================
// PhysicsWorld.cpp
// Implements Box2D v3.2 RAII wrappers (Box2DWorld, Box2DBody), fixed 60 Hz
// timestep accumulation, body/shape creation, and sensor event dispatching.
// ============================================================================
#include "PhysicsWorld.h"
#include <algorithm>

namespace engine {

// ---------------------------------------------------------------------------
// Box2DWorld
// ---------------------------------------------------------------------------

// Creates a new Box2D physics world with the requested gravity (in m/s^2).
Box2DWorld::Box2DWorld(b2Vec2 gravity) {
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = gravity;
    worldId_ = b2CreateWorld(&worldDef);
}

// Destroys the owned Box2D world automatically.
Box2DWorld::~Box2DWorld() {
    release();
}

// Transfers world handle ownership from other and resets other to b2_nullWorldId.
Box2DWorld::Box2DWorld(Box2DWorld&& other) noexcept : worldId_{other.worldId_} {
    other.worldId_ = b2_nullWorldId;
}

// Destroys any currently owned world and takes ownership from other.
Box2DWorld& Box2DWorld::operator=(Box2DWorld&& other) noexcept {
    if (this != &other) {
        release();
        worldId_ = other.worldId_;
        other.worldId_ = b2_nullWorldId;
    }
    return *this;
}

// Advances Box2D's internal solver by timeStep seconds using subStepCount solver sub-steps.
void Box2DWorld::step(float timeStep, int subStepCount) noexcept {
    if (b2World_IsValid(worldId_)) {
        b2World_Step(worldId_, timeStep, subStepCount);
    }
}

// Releases the Box2D world if still valid and clears worldId_.
void Box2DWorld::release() noexcept {
    if (b2World_IsValid(worldId_)) {
        b2DestroyWorld(worldId_);
        worldId_ = b2_nullWorldId;
    }
}

// ---------------------------------------------------------------------------
// Box2DBody
// ---------------------------------------------------------------------------

// Creates a rigid body inside worldId using the properties in bodyDef.
Box2DBody::Box2DBody(b2WorldId worldId, const b2BodyDef& bodyDef)
    : bodyId_{b2CreateBody(worldId, &bodyDef)} {}

// Destroys the rigid body and all attached shapes automatically.
Box2DBody::~Box2DBody() {
    release();
}

// Transfers body ownership from other and clears other's handle.
Box2DBody::Box2DBody(Box2DBody&& other) noexcept : bodyId_{other.bodyId_} {
    other.bodyId_ = b2_nullBodyId;
}

// Destroys any currently owned body and takes ownership from other.
Box2DBody& Box2DBody::operator=(Box2DBody&& other) noexcept {
    if (this != &other) {
        release();
        bodyId_ = other.bodyId_;
        other.bodyId_ = b2_nullBodyId;
    }
    return *this;
}

// Reads the body's world position in meters and converts it back to screen pixels.
Vec2 Box2DBody::getPositionPx() const noexcept {
    if (b2Body_IsValid(bodyId_)) {
        const b2Vec2 pos = b2Body_GetPosition(bodyId_);
        return Vec2{units::toPixels(pos.x), units::toPixels(pos.y)};
    }
    return Vec2{0.0f, 0.0f};
}

// Converts positionPx to meters and moves the body directly while keeping its current rotation.
void Box2DBody::setPositionPx(Vec2 positionPx) noexcept {
    if (b2Body_IsValid(bodyId_)) {
        const b2Rot rot = b2Body_GetRotation(bodyId_);
        b2Body_SetTransform(bodyId_,
                            b2Vec2{units::toMeters(positionPx.x), units::toMeters(positionPx.y)},
                            rot);
    }
}

// Reads the body's linear velocity in m/s and converts it to px/s.
Vec2 Box2DBody::getVelocityPx() const noexcept {
    if (b2Body_IsValid(bodyId_)) {
        const b2Vec2 vel = b2Body_GetLinearVelocity(bodyId_);
        return Vec2{units::toPixels(vel.x), units::toPixels(vel.y)};
    }
    return Vec2{0.0f, 0.0f};
}

// Converts velocityPx from px/s to m/s and applies it directly to the Box2D body.
void Box2DBody::setVelocityPx(Vec2 velocityPx) noexcept {
    if (b2Body_IsValid(bodyId_)) {
        b2Body_SetLinearVelocity(bodyId_,
                                 b2Vec2{units::toMeters(velocityPx.x), units::toMeters(velocityPx.y)});
    }
}

// Extracts the rotation angle in radians [-pi, pi] from Box2D's (cos, sin) b2Rot representation.
float Box2DBody::getRotationRad() const noexcept {
    if (b2Body_IsValid(bodyId_)) {
        return b2Rot_GetAngle(b2Body_GetRotation(bodyId_));
    }
    return 0.0f;
}

// Sets the body's rotation from an angle in radians while keeping its current position.
void Box2DBody::setRotationRad(float radians) noexcept {
    if (b2Body_IsValid(bodyId_)) {
        const b2Vec2 pos = b2Body_GetPosition(bodyId_);
        b2Body_SetTransform(bodyId_, pos, b2MakeRot(radians));
    }
}

// Destroys the Box2D body if still valid (generation counter guards against already-freed worlds).
void Box2DBody::release() noexcept {
    if (b2Body_IsValid(bodyId_)) {
        b2DestroyBody(bodyId_);
        bodyId_ = b2_nullBodyId;
    }
}

// ---------------------------------------------------------------------------
// PhysicsWorld
// ---------------------------------------------------------------------------

// Creates the underlying Box2DWorld with gravity converted from px/s^2 to m/s^2.
PhysicsWorld::PhysicsWorld(Vec2 gravity)
    : world_{b2Vec2{units::toMeters(gravity.x), units::toMeters(gravity.y)}} {}

// Checks whether adding clamped deltaTime to accumulator_ will execute at least one 60 Hz step.
bool PhysicsWorld::willStep(float deltaTime) const noexcept {
    return (accumulator_ + std::min(deltaTime, kMaxFrameTime)) >= kTimeStep;
}

// Advances the simulation in fixed 1/60 s increments independent of display frame rate,
// processing sensor overlaps after every step. Returns how many steps ran this frame.
int PhysicsWorld::step(float deltaTime) {
    // Clamp deltaTime to prevent the "spiral of death" if the app stalls or hits a breakpoint.
    accumulator_ += std::min(deltaTime, kMaxFrameTime);
    int steps = 0;
    while (accumulator_ >= kTimeStep) {
        world_.step(kTimeStep, kSubStepCount);
        processSensorEvents();
        accumulator_ -= kTimeStep;
        ++steps;
    }
    return steps;
}

// Reads all sensor begin-touch overlaps collected during the latest b2World_Step() and
// dispatches onOverlapBegin() to BOTH participating Actors via their stored userData pointers.
void PhysicsWorld::processSensorEvents() {
    // Sensor event arrays are only valid until the next b2World_Step().
    const b2SensorEvents events = b2World_GetSensorEvents(world_.getId());
    for (int i = 0; i < events.beginCount; ++i) {
        const b2SensorBeginTouchEvent& ev = events.beginEvents[i];
        if (!b2Shape_IsValid(ev.sensorShapeId) || !b2Shape_IsValid(ev.visitorShapeId)) {
            continue;
        }
        const b2BodyId bodyA = b2Shape_GetBody(ev.sensorShapeId);
        const b2BodyId bodyB = b2Shape_GetBody(ev.visitorShapeId);

        // Recover the owning Actor pointers attached to each body via bodyDef.userData.
        auto* actorA = static_cast<Actor*>(b2Body_GetUserData(bodyA));
        auto* actorB = static_cast<Actor*>(b2Body_GetUserData(bodyB));

        // Notify both actors before either is erased; actors call deactivate() instead of deleting
        // themselves mid-loop so pointers stay valid throughout the entire event pass.
        if (actorA && actorB && actorA != actorB && actorA->isActive() && actorB->isActive()) {
            actorA->onOverlapBegin(*actorB);
            actorB->onOverlapBegin(*actorA);
        }
    }
}

// Builds a Box2D rigid body and an axis-aligned box polygon shape from an engine BodyConfig,
// converting all pixel dimensions to meters and configuring sensor events and collision masks.
std::unique_ptr<Box2DBody> PhysicsWorld::createBody(Actor* owner,
                                                    const BodyConfig& config,
                                                    Vec2 positionPx,
                                                    Vec2 velocityPx) {
    // Step 1: Populate a temporary stack b2BodyDef (Box2D copies values internally).
    b2BodyDef bodyDef = b2DefaultBodyDef();
    switch (config.type) {
        case BodyType::Static:    bodyDef.type = b2_staticBody; break;
        case BodyType::Kinematic: bodyDef.type = b2_kinematicBody; break;
        case BodyType::Dynamic:   bodyDef.type = b2_dynamicBody; break;
    }
    bodyDef.position = b2Vec2{units::toMeters(positionPx.x), units::toMeters(positionPx.y)};
    bodyDef.linearVelocity = b2Vec2{units::toMeters(velocityPx.x), units::toMeters(velocityPx.y)};
    bodyDef.isBullet = config.isBullet;                 // Sweeps fast projectiles to prevent tunneling
    bodyDef.motionLocks.angularZ = config.lockRotation; // Box2D v3.2 rotation lock
    bodyDef.userData = owner;                           // Links physics events back to the owning Actor

    // Step 2: Create the RAII body in the Box2D world.
    auto body = std::make_unique<Box2DBody>(world_.getId(), bodyDef);

    // Step 3: Build box geometry using half-extents converted to meters.
    const b2Polygon box = b2MakeBox(units::toMeters(config.halfExtents.x),
                                    units::toMeters(config.halfExtents.y));

    // Step 4: Attach the shape with collision filtering (categoryBits vs maskBits).
    // Note: In Box2D v3.2, enableSensorEvents must be true on BOTH the sensor and the visitor shape.
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = config.isSensor ? 0.0f : config.density;
    shapeDef.isSensor = config.isSensor;
    shapeDef.enableSensorEvents = config.enableSensorEvents;
    shapeDef.filter.categoryBits = config.categoryBits;
    shapeDef.filter.maskBits = config.maskBits;

    b2CreatePolygonShape(body->getId(), &shapeDef, &box);
    return body;
}

} // namespace engine
