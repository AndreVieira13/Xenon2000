// ============================================================================
// Texture.cpp
// Loads BMP sprite sheets via SDL3, converts them to RGBA32, flips rows for
// OpenGL's coordinate system, keys magenta (255, 0, 255) to alpha = 0, and
// uploads the resulting pixel buffer to GPU memory.
// ============================================================================
#include "engine/Texture.h"
#include "SDLWrappers.h"
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace engine {

// Loads a BMP file, prepares an OpenGL-ready RGBA pixel buffer with magenta transparency,
// and creates/configures the OpenGL 2D texture object.
Texture::Texture(const std::string& filePath, TextureWrap wrapMode) {
    // Wrap both temporary SDL surfaces in RAII SurfacePtr so any exception frees them cleanly.
    SurfacePtr raw{SDL_LoadBMP(filePath.c_str())};
    if (!raw) {
        throw std::runtime_error("Failed to load BMP '" + filePath + "': " + SDL_GetError());
    }

    // Convert from 24-bit BMP BGR/RGB into a uniform 32-bit RGBA layout (4 bytes per pixel).
    SurfacePtr rgba{SDL_ConvertSurface(raw.get(), SDL_PIXELFORMAT_RGBA32)};
    raw.reset();
    if (!rgba) {
        throw std::runtime_error("Failed to convert BMP to RGBA32 '" + filePath + "': " + SDL_GetError());
    }

    width_ = rgba->w;
    height_ = rgba->h;

    std::vector<std::uint8_t> pixels(static_cast<std::size_t>(width_) * height_ * 4);

    // Lock the surface if SDL requires it before reading raw pixel memory directly.
    const bool mustLock = SDL_MUSTLOCK(rgba.get());
    if (mustLock && !SDL_LockSurface(rgba.get())) {
        throw std::runtime_error("Failed to lock surface '" + filePath + "': " + SDL_GetError());
    }

    const auto* src = static_cast<const std::uint8_t*>(rgba->pixels);
    const int pitch = rgba->pitch;

    // Copy rows in reverse vertical order (BMP row 0 is top, OpenGL UV V=0 is bottom)
    // and replace pure magenta (255, 0, 255) pixels with transparent alpha = 0.
    for (int y = 0; y < height_; ++y) {
        const std::uint8_t* srcRow = src + static_cast<std::size_t>(height_ - 1 - y) * pitch;
        std::uint8_t* dstRow = pixels.data() + static_cast<std::size_t>(y) * width_ * 4;
        for (int x = 0; x < width_; ++x) {
            const std::uint8_t r = srcRow[x * 4 + 0];
            const std::uint8_t g = srcRow[x * 4 + 1];
            const std::uint8_t b = srcRow[x * 4 + 2];
            std::uint8_t a = srcRow[x * 4 + 3];

            if (r == 255 && g == 0 && b == 255) {
                a = 0;
            }

            dstRow[x * 4 + 0] = r;
            dstRow[x * 4 + 1] = g;
            dstRow[x * 4 + 2] = b;
            dstRow[x * 4 + 3] = a;
        }
    }

    if (mustLock) {
        SDL_UnlockSurface(rgba.get());
    }

    // Create the GPU texture, configure wrap/filter parameters, and upload RGBA pixels.
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);

    // Select OpenGL texture wrapping mode (CLAMP for sprites, REPEAT / MIRRORED_REPEAT for backgrounds).
    GLint glWrapMode = GL_CLAMP_TO_EDGE;
    switch (wrapMode) {
        case TextureWrap::Clamp:          glWrapMode = GL_CLAMP_TO_EDGE; break;
        case TextureWrap::Repeat:         glWrapMode = GL_REPEAT; break;
        case TextureWrap::MirroredRepeat: glWrapMode = GL_MIRRORED_REPEAT; break;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrapMode);

    // GL_NEAREST keeps retro pixel-art sprites sharp without linear blurring.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Tight byte-row alignment avoids driver assumptions about row padding during upload.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Frees the owned GPU texture on destruction.
Texture::~Texture() {
    release();
}

// Transfers GPU texture ownership from other and zeroes other so it won't delete our handle.
Texture::Texture(Texture&& other) noexcept
    : id_{other.id_}, width_{other.width_}, height_{other.height_} {
    other.id_ = 0;
    other.width_ = 0;
    other.height_ = 0;
}

// Releases any existing GPU texture and steals ownership from other.
Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        release();
        id_ = other.id_;
        width_ = other.width_;
        height_ = other.height_;
        other.id_ = 0;
        other.width_ = 0;
        other.height_ = 0;
    }
    return *this;
}

// Selects texture unit (GL_TEXTURE0 + unit) and binds this 2D texture for shader sampling.
void Texture::bind(unsigned int unit) const noexcept {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id_);
}

// Deletes the OpenGL texture ID from GPU memory if valid.
void Texture::release() noexcept {
    if (id_ != 0) {
        glDeleteTextures(1, &id_);
        id_ = 0;
    }
}

} // namespace engine
