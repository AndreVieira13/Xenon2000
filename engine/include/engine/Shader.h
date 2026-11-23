// ============================================================================
// Shader.h
// Move-only RAII wrapper for a linked OpenGL GLSL shader program.
// Supports cached integer uniform locations so draw loops skip string lookups.
// ============================================================================
#ifndef ENGINE_SHADER_H
#define ENGINE_SHADER_H

#include <string>

namespace engine {

class Shader {
public:
    // Constructs an empty shader handle (programId_ == 0).
    Shader() = default;

    // Compiles vertexSource and fragmentSource and links them into a GPU shader program.
    // Throws std::runtime_error with OpenGL's info log if compilation or linking fails.
    Shader(const char* vertexSource, const char* fragmentSource);

    // Deletes the GPU shader program automatically.
    ~Shader();

    // Reads vertex and fragment GLSL source files from disk and builds a compiled Shader.
    static Shader fromFiles(const std::string& vertexPath, const std::string& fragmentPath);

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Transfers GPU program ownership from other and zeroes other's handle.
    Shader(Shader&& other) noexcept;

    // Releases any currently owned program, then transfers ownership from other.
    Shader& operator=(Shader&& other) noexcept;

    // Makes this shader program active for subsequent OpenGL draw calls.
    void use() const noexcept;

    // Queries OpenGL once for a uniform variable's integer location (cache this for hot loops).
    int getUniformLocation(const char* name) const noexcept;

    // Uploads a single float uniform using a pre-cached location.
    void setFloat(int location, float value) const noexcept;

    // Uploads a 4-component float vector (vec4) uniform using a pre-cached location.
    void setVec4(int location, float x, float y, float z, float w) const noexcept;

    // Uploads an integer (or sampler unit index) uniform using a pre-cached location.
    void setInt(int location, int value) const noexcept;

    // Uploads a column-major 4x4 float matrix (mat4) uniform using a pre-cached location.
    void setMat4(int location, const float* matrixPtr) const noexcept;

    // Convenience overload: looks up 'name' and sets a float uniform (intended for one-time setup).
    void setFloat(const char* name, float value) const noexcept;

    // Convenience overload: looks up 'name' and sets a vec4 uniform.
    void setVec4(const char* name, float x, float y, float z, float w) const noexcept;

    // Convenience overload: looks up 'name' and sets an int uniform.
    void setInt(const char* name, int value) const noexcept;

    // Convenience overload: looks up 'name' and sets a mat4 uniform.
    void setMat4(const char* name, const float* matrixPtr) const noexcept;

    // Returns the underlying OpenGL shader program handle.
    unsigned int getId() const noexcept { return programId_; }

private:
    // Compiles a single shader stage (vertex or fragment) and throws with info log on failure.
    static unsigned int compileStage(unsigned int type, const char* source, const std::string& label);

    // Deletes the shader program if valid and resets programId_ to 0.
    void release() noexcept;

    unsigned int programId_{0};
};

} // namespace engine

#endif
