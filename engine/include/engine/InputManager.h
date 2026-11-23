// ============================================================================
// InputManager.h
// Abstracts keyboard scancodes and Xbox/PlayStation gamepads into game Actions.
// Uses the Pimpl idiom so SDL3 headers never leak into public engine headers.
// ============================================================================
#ifndef ENGINE_INPUT_MANAGER_H
#define ENGINE_INPUT_MANAGER_H

#include "engine/Vec2.h"
#include <array>
#include <cstddef>
#include <memory>

namespace engine {

enum class Action {
    MoveLeft,
    MoveRight,
    MoveUp,
    MoveDown,
    Fire,
    Jump,
    Quit,
    Count
};

class InputManager {
public:
    // Allocates internal gamepad state and opens the first connected controller if present.
    InputManager();

    // Closes any open gamepad handle automatically via Impl's RAII GamepadPtr.
    ~InputManager();

    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;
    InputManager(InputManager&&) = delete;
    InputManager& operator=(InputManager&&) = delete;

    // Samples keyboard and gamepad state for the current frame and updates edge-trigger history.
    void update();

    // Opens a newly plugged-in gamepad if no controller is currently active.
    void onGamepadAdded(unsigned int instanceId);

    // Closes the active gamepad if the disconnected device matches our open controller.
    void onGamepadRemoved(unsigned int instanceId);

    // Returns true while the given action is currently pressed down.
    bool isActionHeld(Action action) const noexcept;

    // Returns true only on the single frame the action transitioned from released to pressed.
    bool isActionJustPressed(Action action) const noexcept;

    // Returns the combined 2D movement vector in [-1, 1] x [-1, 1] from keyboard and gamepad.
    Vec2 getMoveAxis() const noexcept;

private:
    struct Impl;

    static constexpr std::size_t kActionCount = static_cast<std::size_t>(Action::Count);

    std::unique_ptr<Impl> impl_;
    std::array<bool, kActionCount> current_{};
    std::array<bool, kActionCount> previous_{};
    Vec2 moveAxis_{0.0f, 0.0f};
};

} // namespace engine

#endif
