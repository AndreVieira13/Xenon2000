// ============================================================================
// SpriteAnimation.h
// Frame-based sprite-sheet animation controller supporting both looping
// animations (enemies, projectiles) and one-shot animations (explosions).
// ============================================================================
#ifndef ENGINE_SPRITE_ANIMATION_H
#define ENGINE_SPRITE_ANIMATION_H

namespace engine {

struct SpriteAnimation {
    int cols{1};
    int rows{1};
    int startFrame{0};
    int frameCount{1};
    float fps{10.0f};
    bool loop{true};

    float timer{0.0f};
    int currentOffset{0};
    bool finished{false};

    // Rewinds playback back to startFrame and clears the finished flag.
    void reset() noexcept {
        timer = 0.0f;
        currentOffset = 0;
        finished = false;
    }

    // Advances the animation clock by deltaTime seconds and steps frames whenever
    // enough time for one frame (1 / fps) has accumulated.
    void update(float deltaTime) noexcept {
        if (finished || frameCount <= 1 || fps <= 0.0f) {
            return;
        }
        const float frameDuration = 1.0f / fps;
        timer += deltaTime;

        // Loop instead of a single 'if' so a slow frame can skip multiple animation frames accurately.
        while (timer >= frameDuration) {
            timer -= frameDuration;
            if (currentOffset + 1 < frameCount) {
                ++currentOffset;
            } else if (loop) {
                currentOffset = 0;
            } else {
                // One-shot animation reached its last frame: freeze here and flag completion.
                finished = true;
                break;
            }
        }
    }

    // Returns the absolute 0-based tile index inside the sprite sheet.
    int currentFrameIndex() const noexcept {
        return startFrame + currentOffset;
    }

    // Converts the 1D tile index into its 0-based horizontal column on the sheet grid.
    int currentCol() const noexcept {
        return cols > 0 ? (currentFrameIndex() % cols) : 0;
    }

    // Converts the 1D tile index into its 0-based vertical row (0 = top row of the image).
    int currentRow() const noexcept {
        return cols > 0 ? (currentFrameIndex() / cols) : 0;
    }
};

} // namespace engine

#endif
