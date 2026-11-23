// ============================================================================
// main.cpp
// Application entry point for Xenon 2000 (links Engine only; zero SDL/GL/Box2D).
// ============================================================================
#include "XenonLevel.h"
#include "engine/Application.h"
#include <iostream>
#include <memory>

// Creates the RAII engine::Application, loads XenonLevel, runs the main loop,
// and catches any initialization or runtime exceptions cleanly.
int main() {
    try {
        constexpr int kWindowWidth = 800;
        constexpr int kWindowHeight = 600;

        engine::Application app{"Xenon 2000 - Engine Project", kWindowWidth, kWindowHeight};
        app.setLevel(std::make_unique<xenon::XenonLevel>(
            static_cast<float>(kWindowWidth),
            static_cast<float>(kWindowHeight)));
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
