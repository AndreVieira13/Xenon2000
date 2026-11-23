// ============================================================================
// ResourceManager.cpp
// Implements hash-map caching for shared Textures and Shaders so no file is
// ever loaded or compiled more than once.
// ============================================================================
#include "engine/ResourceManager.h"

namespace engine {

// Looks up (filePath, wrapMode) in textures_; returns the existing shared_ptr on a cache hit,
// or loads, caches, and returns a new Texture on a cache miss.
std::shared_ptr<Texture> ResourceManager::getTexture(const std::string& filePath, TextureWrap wrapMode) {
    // Include wrap mode in the key so a file requested with different wrap settings stays distinct.
    const char* suffix = "|clamp";
    if (wrapMode == TextureWrap::Repeat) {
        suffix = "|repeat";
    } else if (wrapMode == TextureWrap::MirroredRepeat) {
        suffix = "|mirror";
    }
    const std::string key = filePath + suffix;
    if (auto it = textures_.find(key); it != textures_.end()) {
        return it->second;
    }

    auto texture = std::make_shared<Texture>(filePath, wrapMode);
    textures_.emplace(key, texture);
    return texture;
}

// Looks up (vertexPath, fragmentPath) in shaders_; returns the cached Shader if already compiled,
// or loads from disk, compiles, caches, and returns it.
std::shared_ptr<Shader> ResourceManager::getShader(const std::string& vertexPath,
                                                   const std::string& fragmentPath) {
    const std::string key = vertexPath + "|" + fragmentPath;
    if (auto it = shaders_.find(key); it != shaders_.end()) {
        return it->second;
    }

    auto shader = std::make_shared<Shader>(Shader::fromFiles(vertexPath, fragmentPath));
    shaders_.emplace(key, shader);
    return shader;
}

// Drops all cached references so GPU textures and shaders are destroyed once unreferenced.
void ResourceManager::clear() noexcept {
    textures_.clear();
    shaders_.clear();
}

} // namespace engine
