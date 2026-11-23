// ============================================================================
// QuadMesh.cpp
// Sets up a reusable centered 1x1 textured quad in GPU memory using VAO/VBO/EBO.
// ============================================================================
#include "QuadMesh.h"
#include <glad/glad.h>

namespace engine {

// Allocates the VAO, VBO, and EBO on the GPU, uploads the 4 corner vertices and
// 6 triangle indices, and records the interleaved vertex attribute layout in the VAO.
QuadMesh::QuadMesh() {
    // Interleaved layout: 4 floats per vertex -> (x, y) local position + (u, v) texture coordinate.
    // Centering the quad on [-0.5, 0.5] makes scaling and Z-rotation pivot naturally around the center.
    constexpr float kVertices[] = {
        -0.5f, -0.5f,   0.0f, 0.0f, // 0: bottom-left
         0.5f, -0.5f,   1.0f, 0.0f, // 1: bottom-right
         0.5f,  0.5f,   1.0f, 1.0f, // 2: top-right
        -0.5f,  0.5f,   0.0f, 1.0f  // 3: top-left
    };

    // Two counter-clockwise triangles reusing the 4 vertices via an Element Buffer Object (EBO).
    constexpr unsigned int kIndices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    // Binding the VAO first causes OpenGL to record all attribute pointers and the EBO inside it.
    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kVertices), kVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kIndices), kIndices, GL_STATIC_DRAW);

    // Attribute 0 -> vertex shader 'layout(location = 0) in vec2 aPos' (stride 16 B, offset 0 B).
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    // Attribute 1 -> vertex shader 'layout(location = 1) in vec2 aTexCoord' (stride 16 B, offset 8 B).
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // IMPORTANT: Always unbind the VAO BEFORE unbinding any EBO; unbinding EBO while VAO is active
    // would detach the index buffer from the VAO!
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Frees all owned GPU buffers on destruction.
QuadMesh::~QuadMesh() {
    release();
}

// Transfers GPU buffer ownership from other and zeroes other's handles to prevent double deletion.
QuadMesh::QuadMesh(QuadMesh&& other) noexcept
    : vao_{other.vao_}, vbo_{other.vbo_}, ebo_{other.ebo_} {
    other.vao_ = 0;
    other.vbo_ = 0;
    other.ebo_ = 0;
}

// Releases existing buffers and transfers ownership from other.
QuadMesh& QuadMesh::operator=(QuadMesh&& other) noexcept {
    if (this != &other) {
        release();
        vao_ = other.vao_;
        vbo_ = other.vbo_;
        ebo_ = other.ebo_;
        other.vao_ = 0;
        other.vbo_ = 0;
        other.ebo_ = 0;
    }
    return *this;
}

// Binds this quad's VAO so subsequent drawBound() calls draw using its vertices and indices.
void QuadMesh::bind() const noexcept {
    glBindVertexArray(vao_);
}

// Submits the indexed triangle draw call (6 indices = 2 triangles) while VAO is already bound.
void QuadMesh::drawBound() const noexcept {
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

// Unbinds the VAO at the end of a render batch/frame.
void QuadMesh::unbind() const noexcept {
    glBindVertexArray(0);
}

// Deletes EBO, VBO, and VAO from GPU memory if they were allocated.
void QuadMesh::release() noexcept {
    if (ebo_ != 0) {
        glDeleteBuffers(1, &ebo_);
        ebo_ = 0;
    }
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
}

} // namespace engine
