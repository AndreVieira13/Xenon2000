// ============================================================================
// Texture.h
// Move-only RAII wrapper for a 2D GPU texture loaded from a BMP image.
// ============================================================================
#ifndef ENGINE_TEXTURE_H
#define ENGINE_TEXTURE_H

#include <string>

namespace engine {

// Controls how OpenGL samples UV coordinates outside the [0, 1] range.
enum class TextureWrap {
    Clamp,          // GL_CLAMP_TO_EDGE (default for sprites)
    Repeat,         // GL_REPEAT (for seamlessly tiling backgrounds)
    MirroredRepeat  // GL_MIRRORED_REPEAT (seamless vertical/horizontal mirroring without edge seams)
};

class Texture {
public:
    // Constructs an empty, invalid texture handle (id_ == 0).
    Texture() = default;

    // Loads a BMP file from disk, keys magenta (255, 0, 255) to transparent, and uploads it to the GPU.
    explicit Texture(const std::string& filePath, TextureWrap wrapMode = TextureWrap::Clamp);

    // Releases the OpenGL texture object if one is owned.
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // Transfers GPU texture ownership from other and leaves other empty (id_ == 0).
    Texture(Texture&& other) noexcept;

    // Releases any currently owned GPU texture, then steals ownership from other.
    Texture& operator=(Texture&& other) noexcept;

    // Activates texture slot (GL_TEXTURE0 + unit) and binds this texture to GL_TEXTURE_2D.
    void bind(unsigned int unit = 0) const noexcept;

    // Returns the raw OpenGL texture handle (0 if uninitialized).
    unsigned int getId() const noexcept { return id_; }

    // Returns the image width in pixels.
    int getWidth() const noexcept { return width_; }

    // Returns the image height in pixels.
    int getHeight() const noexcept { return height_; }

    // Returns true if this object owns a valid GPU texture.
    bool isValid() const noexcept { return id_ != 0; }

private:
    // Deletes the OpenGL texture handle if non-zero and resets id_ to 0.
    void release() noexcept;

    unsigned int id_{0};
    int width_{0};
    int height_{0};
};

} // namespace engine

#endif
