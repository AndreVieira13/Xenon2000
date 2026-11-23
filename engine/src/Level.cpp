// ============================================================================
// Level.cpp
// Coordinates per-frame actor logic, fixed-timestep Box2D stepping, render
// position interpolation, deferred actor insertion, and O(N) dead-actor cleanup.
// ============================================================================
#include "engine/Level.h"
#include "PhysicsWorld.h"
#include <algorithm>
#include <stdexcept>

namespace engine {

// Validates positive dimensions and initializes this level's dedicated PhysicsWorld.
Level::Level(float width, float height, Vec2 gravity)
    : width_{width}
    , height_{height}
    , physics_{std::make_unique<PhysicsWorld>(gravity)} {
    if (width <= 0.0f || height <= 0.0f) {
        throw std::invalid_argument("Level dimensions must be positive");
    }
}

// Defined out-of-line so std::unique_ptr<PhysicsWorld> sees complete type PhysicsWorld.
// Members destruct in reverse order: pendingActors_ -> actors_ -> physics_.
Level::~Level() = default;

// Links a newly created Actor to this Level, optionally builds its physics body, and either
// appends it directly or queues it in pendingActors_ if we are currently iterating actors_.
void Level::addActorInternal(std::unique_ptr<Actor> actor,
                             bool withPhysics,
                             const BodyConfig* bodyConfig) {
    if (!actor) {
        return;
    }
    actor->attachToLevel(this, physics_.get());
    if (withPhysics && bodyConfig) {
        actor->initPhysicsBody(*bodyConfig);
    }

    // Queue spawns triggered mid-update (e.g., firing missiles or spawning explosions in
    // collision callbacks) so pushing to actors_ cannot reallocate and invalidate iterators.
    if (updating_) {
        pendingActors_.push_back(std::move(actor));
    } else {
        actors_.push_back(std::move(actor));
    }
}

// Moves all actors queued during the update phase into actors_ in one pre-reserved batch.
void Level::flushPendingActors() {
    if (pendingActors_.empty()) {
        return;
    }
    actors_.reserve(actors_.size() + pendingActors_.size());
    for (auto& pending : pendingActors_) {
        actors_.push_back(std::move(pending));
    }
    pendingActors_.clear();
}

// Executes one complete simulation frame:
// 1) actor logic -> 2) pre-step position snapshot -> 3) fixed-step Box2D + sensor events ->
// 4) transform interpolation -> 5) flush queued spawns & erase deactivated actors in O(N).
void Level::update(float deltaTime, const InputManager& input) {
    updating_ = true;

    // 1. Advance actor logic, input responses, and sprite animations.
    for (auto& actor : actors_) {
        if (actor && actor->isActive()) {
            actor->update(deltaTime, input);
        }
    }

    // 2. If the accumulator will cross 1/60 s, snapshot current positions before stepping Box2D.
    const bool stepped = physics_->willStep(deltaTime);
    if (stepped) {
        for (auto& actor : actors_) {
            if (actor && actor->isActive()) {
                actor->snapshotPreviousTransform();
            }
        }
    }

    // 3. Step Box2D at fixed 60 Hz (also dispatches onOverlapBegin) and pull updated transforms.
    if (physics_->step(deltaTime) > 0) {
        for (auto& actor : actors_) {
            if (actor && actor->isActive()) {
                actor->syncFromPhysics();
            }
        }
    }

    // 4. Compute interpolated render positions from the leftover physics accumulator fraction.
    const float alpha = physics_->getInterpolationAlpha();
    for (auto& actor : actors_) {
        if (actor && actor->isActive()) {
            actor->updateRenderPosition(alpha);
        }
    }

    updating_ = false;
    flushPendingActors();

    // 5. Erase all inactive actors in O(N) using the C++17 erase-remove idiom.
    //    std::remove_if compacts active unique_ptrs to the front; erase destroys dead actors
    //    (and their RAII Box2DBodies) in a single tail truncation.
    actors_.erase(
        std::remove_if(actors_.begin(), actors_.end(),
            [](const std::unique_ptr<Actor>& actor) {
                return !actor || !actor->isActive();
            }),
        actors_.end());
}

// Renders background layers first, then draws every active actor on top.
void Level::render(Renderer& renderer) const {
    renderBackground(renderer);
    for (const auto& actor : actors_) {
        if (actor && actor->isActive()) {
            actor->render(renderer);
        }
    }
}

} // namespace engine
