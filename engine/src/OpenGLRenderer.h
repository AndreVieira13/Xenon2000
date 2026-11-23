// ============================================================================
// OpenGLRenderer.h (Private Engine Header)
// OpenGL 3.3 Core back end implementing the abstract Renderer interface.
// Caches projection matrix, uniform locations, and last-bound texture ID to
// minimize redundant OpenGL driver calls during rendering.
// ============================================================================
#ifndef ENGINE_OPENGL_RENDERER_H
#define ENGINE_OPENGL_RENDERER_H

#include "QuadMesh.h"
#include "engine/Renderer.h"
#include "engine/Shader.h"
#include <glm/glm.hpp>
#include <memory>

namespace engine {

class ResourceManager;

class OpenGLRenderer final : public Renderer {
public:
    // Enables alpha blending, loads the cached sprite shader, and pre-caches all uniform locations.
    OpenGLRenderer(float viewportWidth, float viewportHeight, ResourceManager& resources);
    ~OpenGLRenderer() override = default;

    // Updates glViewport and recomputes the cached orthographic projection matrix.
    void setViewport(float width, float height) noexcept override;

    // Returns the current viewport width in pixels.
    float getWidth() const noexcept override { return width_; }

    // Returns the current viewport height in pixels.
    float getHeight() const noexcept override { return height_; }

    // Clears the OpenGL color buffer with the specified RGBA background color.
    void clear(float r = 0.05f, float g = 0.05f, float b = 0.1f, float a = 1.0f) const noexcept override;

    // Binds the sprite shader, uploads the cached projection matrix, and binds the quad VAO once.
    void beginFrame() noexcept override;

    // Unbinds the quad VAO and resets per-frame texture tracking.
    void endFrame() noexcept override;

    // Builds the sprite's model matrix and UV sub-rectangle, binds its texture if needed, and draws.
    void drawSprite(const Texture& texture,
                    Vector2D position,
                    Vector2D size,
                    float rotationRad = 0.0f,
                    int frameCol = 0,
                    int frameRow = 0,
                    int cols = 1,
                    int rows = 1,
                    float alpha = 1.0f) noexcept override;

    // Draws a full-screen quad with vertically offset UVs to create seamless parallax scrolling.
    void drawScrollingBackground(const Texture& texture,
                                 float scrollYNormalized,
                                 float repeatX = 1.0f,
                                 float repeatY = 1.0f,
                                 float alpha = 1.0f) noexcept override;

private:
    // Binds 'texture' to GL_TEXTURE0 only when its ID differs from the currently bound texture.
    void bindTextureIfChanged(const Texture& texture) noexcept;

    float width_{800.0f};
    float height_{600.0f};
    glm::mat4 projection_{1.0f};

    QuadMesh quad_;
    std::shared_ptr<Shader> shader_;

    int locProjection_{-1};
    int locModel_{-1};
    int locUvRect_{-1};
    int locAlpha_{-1};

    unsigned int boundTextureId_{0};
};

} // namespace engine

#endif
