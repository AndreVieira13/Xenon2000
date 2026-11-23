// ============================================================================
// Application.cpp
// Bootstraps SDL3, OpenGL 3.3 Core, GLAD, OpenGLRenderer, and InputManager,
// and executes the main game loop separating update logic from rendering.
// ============================================================================
#include "engine/Application.h"
#include "OpenGLRenderer.h"
#include "SDLWrappers.h"
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <algorithm>
#include <stdexcept>

namespace engine {

namespace {
constexpr float kMaxDeltaTime = 0.25f;
} // namespace

// Private platform implementation owning SDL3, the OS window, and the OpenGL context.
struct Application::Impl {
    // Member order enforces reverse destruction: glContext -> window -> sdl (SDL_Quit last).
    SDLSystem sdl;
    Window window;
    GLContext glContext;

    // Initializes SDL video/gamepad, creates the OpenGL 3.3 Core window/context, loads GLAD,
    // enables VSync, and configures the initial OpenGL viewport.
    Impl(const std::string& title, int width, int height)
        : sdl{SDL_INIT_VIDEO | SDL_INIT_GAMEPAD}
        , window{createGLWindow(title, width, height)}
        , glContext{window.get()} {
        // Resolve all OpenGL 3.3 Core function pointers from the active graphics driver via GLAD.
        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
            throw InitError{"Failed to initialize GLAD"};
        }
        SDL_GL_SetSwapInterval(1);
        glViewport(0, 0, width, height);
    }

private:
    // Validates window size, configures OpenGL 3.3 Core attributes, and returns the RAII Window.
    static Window createGLWindow(const std::string& title, int width, int height) {
        if (width <= 0 || height <= 0) {
            throw std::invalid_argument("Window dimensions must be positive");
        }
        // OpenGL context attributes MUST be set before calling SDL_CreateWindow().
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        return Window{title, width, height, SDL_WINDOW_OPENGL};
    }
};

// Initializes platform subsystems (impl_) first so InputManager can immediately query gamepads
// and OpenGLRenderer can compile shaders and create GPU buffers safely.
Application::Application(const std::string& title, int width, int height)
    : impl_{std::make_unique<Impl>(title, width, height)}
    , width_{width}
    , height_{height}
    , resources_{}
    , input_{}
    , renderer_{std::make_unique<OpenGLRenderer>(
          static_cast<float>(width),
          static_cast<float>(height),
          resources_)} {}

// Defined out-of-line so std::unique_ptr<Impl> sees complete type Impl.
Application::~Application() = default;

// Installs a new active Level and calls its onLoad() hook so it can cache textures and spawn actors.
void Application::setLevel(std::unique_ptr<Level> level) {
    level_ = std::move(level);
    if (level_) {
        level_->onLoad(resources_);
    }
}

// Runs the main loop until the user closes the window or presses Quit (Escape):
// polls OS/gamepad events -> measures clamped deltaTime -> updates input & level -> renders -> swaps.
void Application::run() {
    Uint64 lastCounter = SDL_GetPerformanceCounter();
    const double freq = static_cast<double>(SDL_GetPerformanceFrequency());

    bool running = true;
    SDL_Event event{};

    while (running) {
        // 1. Drain all queued OS events every frame so the window stays responsive and handles hot-plugs.
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_GAMEPAD_ADDED) {
                input_.onGamepadAdded(event.gdevice.which);
            } else if (event.type == SDL_EVENT_GAMEPAD_REMOVED) {
                input_.onGamepadRemoved(event.gdevice.which);
            }
        }

        // 2. Measure high-resolution elapsed frame time and clamp spikes (e.g., window drags).
        const Uint64 now = SDL_GetPerformanceCounter();
        const float rawDeltaTime = static_cast<float>(static_cast<double>(now - lastCounter) / freq);
        const float deltaTime = std::min(rawDeltaTime, kMaxDeltaTime);
        lastCounter = now;

        // 3. Sample keyboard and gamepad state.
        input_.update();
        if (input_.isActionJustPressed(Action::Quit)) {
            running = false;
        }

        // 4. UPDATE PHASE: advance all game logic, animations, and fixed-timestep physics.
        if (level_) {
            level_->update(deltaTime, input_);
        }

        // 5. RENDER PHASE: clear framebuffer, bind shared shader/VAO once, draw scene, and present.
        renderer_->clear(0.02f, 0.02f, 0.06f, 1.0f);
        renderer_->beginFrame();
        if (level_) {
            level_->render(*renderer_);
        }
        renderer_->endFrame();

        impl_->window.swapBuffers();
    }
}

} // namespace engine
