// ============================================================================
// SDLWrappers.h (Private Engine Header)
// RAII wrappers for SDL3 C handles using a zero-overhead stateless custom deleter.
// ============================================================================
#ifndef ENGINE_SDL_WRAPPERS_H
#define ENGINE_SDL_WRAPPERS_H

#include <SDL3/SDL.h>
#include <exception>
#include <memory>
#include <string>
#include <utility>

namespace engine {

// Exception thrown when SDL3 or OpenGL initialization fails.
class InitError : public std::exception {
public:
    // Captures SDL's latest thread-local error message automatically via SDL_GetError().
    InitError() : message_{SDL_GetError()} {}

    // Constructs an InitError with a custom diagnostic message.
    explicit InitError(std::string message) : message_{std::move(message)} {}

    // Returns the stored error description as a null-terminated C string.
    const char* what() const noexcept override {
        return message_.c_str();
    }

private:
    std::string message_;
};

// Stateless custom deleter allowing std::unique_ptr to manage SDL C handles
// with zero size overhead (sizeof(WindowPtr) == sizeof(SDL_Window*)).
struct SDLDeleter {
    // Frees an SDL_Window handle.
    void operator()(SDL_Window* window) const noexcept {
        if (window) SDL_DestroyWindow(window);
    }

    // Frees an SDL_Surface handle.
    void operator()(SDL_Surface* surface) const noexcept {
        if (surface) SDL_DestroySurface(surface);
    }

    // Closes an opened SDL_Gamepad handle.
    void operator()(SDL_Gamepad* gamepad) const noexcept {
        if (gamepad) SDL_CloseGamepad(gamepad);
    }
};

using WindowPtr  = std::unique_ptr<SDL_Window, SDLDeleter>;
using SurfacePtr = std::unique_ptr<SDL_Surface, SDLDeleter>;
using GamepadPtr = std::unique_ptr<SDL_Gamepad, SDLDeleter>;

// RAII scope guard for SDL_Init() and SDL_Quit().
class SDLSystem {
public:
    // Initializes the requested SDL3 subsystems or throws InitError on failure.
    explicit SDLSystem(SDL_InitFlags flags) {
        if (!SDL_Init(flags)) {
            throw InitError{};
        }
    }

    // Shuts down all initialized SDL3 subsystems when this guard leaves scope.
    ~SDLSystem() {
        SDL_Quit();
    }

    SDLSystem(const SDLSystem&) = delete;
    SDLSystem& operator=(const SDLSystem&) = delete;
    SDLSystem(SDLSystem&&) = delete;
    SDLSystem& operator=(SDLSystem&&) = delete;
};

// Move-only RAII wrapper owning an SDL_Window via WindowPtr.
class Window {
public:
    // Creates a centered SDL3 window with the requested dimensions and flags, or throws InitError.
    Window(const std::string& title, int width, int height, SDL_WindowFlags flags)
        : handle_{SDL_CreateWindow(title.c_str(), width, height, flags)} {
        if (!handle_) {
            throw InitError{};
        }
    }

    ~Window() = default;

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) noexcept = default;
    Window& operator=(Window&&) noexcept = default;

    // Returns the non-owning raw SDL_Window* needed by SDL OpenGL calls.
    SDL_Window* get() const noexcept { return handle_.get(); }

    // Swaps front and back OpenGL framebuffers to present the rendered frame.
    void swapBuffers() const noexcept {
        SDL_GL_SwapWindow(handle_.get());
    }

private:
    WindowPtr handle_;
};

// Move-only RAII wrapper owning an OpenGL context bound to an SDL_Window.
class GLContext {
public:
    // Creates an OpenGL context for 'window' using previously configured SDL_GL attributes.
    explicit GLContext(SDL_Window* window)
        : context_{SDL_GL_CreateContext(window)} {
        if (!context_) {
            throw InitError{};
        }
    }

    // Destroys the OpenGL context automatically before the owning window closes.
    ~GLContext() {
        release();
    }

    GLContext(const GLContext&) = delete;
    GLContext& operator=(const GLContext&) = delete;

    // Transfers ownership of the OpenGL context handle from other.
    GLContext(GLContext&& other) noexcept : context_{other.context_} {
        other.context_ = nullptr;
    }

    // Releases any existing context and transfers ownership from other.
    GLContext& operator=(GLContext&& other) noexcept {
        if (this != &other) {
            release();
            context_ = other.context_;
            other.context_ = nullptr;
        }
        return *this;
    }

    // Returns the raw SDL_GLContext handle.
    SDL_GLContext get() const noexcept { return context_; }

private:
    // Destroys the OpenGL context if non-null and resets context_ to nullptr.
    void release() noexcept {
        if (context_) {
            SDL_GL_DestroyContext(context_);
            context_ = nullptr;
        }
    }

    SDL_GLContext context_{nullptr};
};

} // namespace engine

#endif
