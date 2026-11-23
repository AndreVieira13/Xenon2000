// ============================================================================
// QuadMesh.h (Private Engine Header)
// RAII wrapper owning the OpenGL VAO, VBO, and EBO for a 1x1 textured unit quad.
// ============================================================================
#ifndef ENGINE_QUAD_MESH_H
#define ENGINE_QUAD_MESH_H

namespace engine {

class QuadMesh {
public:
    // Generates VAO/VBO/EBO, uploads the 4 quad vertices and 6 indices, and configures attributes.
    QuadMesh();

    // Deletes the owned OpenGL EBO, VBO, and VAO buffers from GPU memory.
    ~QuadMesh();

    QuadMesh(const QuadMesh&) = delete;
    QuadMesh& operator=(const QuadMesh&) = delete;

    // Transfers ownership of the OpenGL buffer handles from other.
    QuadMesh(QuadMesh&& other) noexcept;

    // Releases currently owned buffers and transfers ownership from other.
    QuadMesh& operator=(QuadMesh&& other) noexcept;

    // Binds the quad VAO (which also restores its vertex attributes and EBO binding).
    void bind() const noexcept;

    // Issues an indexed draw call for the 2 triangles (6 indices) assuming bind() was called.
    void drawBound() const noexcept;

    // Unbinds the currently active VAO.
    void unbind() const noexcept;

private:
    // Frees any non-zero EBO, VBO, and VAO IDs on the GPU.
    void release() noexcept;

    unsigned int vao_{0};
    unsigned int vbo_{0};
    unsigned int ebo_{0};
};

} // namespace engine

#endif
