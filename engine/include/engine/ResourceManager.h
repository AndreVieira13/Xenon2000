// ============================================================================
// ResourceManager.h
// Central GPU resource cache ensuring each texture and shader file is loaded
// at most once and safely shared via std::shared_ptr across actors.
// ============================================================================
#ifndef ENGINE_RESOURCE_MANAGER_H
#define ENGINE_RESOURCE_MANAGER_H

#include "engine/Shader.h"
#include "engine/Texture.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace engine {

class ResourceManager {
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    // Returns a cached Texture for (filePath, wrapMode), or loads and caches it on first request.
    std::shared_ptr<Texture> getTexture(const std::string& filePath,
                                        TextureWrap wrapMode = TextureWrap::Clamp);

    // Returns a cached Shader for (vertexPath, fragmentPath), or compiles and caches it on first request.
    std::shared_ptr<Shader> getShader(const std::string& vertexPath, const std::string& fragmentPath);

    // Empties both caches so GPU resources are freed once no external shared_ptr references them.
    void clear() noexcept;

private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures_;
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders_;
};

} // namespace engine

#endif
