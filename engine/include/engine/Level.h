// ============================================================================
// Level.h
// Owns all Actors and the PhysicsWorld for one scene/level.
// Member declaration order guarantees actors_ and pendingActors_ (and every
// Box2DBody) are destroyed BEFORE physics_ (the Box2DWorld).
// ============================================================================
#ifndef ENGINE_LEVEL_H
#define ENGINE_LEVEL_H

#include "engine/Actor.h"
#include "engine/Vec2.h"
#include <memory>
#include <utility>
#include <vector>

namespace engine {

class InputManager;
class PhysicsWorld;
class Renderer;
class ResourceManager;

class Level {
public:
    // Creates a level with the given pixel dimensions and gravity vector.
    // Throws std::invalid_argument if width or height is non-positive.
    Level(float width, float height, Vec2 gravity = {0.0f, 0.0f});

    // Destroys all actors first, then destroys PhysicsWorld automatically.
    virtual ~Level();

    Level(const Level&) = delete;
    Level& operator=(const Level&) = delete;
    Level(Level&&) = delete;
    Level& operator=(Level&&) = delete;

    // Hook invoked once after attaching to Application to load resources and spawn initial actors.
    virtual void onLoad(ResourceManager& /*resources*/) {}

    // Advances actor logic, steps Box2D at a fixed rate, interpolates transforms,
    // flushes newly spawned actors, and removes deactivated actors.
    virtual void update(float deltaTime, const InputManager& input);

    // Draws background layers first, then renders every active actor on top.
    virtual void render(Renderer& renderer) const;

    // Allocates and registers a purely visual Actor (no physics body) owned by this Level.
    // Safe to call during update() or collision callbacks; returns a non-owning observer pointer.
    template <typename T, typename... Args>
    T* spawnActor(Args&&... args) {
        auto actor = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw = actor.get();
        addActorInternal(std::move(actor), false);
        return raw;
    }

    // Allocates and registers an Actor along with a configured Box2D physics body.
    template <typename T, typename... Args>
    T* spawnActorWithPhysics(const BodyConfig& bodyConfig, Args&&... args) {
        auto actor = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw = actor.get();
        addActorInternal(std::move(actor), true, &bodyConfig);
        return raw;
    }

    // Searches active actors for the first instance of derived type T using dynamic_cast.
    // Returns nullptr if none exists; callers must never delete or store the pointer across frames.
    template <typename T>
    T* findActor() const noexcept {
        for (const auto& actor : actors_) {
            if (actor && actor->isActive()) {
                if (auto* matched = dynamic_cast<T*>(actor.get())) {
                    return matched;
                }
            }
        }
        return nullptr;
    }

    // Returns the playable level width in pixels.
    float getWidth() const noexcept { return width_; }

    // Returns the playable level height in pixels.
    float getHeight() const noexcept { return height_; }

    // Provides read-only access to the list of owned actors.
    const std::vector<std::unique_ptr<Actor>>& getActors() const noexcept { return actors_; }

protected:
    // Hook overridden by concrete levels to draw backgrounds (e.g., parallax layers) behind actors.
    virtual void renderBackground(Renderer& /*renderer*/) const {}

private:
    // Attaches actor to this level, optionally creates its Box2D body, and inserts or queues it.
    void addActorInternal(std::unique_ptr<Actor> actor,
                          bool withPhysics,
                          const BodyConfig* bodyConfig = nullptr);

    // Moves all actors queued during update() into the active actors_ list.
    void flushPendingActors();

    float width_{800.0f};
    float height_{600.0f};

    // IMPORTANT: physics_ MUST be declared before actors_ and pendingActors_ so bodies die before the world.
    std::unique_ptr<PhysicsWorld> physics_;
    std::vector<std::unique_ptr<Actor>> actors_;
    std::vector<std::unique_ptr<Actor>> pendingActors_;
    bool updating_{false};
};

} // namespace engine

#endif
