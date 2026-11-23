// ============================================================================
// InputManager.cpp
// Samples keyboard physical scancodes and SDL3's standardized Gamepad API into
// device-independent engine::Action flags and a combined 2D movement axis.
// ============================================================================
#include "engine/InputManager.h"
#include "SDLWrappers.h"
#include <SDL3/SDL.h>
#include <algorithm>
#include <cmath>

namespace engine {

namespace {
constexpr float kStickDeadzone = 0.2f;
} // namespace

// Private Pimpl holding the RAII SDL_Gamepad handle so <SDL3/SDL.h> stays hidden.
struct InputManager::Impl {
    GamepadPtr gamepad;

    // Scans already-connected gamepads at startup and opens the first one found.
    Impl() {
        int count = 0;
        SDL_JoystickID* ids = SDL_GetGamepads(&count);
        if (ids) {
            if (count > 0) {
                gamepad.reset(SDL_OpenGamepad(ids[0]));
            }
            // SDL_GetGamepads allocates its ID array via SDL_malloc; always release it with SDL_free.
            SDL_free(ids);
        }
    }
};

// Allocates the private gamepad implementation struct.
InputManager::InputManager() : impl_{std::make_unique<Impl>()} {}

// Defined here so std::unique_ptr<Impl> sees the complete definition of Impl.
InputManager::~InputManager() = default;

// Opens a newly connected gamepad if no controller is currently bound.
void InputManager::onGamepadAdded(unsigned int instanceId) {
    if (!impl_->gamepad) {
        impl_->gamepad.reset(SDL_OpenGamepad(static_cast<SDL_JoystickID>(instanceId)));
    }
}

// Releases our open gamepad handle if the disconnected device ID matches it.
void InputManager::onGamepadRemoved(unsigned int instanceId) {
    if (impl_->gamepad) {
        if (SDL_GetGamepadID(impl_->gamepad.get()) == static_cast<SDL_JoystickID>(instanceId)) {
            impl_->gamepad.reset();
        }
    }
}

// Polls keyboard scancodes and the active gamepad once per frame, updating held/just-pressed
// action arrays and merging digital + analog movement into moveAxis_.
void InputManager::update() {
    // Preserve previous frame's state so isActionJustPressed() can detect up -> down transitions.
    previous_ = current_;
    current_.fill(false);

    // 1. Sample physical keyboard scancodes so WASD stays in the same physical spot across layouts.
    const bool* keys = SDL_GetKeyboardState(nullptr);
    if (keys) {
        current_[static_cast<std::size_t>(Action::MoveLeft)]  = keys[SDL_SCANCODE_LEFT]  || keys[SDL_SCANCODE_A];
        current_[static_cast<std::size_t>(Action::MoveRight)] = keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D];
        current_[static_cast<std::size_t>(Action::MoveUp)]    = keys[SDL_SCANCODE_UP]    || keys[SDL_SCANCODE_W];
        current_[static_cast<std::size_t>(Action::MoveDown)]  = keys[SDL_SCANCODE_DOWN]  || keys[SDL_SCANCODE_S];
        current_[static_cast<std::size_t>(Action::Fire)]      = keys[SDL_SCANCODE_SPACE];
        current_[static_cast<std::size_t>(Action::Jump)]      = keys[SDL_SCANCODE_SPACE] || keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP];
        current_[static_cast<std::size_t>(Action::Quit)]      = keys[SDL_SCANCODE_ESCAPE];
    }

    float axisX = 0.0f;
    float axisY = 0.0f;
    if (isActionHeld(Action::MoveLeft))  axisX -= 1.0f;
    if (isActionHeld(Action::MoveRight)) axisX += 1.0f;
    if (isActionHeld(Action::MoveDown))  axisY -= 1.0f;
    if (isActionHeld(Action::MoveUp))    axisY += 1.0f;

    // 2. Merge standardized gamepad inputs (D-Pad, face buttons, and left analog stick).
    if (SDL_Gamepad* gp = impl_->gamepad.get()) {
        if (SDL_GetGamepadButton(gp, SDL_GAMEPAD_BUTTON_DPAD_LEFT))  current_[static_cast<std::size_t>(Action::MoveLeft)] = true;
        if (SDL_GetGamepadButton(gp, SDL_GAMEPAD_BUTTON_DPAD_RIGHT)) current_[static_cast<std::size_t>(Action::MoveRight)] = true;
        if (SDL_GetGamepadButton(gp, SDL_GAMEPAD_BUTTON_DPAD_UP))    current_[static_cast<std::size_t>(Action::MoveUp)] = true;
        if (SDL_GetGamepadButton(gp, SDL_GAMEPAD_BUTTON_DPAD_DOWN))  current_[static_cast<std::size_t>(Action::MoveDown)] = true;

        // SDL3 normalizes Xbox 'A' and PlayStation 'Cross (X)' to SDL_GAMEPAD_BUTTON_SOUTH.
        if (SDL_GetGamepadButton(gp, SDL_GAMEPAD_BUTTON_SOUTH)) {
            current_[static_cast<std::size_t>(Action::Fire)] = true;
            current_[static_cast<std::size_t>(Action::Jump)] = true;
        }

        // Normalize 16-bit signed analog stick range [-32768, 32767] into [-1.0, 1.0]
        // and filter out small resting-center stick drift via kStickDeadzone.
        const float rawX = static_cast<float>(SDL_GetGamepadAxis(gp, SDL_GAMEPAD_AXIS_LEFTX)) / 32767.0f;
        const float rawY = static_cast<float>(SDL_GetGamepadAxis(gp, SDL_GAMEPAD_AXIS_LEFTY)) / 32767.0f;

        if (std::abs(rawX) > kStickDeadzone) {
            axisX = std::clamp(axisX + rawX, -1.0f, 1.0f);
            if (rawX < -kStickDeadzone) current_[static_cast<std::size_t>(Action::MoveLeft)] = true;
            if (rawX >  kStickDeadzone) current_[static_cast<std::size_t>(Action::MoveRight)] = true;
        }

        // SDL gamepad Y axis is negative-up, whereas our engine world is Y-up -> invert sign.
        if (std::abs(rawY) > kStickDeadzone) {
            axisY = std::clamp(axisY - rawY, -1.0f, 1.0f);
            if (rawY < -kStickDeadzone) current_[static_cast<std::size_t>(Action::MoveUp)] = true;
            if (rawY >  kStickDeadzone) current_[static_cast<std::size_t>(Action::MoveDown)] = true;
        }

        // D-Pad overrides analog axis cleanly when pressed.
        if (SDL_GetGamepadButton(gp, SDL_GAMEPAD_BUTTON_DPAD_LEFT))  axisX = -1.0f;
        if (SDL_GetGamepadButton(gp, SDL_GAMEPAD_BUTTON_DPAD_RIGHT)) axisX =  1.0f;
        if (SDL_GetGamepadButton(gp, SDL_GAMEPAD_BUTTON_DPAD_UP))    axisY =  1.0f;
        if (SDL_GetGamepadButton(gp, SDL_GAMEPAD_BUTTON_DPAD_DOWN))  axisY = -1.0f;
    }

    moveAxis_ = Vec2{axisX, axisY};
}

// Returns true if the given action is pressed on the current frame.
bool InputManager::isActionHeld(Action action) const noexcept {
    return current_[static_cast<std::size_t>(action)];
}

// Returns true only on the rising edge when the action was not held last frame but is held now.
bool InputManager::isActionJustPressed(Action action) const noexcept {
    const auto idx = static_cast<std::size_t>(action);
    return current_[idx] && !previous_[idx];
}

// Returns the combined 2D movement vector sampled during the most recent update().
Vec2 InputManager::getMoveAxis() const noexcept {
    return moveAxis_;
}

} // namespace engine
