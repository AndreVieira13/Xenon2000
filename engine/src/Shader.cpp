// ============================================================================
// Shader.cpp
// Loads GLSL source files, compiles and links OpenGL shader programs with
// full info-log error reporting, and exposes uniform upload helpers.
// ============================================================================
#include "engine/Shader.h"
#include <glad/glad.h>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace engine {

// Reads vertex and fragment GLSL text files from disk and constructs a linked Shader program.
Shader Shader::fromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    auto readFile = [](const std::string& path) -> std::string {
        std::ifstream file{path};
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open shader file: " + path);
        }
        std::ostringstream ss;
        ss << file.rdbuf();
        return ss.str();
    };

    const std::string vertSrc = readFile(vertexPath);
    const std::string fragSrc = readFile(fragmentPath);
    return Shader{vertSrc.c_str(), fragSrc.c_str()};
}

// Compiles both shader stages, links them into an OpenGL program, deletes the temporary
// stage objects, and validates GL_LINK_STATUS via the program info log.
Shader::Shader(const char* vertexSource, const char* fragmentSource) {
    // Compile vertex stage first; if fragment compilation throws, free 'vs' to avoid leaking it.
    const GLuint vs = compileStage(GL_VERTEX_SHADER, vertexSource, "VERTEX");
    GLuint fs = 0;
    try {
        fs = compileStage(GL_FRAGMENT_SHADER, fragmentSource, "FRAGMENT");
    } catch (...) {
        glDeleteShader(vs);
        throw;
    }

    programId_ = glCreateProgram();
    glAttachShader(programId_, vs);
    glAttachShader(programId_, fs);
    glLinkProgram(programId_);

    // Stage objects can be deleted right after linking; the linked program holds the compiled code.
    glDeleteShader(vs);
    glDeleteShader(fs);

    // Check linking status and fetch the exact driver error log if linking failed.
    GLint success = 0;
    glGetProgramiv(programId_, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLen = 0;
        glGetProgramiv(programId_, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> infoLog(static_cast<std::size_t>(logLen > 1 ? logLen : 1));
        glGetProgramInfoLog(programId_, logLen, nullptr, infoLog.data());
        std::string msg = std::string("Shader program link error: ") + infoLog.data();
        std::cerr << msg << '\n';
        release();
        throw std::runtime_error(msg);
    }
}

// Deletes the GPU shader program automatically.
Shader::~Shader() {
    release();
}

// Transfers GPU program ownership from other and zeroes other's handle.
Shader::Shader(Shader&& other) noexcept : programId_{other.programId_} {
    other.programId_ = 0;
}

// Releases any currently owned program and steals ownership from other.
Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        release();
        programId_ = other.programId_;
        other.programId_ = 0;
    }
    return *this;
}

// Compiles a single GLSL stage (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER), checks
// GL_COMPILE_STATUS, and reports any GLSL syntax/type errors via glGetShaderInfoLog.
unsigned int Shader::compileStage(unsigned int type, const char* source, const std::string& label) {
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> infoLog(static_cast<std::size_t>(logLen > 1 ? logLen : 1));
        glGetShaderInfoLog(shader, logLen, nullptr, infoLog.data());
        std::string msg = label + " shader compile error: " + infoLog.data();
        std::cerr << msg << '\n';
        glDeleteShader(shader);
        throw std::runtime_error(msg);
    }
    return shader;
}

// Binds this shader program as the active GPU pipeline program.
void Shader::use() const noexcept {
    glUseProgram(programId_);
}

// Looks up the integer uniform location of 'name' in this linked shader program.
int Shader::getUniformLocation(const char* name) const noexcept {
    return static_cast<int>(glGetUniformLocation(programId_, name));
}

// Sends a float uniform value to a pre-cached location.
void Shader::setFloat(int location, float value) const noexcept {
    glUniform1f(location, value);
}

// Sends a vec4 uniform value to a pre-cached location.
void Shader::setVec4(int location, float x, float y, float z, float w) const noexcept {
    glUniform4f(location, x, y, z, w);
}

// Sends an int (or sampler unit) uniform value to a pre-cached location.
void Shader::setInt(int location, int value) const noexcept {
    glUniform1i(location, value);
}

// Sends a 4x4 column-major matrix uniform (GL_FALSE = no transpose, matching GLM's layout).
void Shader::setMat4(int location, const float* matrixPtr) const noexcept {
    glUniformMatrix4fv(location, 1, GL_FALSE, matrixPtr);
}

// Resolves 'name' and sets a float uniform.
void Shader::setFloat(const char* name, float value) const noexcept {
    setFloat(getUniformLocation(name), value);
}

// Resolves 'name' and sets a vec4 uniform.
void Shader::setVec4(const char* name, float x, float y, float z, float w) const noexcept {
    setVec4(getUniformLocation(name), x, y, z, w);
}

// Resolves 'name' and sets an int uniform.
void Shader::setInt(const char* name, int value) const noexcept {
    setInt(getUniformLocation(name), value);
}

// Resolves 'name' and sets a 4x4 matrix uniform.
void Shader::setMat4(const char* name, const float* matrixPtr) const noexcept {
    setMat4(getUniformLocation(name), matrixPtr);
}

// Deletes the GPU shader program if valid and resets programId_ to 0.
void Shader::release() noexcept {
    if (programId_ != 0) {
        glDeleteProgram(programId_);
        programId_ = 0;
    }
}

} // namespace engine
