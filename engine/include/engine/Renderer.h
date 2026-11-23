// ============================================================================
// Renderer.h
// Pure abstract rendering interface decoupling game entities from OpenGL.
// Actors describe WHAT to draw; concrete subclasses (OpenGLRenderer) handle HOW.
// ============================================================================
#ifndef ENGINE_RENDERER_H
#define ENGINE_RENDERER_H

#include "engine/Texture.h"
#include "engine/Vec2.h"

namespace engine {

class Renderer {
public:
    // Virtual destructor ensures polymorphic deletion cleans up the concrete back end.
    virtual ~Renderer() = default;

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    // Updates the drawing viewport dimensions and recalculates the projection matrix.
    virtual void setViewport(float width, float height) noexcept = 0;

    // Returns the current viewport width in pixels.
    virtual float getWidth() const noexcept = 0;

    // Returns the current viewport height in pixels.
    virtual float getHeight() const noexcept = 0;

    // Clears the framebuffer to the specified RGBA color before rendering a frame.
    virtual void clear(float r = 0.05f, float g = 0.05f, float b = 0.1f, float a = 1.0f) const noexcept = 0;

    // Prepares shared GPU state (shader, projection matrix, quad VAO) once at the start of a frame.
    virtual void beginFrame() noexcept = 0;

    // Unbinds shared GPU state at the end of a frame.
    virtual void endFrame() noexcept = 0;

    // Renders a textured quad (or one tile from a cols x rows sprite sheet) at pixel position,
    // scaled to size, rotated by rotationRad around Z, and tinted by opacity alpha.
    virtual void drawSprite(const Texture& texture,
                            Vec2 position,
                            Vec2 size,
                            float rotationRad = 0.0f,
                            int frameCol = 0,
                            int frameRow = 0,
                            int cols = 1,
                            int rows = 1,
                            float alpha = 1.0f) noexcept = 0;

    // Renders a full-screen background quad with vertically scrolling UV coordinates.
    virtual void drawScrollingBackground(const Texture& texture,
                                         float scrollYNormalized,
                                         float repeatX = 1.0f,
                                         float repeatY = 1.0f,
                                         float alpha = 1.0f) noexcept = 0;

protected:
    Renderer() = default;
};

} // namespace engine

#endif
