// ============================================================================
// OpenGLRenderer.cpp
// Implements 2D sprite-sheet and parallax background rendering in OpenGL 3.3 Core
// using GLM orthographic projection and per-sprite model matrices.
// ============================================================================
#include "OpenGLRenderer.h"
#include "engine/ResourceManager.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace engine {

// Initializes the orthographic projection (Y-up pixel coordinates matching Box2D),
// enables alpha blending, loads the cached shader, and pre-fetches uniform locations.
OpenGLRenderer::OpenGLRenderer(float viewportWidth, float viewportHeight, ResourceManager& resources)
    : width_{viewportWidth}
    , height_{viewportHeight}
    , projection_{glm::ortho(0.0f, viewportWidth, 0.0f, viewportHeight, -1.0f, 1.0f)}
    , shader_{resources.getShader("shaders/sprite.vert", "shaders/sprite.frag")} {
    // Enable standard alpha blending: finalColor = srcColor * srcAlpha + dstColor * (1 - srcAlpha).
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (shader_) {
        // Query uniform locations once so the per-sprite render loop never does driver string lookups.
        locProjection_ = shader_->getUniformLocation("projection");
        locModel_      = shader_->getUniformLocation("model");
        locUvRect_     = shader_->getUniformLocation("uvRect");
        locAlpha_      = shader_->getUniformLocation("alpha");

        // Tell the fragment shader's sampler2D to always read from texture unit GL_TEXTURE0.
        shader_->use();
        shader_->setInt("spriteTexture", 0);
    }
}

// Resizes the OpenGL viewport and updates the orthographic projection matrix to match.
void OpenGLRenderer::setViewport(float width, float height) noexcept {
    width_ = width;
    height_ = height;
    projection_ = glm::ortho(0.0f, width_, 0.0f, height_, -1.0f, 1.0f);
    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
}

// Clears the screen color buffer before drawing the next frame.
void OpenGLRenderer::clear(float r, float g, float b, float a) const noexcept {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

// Binds shared GPU state (shader program, projection uniform, and quad VAO) once per frame
// to avoid repeating expensive OpenGL state switches on every sprite draw call.
void OpenGLRenderer::beginFrame() noexcept {
    boundTextureId_ = 0;
    if (shader_) {
        shader_->use();
        shader_->setMat4(locProjection_, glm::value_ptr(projection_));
    }
    quad_.bind();
}

// Unbinds the shared quad VAO and clears the active texture tracker after all draws finish.
void OpenGLRenderer::endFrame() noexcept {
    quad_.unbind();
    boundTextureId_ = 0;
}

// Avoids redundant glBindTexture() calls when consecutive sprites use the same sprite sheet.
void OpenGLRenderer::bindTextureIfChanged(const Texture& texture) noexcept {
    const unsigned int id = texture.getId();
    if (id != boundTextureId_) {
        texture.bind(0);
        boundTextureId_ = id;
    }
}

// Computes the per-sprite Model matrix and UV tile rectangle, uploads them to the active
// shader using cached uniform locations, and draws the unit quad.
void OpenGLRenderer::drawSprite(const Texture& texture,
                                Vector2D position,
                                Vector2D size,
                                float rotationRad,
                                int frameCol,
                                int frameRow,
                                int cols,
                                int rows,
                                float alpha) noexcept {
    if (!texture.isValid() || !shader_ || cols <= 0 || rows <= 0) {
        return;
    }

    // Matrix multiplication applies right-to-left (read in reverse order):
    // 1) scale unit quad to pixel size -> 2) rotate around Z -> 3) translate to screen position.
    glm::mat4 model{1.0f};
    model = glm::translate(model, glm::vec3{position.x, position.y, 0.0f});
    if (rotationRad != 0.0f) {
        model = glm::rotate(model, rotationRad, glm::vec3{0.0f, 0.0f, 1.0f});
    }
    model = glm::scale(model, glm::vec3{size.x, size.y, 1.0f});

    // Slice one tile out of the (cols x rows) sprite sheet via normalized UV coordinates [0..1].
    // Frame row 0 is the TOP row of the image; because OpenGL's V=1 is top and V=0 is bottom,
    // vOffset steps downward from 1.0 by (frameRow + 1) * vScale.
    const float uScale = 1.0f / static_cast<float>(cols);
    const float vScale = 1.0f / static_cast<float>(rows);
    const float uOffset = static_cast<float>(frameCol) * uScale;
    const float vOffset = 1.0f - static_cast<float>(frameRow + 1) * vScale;

    shader_->setFloat(locAlpha_, alpha);
    shader_->setMat4(locModel_, glm::value_ptr(model));
    shader_->setVec4(locUvRect_, uOffset, vOffset, uScale, vScale);

    bindTextureIfChanged(texture);
    quad_.drawBound();
}

// Renders a full-screen background quad shifted vertically by scrollYNormalized UV units.
// Works seamlessly when the texture was loaded with repeatWrap = true (GL_REPEAT).
void OpenGLRenderer::drawScrollingBackground(const Texture& texture,
                                             float scrollYNormalized,
                                             float repeatX,
                                             float repeatY,
                                             float alpha) noexcept {
    if (!texture.isValid() || !shader_) {
        return;
    }

    // Center the unit quad at the middle of the viewport and scale it to cover the full screen.
    glm::mat4 model{1.0f};
    model = glm::translate(model, glm::vec3{width_ * 0.5f, height_ * 0.5f, 0.0f});
    model = glm::scale(model, glm::vec3{width_, height_, 1.0f});

    shader_->setFloat(locAlpha_, alpha);
    shader_->setMat4(locModel_, glm::value_ptr(model));
    shader_->setVec4(locUvRect_, 0.0f, scrollYNormalized, repeatX, repeatY);

    bindTextureIfChanged(texture);
    quad_.drawBound();
}

} // namespace engine
