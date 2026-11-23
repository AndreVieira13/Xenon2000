// ============================================================================
// Application.h
// Top-level engine coordinator owning SDL3, OpenGL context, ResourceManager,
// InputManager, abstract Renderer, and active Level.
//
// Reverse member destruction order guarantees safe teardown:
// level_ -> renderer_ -> input_ -> resources_ -> impl_ (glContext -> window -> sdl).
// ============================================================================
#ifndef ENGINE_APPLICATION_H
#define ENGINE_APPLICATION_H

#include "engine/InputManager.h"
#include "engine/Level.h"
#include "engine/Renderer.h"
#include "engine/ResourceManager.h"
#include <memory>
#include <string>

namespace engine {

class Application {
public:
    // Initializes SDL3, OpenGL 3.3 Core window/context, GLAD, input, and OpenGLRenderer.
    // Throws std::invalid_argument on invalid dimensions or InitError on platform failure.
    Application(const std::string& title, int width, int height);

    // Tears down level, renderer, input, cached GPU resources, and SDL/OpenGL in reverse order.
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    // Takes ownership of a new Level and triggers its onLoad() resource initialization.
    void setLevel(std::unique_ptr<Level> level);

    // Runs the main game loop (poll events -> sample input -> update -> render -> swap) until quit.
    void run();

    // Provides access to the shared texture and shader cache.
    ResourceManager& getResources() noexcept { return resources_; }

    // Returns the window width in pixels.
    int getWidth() const noexcept { return width_; }

    // Returns the window height in pixels.
    int getHeight() const noexcept { return height_; }

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

    int width_{800};
    int height_{600};
    ResourceManager resources_;
    InputManager input_;
    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<Level> level_;
};

} // namespace engine

#endif
